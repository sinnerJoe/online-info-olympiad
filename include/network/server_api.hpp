#include <iostream>
#include <poll.h>
#include <mutex>
#include <thread>
#include <string>
#include <utility>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <network/protocol_message.hpp>
#include <network/client_class.hpp>
#include <database/database.hpp>
#include <fsystem/fsystem.hpp>
#include <fsystem/compilation.hpp>
#include <utility/logged_val.hpp>
using namespace std;

#ifndef SERVER_API_HPP
#define SERVER_API_HPP

void bogusRead(int fd){
    char byte;
    read(fd, (void*)&byte, 1);
}

void executeLogin(int fd,  ClientPoll & worker);
void executeRegister(int fd,  ClientPoll & worker);
void executeStatus(int fd,  ClientPoll & worker);
void executeLogout(int fd,  ClientPoll & worker);
void sendError(int fd, string message);
void sendConfirm(int fd, string message);
void executeLangAdd(int fd,  ClientPoll & worker);
void executeLangRemove(int fd,  ClientPoll & worker);
void executeOlympCreate(int fd,  ClientPoll & worker);
void executeOlympList( int fd,  ClientPoll & worker);
void executeOlympRemove(int fd,  ClientPoll & worker);
void executeOlympApply(int fd,  ClientPoll & worker);
void executeOlympUndoApply(int fd,  ClientPoll & worker);
void executeOlympAddExercise(int fd,  ClientPoll & worker);
void executeOlympRemExercise(int fd,  ClientPoll & worker);
void executeOlympAddLang(int fd,  ClientPoll & worker);
void executeOlympRemLang(int fd,  ClientPoll & worker);
void executeOlympListExercise(int fd,  ClientPoll & worker);
void executeOlympListLang(int fd,  ClientPoll & worker);
void executeExerciseCompile(int fd,  ClientPoll & worker);
void executeExerciseSubmit(int fd,  ClientPoll & worker);
void executeExerciseView(int fd,  ClientPoll & worker);
void executeGetUsrExercises(int fd,  ClientPoll & worker);
void executeGetLeaderboard(int fd,  ClientPoll & worker);
void executeCommand(int fd,  ClientPoll & worker)
{
    MessageType select = receiveFlag(fd);
    cout << "string select = " << (int)select << endl;
    switch (select)
    {
        case MessageType::LOGIN: executeLogin(fd, worker); break;
        case MessageType::REGISTER: executeRegister(fd, worker); break;
        case MessageType::USR_GETSTATUS: executeStatus(fd, worker); break;
        case MessageType::LOGOUT: executeLogout(fd, worker); break;
        case MessageType::LANG_ADD: executeLangAdd(fd, worker); break;
        case MessageType::LANG_REMOVE: executeLangRemove(fd, worker); break;
        case MessageType::OL_CREATE: executeOlympCreate(fd, worker); break;
        case MessageType::OL_LIST: executeOlympList(fd, worker); break;
        case MessageType::OL_REMOVE: executeOlympRemove(fd, worker); break;
        case MessageType::OL_APPLY: executeOlympApply(fd, worker); break;
        case MessageType::OL_UNDO_APPLY: executeOlympUndoApply(fd, worker); break;
        case MessageType::OL_ADD_EX: executeOlympAddExercise(fd, worker); break;
        case MessageType::OL_REMOVE_EX: executeOlympRemExercise(fd, worker); break;
        case MessageType::OL_ADD_LANG: executeOlympAddLang(fd, worker); break;
        case MessageType::OL_REMOVE_LANG: executeOlympRemLang(fd, worker); break;
        case MessageType::OL_LIST_EXERCISE: executeOlympListExercise(fd, worker); break;
        case MessageType::OL_LIST_LANG: executeOlympListLang(fd, worker); break;
        case MessageType::EX_COMPILE: executeExerciseCompile(fd, worker); break;
        case MessageType::EX_SUBMIT: executeExerciseSubmit(fd, worker); break;
        case MessageType::EX_GETINFO: executeExerciseView(fd, worker); break;
        case MessageType::MY_EX_LIST: executeGetUsrExercises(fd, worker); break;
        case MessageType::OL_LEADERBOARD: executeGetLeaderboard(fd, worker); break;
        default: {
            char * c = (char *) &select;
            cout << "first 2 bytes: " << (unsigned short) select << endl;
            cout << "default messagetype " << c[0] << " " << c[1] << " " << 
                        c[2] << " " << c[3] << endl;
            
            }
             break;
    }
}



void sendError(int fd, string message)
{
    cout << message << endl;
    sendFlag(fd, MessageType::ERROR);
    sendString(fd, message);
}

void sendConfirm(int fd, string message)
{
    cout << message;
    sendFlag(fd, MessageType::OK);
    sendString(fd, message);
}

void executeGetLeaderboard(int fd,  ClientPoll & worker){
    int idOl = receiveInt(fd);
    string res = dbManager.listLeaderoboard(idOl);
    sendString(fd, res);
}

void executeGetUsrExercises(int fd,  ClientPoll & worker){
    auto usr = worker.getClient(fd);
    if(usr==nullptr){
        sendError(fd, "You should log in.");
    }
    else{
        sendConfirm(fd, dbManager.listUserExercises(usr->cl_id));
    }
}

void executeExerciseView(int fd,  ClientPoll & worker){
    int exId = receiveInt(fd);
    auto usr = worker.getClient(fd);
    if (usr == nullptr)
    {
        sendError(fd, "You don't have rights for this operation.");
    }
    else if (dbManager.isExerciseAvailable(exId, usr->cl_id))
    {
        usr->UpdateState();
        sendConfirm(fd, dbManager.viewExercise(exId));
    }else{
        sendError(fd, "You are not a participant or exercise doesn't exist.");
    }
}

void executeExerciseSubmit(int fd,  ClientPoll & worker){
    int exId = receiveInt(fd);
    string langName = receiveString(fd);
    string code = receiveString(fd);
    auto usr = worker.getClient(fd);
    if(usr == nullptr){
        sendError(fd, "You don't have rights for this operation.");
    }else{
            usr->UpdateState();
        if (dbManager.isLanguageAvailableInOlympic(exId, langName) &&
            dbManager.isExerciseAvailable(exId, usr->cl_id) )
        {
            auto commExt = dbManager.getLanguageCommandExt(langName);
            auto result = codeEvaluation(usr->cl_id, exId, code, commExt.first, commExt.second, "temp");
            string mess = to_string(result.val) + " tests passed successfully \n" + result.log;
            if(dbManager.submitExercise(usr->cl_id, exId, result.val))
                sendConfirm(fd, mess);
            else{
                sendError(fd, "Exercise has been already submitted");
            }
        }
        else
            sendError(fd, "Error on operation execution");
    }
}

void executeExerciseCompile(int fd,  ClientPoll & worker){
    int exId = receiveInt(fd);
    string langName = receiveString(fd);
    string code = receiveString(fd);
    auto usr = worker.getClient(fd);
    if (usr == nullptr)
    {
        sendError(fd, "You don't have rights for this operation.");
    }
    else
    {
        if (dbManager.isLanguageAvailableInOlympic(exId, langName) &&
            dbManager.isExerciseAvailable(exId, usr->cl_id) ){
            usr->UpdateState();
            auto commExt = dbManager.getLanguageCommandExt(langName);
            cout << "AFTER GET LANGUAGE EXTENSION";
            auto result = codeEvaluation(usr->cl_id, exId, code, commExt.first, commExt.second, "temp");
            string mess = to_string(result.val) + " tests passed successfully \n" + result.log;
            sendConfirm(fd, mess);
        }
        else
            sendError(fd, "Error on operation execution");
    }
}

void executeOlympListLang(int fd,  ClientPoll & worker){
    int idOl = receiveInt(fd);
    sendString(fd, dbManager.olympicListLang(idOl));
}

void executeOlympListExercise(int fd,  ClientPoll & worker){
    int idOl = receiveInt(fd);
    sendString(fd, dbManager.olympicListExercise(idOl));
}

void executeOlympApply(int fd,  ClientPoll & worker){
    int idOl = receiveInt(fd);
    auto usr = worker.getClient(fd);
    if (usr == nullptr)
    {
        sendError(fd, "You don't have rights for this operation.");
    }
    else{
        usr->UpdateState();
        if(dbManager.applyOlympic(usr->cl_id, idOl))
            sendConfirm(fd, "Applied to olymic successfully.");
        else
            sendError(fd, "Error on operation execution");   
    }

}
void executeOlympUndoApply(int fd,  ClientPoll & worker){
    int idOl = receiveInt(fd);
    auto usr = worker.getClient(fd);
    if (usr == nullptr)
    {
        sendError(fd, "You don't have rights for this operation.");
    }
    else
    {
        usr->UpdateState();
        if (dbManager.undoApplyOlympic(usr->cl_id, idOl))
            sendConfirm(fd, "You were successfully removed from olympic.");
        else
            sendError(fd, "Error on operation execution");
    }
}

void executeOlympAddExercise(int fd,  ClientPoll & worker){
    int idOl = receiveInt(fd);
    string title = receiveString(fd);
    string content = receiveString(fd);
    int testCount = receiveInt(fd);
    vector<string> inputs;
    vector<string> outputs;
    for(int i=0; i<testCount; i++){
        inputs.push_back(receiveString(fd));
        outputs.push_back(receiveString(fd));
    }

    auto usr = worker.getClient(fd);
    if (usr == nullptr || usr->clPriv == ClientType::user)
    {
        sendError(fd, "You don't have rights for this operation");
    }
    else
    {
        if (dbManager.olympicAddExercise(idOl, title, content)){
            saveTestCases(dbManager.getExerciseId(title), inputs, outputs);
            sendConfirm(fd, "Exercise created succesfully.");
        }
        else 
            sendError(fd, "Error on execution");
        
    }
}

void executeOlympRemExercise(int fd,  ClientPoll & worker){

    int ex_id = receiveInt(fd);

    auto usr = worker.getClient(fd);
    if (usr == nullptr || usr->clPriv == ClientType::user)
    {
        sendError(fd, "You don't have rights for this operation");
    }
    else
    {
        if(dbManager.olympicRemExercise(ex_id)){
            removeTestCases(ex_id);
            sendConfirm(fd, "Exercise removed successfully.");
        }
        else
            sendError(fd, "Error on execution.");
    }
}
void executeOlympAddLang(int fd,  ClientPoll & worker){
    int id_ol = receiveInt(fd);
    string langName = receiveString(fd);
    auto usr = worker.getClient(fd);

    if (usr == nullptr || usr->clPriv == ClientType::user)
    {
        sendError(fd, "You don't have rights for this operation");
    }
    else
    {
        if(dbManager.olympicAddLang(id_ol, langName))
            sendConfirm(fd, "Language "+langName + " was added to olympic" );
        else
            sendError(fd, "Error on execution");
    }
}

void executeOlympRemLang(int fd,  ClientPoll & worker){
    int id_ol = receiveInt(fd);
    string langName = receiveString(fd);
    auto usr = worker.getClient(fd);

    if (usr == nullptr || usr->clPriv == ClientType::user)
    {
        sendError(fd, "You don't have rights for this operation");
    }
    else
    {
        if (dbManager.olympicRemLang(id_ol, langName))
            sendConfirm(fd, "Language " + langName + " was removed from olympic");
        else
            sendError(fd, "Error on execution");
    }
}

void executeOlympList(int fd,  ClientPoll & worker){
    cout << "EXECUTE OLYMP" << endl;
    sendString(fd, dbManager.getOlympicList());
}

void executeOlympCreate(int fd,  ClientPoll & worker){
    string title = receiveString(fd);
    string description = receiveString(fd);
    string start_time = receiveString(fd);
    string duration = receiveString(fd);
    auto usr = worker.getClient(fd);
    if(usr == nullptr || usr->clPriv == ClientType::user){
        sendError(fd, "You don't have rights for this operation");
    }else{
        if(dbManager.createOlympic(title, description, start_time, duration))
            sendConfirm(fd, "Olympic created succesfully.");
        else
            sendError(fd, "Error on execution");
    }
}

void executeOlympRemove(int fd,  ClientPoll & worker){
    int id = receiveInt(fd);
    auto usr = worker.getClient(fd);
    if(usr == nullptr || usr->clPriv == ClientType::user){
        sendError(fd, "You don't have rights for this operation");
    }else{
        if(dbManager.removeOlympic(id))
            sendConfirm(fd, "Olympic removed succesfully.");
        else
            sendError(fd, "Error on execution");
    }
}

void executeLogin(int fd,  ClientPoll & worker){
    cout << "Execute Login" << endl;
    string username = receiveString(fd);
    string password = receiveString(fd);
    cout << "USERNAME RECEIVED: " << username << endl;
    if(dbManager.isRegistered(username)){
        cout << "User is registerd" << endl;
        ClientInfo *cl = dbManager.getClient(username, password);
        if(cl != nullptr){
            cout << "Client info fetched" << endl;
            ClientState * st = new ClientState();
            st->cl_id = cl->id;
            st->cl_username = cl->id;
            st->clPriv = cl->role;
            st->_sd = fd;
            cout << "Client State initialized" << endl;
            worker.setClientState(fd, st);
            sendConfirm(fd, "Successful login");
        }
        else
            sendError(fd, "Wrong password");
    }
    else
        sendError(fd, "User doesn't exist.");
    
}

void executeLangAdd(int fd,  ClientPoll & worker){
    string name = receiveString(fd);
    string command = receiveString(fd);
    string ext = receiveString(fd);
    auto usr = worker.getClient(fd);
    if(usr == nullptr){
        sendError(fd, "You most log in first.");
        return;
    }
    if(usr->clPriv == ClientType::admin){
        if(dbManager.addLanguage(name, command, ext))
            sendConfirm(fd, "Language added successfully.");
        else
            sendError(fd, "Error on inserting in database.");
    }else
        sendError(fd, "You have no rights for this operation.");
    
}

void executeLangRemove(int fd,  ClientPoll & worker){
    string name = receiveString(fd);
    auto usr = worker.getClient(fd);
    if(usr == nullptr){
        sendError(fd, "You most log in first.");
        return;
    }
    if(usr->clPriv == ClientType::admin){
        if(dbManager.removeLanguage(name))
            sendConfirm(fd, "Language removed successfully.");
        else
            sendError(fd, "Error on database query.");
    }else
        sendError(fd, "You have no rights for this operation.");
    
}

void executeRegister(int fd,  ClientPoll & worker){
    ClientInfo clData;

    clData.username = receiveString(fd);
    clData.password = receiveString(fd);
    clData.email = receiveString(fd);
    clData.realName = receiveString(fd);
    clData.birthDate = receiveString(fd);
    clData.university = receiveString(fd);

    if(dbManager.isRegistered(clData.username)){
        sendError(fd, "User with such username already exists.");
    }
    else if(!dbManager.registerUser(clData)){
        sendError(fd, "Error on registration.");
    }
    else
        sendConfirm(fd, "User succesfully registered.");

}

void executeStatus(int fd,  ClientPoll & worker){
    auto ptr = worker.getClient(fd);

    if (ptr == nullptr) {
        sendString(fd, "guest");
    }else{
        string roleStr;
        ptr->UpdateState();
        switch(ptr->clPriv){
            case ClientType::user: roleStr = "[user]"; break;
            case ClientType::moderator: roleStr = "[moderator]"; break;
            case ClientType::admin: roleStr = "[admin]"; break;
        }
        sendString(fd, ptr->cl_username + roleStr);
    }
}


void executeLogout(int fd,  ClientPoll & worker){
    auto ptr = worker.getClient(fd);
    if(ptr == nullptr) 
        sendError(fd, "You are not logged in.");
    else{
        delete worker.getClient(fd);
        worker.setClientState(fd, nullptr);
        sendConfirm(fd, "You have successfully logged out.");
    }
}

#endif