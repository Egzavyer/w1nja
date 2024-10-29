#include "../../include/connectionManager.h"

ConnectionManager::ConnectionManager()
{
    ConnectionManager::initWinsock();
    serverSock = ConnectionManager::createServerSocket();
}

SocketInfo ConnectionManager::getServerSock()
{
    return serverSock;
}

SocketInfo ConnectionManager::getClientSock()
{
    return clientSock;
}

void ConnectionManager::initWinsock()
{
    WSADATA wsaData;
    int iResult;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        errHandler("WSAStartup failed: ");
    }
}

SocketInfo ConnectionManager::createServerSocket()
{
    SOCKET listenSocket = INVALID_SOCKET;
    SocketInfo sock;
    int iResult;
    struct addrinfo *result = nullptr, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server

    if ((iResult = getaddrinfo(NULL, PORT, &hints, &result)) != 0)
    {
        WSACleanup();
        errHandler("getaddrinfo failed: ");
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

    sock.sockFD = listenSocket;
    freeaddrinfo(result);
    return sock;
}

SocketInfo ConnectionManager::createClientSocket(const char *serverAddr)
{
    struct addrinfo *result = nullptr, *ptr = nullptr, hints;
    SocketInfo sock;
    int iResult;
    char ipBuf[INET_ADDRSTRLEN];
    SOCKET connectSocket = INVALID_SOCKET;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if ((iResult = getaddrinfo(serverAddr, PORT, &hints, &result)) != 0)
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

    struct sockaddr_in sockaddr;
    int namelen = sizeof(sockaddr);
    if (!getpeername(connectSocket, (struct sockaddr *)&sockaddr, &namelen))
    {
        printf("Connected to: %s\n", inet_ntop(AF_INET, &sockaddr.sin_addr, ipBuf, INET_ADDRSTRLEN));
    }

    // should try to connect to the next address from getaddrinfo if fails
    freeaddrinfo(result);

    if (connectSocket == INVALID_SOCKET)
    {
        WSACleanup();
        throw std::runtime_error("Unable to connect to server");
    }
    sock.sockFD = connectSocket;
    return sock;
}

void ConnectionManager::errHandler(const std::string &errLocation)
{
    const size_t errmsglen = 256;
    char errmsg[errmsglen];
    strerror_s(errmsg, errmsglen, errno);
    throw std::runtime_error(errLocation + std::string(errmsg));
}