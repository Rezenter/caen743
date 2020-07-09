//
// Created by ts_group on 6/28/2020.
//

#ifndef CAEN743_CRATE_H
#define CAEN743_CRATE_H

#include "CAEN743.h"
#include "common.h"


class Crate{
private:
    Config& config;
    CAEN743 caens[MAX_CAENS];

public:
    explicit Crate(Config& config);

    bool init();

    void multiThreaded();
};


#endif //CAEN743_CRATE_H
