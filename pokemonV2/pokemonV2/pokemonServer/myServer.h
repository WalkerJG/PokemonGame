#ifndef _MYSERVER_H
#define _MYSERVER_H

#include "mySocket.h"
#include "ThreadPool.h"

#include<mutex>
#include<unordered_map>

using namespace mySocket;
#define SERVER_PORT "5902"
#define BACKLOG 5
#define BUF_LEN 4096
#define QUEUE_LENGTH 10
#define NUM_THREAD 6
namespace MyServer {
    class Server :sockInit {
    public:
        Server();
        ~Server();

        int incClientNum()
        {
            return ++_onlineNum;
        }
        int decClientNum()
        {
            return --_onlineNum;
        }
        int getClientNum()
        {
            return _onlineNum;
        }

        std::unordered_map<std::string,SOCKET>& getOnlineClient() {
            return _onlineClient;
        }
    private:
        SOCKET serverSock;
        fivestar::ThreadPool threadPool;//Using thread pool to provide better response
        int _onlineNum;
        std::unordered_map<std::string,SOCKET> _onlineClient;

    };

    //Handle the connect of the SOCKET connection
    void connectHandler(SOCKET  connection, Server * server, std::mutex & mtx);
}
#endif