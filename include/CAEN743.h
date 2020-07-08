//
// Created by user on 6/18/2020.
//
#ifndef CAEN743_CAEN743_H
#define CAEN743_CAEN743_H

#include "CAENDigitizer.h"
#include "common.h"
#include "Stoppable.h"
#include <vector>

#define CAEN_USE_DIGITIZERS
#define IGNORE_DPP_DEPRECATED

#define MAX_RECORD_LENGTH 1024 //maximum samples per event
#define MAX_TRANSFER 10 //maximum events per transaction
#define INTERRUPTION_THRESHOLD 5//events in buffer before interruption
#define MAX_BUFFER 10000 // maximum transactions before processing
#define MASTER 0 // address of the master board
#define EVT_SIZE 34832

typedef enum CAEN_ErrorCode {
    CAEN_Success = 0, //ok
    CAEN_Error_Connection = -1, // Can't open digitizer
}CAEN_ErrorCode;

class CAEN743 final : public Stoppable{
private:
    Config* config;
    static unsigned char caenCount;
    const unsigned char address; //optical link number

    CAEN_DGTZ_ErrorCode ret;
    int	handle;

    char* buffer;
    uint32_t bufferSize;

    std::vector<char*> events;

    CAEN_DGTZ_BoardInfo_t BoardInfo;

    bool payload() override;
    void beforePayload() override;
    void afterPayload() override;
    bool initialized = false;
    uint32_t numEvents;
    int counter = 0;
    CAEN_DGTZ_EventInfo_t eventInfo;
    char* eventEncoded = nullptr;

public:
    CAEN743() : address(caenCount){caenCount++;};
    ~CAEN743();
    bool isAlive();
    int init(Config& config);
    bool arm();
    bool disarm();
    bool cyclicReadout();
    bool singleRead();
    bool waitTillProcessed();
    void process();
    void disarmForCrate(){afterPayload();};
    [[nodiscard]] int getHandle() const{return handle;};
};

#endif //CAEN743_CAEN743_H