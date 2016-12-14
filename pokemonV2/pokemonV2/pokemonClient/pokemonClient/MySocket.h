#ifndef _MYSOCKET_H
#define _MYSOCKET_H
#include<winsock2.h>
#include<WS2tcpip.h>
#include<iostream>
#include<exception>
#pragma comment(lib, "ws2_32.lib")
namespace mySocket {
    class sockInit {
        //static size_t refCount;
    protected:
        WSADATA wsaData;
    public:
        sockInit() {
            //  if (!refCount) {
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
                throw std::runtime_error("WSAStartup failed");
            //refCount++;
        //}
        }
        ~sockInit() {
            //refCount--;
            //if (!refCount) {
            WSACleanup();
            //}
        }
    };
    //size_t sockInit::refCount = 0;
}
#endif