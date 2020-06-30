//
// Created by ts_group on 6/28/2020.
//

#include "Crate.h"

//debug
#include <iostream>
//debug

Crate::Crate(Config &config) {
    for(int count = 0; count < config.caenCount; count++){
        caens[count].init(config);
        if(!caens[count].isAlive()){
            std::cout << "caen not initialised!" << std::endl;
        }
    }
    for(int count = 0; count < config.caenCount; count++){
        caens[count].arm();
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
    for(int count = 0; count < config.caenCount; count++){
        caens[count].disarm();
    }
}
