//
// Created by ts_group on 6/28/2020.
//

#ifndef CAEN743_CRATE_H
#define CAEN743_CRATE_H

#include "CAEN743.h"
#include "common.h"


class Crate{
private:
    CAEN743 caens[MAX_CAENS];

public:
    Crate(Config& config);
};


#endif //CAEN743_CRATE_H
