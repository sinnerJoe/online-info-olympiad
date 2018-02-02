#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <exception>
using namespace std;

#ifndef PROTOCOL_MESSAGE_HPP
#define PROTOCOL_MESSAGE_HPP

enum MessageType
{
    //ERRORS
    ERROR, //this flag is followed by a string describing the error
    OK,
    //API for all users

    LOGIN,
    REGISTER,
    USR_GETSTATUS,       //get user participating in smth status
    LOGOUT,
    OL_APPLY,            //utilizatorul se inscrie la olimpiada aleasa
    OL_UNDO_APPLY,       //utilizatorul anuleaza inscrierea la olimpiada
    OL_GET_GENERAL_INFO, //utilizatorul solicita descrierea olimpiezii
    EX_COMPILE,          //clientul transmite fisierul
                         //serverul il compileaza si transmite inapoi rezultatul copararii
                         //output-ului dorit cu acel obtinut
    EX_GIVE_UP,          //renunti la rezolvarea exercitiului curent
    EX_SUBMIT,           //utilizatorul transmite rezolvarea finala a exercitiului
    EX_GETINFO,          //solicitarea informatiei despre exercitiul curent
    OL_GET_LANG_LIST,

    //API for moderators
    OL_DENY = 100,     //deny a user from participation
    OL_CONFIRM,        //confirmarea inscrierii la olimpiada
    OL_GET_APPLIES_LIST,      //solicita lista de utilizatori care au aplicat pentru participare
    OL_GET_PARTICIPANTS_LIST, //solicita lista participantilor acceptati
    OL_CREATE,                //creeaza olimpiada cu denumirea si descrierea specificata
    OL_ADD_EX,                //creeaza exercitiul si-l adauga la olimpiada
    OL_REMOVE_EX,             //sterge exercitiul
    OL_REMOVE,                //sterge olimpiada
    OL_RENAME,                //redenumeste olimpiada
    OL_ADD_LANG,              //adauga limbajul de programare la olimpiada
    OL_REMOVE_LANG,           //sterge limbajul de programare de la olimpiada
    OL_LIST,                   //afiseaza lista olimpiadelor
    OL_LIST_EXERCISE,
    OL_LIST_LANG,
    MY_EX_LIST,
    OL_LEADERBOARD,

    //API for admin
    DELETE_USER=200,              //sterge utilizatorul specificat
    PROMOTE_USER,             // acorda drepturile de moderator utilizatorului
    DEMOTE_USER,              // retrage drepturile de moderator de la utilizator
    LANG_ADD,                 // adauga limbajul de programare 
    LANG_REMOVE               // sterge limbajul de programare
};

class ProtocolException : exception{
    const string error_message;
    public:
        ProtocolException(string &str);
        ProtocolException(const char *str);
        virtual char* what();
};

void sendString(int fd, string str);

void sendInt(int fd, int data);

void sendFlag(int fd, MessageType flag);

string receiveString(int fd);

int receiveInt(int fd);
MessageType receiveFlag(int fd);

#endif