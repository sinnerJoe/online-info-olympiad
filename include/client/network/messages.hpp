#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include <string>
#include <iostream>
#include <vector>
#include <network/protocol_message.hpp>
#include <exception>
using namespace std;

extern int sd;
class SimpleException : ProtocolException{
    public:
        SimpleException( string mess );
        virtual char * what();
};

string sendLogin(string usrname, string password);
string sendUsrForm(string usrname, string password,
                     string email, string realName,
                     string univer, string birthDate);
string sendLogout();
string sendStatus();

string sendLangRemove(string name);
string sendLangAdd(string name, string command, string ext);

string sendOlympicCreate(string title, string desc, string start_time, string dur);

string sendOlympicList();
string sendOlympicRemove(int id);


string sendOlympicApply(int id);
string sendOlympicUndoApply(int id);

string sendOlympicAddExercise(int id_ol,string title, string content,
                 vector<string> &inputs,
                 vector<string> &outputs);

string sendOlympicRemExercise(int id);
string sendOlympicListExercise(int id_ol);

string sendOlympicAddLang(int id_ol, string langName);
string sendOlympicRemLang(int id_ol, string langName);
string sendOlympicListLang(int id_ol);

string sendExerciseCompile(int exId, string langName, string code);

string sendExerciseSubmit(int exId, string langName, string code);
string sendExerciseView(int exId);
string sendGetMyExercises();

string sendGetLeaderboard(int ol_id);

#endif