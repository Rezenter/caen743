//
// Created by ts_group on 6/28/2020.
//

#include "FastSystem.h"

FastSystem::FastSystem(Config& config) : config(config), crate(Crate(config)), storage(config){
    init();

    if(!isAlive()){
        std::cout << "FastSystem init failed" << std::endl;
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
    /*
    if(!crate.isAlive()){
        return false;
    }
     */
    if(!storage.isAlive()){
        return false;
    }

    return true;
}

bool FastSystem::init() {
    if(!chatter.init(config)){
        return false;
    }
    if(false && !crate.init()){
        return false;
    }
    return true;
}

bool FastSystem::payload() {
    return false;
}
