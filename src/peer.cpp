#include "../include/peer.h"
#define PORT "8080"

void Peer::initWinsock()
{
    WSADATA wsaData;
    int iResult;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        throw std::runtime_error("WSAStartup failed: " + std::string(strerror(errno)));
    }
}

SOCKET Peer::createSocket()
{
    initWinsock();
    SOCKET listenSocket = INVALID_SOCKET;
    int iResult;
    struct addrinfo *result = nullptr, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo(NULL, PORT, &hints, &result);
    if (iResult != 0)
    {
        WSACleanup();
        throw std::runtime_error("getaddrinfo failed: " + std::string(strerror(errno)));
    }

    // Create a SOCKET for the server to listen for client connections
    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (listenSocket == INVALID_SOCKET)
    {
        freeaddrinfo(result);
        WSACleanup();
        throw std::runtime_error("socket failed: " + WSAGetLastError());
    }

    // Setup the TCP listening socket
    iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        throw std::runtime_error("bind failed: " + WSAGetLastError());
    }

    freeaddrinfo(result);
    return listenSocket;
}

void Peer::runServer()
{
    SOCKET sock = createSocket();
    SOCKET clientSock = INVALID_SOCKET;

    if (listen(sock, SOMAXCONN) == SOCKET_ERROR)
    {
        closesocket(sock);
        WSACleanup();
        throw std::runtime_error("listen failed: " + WSAGetLastError());
    }

    struct sockaddr_in sin;
    int addrlen = sizeof(sin);
    if (getsockname(sock, (struct sockaddr *)&sin, &addrlen) == 0 &&
        sin.sin_family == AF_INET &&
        addrlen == sizeof(sin))
    {
        int local_port = ntohs(sin.sin_port);

        std::cout << "Server listening on " << inet_ntoa(sin.sin_addr) << ":" << local_port << " on socket: " << sock << std::endl;
    }

    while (running)
    {
        if ((clientSock = accept(sock, nullptr, nullptr)) == INVALID_SOCKET)
        {
            closesocket(sock);
            WSACleanup();
            throw std::runtime_error("accept failed: " + WSAGetLastError());
        }

        struct sockaddr_in sockaddr;
        int namelen = sizeof(sockaddr);
        if (!getpeername(clientSock, (struct sockaddr *)&sockaddr, &namelen))
        {
            printf("Accepted connection from: %s\n", inet_ntoa((in_addr)(*(in_addr *)&sockaddr.sin_addr.S_un.S_addr)));
        }

        connections[clientSock] = std::thread(&Peer::handleConnection, clientSock);
    }

    for (auto &conn : connections)
    {
        if (conn.second.joinable())
        {
            std::cout << conn.first << " : " << conn.second.get_id() << std::endl;
            conn.second.join();
            closesocket(conn.first);
        }
    }
    WSACleanup();
}

void Peer::runClient(const char *serverIP)
{
    initWinsock();
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    int iResult;
    SOCKET connectSocket = INVALID_SOCKET;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if ((iResult = getaddrinfo(serverIP, PORT, &hints, &result)) != 0)
    {
        WSACleanup();
        throw std::runtime_error("getaddrinfo failed: " + std::to_string(iResult));
    }

    ptr = result;
    connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (connectSocket == INVALID_SOCKET)
    {
        freeaddrinfo(result);
        WSACleanup();
        throw std::runtime_error("socket failed: " + WSAGetLastError());
    }

    if ((iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen)) == SOCKET_ERROR)
    {
        closesocket(connectSocket);
        connectSocket = INVALID_SOCKET;
    }

    struct sockaddr_in sockaddr;
    int namelen = sizeof(sockaddr);
    if (!getpeername(connectSocket, (struct sockaddr *)&sockaddr, &namelen))
    {
        printf("Connected to: %s\n", inet_ntoa((in_addr)(*(in_addr *)&sockaddr.sin_addr.S_un.S_addr)));
    }

    // should try to connect to the next address from getaddrinfo if fails
    freeaddrinfo(result);

    if (connectSocket == INVALID_SOCKET)
    {
        WSACleanup();
        throw std::runtime_error("Unable to connect to server");
    }

    int recvbuflen = BUFFER_SIZE;

    const char *sendbuf = "Hello from Client!\0";
    char recvbuf[BUFFER_SIZE];

    if ((iResult = send(connectSocket, sendbuf, (int)strlen(sendbuf), 0)) == SOCKET_ERROR)
    {
        closesocket(connectSocket);
        WSACleanup();
        throw std::runtime_error("send failed:" + WSAGetLastError());
    }

    std::cout << "Bytes sent: " << std::to_string(iResult) << std::endl;

    if ((iResult = shutdown(connectSocket, SD_SEND)) == SOCKET_ERROR)
    {
        closesocket(connectSocket);
        WSACleanup();
        throw std::runtime_error("shutdown failed:" + WSAGetLastError());
    }

    while ((iResult = recv(connectSocket, recvbuf, recvbuflen, 0)) > 0)
    {
        std::cout << "Bytes received: " << std::to_string(iResult) << std::endl;
        std::cout << recvbuf << std::endl;
    }

    if (iResult == 0)
    {
        std::cout << "Connection closed" << std::endl;
    }
    else
    {
        throw std::runtime_error("recv failed:" + WSAGetLastError());
    }

    if ((iResult = shutdown(connectSocket, SD_SEND)) == SOCKET_ERROR)
    {
        closesocket(connectSocket);
        WSACleanup();
        throw std::runtime_error("shutdown failed:" + WSAGetLastError());
    }

    closesocket(connectSocket);
    WSACleanup();
}

void Peer::stop()
{
    running = false;
}

void Peer::handleConnection(SOCKET client)
{
    char recvbuf[BUFFER_SIZE];
    int iResult, iSendResult;
    int recvbuflen = BUFFER_SIZE;

    while ((iResult = recv(client, recvbuf, recvbuflen, 0)) > 0)
    {
        std::cout << "Bytes received: " << iResult << std::endl;
        if ((iSendResult = send(client, recvbuf, iResult, 0)) == SOCKET_ERROR)
        {
            closesocket(client);
            WSACleanup();
            throw std::runtime_error("send failed: " + WSAGetLastError());
        }

        std::cout << "Bytes sent: " << iSendResult << std::endl;
    }

    if (iResult == 0)
    {
        std::cout << "Connection closing..." << std::endl;
    }
    else
    {
        closesocket(client);
        WSACleanup();
        throw std::runtime_error("recv failed: " + WSAGetLastError());
    }

    if ((iResult = shutdown(client, SD_SEND)) == SOCKET_ERROR)
    {
        closesocket(client);
        WSACleanup();
        throw std::runtime_error("shutdown failed: " + WSAGetLastError());
    }
}