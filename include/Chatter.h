//
// Created by ts_group on 10.07.2020.
//

#ifndef CAEN743_CHATTER_H
#define CAEN743_CHATTER_H

#ifndef UNICODE
#define UNICODE 1
#endif

#include "Stoppable.h"
#include "Config.h"
#include "json.hpp"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <chrono>
//#include <stdlib.h>
//#include <stdio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

using Json = nlohmann::json;


class Chatter : public Stoppable{
private:
    Config* config;

    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime = std::chrono::high_resolution_clock::now();

    WSADATA wsaData;
    int iResult;

    SOCKET listenSocket = INVALID_SOCKET;
    SOCKET clientSocket = INVALID_SOCKET;

    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    bool payload() override;
    //void beforePayload() override;
    void afterPayload() override;
public:
    ~Chatter();
    bool init(Config& config);
};


#endif //CAEN743_CHATTER_H
