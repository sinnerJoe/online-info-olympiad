#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <network/protocol_message.hpp>
#include <client/network/messages.hpp>
#include <client/shell.hpp>
using namespace std;

#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT 10069

int main(){

    struct sockaddr_in server;
    char buf[50];
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Eroare la socket().\n");
        return errno;
    }

    server.sin_family = AF_INET;
    /* adresa IP a serverului */
    server.sin_addr.s_addr = inet_addr(DEFAULT_IP);
    /* portul de conectare */
    server.sin_port = htons(DEFAULT_PORT);

    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[client]Eroare la connect().\n");
        return errno;
    }

    read(sd, buf, 50);
    std::cout << buf << std::endl;

    while (menuSwitch());
}

