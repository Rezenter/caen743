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



using Json = nlohmann::json;

class Chatter : public Stoppable{
private:
    Config* config;

    bool payload() override;
    //void beforePayload() override;
    //void afterPayload() override;
public:
    bool init(Config& config);
    [[nodiscard]] bool isAlive();
};


#endif //CAEN743_CHATTER_H
