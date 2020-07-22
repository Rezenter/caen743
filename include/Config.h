//
// Created by ts_group on 6/28/2020.
//

#ifndef CAEN743_CONFIG_H
#define CAEN743_CONFIG_H

#include <string>
#include "CAENDigitizerType.h"
#include "json.hpp"
#include <filesystem>
#include <fstream>

using Json = nlohmann::json;

class Config {
private:

public:
    unsigned char caenCount = 4;
    int acquisitionTime = 1; //seconds
    std::string plasmaPath = "d:/data/fastDump/plasma/";
    std::string debugPath = "d:/data/fastDump/debug/";

    unsigned int connectionTimeout = 1; // seconds between connections
    unsigned int commandTimeout = 100; // milliseconds between command readout attempts
    unsigned int connectionDeadTime = 120; // seconds before connection close
    unsigned int messagePoolingInterval = 100; // milliseconds between message reading

    unsigned int recordLength = 1024;
    unsigned int triggerDelay = 18; // *16 cells
    CAEN_DGTZ_SAMFrequency_t frequency = CAEN_DGTZ_SAM_3_2GHz; // 1_6G, 800M, 400M


    unsigned int debugShot = 0;
    unsigned int plasmaShot = 0;
    bool isPlasma = false;

    bool load(std::string path = "d:/data/fastDump/config.json");
};


#endif //CAEN743_CONFIG_H
