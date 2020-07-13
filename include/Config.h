//
// Created by ts_group on 6/28/2020.
//

#ifndef CAEN743_CONFIG_H
#define CAEN743_CONFIG_H

#include <string>

class Config {
private:

public:
    unsigned char caenCount = 4;
    int acquisitionTime = 1; //seconds
    std::string plasmaPath = "d:/data/fastDump/plasma/";
    std::string debugPath = "d:/data/fastDump/debug/";

    unsigned int connectionTimeout = 2; // seconds between connections
    unsigned int commandTimeout = 200; // milliseconds between command readout attempts
    unsigned int maxDeadTime = 1; // seconds after last packet before socket closing

    unsigned int recordLength = 1024;

    unsigned int debugShot = 0;
    unsigned int plasmaShot = 0;
    bool isPlasma = false;


};


#endif //CAEN743_CONFIG_H
