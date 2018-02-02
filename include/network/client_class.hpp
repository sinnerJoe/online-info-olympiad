
#ifndef CLIENT_CLASS_HPP
#define CLIENT_CLASS_HPP

#include <map>
#include <mutex>
#include <vector>
#include <poll.h>
#include <utility>
#include <set>
#include <thread>
#include <atomic>
using namespace std;

class ClientPoll;
constexpr short thNr = 4;



enum ClientType {user, moderator, admin};

struct ClientInfo{
    unsigned long id;
    string username;
    string password;
    string realName;
    string email;
    string birthDate;
    string university;
    ClientType role;

};

class ClientState
{
  public:
    int _sd;
    int cl_id;
    int vectorPos;
    ClientType clPriv;
    string cl_username;
    ClientState(int sd, int vPos);
    ClientState(ClientState& clState);
    ClientState();
    ClientState(ClientType clPrin);
    void UpdateState();
};

class ClientPoll{
    private:
        int workerNr;
        array<struct pollfd, 1000> pollFds;
        map<int, ClientState*> clients;
        mutex mtx;
        void treat();
        void checkAndExecute(struct pollfd& pfd);
    public:
        atomic<unsigned> size;
        ClientPoll();
        void deleteClient(int fd);
        void deleteClient(struct pollfd &pfd);
        void addClient(int fd);
        ClientState* getClient(int fd);
        array<struct pollfd, 1000> getPollFds();
        void lock();
        void unlock();
        void setClientState(int fd, ClientState *clState);

};


#endif