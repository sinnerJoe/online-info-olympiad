
#include <map>
#include <vector>
#include <mutex>
#include <poll.h>
#include <utility>
#include <network/client_class.hpp>
#include <network/protocol_message.hpp>
#include <database/database.hpp>
#include <functional>
#include <chrono>
#include <network/server_api.hpp>
#include <thread>
using namespace std;



ClientState::ClientState(int sd, int vPos) : _sd{sd}, vectorPos{vPos} {}
ClientState::ClientState(ClientState &clState) : _sd{clState._sd},
                                                 vectorPos{clState.vectorPos},
                                                 cl_id{clState.cl_id},
                                                 cl_username{clState.cl_username} {}
ClientState::ClientState() : _sd{-1}, cl_id{-1}, vectorPos{-1} {}

ClientState::ClientState(ClientType clPriv) : clPriv{clPriv} {} 

void ClientState::UpdateState(){
    dbManager.updateState(this);
}

int workerCurrentNumber = 0;

ClientPoll::ClientPoll() : size{0} {
    workerNr = workerCurrentNumber++; 
    for(struct pollfd& pfd : pollFds){
        pfd.events = POLLIN;
        pfd.fd=-1;
    }
    thread {&ClientPoll::treat, this}.detach();
}
void ClientPoll::deleteClient(int fd){
    lock();
    for(int i=0; i<size; i++)
        if(fd == pollFds[i].fd){
            size--;
            pollFds[i].fd = pollFds[size].fd;
            pollFds[size-1].fd = -1;
            delete clients[fd];
            clients[fd] = nullptr;
            break;
        }
    unlock();
}

void ClientPoll::deleteClient(struct pollfd &pfd){
    lock();
    size--;
    pfd.fd = pollFds[size].fd;
    pollFds[size].fd = -1;
}



void ClientPoll::addClient(int fd){
    lock();
    pollFds[size].fd = fd;
    size++;
    unlock();
}
ClientState * ClientPoll::getClient(int fd){
    lock();
    ClientState * cl = this->clients[fd];
    unlock();
    return cl;
}
array<struct pollfd, 1000> ClientPoll::getPollFds(){
    return this->pollFds;
}
void ClientPoll::lock(){
    mtx.lock();
}
void ClientPoll::unlock(){
    mtx.unlock();
}

void ClientPoll::treat()
{
    while (true)
    {
        if(size == 0){
            this_thread::sleep_for(0.1s);
            continue;
        }
        poll(pollFds.begin(), size, 500);
        for(int i=0; i<size; i++){
            checkAndExecute(pollFds[i]);
        }
    }
}

void ClientPoll::checkAndExecute(pollfd &pfd)
{
    char check; //one byte
    if (recv(pfd.fd, (void *)&check, 1, MSG_PEEK) == 0)
    {
        cout << "Removing disconected client by thr." << this->workerNr << "\n";
        this->deleteClient(pfd);
    }
    else if ((pfd.revents & POLLIN) > 0)
    {
        pfd.revents = 0;
        cout << "POLLIN EXECUTING! " << " THR." << this->workerNr  << endl;
        try
        {
            executeCommand(pfd.fd, *this);
        }
        catch (ProtocolException &e)
        {
            cout << "VIEWING ProtocolException: " " THR." << this->workerNr;
            cout << e.what() << endl;
            char c;
            while(read(pfd.fd, &c, 1)!=-1);
        }
        // catch(std::bad_alloc & ba)
        // {
        //     std::cout << "bad_alloc caught: " << ba.what() << '\n';
        // }
    }
    else if (pfd.revents != 0)
    {
        cout << "Some error in revents.\n" " THR." << this->workerNr;
        
    }
    // cout << "end of checkAndExecute()" " THR." << this->workerNr << endl;
}

void ClientPoll::setClientState(int fd, ClientState *clState){
    lock();
    this->clients[fd] = clState;
    unlock();
}
