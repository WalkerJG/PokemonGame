#include "myServer.h"
#include "dispatcher.h"
//#include "include/ORMLie.h"
#include "clientStruct.h"
using namespace MyServer;
using namespace BOT_ORM;

Server::Server() :sockInit(), _onlineNum(0)
{
    //Judge whethre exists a table,if not create one.
    sqlite3 *db = NULL;
    int result = sqlite3_open("data.db", &db);
    if (result != SQLITE_OK)
        throw std::runtime_error("Fail at open the data base!");
    sqlite3_close(db);
    ORMapper mapper("data.db");
    try {
        mapper.CreateTbl(clientInfo{});
        mapper.CreateTbl(pokemonInfo{});
    }
    catch (std::exception &e) {
        std::cout << "[Info] Successfully load the data base!" << std::endl;
        e.what();
        //std::cout << "(" << e.what() << ")" << std::endl;
    }
    threadPool.setMaxQueueSize(QUEUE_LENGTH);//set max length of queue
    threadPool.start(NUM_THREAD);//Create NUM_THREAD threads in the thread pool
    addrinfo hints, *res;

    sockaddr_storage other_addr;
    char BUF[BUF_LEN];

    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;//using the address of the server
    hints.ai_socktype = SOCK_STREAM;

    //socket and bind
    if (0 != getaddrinfo(NULL, SERVER_PORT, &hints, &res)) {
        throw std::runtime_error("Fail at get address information!");
    }

    for (addrinfo *p = res;p != NULL;p = p->ai_next) {
        serverSock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (-1 == serverSock) {
            serverSock = SOCKET_ERROR;
            continue;
        }
        if (-1 == bind(serverSock, p->ai_addr, p->ai_addrlen)) {
            closesocket(serverSock);
            serverSock = SOCKET_ERROR;
            continue;
        }
        break;
    }
    if (serverSock == SOCKET_ERROR) {
        throw std::runtime_error("Fail at socket!");
    }
    freeaddrinfo(res);

    //listen
    if (-1 == listen(serverSock, 5)) {
        closesocket(serverSock);
        throw std::runtime_error("Fail at listen!");
    }

    std::cout << "[Info] Waiting for connection!" << std::endl;

    std::mutex mtx;
    while (true) {
        int addrSize = sizeof(other_addr);
        SOCKET newSock = accept(serverSock, (sockaddr*)&other_addr, &addrSize);
        if (-1 == newSock) {
            continue;
        }
        inet_ntop(other_addr.ss_family, &(((sockaddr_in*)((addrinfo*)&other_addr))->sin_addr), BUF, BUF_LEN);
        std::cout << "[Info] Get connection from " << BUF << std::endl;
        auto task = std::bind(connectHandler, newSock, this, std::ref(mtx));
        threadPool.run(task);
    }

    threadPool.stop();

}

Server::~Server()
{

    shutdown(serverSock, SD_BOTH);
    closesocket(serverSock);
}

std::string MyServer::Server::getSockClient(SOCKET clientSock)
{
    for (auto i : _onlineClient) {
        if (i.second == clientSock)
            return i.first;
    }
    return std::string();
}


void MyServer::connectHandler(SOCKET  connection, Server * server, std::mutex & mtx)
{
    if (connection < 0)
    {
        std::cout << "[ERROR] Failed at accept" << std::endl;
        return;
    }
    else
    {
        char recvBuf[BUF_LEN];
        int recvBufLen = BUF_LEN;
        dispatcher _dispatcher(server, connection);
        std::cout << "[Info] New come, now " << server->incClientNum() << " connections in total" << std::endl;

        while (true)
        {
            int recvLength = recv(connection, recvBuf, recvBufLen, 0);
            if (recvLength <= 0)
            {
                //lock for mutual exclusion
                (mtx).lock();
                std::cout << "[Info] Someone offline, now " << server->decClientNum() << " connections in total" << std::endl;
                std::string name  = server->getSockClient(connection);
                if (name != std::string()) {
                    json request;
                    request["type"] = LOG_OUT;
                    request["connection"] = connection;
                    request["account"] = name;
                    _dispatcher.dispatch(request);
                }
                (mtx).unlock();
                shutdown(connection, SD_BOTH);
                closesocket(connection);
                break;
            }
            recvBuf[recvLength] = 0;
            std::cout << "[Info] message: " << recvBuf << std::endl;

            std::string responseStr;
            try
            {
                responseStr = _dispatcher.dispatch(std::move(json::parse(recvBuf)));
            }
            catch (std::exception e)
            {
                shutdown(connection, SD_BOTH);
                closesocket(connection);
                std::cout << "[ERROR] " << e.what() << std::endl;
            }
            send(connection, responseStr.c_str(), responseStr.length(), 0);
        }
    }
}

