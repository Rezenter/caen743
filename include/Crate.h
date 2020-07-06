//
// Created by ts_group on 6/28/2020.
//

#ifndef CAEN743_CRATE_H
#define CAEN743_CRATE_H

#include "CAEN743.h"
#include "common.h"


class Crate : public Stoppable{
private:
    Config& config;
    CAEN743 caens[MAX_CAENS];

    bool payload() override;
    void beforePayload() override;
    void afterPayload() override;
public:
    explicit Crate(Config& config);

    void arm();
    void disarm();

    void multiThreaded();
    void singleThreaded();
};


#endif //CAEN743_CRATE_H
