#include "../include/peer.h"
#include "../include/fileHandler.h"
#define PORT "8080"

void Peer::errHandler(const std::string &errLocation)
{
    const size_t errmsglen = 256;
    char errmsg[errmsglen];
    strerror_s(errmsg, errmsglen, errno);
    throw std::runtime_error(errLocation + std::string(errmsg));
}

void Peer::initWinsock()
{
    WSADATA wsaData;
    int iResult;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        errHandler("WSAStartup failed: ");
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

    freeaddrinfo(result);
    return listenSocket;
}

void Peer::runServer()
{
    SOCKET sock = createSocket();
    SOCKET clientSock = INVALID_SOCKET;
    char ipBuf[INET_ADDRSTRLEN];

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

        std::cout << "Server listening on " << inet_ntop(AF_INET, &sin.sin_addr, ipBuf, INET_ADDRSTRLEN) << ":" << local_port << " on socket: " << sock << std::endl;
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
            printf("Accepted connection from: %s\n", inet_ntop(AF_INET, &sockaddr.sin_addr, ipBuf, INET_ADDRSTRLEN));
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
    char ipBuf[INET_ADDRSTRLEN];
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
        printf("Connected to: %s\n", inet_ntop(AF_INET, &sockaddr.sin_addr, ipBuf, INET_ADDRSTRLEN));
    }

    // should try to connect to the next address from getaddrinfo if fails
    freeaddrinfo(result);

    if (connectSocket == INVALID_SOCKET)
    {
        WSACleanup();
        throw std::runtime_error("Unable to connect to server");
    }

    int recvbuflen = BUFFER_SIZE;
    char recvbuf[BUFFER_SIZE];

    try
    {
        std::ofstream outf{"output.exe", std::ios::binary | std::ios::out};
        if (!outf)
        {
            throw std::runtime_error("Failed to open output file");
        }

        std::string sendbuf;
        std::cout << "Enter message: " << std::endl;
        std::getline(std::cin, sendbuf);

        if ((iResult = send(connectSocket, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0)) == SOCKET_ERROR)
        {
            throw std::runtime_error("send failed: " + std::to_string(WSAGetLastError()));
        }

        std::cout << "Bytes sent: " << iResult << std::endl;

        while ((iResult = recv(connectSocket, recvbuf, recvbuflen, 0)) > 0)
        {
            std::cout << "Bytes received: " << iResult << std::endl;

            if (!outf.write(recvbuf, iResult))
            {
                throw std::runtime_error("Failed to write to file");
            }
        }
        outf.close();

        if (iResult == 0)
        {
            std::cout << "Connection closed normally" << std::endl;
        }
        else if (iResult == SOCKET_ERROR)
        {
            throw std::runtime_error("recv failed: " + std::to_string(WSAGetLastError()));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        closesocket(connectSocket);
        WSACleanup();
        return;
    }

    if (shutdown(connectSocket, SD_SEND) == SOCKET_ERROR)
    {
        std::cerr << "shutdown failed: " + std::to_string(WSAGetLastError()) << std::endl;
    }

    closesocket(connectSocket);
    WSAGetLastError();
}

void Peer::stop()
{
    running = false;
}

void Peer::handleConnection(SOCKET client)
{
    char recvbuf[BUFFER_SIZE];
    char sendbuf[BUFFER_SIZE];
    int iResult, iSendResult;
    int recvbuflen = BUFFER_SIZE;
    std::vector<std::string> files;

    try
    {
        while ((iResult = recv(client, recvbuf, recvbuflen - 1, 0)) > 0)
        {
            // client sends initial request to server (see avail files)
            // server parses request, responds accordingly (sends list of avail files)
            // if file list too big, send chunks until end of vector
            // client sends request for a file
            // server sends file over
            recvbuf[iResult] = '\0';
            std::cout << "Bytes received: " << iResult << std::endl;
            std::cout << recvbuf << std::endl;

            std::string contents;
            files = FileHandler::getFiles("../../../files");
            try
            {
                contents = FileHandler::readFromFile("../../../files", "helloworld.exe");
            }
            catch (const std::exception &e)
            {
                std::cerr << "Failed to read file: " << e.what() << std::endl;
                break;
            }

            size_t totalSent = 0;
            while (totalSent < contents.size())
            {
                size_t remaining = contents.size() - totalSent;
                size_t chunkSize = std::min(remaining, static_cast<size_t>(BUFFER_SIZE - 1));

                memcpy(sendbuf, contents.c_str() + totalSent, chunkSize);

                if ((iSendResult = send(client, sendbuf, static_cast<int>(chunkSize), 0)) == SOCKET_ERROR)
                {
                    throw std::runtime_error("send failed: " + std::to_string(WSAGetLastError()));
                }

                std::cout << "Bytes sent: " << iSendResult << std::endl;
                totalSent += iSendResult;
            }
            std::cout << "Total bytes sent: " << totalSent << std::endl;
        }

        if (iResult == 0)
        {
            std::cout << "Connection closing..." << std::endl;
        }
        else if (iResult < 0)
        {
            throw std::runtime_error("recv failed: " + std::to_string(WSAGetLastError()));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error in handleConnection: " << e.what() << std::endl;
    }

    if ((iResult = shutdown(client, SD_SEND)) == SOCKET_ERROR)
    {
        closesocket(client);
        WSACleanup();
        throw std::runtime_error("shutdown failed: " + WSAGetLastError());
    }
}