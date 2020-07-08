//
// Created by ts_group on 6/28/2020.
//

#include "Crate.h"

//debug
#include <iostream>
//debug

Crate::Crate(Config &config) : config(config){
    multiThreaded();
    //singleThreaded();
}
void Crate::arm() {
    associatedThread = std::thread([&](){
        run();
    });
}

void Crate::disarm() {
    requestStop();
}

bool Crate::payload() {
    /*
    if(caens[0].singleRead()){
        return true;
    }
    if(caens[1].singleRead()){
        return true;
    }
    if(caens[2].singleRead()){
        return true;
    }
    if(caens[3].singleRead()){
        return true;
    }
    */
    for(int count = 0; count < config.caenCount; count++){
        if(caens[count].singleRead()){
            return true;
        }
    }

    return false;
}

void Crate::beforePayload() {
    for(int count = 0; count < config.caenCount; count++){
        caens[count].arm();
    }
}

void Crate::afterPayload() {
    for(int count = 0; count < config.caenCount; count++){
        caens[count].disarmForCrate();
    }
}

void Crate::singleThreaded() {
    init();
    arm();

    std::this_thread::sleep_for(std::chrono::seconds(config.acquisitionTime));

    disarm();
    associatedThread.join();
}

void Crate::multiThreaded() {
    init();

    for(int count = 0; count < config.caenCount; count++){
        caens[count].arm();
    }

    for(int count = 0; count < config.caenCount; count++){
        caens[count].cyclicReadout();
    }

    std::this_thread::sleep_for(std::chrono::seconds(config.acquisitionTime));

    for(int count = 0; count < config.caenCount; count++){
        caens[count].disarm();
    }

    //std::cout << "all disarmed" << std::endl;
    for(int count = 0; count < config.caenCount; count++){
        caens[count].waitTillProcessed();
    }
}

void Crate::init() {
    for(int count = 0; count < config.caenCount; count++){
        caens[count].init(config);
        if(!caens[count].isAlive()){
            std::cout << "caen not initialised!" << std::endl;
            requestStop();
            return;
        }
        handles[count] = caens[count].getHandle();
    }
}
