#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <thread>
#include <poll.h>   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <network/client_class.hpp>
#include <network/protocol_message.hpp>
#include <errno.h>
using std::vector;


#ifndef NETWORK_HPP
#define NETWORK_HPP

#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT 10069

void acceptNewClient();

int sd;

array<ClientPoll, thNr> workers;
void mainLoop(){

    struct sockaddr_in server;
    // fd_set read_fds, write_fds, except_fds;

    
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) //crearea socketului
    {
        perror("[server]Eroare la socket().\n");
        return;
    }
    int on = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,(char *)&on, sizeof(on)) < 0){
        close(sd);
        perror("[server]Eroare la setarea optiunilor.\n");
        exit(1);
    }
    // fcntl(client, F_SETFL, O_NONBLOCK);
    
    memset((void *)&server, 0, sizeof(server));
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (DEFAULT_PORT);

    if(bind(sd, (struct sockaddr*)&server, sizeof(struct sockaddr))==-1){
        cout << "ERROR ON BIND\n";
        exit(1);
    }

    if(listen(sd, 2) == -1){
        cout << "ERROR ON LISTEN\n";
        exit(1);
    }
    cout << "start of accepting loop\n";
    while(true){
        acceptNewClient();
    }


}

void acceptNewClient()
{
    struct sockaddr_in csd;
    int client;
    memset(&csd, 0, sizeof(csd));
    int length = sizeof(csd);
    if ((client = accept(sd, (struct sockaddr *)&csd, (socklen_t *)&length)) < 0)
    {
        cout << "Eroare la accept().\n";
    }
    else
    {
        int on=1;
        // if (ioctl(client, FIONBIO, (char *)&on) < 0)
        if (fcntl(client, F_SETFL, O_NONBLOCK) < 0)
        {
            cout << "Error on using ioctl to make nonblocking socket";
        }
        int index =0;
        int sz = workers[0].size;
        for(int i=1; i<thNr; i++)
            if (workers[i].size < sz){
                sz = workers[i].size;
                index = i;
            }
        workers[index].addClient(client);
    }
    char response[50] = "Hello from mainthread";
    write(client, response, 50);
    cout << "End of accept" << endl;
}


#endif