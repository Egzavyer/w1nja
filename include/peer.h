#ifndef PEER_H
#define PEER_H
#pragma once

#define NOMINMAX
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdexcept>
#include <errno.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <atomic>
#include <thread>
#include <unordered_map>
#include <iostream>
#include <MSWSock.h>
#include <algorithm>

class Peer
{
private:
    static constexpr int BUFFER_SIZE = 1024;
    std::atomic<bool> running{true};
    inline static std::unordered_map<SOCKET, std::thread> connections;

    static void initWinsock();
    static SOCKET createSocket();
    static void handleConnection(SOCKET client);
    static void errHandler(const std::string &errLocation);

public:
    void runServer();
    void stop();
    void runClient(const char *serverIP);
};

#endif /* PEER_H */