//
// Created by user on 6/18/2020.
//

#include <deque>
#include <thread>
#include <chrono>
#include <future>

#ifndef CAEN743_CAEN743_H
#define CAEN743_CAEN743_H

#include "CAENDigitizer.h"

#define CAEN_USE_DIGITIZERS
#define IGNORE_DPP_DEPRECATED

#define MAX_CONNECTIONS 4 /* Number of simultaneous connections */
#define OK 1 //good return status, required by MDSPlus
#define RECORD_LENGTH 1024

#include "root.h"

class Stoppable{
private:
    std::promise<void> exitSignal;
    std::future<void> futureObj;

public:
    std::thread associatedThread;

    Stoppable() : futureObj(exitSignal.get_future()) {}
    Stoppable(Stoppable && obj)  noexcept : exitSignal(std::move(obj.exitSignal)), futureObj(std::move(obj.futureObj)){}
    Stoppable & operator = (Stoppable && obj) noexcept {
        exitSignal = std::move(obj.exitSignal);
        futureObj = std::move(obj.futureObj);
        return *this;
    }

    bool stopRequested(){
        return !(futureObj.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout);
    }

    void stop(){
        exitSignal.set_value();
    }
};

class CAEN743: public Stoppable{
private:
    CAEN_DGTZ_ErrorCode ret;
    size_t maxPageCount = 0;
    int	handle;
    char *buffer = nullptr;
    CAEN_DGTZ_BoardInfo_t BoardInfo;
    CAEN_DGTZ_EventInfo_t eventInfo;
    CAEN_DGTZ_UINT16_EVENT_t *Evt = nullptr;

    int event_index;
    int MajorNumber;
    int c = 0, count;
    char * evtptr = nullptr;
    uint32_t size,bsize;
    uint32_t numEvents;


public:
    explicit CAEN743(unsigned char address) : address(address) {};
    const unsigned char address; //optical link number and first hex digit of VME address

    int init();
    void run();
};


std::deque<CAEN743> ADCs;

unsigned char arm(unsigned char address);
unsigned char disarm(unsigned char address);

#endif //CAEN743_CAEN743_H
