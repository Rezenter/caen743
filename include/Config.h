//
// Created by ts_group on 6/28/2020.
//

#ifndef CAEN743_CONFIG_H
#define CAEN743_CONFIG_H

typedef enum Trigger_mode{
    Trigger_hardware        = 0, // LEMO socket trigger
    Trigger_channel         = 1, // start from any channel
    Trigger_software_master = 2, // software trigger of master board only
    Trigger_software_all    = 3, // software trigger of every board independently
} Trigger_mode;

class Config {
private:

public:
    unsigned char caenCount = 4;
    unsigned short triggerCount = 1000;
    Trigger_mode triggerMode = Trigger_software_all;
    int acquisitionTime = 5; //seconds
};


#endif //CAEN743_CONFIG_H
