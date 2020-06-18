//
// Created by user on 6/18/2020.
//

//debug
#include <iostream>
#include <thread>
#include <string>
//debug

#include "CAEN743.h"

unsigned char CAEN743::arm() {
    return 0;
}

unsigned char CAEN743::disarm() {
    return 0;
}

unsigned char arm(unsigned char address){
    std::cout << "arm" << std::endl;
    if(ADCs.size() < MAX_CONNECTIONS){
        for(CAEN743 adc : ADCs){
            if(adc.address == address){
                return 4;
            }
        }
        ADCs.push_back(CAEN743(address));
        return 1;
    }
    return 2;
}

unsigned char disarm(){

}