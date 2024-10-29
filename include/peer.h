#pragma once
#ifndef PEER_H
#define PEER_H

#include "connectionManager.h"

class Peer
{
private:
    ConnectionManager connMngr;

public:
    Peer();
    void runServer();
    void runClient(const char *serverAddr = "127.0.0.1");
    static void stop();
};

#endif /* PEER_H */