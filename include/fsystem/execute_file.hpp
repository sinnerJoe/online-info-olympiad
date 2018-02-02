#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <chrono>
#include <thread>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <boost/tokenizer.hpp>
using namespace std;

vector<char *> stringToArgs(string line)
{
    typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
    tokenizer tok{line, boost::char_separator<char>{" "}};
    vector<char *> result;

    for (tokenizer::iterator it = tok.begin(); it != tok.end(); ++it)
    {
        int toksize = (*it).size() + 1;
        char *token = new char[toksize];
        // cerr << "Token: " << *it << endl;
        memcpy(token, (*it).c_str(), toksize);
        result.push_back(token);
    }
    return result;
}

string getFirstWord(const string &command)
{
    int i = command.find_first_of(' ');
    if (i == command.size())
        return command;
    else
        return command.substr(0, i);
}

bool executeProgram(string inPath, string outputSaveFile, string exec, string command = "")
{
    int pid = fork();

    if (pid == 0)
    { // child
        cout << "child start" << endl;
        cout << "Output path: " << outputSaveFile << endl;
        char **arguments = (command == "") ? nullptr : &stringToArgs(command)[0];
        remove(outputSaveFile.c_str());
        int resFile = open(outputSaveFile.c_str(), O_WRONLY | O_CREAT, 0666);
        int inFile = open(inPath.c_str(), O_RDONLY);
        dup2(inFile, 0);
        dup2(resFile, 1);
        close(resFile);
        close(inFile);
        if (arguments)
        {
            cout << "INTERPRETING CODE " << endl;
            execvp(exec.c_str(), arguments);
        }
        else
            execl(exec.c_str(), exec.c_str(), NULL);
        exit(1);
    }
    else
    {
        cout << "pid:" << pid << endl;
        bool programEnded = false;

        for (int i = 0; i < 9; i++)
        {
            this_thread::sleep_for(0.1s);
            int status = 0;
            int ret = waitpid(pid, &status, WNOHANG);
            if (ret > 0 && WIFEXITED(status))
            {
                programEnded = true;
                break;
            }
        }
        if (programEnded)
        {
            return true;
        }
        kill(pid, SIGINT);
        cout << "Remove=" << remove(outputSaveFile.c_str()) << endl;
        return false;
    }
}