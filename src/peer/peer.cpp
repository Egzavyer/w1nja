#include "../../include/peer.h"

Peer::Peer()
{
}

void Peer::runServer()
{
    SOCKET clientSock = INVALID_SOCKET;
    char ipBuf[INET_ADDRSTRLEN];

    if (listen(connMngr.getServerSock().sockFD, SOMAXCONN) == SOCKET_ERROR)
    {
        closesocket(connMngr.getServerSock().sockFD);
        WSACleanup();
        throw std::runtime_error("listen failed: " + WSAGetLastError());
    }

    struct sockaddr_in sin;
    int addrlen = sizeof(sin);
    if (getsockname(connMngr.getServerSock().sockFD, (struct sockaddr *)&sin, &addrlen) == 0 &&
        sin.sin_family == AF_INET &&
        addrlen == sizeof(sin))
    {
        int local_port = ntohs(sin.sin_port);

        std::cout << "Server listening on " << inet_ntop(AF_INET, &sin.sin_addr, ipBuf, INET_ADDRSTRLEN) << ":" << local_port << " on socket: " << connMngr.getServerSock().sockFD << std::endl;
    }

    while (true) // TODO:fix
    {
        if ((clientSock = accept(connMngr.getServerSock().sockFD, nullptr, nullptr)) == INVALID_SOCKET)
        {
            closesocket(connMngr.getServerSock().sockFD);
            WSACleanup();
            throw std::runtime_error("accept failed: " + WSAGetLastError());
        }

        struct sockaddr_in sockaddr;
        int namelen = sizeof(sockaddr);
        if (!getpeername(clientSock, (struct sockaddr *)&sockaddr, &namelen))
        {
            printf("Accepted connection from: %s\n", inet_ntop(AF_INET, &sockaddr.sin_addr, ipBuf, INET_ADDRSTRLEN));
        }

        // ConnectionManager::connections[clientSock] = std::thread(&Peer::handleConnection, clientSock);
        ConnectionManager::connections[clientSock] = std::thread(&Peer::stop);
    }
}

void Peer::runClient(const char *serverAddr)
{
    connMngr.createClientSocket(serverAddr);
}

void Peer::stop()
{
}