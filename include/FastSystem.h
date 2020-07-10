//
// Created by ts_group on 6/28/2020.
//

#ifndef CAEN743_FASTSYSTEM_H
#define CAEN743_FASTSYSTEM_H

#include "Crate.h"
#include "Storage.h"

class FastSystem {
private:
    const Crate crate;
    const Storage storage;

    int shotn = 0;
public:
    explicit FastSystem(Config& config);
};


#endif //CAEN743_FASTSYSTEM_H
