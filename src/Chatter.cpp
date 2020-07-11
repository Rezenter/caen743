//
// Created by ts_group on 10.07.2020.
//

#include "Chatter.h"

bool Chatter::init(Config &config) {
    this->config = &config;

    WSADATA wsaData = {0};
    int iResult = 0;
    SOCKADDR_IN serverAddr, clientAddr;
    SOCKET server, client;;

//    int i = 1;

    SOCKET sock = INVALID_SOCKET;
    int iFamily = AF_INET;
    int iType = SOCK_STREAM;
    int iProtocol = IPPROTO_TCP;


    sock = socket(iFamily, iType, iProtocol);
    if (sock == INVALID_SOCKET)
        wprintf(L"socket function failed with error = %d\n", WSAGetLastError() );
    else {
        wprintf(L"socket function succeeded\n");

        // Close the socket to release the resources associated
        // Normally an application calls shutdown() before closesocket
        //   to  disables sends or receives on a socket first
        // This isn't needed in this simple sample
        iResult = closesocket(sock);
        if (iResult == SOCKET_ERROR) {
            wprintf(L"closesocket failed with error = %d\n", WSAGetLastError() );
            WSACleanup();
            return 1;
        }
    }

    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);

    bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
    listen(server, 0);

    char buffer[1024];
    int clientAddrSize = sizeof(clientAddr);
    if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
    {
        std::cout << "Client connected!" << std::endl;
        recv(client, buffer, sizeof(buffer), 0);
        std::cout << "Client says: " << buffer << std::endl;
        memset(buffer, 0, sizeof(buffer));

        closesocket(client);
        std::cout << "Client disconnected." << std::endl;
    }

    WSACleanup();

    return true;
}

bool Chatter::payload() {
    return false;
}

bool Chatter::isAlive() {
    return false;
}
