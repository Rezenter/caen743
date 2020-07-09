//
// Created by ts_group on 6/28/2020.
//

#include "Crate.h"

//debug
#include <iostream>
//debug

Crate::Crate(Config &config) : config(config){
    multiThreaded();
}

void Crate::multiThreaded() {
    if(!init()){
        std::cout << "init failed" << std::endl;
        return;
    }

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

    for(int count = 0; count < config.caenCount; count++){
        caens[count].waitTillProcessed();
    }
}

bool Crate::init() {
    for(int count = 0; count < config.caenCount; count++){
        caens[count].init(config);
        if(!caens[count].isAlive()){
            std::cout << "board " << count << " not initialised!" << std::endl;
            return false;
        }
    }
    return true;
}
