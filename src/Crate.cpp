//
// Created by ts_group on 6/28/2020.
//

#include "Crate.h"

//debug
#include <iostream>
//debug

#include <io.h>

Crate::Crate(Config &config) : config(config){
    init();
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

    Json result = {
            {"eventLength", config.recordLength},
            {"frequency", 3.2},
            {"boards", Json::array()}
    };
    std::string path = config.savePath + "00000/";
    mkdir(path.c_str());
    std::stringstream filename;
    std::ofstream outFile;
    for(int count = 0; count < config.caenCount; count++) {
        filename.str(std::string());
        filename << "board_" << count << ".json";
        outFile.open(path + filename.str());
        outFile << std::setw(2) << caens[count].waitTillProcessed() << std::endl;
        outFile.close();
        caens[count].releaseMemory();
        result["boards"].push_back(count);
    }
    std::cout << "all joined" << std::endl;

    filename.str(std::string());
    filename << "header.json";
    outFile.open(path + filename.str());
    outFile << std::setw(2) << result << std::endl;
    outFile.close();
}

bool Crate::init() {
    for(int count = 0; count < config.caenCount; count++){
        caens[count].init(config);
        if(!caens[count].isAlive()){
            std::cout << "board " << count << " not initialised!" << std::endl;
            online = false;
            return false;
        }
    }
    online = true;
    return true;
}

bool Crate::arm() {
    return false;
}

Json Crate::disarm() {
    return nullptr;
}

bool Crate::isAlive() {
    return false;
}
