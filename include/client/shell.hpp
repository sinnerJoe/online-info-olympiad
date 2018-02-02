#include <string>
#include <iostream>
#include <vector>
#include <array>
#include <client/network/messages.hpp>
#include <boost/filesystem.hpp>
#include <iomanip>
#include <fstream>
using namespace std;

string promptName = "guest";

const string options[] ={
    "login",
    "logout",
    "exit",
    "register",
    "lang_add",
    "lang_remove",
    "new_olympic",
    "list_olympic",
    "del_olympic",
    "apply",
    "undo_apply",
    "exercise_add",
    "exercise_rem",
    "list_ex",
    "lang_olympic_add",
    "lang_olympic_rem",
    "lang_olympic_list",
    "compile_exercise",
    "submit_exercise",
    "current_exercises",
    "view_exercise",
    "view_leaderboard",
    "help"
};
const string description[] = {
    "command to log in",
    "command to log out",
    "exit program",
    "register new user",
    "add support for a programming language to server",
    "remove programming language support from server",
    "create new olympic",
    "list all olympics",
    "remove an olympic",
    "choose to participate to an olympic",
    "choose to not participate to an olympic",
    "create an exercise and add it to an olympic",
    "delete an exercise from an olympic",
    "view all exercises from an olympic",
    "add programming language support to olympic",
    "remove programming language support to olympic",
    "view all programming languages available on an olympic",
    "check if your solution is right",
    "submit your exercise solution",
    "view exercises from olympics that are current running and you are a participant of",
    "view exercise description",
    "show leaderboard of an olympic",
    "view help"
};
const unsigned optionsCount = sizeof(options)/ sizeof(string);

string readWord(){
    string word;
    cin >> word;
    cin.clear();
    cin.ignore(10000,'\n');
    return word;
}

int readInt(){
    int val;
    cin >> val;
    cin.clear();
    cin.ignore(10000,'\n');
    return val;
}

string readFile(string path){
    ifstream f{path};
    string res, buff;
    do{
        getline(f, buff);
        res += buff + "\n";
    } while(!f.eof());
    
    return res;
}

int findOption(string option){
    for(unsigned i =0; i<optionsCount; i++){
        if(option == options[i])
            return i;
    }
    return -1;
}


void loginOption();
void logoutOption();
void renewStatus();
void langAddOption();
void langRemoveOption();
void registerOption();
void createOlympicOption();
void listOlympicOption();
void removelympicOption();
void applyOlimpicOption();
void undoApplyOlimpicOption();
void addExerciseOption();
void remExerciseOption();
void listExerciseOption();
void addLangOlympicOption();
void remLangOlympicOption();
void listLangOlympicOption();
void compileExerciseOption();
void submitExerciseOption();
void viewExerciseOption();
void currentExercisesOption();
void viewLeaderboardOption();
void viewHelpOption();
bool menuSwitch(){
    cout << promptName << ": ";
    int comm = findOption(readWord());
    switch(comm){
        case 0: { //login
            loginOption();
        } break;
        case 1: { //logout
            logoutOption();
        } break;
        case 2: //exit
            cout << "Bye ;)\n";
            return false;
        case 3: //register
            registerOption(); break;
        case 4: //lang_add
            langAddOption(); break;
        case 5: //lang_remove
            langRemoveOption(); break;
        case 6: //new_olympic
            createOlympicOption(); break;
        case 7: //list_olympic
            listOlympicOption(); break;
        case 8: //del_olympic
            removelympicOption(); break;
        case 9:
            applyOlimpicOption(); break;
        case 10:
            undoApplyOlimpicOption(); break;
        case 11:
            addExerciseOption(); break;
        case 12:
            remExerciseOption(); break;
        case 13:
            listExerciseOption(); break;
        case 14:
            addLangOlympicOption(); break;
        case 15:
            remLangOlympicOption(); break;
        case 16:
            listLangOlympicOption(); break;
        case 17:
            compileExerciseOption(); break;
        case 18:
            submitExerciseOption(); break;
        case 19:
            currentExercisesOption(); break;
        case 20:
            viewExerciseOption(); break;
        case 21:
            viewLeaderboardOption(); break;
        case 22:
            viewHelpOption(); break;
        default:{
            cout << "[ERROR] Unknown command\n";
        }
    }
    renewStatus();
    return true;
}

void viewHelpOption(){
    for(int i=0; i<optionsCount; i++){
        cout << left << setw(20) << options[i] << '-' << description[i] << endl;
    }
}

void viewLeaderboardOption(){
    cout << sendOlympicList() << endl;
    cout << "ID: ";
    int id = readInt();
    cout << sendGetLeaderboard(id) << endl;
}

void currentExercisesOption(){
    cout << sendGetMyExercises() << endl;
}
void viewExerciseOption(){
    cout << "ID exercise: ";
    int id = readInt();
    cout << sendExerciseView(id) << endl;
}
void submitExerciseOption(){
    cout << "ID exercise: ";
    int idEx = readInt();
    cout << "Language used: ";
    string lang = readWord();
    string path;
    do
    {
        cout << "Path to file: ";
        getline(cin, path);
    } while (!boost::filesystem::is_regular_file(path));
    string code = readFile(path);
    cout << sendExerciseSubmit(idEx, lang, code) << endl;
}

void compileExerciseOption(){
    cout << "ID exercise: ";
    int idEx = readInt();
    cout << "Language used: ";
    string lang = readWord();
    string path;
    do{
    cout << "Path to file: ";
    getline(cin, path);
    }while(!boost::filesystem::is_regular_file(path));
    string code = readFile(path);
    cout << sendExerciseCompile(idEx, lang, code) << endl;

}

void applyOlimpicOption(){
    listOlympicOption();
    int id;
    cout << "ID olympic:";
    id = readInt();
    cout << sendOlympicApply(id);
}
void undoApplyOlimpicOption(){
    listOlympicOption();
    int id;
    cout << "ID olympic:";
    id = readInt();
    cout << sendOlympicUndoApply(id);
}
void addExerciseOption(){
    cout << "ID olympic: ";
    int id_ol = readInt();
    cout << sendOlympicListExercise(id_ol) << endl;
    string title, content;
    cout << "Title: ";
    getline(cin, title);
    cout << "Exercise Content: ";
    string buff;
    do{
        getline(cin, buff);
        content += buff;
    }while(buff != " ");

    cout << "How many test cases: ";
    int testSz = readInt();
    string path;
    vector<string> inputs;
    vector<string> outputs;
    for(int i=0; i<testSz; i++){
        cout << "Test nr." << i << endl;   
        do{
            cout << "Input file path:";
            getline(cin, path);
        }
        while(!boost::filesystem::is_regular_file(path));
        inputs.push_back(readFile(path));
        
        do{
            cout << "Output file path:";
            getline(cin, path);
        }
        while(!boost::filesystem::is_regular_file(path));
        outputs.push_back(readFile(path));
    }
    cout << sendOlympicAddExercise(id_ol, title, content, inputs, outputs) << endl;

}
void remExerciseOption(){
    cout << "ID exercise: ";
    int id = readInt();
    cout << sendOlympicRemExercise(id) << endl;
}
void listExerciseOption(){
    cout << "ID Olympic: ";
    int ol_id = readInt();
    cout << sendOlympicListExercise(ol_id) << endl;
}
void addLangOlympicOption(){
    cout << "ID Olympic: ";
    int ol_id = readInt();
    cout << sendOlympicListLang(ol_id) << endl;
    cout << "Language name: ";
    string name = readWord();
    cout << sendOlympicAddLang(ol_id, name) << endl;
}
void remLangOlympicOption(){
    cout << "ID Olympic: ";
    int ol_id = readInt();
    cout << "Language name: ";
    string name = readWord();
    cout << sendOlympicRemLang(ol_id, name) << endl;
}
void listLangOlympicOption(){
    cout << "ID Olympic: ";
    int ol_id = readInt();
    cout << sendOlympicListLang(ol_id) << endl;
}

void removelympicOption(){
    listOlympicOption();
    cout << "Id to remove: ";
    int id;
    cin >> id;
    cout << sendOlympicRemove(id) << endl;
}

void listOlympicOption(){
    cout << sendOlympicList() << endl;
}

void loginOption(){
    string username, password;
    cout << "Username: ";
    username = readWord();
    cout << "Password: ";
    password = readWord();
    cout << "Response: " << sendLogin(username, password) << endl;
}

void registerOption(){
    string username, password, email, univer, data_nastere, realName;
    cout << "Username*: ";
    username = readWord();
    cout << "Password*: ";
    password = readWord();
    cout << "Your name: ";
    getline(cin, realName);
    cout << "Email: ";
    email = readWord();
    cout << "University: ";
    getline(cin, univer);
    cout << "Data nastere(yyyy-mm-dd): ";
    data_nastere = readWord();
    try{
        cout << sendUsrForm(username, password, email, realName, univer, data_nastere);
    }catch(SimpleException &e){
        cout << "ERROR: " << e.what() << endl;
    }
}

void logoutOption(){
    cout << sendLogout() << endl;
}

void renewStatus(){
     promptName = sendStatus();
}

void langAddOption(){
    cout << "Language name:";
    string name = readWord();
    cout << "Command(example: gcc <src> -o <out>): ";
    string command;
    getline(cin, command);
    cout << "File extension: ";
    string ext = readWord();
    cout << sendLangAdd(name, command, ext) << endl;
}

void langRemoveOption(){
    cout << "Language name:";
    string name = readWord();
    cout << sendLangRemove(name) << endl;
}

void createOlympicOption(){
    string title, desc, start, dur;
    cout << "Title:";
    getline(cin, title);
    cout << "Description:";
    getline(cin, desc);
    cout << "Start time(yyyy-mm-dd hh:mm:ss): ";
    getline(cin, start);
    cout << "Duration(ex: 1 hour, 1:20): ";
    getline(cin, dur);
    cout << sendOlympicCreate(title, desc,  start, dur) << endl;
}