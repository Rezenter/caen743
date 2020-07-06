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
    for(auto& caen : caens){
        if(caen.singleRead()){
            return true;
        }
    }
    return false;
}

void Crate::beforePayload() {
    for(auto& caen : caens){
        caen.init(config);
        if(!caen.isAlive()){
            std::cout << "caen not initialised!" << std::endl;
        }
    }
    for(auto& caen : caens){
        caen.arm();
    }
}

void Crate::afterPayload() {
    Stoppable::afterPayload();
}

void Crate::singleThreaded() {
    arm();

    std::this_thread::sleep_for(std::chrono::seconds(config.acquisitionTime));

    disarm();
    associatedThread.join();
}

void Crate::multiThreaded() {
    for(int count = 0; count < config.caenCount; count++){
        caens[count].init(config);
        if(!caens[count].isAlive()){
            std::cout << "caen not initialised!" << std::endl;
        }
    }
    for(int count = 0; count < config.caenCount; count++){
        caens[count].arm();
    }

    for(int count = 0; count < config.caenCount; count++){
        caens[count].cyclycReadout();
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
