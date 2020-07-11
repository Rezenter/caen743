//
// Created by ts_group on 6/28/2020.
//

#include "FastSystem.h"

FastSystem::FastSystem(Config& config) : crate(Crate(config)), storage(config){
    chatter.init(config);

    if(!isAlive()){
        std::cout << "init failed" << std::endl;
        return;
    }

    /*
    arm();

    std::this_thread::sleep_for(std::chrono::seconds(config.acquisitionTime));

    disarm();
     */
}

bool FastSystem::arm() {
    //set shot number
    crate.arm();
    return false;
}

bool FastSystem::disarm() {
    storage.saveDischarge(crate.disarm());
    return false;
}

bool FastSystem::isAlive() {
    if(!crate.isAlive()){
        return false;
    }
    if(!storage.isAlive()){
        return false;
    }
    if(!chatter.isAlive()){
        return false;
    }
    return true;
}

bool FastSystem::init() {
    crate.init();
    return false;
}
