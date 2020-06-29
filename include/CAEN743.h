//
// Created by user on 6/18/2020.
//
#ifndef CAEN743_CAEN743_H
#define CAEN743_CAEN743_H

#include "CAENDigitizer.h"
#include "common.h"
#include "Stoppable.h"

#define CAEN_USE_DIGITIZERS
#define IGNORE_DPP_DEPRECATED

#define MAX_RECORD_LENGTH 1024 //maximum samples per event
#define MAX_TRANSFER 7 //maximum events per transaction
#define MAX_BUFFER 200 // maximum transactions before processing

typedef enum CAEN_ErrorCode {
    CAEN_Success = 0, //ok
    CAEN_Error_Connection = -1, // Can't open digitizer

}CAEN_ErrorCode;

class CAEN743 : public Stoppable{
private:
    static unsigned char caenCount;
    const unsigned char address; //optical link number

    CAEN_DGTZ_ErrorCode ret;
    int	handle;

    char* buffer[MAX_BUFFER];
    uint32_t sizes[MAX_BUFFER];

    //uint32_t size,bsize;
    unsigned int current_buffer = 0;
    CAEN_DGTZ_BoardInfo_t BoardInfo;

    void run() override;
    void process();
public:
    CAEN743() : address(caenCount){caenCount++;};
    ~CAEN743();
    bool isAlive();
    int init(Config& config);
    bool arm();
    bool disarm();
};

#endif //CAEN743_CAEN743_H
