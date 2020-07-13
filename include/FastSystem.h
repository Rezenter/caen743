//
// Created by ts_group on 6/28/2020.
//

#ifndef CAEN743_FASTSYSTEM_H
#define CAEN743_FASTSYSTEM_H

#include "Crate.h"
#include "Storage.h"
#include "Chatter.h"

class FastSystem {
private:
    Crate crate;
    Storage storage;
    Chatter chatter;
    Config& config;
    bool exit = false;

public:
    explicit FastSystem(Config& config);

    bool arm();
    bool disarm();
    bool isAlive();

    bool init();
    bool exitRequested(){return exit;};
};


#endif //CAEN743_FASTSYSTEM_H
