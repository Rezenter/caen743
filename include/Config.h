//
// Created by ts_group on 6/28/2020.
//

#ifndef CAEN743_CONFIG_H
#define CAEN743_CONFIG_H

typedef enum Trigger_mode{
    Trigger_hardware    = 0, // LEMO socket trigger
    Trigger_channel     = 1, // start from ane channel
    Trigger_software    = 2, // software
} Trigger_mode;

class Config {
private:

public:
    unsigned char caenCount = 3;
    unsigned short triggerCount = 1000;
    Trigger_mode triggerMode = Trigger_software;
};


#endif //CAEN743_CONFIG_H
