//
// Created by ts_group on 10.07.2020.
//

#include "Chatter.h"

bool Chatter::init(Config &config) {
    std::cout << "init chatter" << std::endl;
    this->config = &config;

    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return false;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *result = nullptr;
    iResult = getaddrinfo(nullptr, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return false;
    }

    listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }

    unsigned long ul = 1;
    int nRet = ioctlsocket(listenSocket, FIONBIO, (unsigned long *) &ul);

    if (nRet == SOCKET_ERROR){
        printf("Put socket to async mode failed with error: %d\n", nRet);
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return false;

    }

    iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    freeaddrinfo(result);

    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    associatedThread = std::thread([&](){
        run();
    });
    return true;
}

bool Chatter::payload() {
    if(clientSocket == INVALID_SOCKET){
        std::cout << "waiting for a client..." << std::endl;
        clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            if(WSAGetLastError() == WSAEWOULDBLOCK){
                std::this_thread::sleep_for(std::chrono::seconds(config->connectionTimeout));
                return false;
            }
            printf("accept failed with error: %d, expected %d\n", WSAGetLastError(), WSAEWOULDBLOCK);
            closesocket(listenSocket);
            WSACleanup();
            return true;
        }else{
            lastTime = std::chrono::high_resolution_clock::now();
            std::cout << "client accepted" << std::endl;
        }
    }

    do {
        iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
        if(WSAGetLastError() == WSAEWOULDBLOCK){
            //add auto closing after timeout
            std::this_thread::sleep_for(std::chrono::milliseconds(config->commandTimeout));
            return false;
        }
        if (iResult > 0) {
            lastTime = std::chrono::high_resolution_clock::now();
            printf("Bytes received: %d\n", iResult);

            Json packet = Json::parse(recvbuf);//tryCatch

            std::cout << "received:\n" << packet << std::endl;

            // Echo the buffer back to the sender
            iSendResult = send(clientSocket, recvbuf, iResult, 0 );
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(clientSocket);
                WSACleanup();
                return true;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0){
                std::chrono::duration<double> deadDuration = std::chrono::high_resolution_clock::now() - lastTime;
                if(deadDuration.count() > config->maxDeadTime){
                    closesocket(clientSocket);
                    clientSocket = INVALID_SOCKET;
                    printf("Closing connection\n");
                    return false;
                }
        }
        else  {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(clientSocket);
            WSACleanup();
            return true;
        }

    }while (iResult > 0);

    return false;
}


Chatter::~Chatter() {
    requestStop();
    if(associatedThread.joinable()){
        associatedThread.join();
    }
    std::cout << "chatter closed" << std::endl;
}

void Chatter::afterPayload() {
    if(listenSocket != INVALID_SOCKET){
        closesocket(listenSocket);
    }

    if(clientSocket != INVALID_SOCKET){
        iResult = shutdown(clientSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(clientSocket);
            WSACleanup();
            return;
        }
        closesocket(clientSocket);
    }
    WSACleanup();
}

