#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <utility>
#include <thread>
#include <future>
#include <chrono>
#include <fsystem/execute_file.hpp>
#include <utility/logged_val.hpp>
using namespace std;



int isCompiled(string command)
{
    boost::regex exp1(".*<src>.*"), exp2(".*<out>.*");
    boost::cmatch dummy;
    if (boost::regex_match(command.c_str(), dummy, exp1))
        cout << "It's ok\n";
    else
        return -1;
    return boost::regex_match(command.c_str(), dummy, exp2);
}

string generateCommand(string command, string src, string out)
{
    boost::regex expr("(<out>)|(<src>)");
    string format = "(?1" + out + ")(?2" + src + ")";
    ostringstream t(std::ios::out);
    std::ostream_iterator<char, char> oi(t);
    boost::regex_replace(oi, command.begin(), command.end(), expr, format.c_str(),
                         boost::match_default | boost::format_all);
    return t.str();
}

bool interpret(string command, string inputFile, string saveOutputFile)
{
    cout << "Interpret( " << inputFile << ", " << saveOutputFile 
         << ", " << getFirstWord(command) <<", " << command << ")" << endl;
    return executeProgram(inputFile, saveOutputFile, getFirstWord(command), command);
}

void compile(string command, string saveOutputFile)
{
    string execComm = command + " > " + saveOutputFile;
    system(execComm.c_str());
}


bool isTestOnDisk(string exerciseFolder, int nr)
{
    return boost::filesystem::is_regular_file(exerciseFolder + to_string(nr) + ".in") &&
           boost::filesystem::is_regular_file(exerciseFolder + to_string(nr) + ".out");
}


LoggedVal compareFiles(string expPath, string execResPath)
{
    ifstream expected{expPath}, out{execResPath};
    char expC, outC;
    string message;
    do
    {
        expected >> expC;
        out >> outC;

        if (expC == outC)
            message.push_back(expC);
        else
            break;
    }
    while (!expected.eof() && !out.eof());
    // boost::filesystem::remove(execResPath);
    if (expected.eof() && out.eof() && expC == outC)
    {
        return LoggedVal("Result is right", true);
    }
    else if (expected.eof() && !out.eof())
    {
        string mess = "Expected end of file but found '";
        mess.push_back(outC);
        mess.push_back('\'');
        mess += "\nOutput: \n" + message;
        cout << mess << endl;
        return LoggedVal(mess, false);
    }
    else if (!expected.eof() && out.eof())
    {
        string mess = "Expected '";
        mess.push_back(expC);
        mess.push_back('\'');
        mess += " but found end of file";
        mess += "\nMatched output:\n" + message;
        cout << mess << endl;
        return LoggedVal(mess, false);
    }
    else if (!expected.eof() && !out.eof())
    {
        string mess = "Expected '";
        mess.push_back(expC);
        mess.push_back('\'');
        mess += " but found '";
        mess.push_back(outC);
        mess.push_back('\'');
        mess += "\nMatched output:\n" + message;
        cout << mess<<endl;
        return LoggedVal(mess, false);
    }
}

LoggedVal evaluateCompiledLanguage(string command,
                                            string src,
                                            string execPath,
                                            string exerciseFolder,
                                            string outputSaveFile)
{

    string generatedCommand = generateCommand(command, src, execPath);
    compile(generatedCommand, execPath);
    int points = 0;
    for (int i = 0; isTestOnDisk(exerciseFolder, i); i++)
    {
        string inputFile = exerciseFolder + to_string(i) + ".in";
        string expectedOutput = exerciseFolder + to_string(i) + ".out";
        cout << "ExecPath: "<< execPath << endl;
        bool success = executeProgram(inputFile, outputSaveFile, execPath);
        if (!success)
            return LoggedVal ("Error on execution of " + to_string(i) +
                             " test case. Execution took more than 1s.", points);
        LoggedVal comp = compareFiles(expectedOutput, outputSaveFile);
        if (!comp.val)
            return LoggedVal (comp.log, points);
        points++;
    }
    return LoggedVal("", points);
}

LoggedVal evaluateInterpretedLanguage(string command, string src, string exerciseFolder, string outputSaveFile)
{
    int points = 0;
    for (int i = 0; isTestOnDisk(exerciseFolder, i); i++)
    {
        string inputFile = exerciseFolder + to_string(i) + ".in";
        string expectedOutput = exerciseFolder + to_string(i) + ".out";
        string generatedCommand = generateCommand(command, src, "");
        bool success = interpret(generatedCommand, inputFile, outputSaveFile);
        // bool success = executeProgram(inputFile, outputSaveFile,
        //          getFirstWord(generatedCommand), generatedCommand);
        if (success)
        {
            auto comp = compareFiles(expectedOutput, outputSaveFile);
            if (!comp.val)
                return LoggedVal(comp.log, points);
            points++;
            cout << "INTERPRET SUCCESS ";
        }
        else
            return LoggedVal ("Script was executing longer than 1s.", points);
        cout << "points = " << points << endl;
    }
    cout << "returned points = " << points << endl;
    return LoggedVal("", points);
}

LoggedVal codeEvaluation(int id_usr, int id_ex, string code,
                                 string command, string ext, string workingFolder)
{
    cout << "CODE EVALUATION START" << endl;
    string exerciseFolder = "exercises/" + to_string(id_ex) + "/";
    string outputFolder = workingFolder + "/" + to_string(id_usr) + "/";
    string src = outputFolder + "srcfile." + ext;
    string out = outputFolder + "usr.exe";
    string outputSaveFile = outputFolder + "usr.out";
    boost::filesystem::create_directories(outputFolder);
    ofstream of(src);
    of << code;
    of.close();
    int compiled = isCompiled(command);
    LoggedVal res;
    if (compiled == -1)
    {
        cout << "Bad db command form!" << endl;
        return LoggedVal("Bad server setup for your programming language", 0);
    }
    else if (compiled)
    {
        cout << "COMPILED LANGUAGE " << endl;
        res = evaluateCompiledLanguage(command, src, out, exerciseFolder, outputSaveFile);
        // boost::filesystem::remove(out);
        // boost::filesystem::remove(outputSaveFile);
        // boost::filesystem::remove(src);
        if(res.log.size()!=0)
            return res;
    }
    else
    {
        cout << "INTERPRETED LANGUAGE \n";
        res = evaluateInterpretedLanguage(command, src, exerciseFolder, outputSaveFile);
        // boost::filesystem::remove(out);
        // boost::filesystem::remove(outputSaveFile);
        // boost::filesystem::remove(src);
        if(res.log.size()!=0)
            return res;
        
    }
    cout << "res.second = " << res.val << endl;
    return LoggedVal("Everything executed well", res.val);
}
