#include<client/network/messages.hpp>
#include<network/protocol_message.hpp>
int sd;

SimpleException :: SimpleException(string mess) : ProtocolException{mess} {}
char * SimpleException::what(){
    ProtocolException::what();
}

string receiveOKError(){
    switch(receiveFlag(sd)){
        case MessageType::OK: return receiveString(sd) + "\n";
        case MessageType::ERROR: return "ERROR: " + receiveString(sd) + "\n";
    }
}

string sendLogin(string usrname, string password)
{
    sendFlag(sd, MessageType::LOGIN);
    sendString(sd, usrname);
    sendString(sd, password);
    return receiveOKError();
}

string sendUsrForm(string usrname, string password,
                   string email, string realName,
                   string univer, string birthDate)
{
    if(usrname.size() < 3) throw SimpleException("User name is shorter than 3 characters");
    if(password.size() < 5) throw SimpleException("Password is shorter than 5 characters");
    sendFlag(sd, MessageType::REGISTER);
    sendString(sd, usrname);
    sendString(sd, password);
    sendString(sd, email);
    sendString(sd, realName);
    sendString(sd, birthDate);
    sendString(sd, univer);
    return receiveOKError();
}

string sendLogout(){
    sendFlag(sd, MessageType::LOGOUT);
    return receiveOKError();
}

string sendStatus(){
    sendFlag(sd, MessageType::USR_GETSTATUS);
    return receiveString(sd);
}

string sendLangRemove(string name){
    sendFlag(sd, MessageType::LANG_REMOVE);
    sendString(sd, name);
    return receiveOKError();
}
string sendLangAdd(string name, string command, string ext){
    sendFlag(sd, MessageType::LANG_ADD);
    sendString(sd, name);
    sendString(sd, command);
    sendString(sd, ext);
    return receiveOKError();
}

string sendOlympicCreate(string title, string desc, string start_time, string dur){
    sendFlag(sd, MessageType::OL_CREATE);
    sendString(sd, title);
    sendString(sd, desc);
    sendString(sd, start_time);
    sendString(sd, dur);
    return receiveOKError();
}

string sendOlympicList(){
    sendFlag(sd, MessageType::OL_LIST);
    return receiveString(sd);
}

string sendOlympicRemove(int id){
    sendFlag(sd, MessageType::OL_REMOVE);
    sendInt(sd, id);
    return receiveOKError();
}

string sendOlympicApply(int id){
    sendFlag(sd, MessageType::OL_APPLY);
    sendInt(sd, id);
    return receiveOKError();
}

string sendOlympicUndoApply(int id)
{
    sendFlag(sd, MessageType::OL_UNDO_APPLY);
    sendInt(sd, id);
    return receiveOKError();
}

string sendOlympicAddExercise(int id_ol, string title, string content,
                              vector<string> &inputs,
                              vector<string> &outputs)
{
    sendFlag(sd, MessageType::OL_ADD_EX);
    sendInt(sd, id_ol);
    sendString(sd, title);
    sendString(sd, content);
    sendInt(sd, inputs.size());
    for(int i=0; i<inputs.size(); i++){
        sendString(sd,inputs[i]);
        sendString(sd, outputs[i]);
    }

    return receiveOKError();
}

string sendOlympicRemExercise(int id){
    sendFlag(sd, MessageType::OL_REMOVE_EX);
    sendInt(sd, id);
    return receiveOKError();
}

string sendOlympicListExercise(int id_ol){
    sendFlag(sd, MessageType::OL_LIST_EXERCISE);
    sendInt(sd, id_ol);
    return receiveString(sd);
}

string sendOlympicAddLang(int id_ol, string langName){
    sendFlag(sd, MessageType::OL_ADD_LANG);
    sendInt(sd, id_ol);
    sendString(sd, langName);
    return receiveOKError();
}
string sendOlympicRemLang(int id_ol, string langName){
    sendFlag(sd, MessageType::OL_REMOVE_LANG);
    sendInt(sd, id_ol);
    sendString(sd, langName);
    return receiveOKError();
}

string sendOlympicListLang(int id_ol){
    sendFlag(sd, OL_LIST_LANG);
    sendInt(sd, id_ol);
    return receiveString(sd);
}

string sendExerciseCompile(int exId, string langName, string code){
    sendFlag(sd, MessageType::EX_COMPILE);
    sendInt(sd, exId);
    sendString(sd, langName);
    sendString(sd,code);
    return receiveOKError();
}

string sendExerciseSubmit(int exId, string langName, string code){
    sendFlag(sd, MessageType::EX_SUBMIT);
    sendInt(sd, exId);
    sendString(sd, langName);
    sendString(sd, code);
    return receiveOKError();
}
string sendExerciseView(int exId){
    sendFlag(sd, MessageType::EX_GETINFO);
    sendInt(sd, exId);
    return receiveOKError();
}

string sendGetMyExercises(){
    sendFlag(sd, MessageType::MY_EX_LIST);
    return receiveOKError();
}

string sendGetLeaderboard(int ol_id){
    sendFlag(sd, MessageType::OL_LEADERBOARD);
    sendInt(sd, ol_id);
    return receiveString(sd);
}