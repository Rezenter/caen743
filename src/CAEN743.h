//
// Created by user on 6/18/2020.
//

#include <deque>

#ifndef CAEN743_CAEN743_H
#define CAEN743_CAEN743_H

#include "CAENDigitizer.h"

#define CAEN_USE_DIGITIZERS
#define IGNORE_DPP_DEPRECATED

#define MAX_CONNECTIONS 4 /* Number of simultaneous connections */

class CAEN743 {
private:
    size_t maxPageCount = 0;

    unsigned char arm();
    unsigned char disarm();

public:
    CAEN743(unsigned char address) : address(address){};
    const unsigned char address; //link number and first hex digit of VME address for all ADCs
};

std::deque<CAEN743> ADCs;

unsigned char arm(unsigned char address);
unsigned char disarm();

#endif //CAEN743_CAEN743_H
