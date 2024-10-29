#pragma once
#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include <atomic>
#include <errno.h>
#include <unordered_map>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdexcept>
#include <stdio.h>
#include <MSWSock.h>
#include <algorithm>

struct SocketInfo
{
    SOCKET sockFD;
    struct sockaddr_in address;
    socklen_t addrlen;
};

class ConnectionManager
{
private:
    static constexpr char *PORT = "8080";
    SocketInfo serverSock, clientSock;

    static SocketInfo createServerSocket();

public:
    inline static std::unordered_map<SOCKET, std::thread> connections;

    ConnectionManager();
    SocketInfo getServerSock();
    SocketInfo getClientSock();

    static void initWinsock();
    static SocketInfo createClientSocket(const char *serverAddr);
    static void errHandler(const std::string &errLocation);
};
#endif

#ifdef __unix__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#endif

#endif /* CONNECTION_MANAGER_H */