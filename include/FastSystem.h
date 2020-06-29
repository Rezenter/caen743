//
// Created by ts_group on 6/28/2020.
//

#ifndef CAEN743_FASTSYSTEM_H
#define CAEN743_FASTSYSTEM_H

#include "Crate.h"

class FastSystem {
private:
    const Crate crate;

public:
    FastSystem(Config& config);
};


#endif //CAEN743_FASTSYSTEM_H
