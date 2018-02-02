
#include <database/database.hpp>
#include <string>
#include <iostream>
#include <pqxx/pqxx>
#include <pqxx/except>
#include <network/client_class.hpp>
#include <fstream>
#include <utility>
using namespace std;

DatabaseManager dbManager;

DatabaseManager::DatabaseManager(){
    ifstream f{"settings.conf"};
    string connStr = "";
    string buff;
    do{
        getline(f, buff);
        connStr += buff;
    }
    while(!f.eof());
    cout << connStr << endl;
    this->conn = new pqxx::connection(connStr);
}



ClientInfo * DatabaseManager::getClient(string usrname, string password){
    dbMutex.lock();
    pqxx::work wrk(*conn);
    try{
        string query = "SELECT * from APPUSR"
                       " where username='" +
                        wrk.esc(usrname) + "' AND "
                        "pass='" + wrk.esc(password) + "'";
        cout << query << endl;

        pqxx::row r = wrk.exec1(query);
        wrk.commit();

        if (r.empty())
        {
            cout << "r.empty()" << endl;
            dbMutex.unlock();
            return nullptr;
        }
        cout << "BEFORE SETTING RES INT USER LOOKUP" << endl;
        ClientInfo * res = new ClientInfo;
        res->id = r[0].as<int>();
        
        res->username = r[1].as<string>(); 
        cout << "USERNAME NOT NULL" << endl;
        res->password = r[2].as<string>(); 
        cout << "password NOT NULL" << endl;
        res->realName = r[3].as<string>(); 
        cout << "realName NOT NULL" << endl;
        res->email = r[4].as<string>(); 
        cout << "email NOT NULL" << endl;
        if(!r[5].is_null())
        res->birthDate = r[5].as<string>(); 
        res->university = r[6].as<string>(); 
        cout << "university NOT NULL" << endl;
        res->role = (ClientType)r[7].as<int>(); 
        cout << "role NOT NULL" << endl;
        dbMutex.unlock();
        return res;
        
    }catch(pqxx::unexpected_rows &e){
        cout << "EROARE SQL LOGARE" << ((exception&)e).what() << endl;
        dbMutex.unlock();
        return nullptr;
    }
}

bool DatabaseManager::isRegistered(string usrname){
    string query = "SELECT * FROM APPUSR "
                   "where username='" +
                   conn->esc(usrname) + "'";
    auto rq = selectQuery(query);
    return rq.size()>0;
}

bool DatabaseManager::registerUser(ClientInfo &usr){
    dbMutex.lock();
    pqxx::work wrk(*conn);
    try{
        string query = "INSERT INTO APPUSR("
                       "userName,"
                       "pass,"
                       "realname,"
                       "email,"
                       "university,"
                       "usrrole) "
                       "VALUES "
                       "('" +
                       wrk.esc(usr.username) + "','" +
                       wrk.esc(usr.password) + "','" +
                       wrk.esc(usr.realName) + "','" +
                       wrk.esc(usr.email) + "','" +
                       wrk.esc(usr.university) + "', 0)";
        cout << query << endl;
        pqxx::result res = wrk.exec(query);
        wrk.commit();
        dbMutex.unlock();
        return res.affected_rows() > 0;
    }catch(pqxx::pqxx_exception &e){
        cout << e.base().what() << endl;
        dbMutex.unlock();
        return false;
    }
}

void DatabaseManager::updateState(ClientState* st){
    dbMutex.lock();
    pqxx::work wrk(*conn);
    string query = "SELECT username, usrrole FROM appusr "
                   "WHERE id=" + to_string(st->cl_id);
    cout << query << endl;
    pqxx::row r = wrk.exec1(query);
    wrk.commit();
    st->cl_username = r[0].as<string>();
    st->clPriv = (ClientType)r[1].as<int>();
    dbMutex.unlock();
}

bool DatabaseManager::addLanguage(string name, string command, string ext){
    dbMutex.lock();
    pqxx::work wrk{*conn};
    try{
        string query = "INSERT INTO pglang(name, cmd, ext) values ('"
        + wrk.esc(name) + "','" + wrk.esc(command) + "'," + quotes(ext)+ ")"; 
        cout << query;
        pqxx::result res = wrk.exec(query);
        wrk.commit();
        dbMutex.unlock();
        return res.affected_rows() > 0;
    }catch(exception &e){
        cout << "Exception: " << e.what() << endl;
        dbMutex.unlock();
        return false;
    }
}

bool DatabaseManager::removeLanguage(string name)
{
    dbMutex.lock();
    pqxx::work wrk{*conn};
    try
    {
        string query = "DELETE FROM pglang where name = '" + wrk.esc(name) + "'";
        cout << query;
        pqxx::result res = wrk.exec(query);
        wrk.commit();
        dbMutex.unlock();
        return res.affected_rows() > 0;
    }
    catch (exception &e)
    {
        cout << "Exception: " << e.what() << endl;
        dbMutex.unlock();
        return false;
    }
}

bool DatabaseManager::insertQuery( string query){
    dbMutex.lock();
    pqxx::work wrk{*conn};
    try
    {
        cout << query;
        pqxx::result res = wrk.exec(query);
        wrk.commit();
        dbMutex.unlock();
        return res.affected_rows() > 0;
    }
    catch (exception &e)
    {
        cout << "Exception: " << e.what() << endl;
        dbMutex.unlock();
        return false;
    }
}

bool DatabaseManager::createOlympic(string title, string descr, string start, string dur){
    string query = "INSERT INTO olympic(name, description,"
                    "  start_time, end_time) "
                    " values ('" 
        + conn->esc(title) +"','"+ conn->esc(descr)+"'," 
         "timestamp '"+ conn->esc(start) + "',timestamp '"
        + conn->esc(start) + "' + interval '"+ conn->esc(dur) +"')";
    return insertQuery(query);
}

pqxx::result DatabaseManager::selectQuery(string query){
    dbMutex.lock();
    pqxx::work wrk(*conn);
    pqxx::result r = wrk.exec(query);
    cout << query << endl;
    wrk.commit();
    dbMutex.unlock();
    return r;
}


string viewOlympics(pqxx::result &res){
    string retMess = "Id | Title \n";
    for (auto row : res)
    {
        retMess += row[0].as<string>() + ". " + row[1].as<string>() + "\n";
    }
    if (res.size() == 0)
        retMess = "No olympics";
    return retMess;
}

string DatabaseManager::getOlympicList(){
    string currentOlympics = "SELECT id, name FROM OLYMPIC WHERE start_time < now() and end_time > now()";
    string passedOlympics = "SELECT id, name FROM OLYMPIC WHERE end_time < now()";
    string intactOlympics = "SELECT id, name FROM OLYMPIC WHERE start_time > now()";
    try{
        pqxx::result res = selectQuery(currentOlympics);
        string mess = "Current Olympics: \n";
        mess += viewOlympics(res) + "\n";
        res = selectQuery(intactOlympics);
        mess += "Olympics that are yet to be started: \n";
        mess += viewOlympics(res) + "\n";
        mess += "Olympics which passed: \n";
        res = selectQuery(passedOlympics);
        mess += viewOlympics(res);
        return mess;
    }catch(exception &e){
        cout << e.what() << endl;
        return "error on query";
    }
}

bool DatabaseManager::removeOlympic(int i){

    string query = "DELETE FROM OLYMPIC WHERE id=" + to_string(i);
    return insertQuery(query);          
}

bool DatabaseManager::undoApplyOlympic(int idUsr, int idOl){
    string query = "DELETE FROM participant where usr_id=" + to_string(idUsr) +
    " AND ol_id=" + to_string(idOl); 
    return insertQuery(query);
}
bool DatabaseManager::applyOlympic(int idUsr, int idOl){
    string query = "INSERT INTO participant values(" + to_string(idUsr) +
                   "," + to_string(idOl) + ")";
    return insertQuery(query);
}

string DatabaseManager::quotes(string s){
    return "'" + conn->esc(s) + "'";
}

bool DatabaseManager::olympicAddExercise(int idOl, string title, string content){
    string query = "INSERT INTO exercise(ol_id, title, content)"
                   "VALUES(" + to_string(idOl) + "," + quotes(title) 
                   + "," + quotes(content) + ")";
    return insertQuery(query); 
}
bool DatabaseManager::olympicRemExercise(int idEx){
    string query = "DELETE FROM exercise where id=" + to_string(idEx);
    return insertQuery(query);
}
string DatabaseManager::olympicListExercise(int idOl){
    string query = "SELECT id, title from exercise where ol_id = "+to_string(idOl);
    pqxx::result qres = selectQuery(query);
    if(qres.size()==0)
        return "No exercises in olympic\n";
    string res = "Exercises: \nID | Title\n";
    for(auto row : qres){
        row[0].as<string>() + ". " + row[1].as<string>() + "\n";
    }
    return res;
}

bool DatabaseManager::olympicAddLang(int idOl, string lang){
    string query = "SELECT id from pglang where name = " + quotes(lang);
    try{
        pqxx::result qres = selectQuery(query);
        if(qres.size() == 0)
            return false;
        string idLang = qres[0][0].as<string>();
        string query = "INSERT INTO olympic_lang values (" + to_string(idOl) + "," + idLang + ")";
        return insertQuery(query);
    }catch(exception &e){
        cout << e.what() << endl;
        return false;
    }
}
bool DatabaseManager::olympicRemLang(int idOl, string lang){
    string query = "SELECT id from pglang where name = " + quotes(lang);
    try
    {
        pqxx::result qres = selectQuery(query);
        if (qres.size() == 0)
            return false;
        string idLang = qres[0][0].as<string>();
        string query = "DELETE FROM olympic_lang where ol_id=" + to_string(idOl) + 
        " and lang_id=" + idLang;
        return insertQuery(query);
    }
    catch (exception &e)
    {
        cout << e.what() << endl;
        return false;
    }
}

string DatabaseManager::olympicListLang(int idOl){
    try{
        string query = "SELECT pglang.name FROM"
                       " PGLANG inner join olympic_lang on id = lang_id"
                       " where ol_id = " + to_string(idOl);
        pqxx::result qres = selectQuery(query);
        if(qres.size()==0)
            return "No languages are available on selected olympic.\n";
        string res = "";

        for(int i=0; i<qres.size(); i++){
            res+= to_string(i) + ". " + qres[i][0].as<string>() + "\n";
        }

        return res;
        
    }catch(exception &e){
        cout << e.what() << endl;
        return "Error on execution";
    }
}

int DatabaseManager::getExerciseId(string title){
    string query = "SELECT id from exercise where title = " + quotes(title) +
                    " ORDER BY id DESC";
    auto qres = selectQuery(query);
    return qres[0][0].as<int>();
}

pair<string, string> DatabaseManager::getLanguageCommandExt(string name){
    string query = "SELECT cmd, ext FROM PGLANG WHERE name = " + quotes(name);
    auto qres = selectQuery(query);
    return pair<string, string> (qres[0][0].as<string>(), qres[0][1].as<string>());
}

bool DatabaseManager::isLanguageAvailableInOlympic(int exId, string name){
    string query = "SELECT * FROM pglang "
                   "INNER JOIN olympic_lang on lang_id = pglang.id "
                   "INNER JOIN olympic on olympic.id = olympic_lang.ol_id "
                   "INNER JOIN exercise on exercise.ol_id = olympic.id "
                   "WHERE exercise.id = " + to_string(exId) + " and "
                   "pglang.name = " + quotes(name);
    auto qres = selectQuery(query);
    return qres.size() > 0; 
}

bool DatabaseManager::isOlympicRunning(int idOl){
    string query = "SELECT * FROM olympic where start_time < now() and now() > end_time "
                    "and id = " + to_string(idOl);
    auto qres = selectQuery(query);
    return qres.size() > 0;
}

bool DatabaseManager::isExerciseAvailable(int idEx, int idUsr){
    string query = "SELECT exercise.id, exercise.title FROM olympic "
                   "INNER JOIN exercise on olympic.id = exercise.ol_id "
                   "INNER JOIN participant on olympic.id = participant.ol_id "
                    "where participant.usr_id =" + to_string(idUsr) + " and "
                    "exercise.id=" + to_string(idEx) +
                    " and exercise.id NOT IN (SELECT ex_id from exercise_points where usr_id="+
                     to_string(idUsr) +") and "
                     " olympic.id NOT IN (SELECT id FROM STALE_OLYMPICS) and" 
                     " olympic.start_time < now()";
    auto qres = selectQuery(query);
    return qres.size() >0;

}

string DatabaseManager::listUserExercises(int idUsr){
    string query = "SELECT exercise.id, exercise.title FROM olympic "
                   "INNER JOIN exercise on olympic.id = exercise.ol_id "
                   "INNER JOIN participant on olympic.id = participant.ol_id "
                    "where participant.usr_id =" + to_string(idUsr) + " and"
                    " exercise.id NOT IN (SELECT ex_id from exercise_points where usr_id="+
                     to_string(idUsr) +") and "
                     " olympic.id NOT IN (SELECT id FROM STALE_OLYMPICS) and"
                     " olympic.start_time < now()";

    auto qres = selectQuery(query);
    if(qres.size()==0)
        return "You have no active exercises :(";
    string res = "Your exercises: \n ID | Title\n";
    for(auto row : qres){
        res += row[0].as<string>() + ". " + row[1].as<string>() + "\n";
    }
    return res;
}
string DatabaseManager::viewExercise(int idEx){
    string query = "SELECT title, content from exercise where id = " + to_string(idEx);
    auto qres = selectQuery(query);
    if(qres.size()==0)
        return "Nonexistent exercise";
    string res = "Title:" + qres[0][0].as<string>() + "\n";
    res += "Description:\n" + qres[0][1].as<string>();
    return res;
}

bool DatabaseManager::submitExercise(int usrId, int exId, int points){
    string query = "INSERT INTO exercise_points(ex_id, usr_id, points)"
                   " VALUES(" + to_string(exId) + "," + to_string(usrId) + "," +
                    to_string(points) + ")";
    return insertQuery(query);
}

string DatabaseManager::listLeaderoboard(int olId){
    string query = "SELECT olympic.name, olympic.description, (now() < end_time and now() > start_time) "
                   " FROM OLYMPIC "
                   " WHERE id=" + to_string(olId);
    auto qres = selectQuery(query);
    if(qres.size() == 0)
        return "No such olympic";
    
    string header = "Olympic: " + qres[0][0].as<string>() + "\n";
    header += "Description: " + qres[0][1].as<string>() + "\n";
    header += "Now running: " + string(qres[0][2].as<bool>() ? "yes" : "no")  + "\n";
    query = "SELECT appusr.username, appusr.university, sum(exercise_points.points) FROM APPUSR"
            " INNER JOIN participant on participant.usr_id = appusr.id "
            " INNER JOIN olympic on participant.ol_id = olympic.id "
            " INNER JOIN exercise on exercise.ol_id = olympic.id  "
            " INNER JOIN exercise_points on exercise.id = exercise_points.ex_id  "
            " where exercise_points.points IS NOT NULL and   "
            " olympic.id = " +
            to_string(olId) +
            " GROUP BY (appusr.id) "
            " ORDER BY sum(exercise_points.points) DESC, max(exercise_points.submit_time) DESC ";
    qres = selectQuery(query);
    string top = "Leaderboard: \n";
    for(int i=0; i<qres.size(); i++){
        top += to_string(i+1) + ".(" + qres[i][2].as<string>() + " pts)"
                 + qres[i][0].as<string>() + " FROM " + qres[i][1].as<string>() + "\n";
    }
    return header + top;
}