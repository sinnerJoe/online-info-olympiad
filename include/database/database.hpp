#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <network/client_class.hpp>
#include <string>
#include <pqxx/pqxx>
#include <mutex>
#include <utility>

using namespace std;


class DatabaseManager{
    pqxx::connection *conn;
    mutex dbMutex;
    string quotes(string s);
    public:
    ClientInfo *getClient(string usrname, string password);
    void updateState(ClientState *);
    bool isRegistered(string usrname);
    bool registerUser(ClientInfo &usr);
    bool addLanguage(string name, string command, string ext);
    bool removeLanguage(string name);
    bool createOlympic(string title, string descr, string start, string dur);
    bool insertQuery(string query);
    pqxx::result selectQuery(string query);
    string getOlympicList();
    bool removeOlympic(int i);
    bool applyOlympic(int idUsr, int idOl);
    bool undoApplyOlympic(int idUsr, int idOl);
    bool olympicAddExercise(int idOl, string title, string content);
    bool olympicRemExercise(int idEx);
    string olympicListExercise(int idOl);
    bool olympicAddLang(int idOl, string lang);
    bool olympicRemLang(int idOl, string lang);
    string olympicListLang(int idOl);
    int getExerciseId(string title);
    pair<string, string> getLanguageCommandExt(string name);
    bool isLanguageAvailableInOlympic(int exId, string name);
    bool isOlympicRunning(int idOl);
    bool isExerciseAvailable(int idEx, int idUsr);
    string listUserExercises(int idUsr);
    string viewExercise(int idEx);
    bool submitExercise(int usrId, int exId, int points);
    string listLeaderoboard(int olId);
    DatabaseManager();
};

extern DatabaseManager dbManager;
#endif