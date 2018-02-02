#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <exception>
#include <network/protocol_message.hpp>
using namespace std;

ProtocolException::ProtocolException(string &str) : error_message{str} {}
ProtocolException::ProtocolException(const char *str) : error_message{str} {}
char *ProtocolException::what()
{
    return (char *)error_message.c_str();
}

void sendString(int fd, string str)
{
    unsigned long sz = str.size();
    // cout << "Write(long) " << sizeof(sz) << endl;
    int res =0;
    while((res = send(fd, &sz, sizeof(sz), 0))==-1);
    if (res == -1)
        throw ProtocolException("SENDSTRING SEND SIZE TROUBLE");

    // cout << "Write(str) " << sz << endl;
    res = write(fd, (void *)str.c_str(), sz);
    if (res == -1)
        throw ProtocolException("SENDSTRING SEND STRING TROUBLE");
}

void sendInt(int fd, int data)
{
    // cout << "Write(int) " << sizeof(int) << endl;
    while(write(fd, (void *)&data, sizeof(int))==-1);
}

void sendFlag(int fd, MessageType flag)
{
    // cout << "Write(flag) " << sizeof(MessageType) << endl;
    while(write(fd, (void *)&flag, sizeof(MessageType))==-1);
}

string receiveString(int fd)
{
    unsigned long sz = 0;
    int readVal;
    // cout << "read(long) " << sizeof(sz) << endl;
    // if((readVal = read(fd, &sz, sizeof(sz))) <= 0){
    //     cout << "Read string size trouble " << readVal << endl;
    //     throw ProtocolException("Read string size trouble");
    // }
    while((readVal = read(fd, &sz, sizeof(sz)))==-1);
    char *buf = new char[sz+1];
    buf[sz] = '\0';
    // cout << "read(str) " << sz << endl;
    if((readVal = read(fd, buf, sz)) <= 0){
        cout << "Read chars trouble "  << readVal << endl;
        // throw ProtocolException("Read chars trouble");
    }
    string res(buf);
    delete[] buf;
    return res;
}

int receiveInt(int fd)
{
    int nr;
    // cout << "read(int) " << sizeof(nr) << endl;
    while(read(fd, &nr, sizeof(nr))==-1);
    return nr;
}

MessageType receiveFlag(int fd)
{
    // cout << "read " << sizeof(MessageType) << endl;
    MessageType flag;
    while(read(fd, &flag, sizeof(flag))==-1);
    return flag;
}
