//
// Created by user on 6/18/2020.
//

//debug
#include <iostream>
//debug

#include "CAEN743.h"

unsigned char CAEN743::caenCount = 0;

int CAEN743::init(Config& config){
    this->config = &config;
    ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_OpticalLink, address, 0, 0, &handle);
    if(ret != CAEN_DGTZ_Success) {
        printf("Can't open digitizer\n");
        return CAEN_Error_Connection;
    }

    printf("Connected to CAEN with address %d\n", address);


    //ret = CAEN_DGTZ_Reset(handle);                                               // Reset Digitizer
    ret = CAEN_DGTZ_SetRecordLength(handle,1024);
    ret = CAEN_DGTZ_SetChannelEnableMask(handle,1);
    ret = CAEN_DGTZ_SetChannelSelfTrigger(handle,CAEN_DGTZ_TRGMODE_DISABLED,0b11111111);  // Set trigger on channel 0 to be ACQ_ONLY
    ret = CAEN_DGTZ_SetSWTriggerMode(handle,CAEN_DGTZ_TRGMODE_DISABLED);
    ret = CAEN_DGTZ_SetExtTriggerInputMode(handle, CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);

    ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle, MAX_TRANSFER);

    //ret = CAEN_DGTZ_SetRunSynchronizationMode(handle, CAEN_DGTZ_RUN_SYNC_Disabled);
    ret = CAEN_DGTZ_SetIOLevel(handle, CAEN_DGTZ_IOLevel_NIM);
    ret = CAEN_DGTZ_SetAcquisitionMode(handle,CAEN_DGTZ_SW_CONTROLLED);          // Set the acquisition mode


    if(ret != CAEN_DGTZ_Success) {
        std::cout << "ADC " << (int)address << " initialisation error " << ret << std::endl;
        return 4;
    }

    //std::cout << "malloc buffer" << std::endl;

    uint32_t size;

    ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &buffer, &size);
    if(ret != CAEN_DGTZ_Success){
        std::cout << "ADC " << (int)address << " allocation error " << ret << std::endl;
        return 6;
    }
    bufferSize = 0;


    //ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &singleBuf, &singleSize);


    if(ret == CAEN_DGTZ_Success) {
        //std::cout << "success" << std::endl;
        initialized = true;
        return CAEN_Success;
    }
    std::cout << "wtf?" << std::endl;
    return 8;
}

bool CAEN743::isAlive(){
    return initialized && CAEN_DGTZ_GetInfo(handle, &BoardInfo) == CAEN_DGTZ_Success;
}

CAEN743::~CAEN743() {
    if(associatedThread.joinable()){
        associatedThread.join();
    }
    //std::cout << "caen destructor...";
    if(initialized){
        CAEN_DGTZ_FreeReadoutBuffer(&buffer); // some shit
    }

    if(handle){
        CAEN_DGTZ_CloseDigitizer(handle);
    }
    //std::cout << "OK" << std::endl;
}

void CAEN743::process() {
    json j;
    j["pi"] = 3.14;
    /*
    CAEN_DGTZ_X743_EVENT_t* eventDecoded = nullptr;
    ret = CAEN_DGTZ_AllocateEvent(handle, (void**)(&eventDecoded));

    std::stringstream filename;
    CAEN_DGTZ_X743_GROUP_t* group;
    int count = 0;
    for(char* event : events){
        filename.str(std::string());
        filename << int(address) << "/" << count++ << ".csv";
        outFile.open(filename.str());
        CAEN_DGTZ_DecodeEvent(handle, event, (void **) &eventDecoded);
        std::stringstream line;

        line << "time" << ", ";
        for(int groupIdx = 0; groupIdx < MAX_V1743_GROUP_SIZE; groupIdx++){
            if(eventDecoded->GrPresent[groupIdx]){
                //line << "time" << groupIdx << ", ";
                for(int ch = 0; ch < MAX_X743_CHANNELS_X_GROUP; ch++){
                    line << "ch" << ch << ", ";
                }
            }
        }
        line << std::endl;
        outFile << line.str();

        line.str(std::string());
        line << "count, ";
        for(unsigned char groupIdx : eventDecoded->GrPresent){
            if(groupIdx){
                //line << "count, ";
                for(int ch = 0; ch < MAX_X743_CHANNELS_X_GROUP; ch++){
                    line << "mV, ";
                }
            }
        }
        line << std::endl;
        outFile << line.str();


        for(int cellIdx = 0; cellIdx < RECORD_LENGTH; cellIdx++){
            line.str(std::string());
            line << cellIdx;
            for(int groupIdx = 0; groupIdx < MAX_V1743_GROUP_SIZE; groupIdx++){
                if(eventDecoded->GrPresent[groupIdx]){
                    group = &eventDecoded->DataGroup[groupIdx];
                    //line << cellIdx << ", ";
                    for(auto & ch : group->DataChannel){
                        line << ", " << ch[cellIdx];
                    }
                }else{
                    std::cout << "wtf? no group " << group << " for adc " << int(address) << std::endl;
                }
            }
            line << std::endl;
            outFile << line.str();
        }
       outFile.close();
       delete[] event;
   }
    ret = CAEN_DGTZ_FreeEvent(handle, (void**)&eventDecoded);
    */
    std::cout << "processed " << events.size() << " events for ADC " << (int)address << std::endl;
}

bool CAEN743::arm() {
    ret = CAEN_DGTZ_ClearData(handle);
    ret = CAEN_DGTZ_SWStartAcquisition(handle);
    return true;

}

bool CAEN743::disarm() {
    requestStop();
    return true;
}

bool CAEN743::payload() {
    CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &bufferSize);
    ret = CAEN_DGTZ_GetNumEvents(handle, buffer, bufferSize, &numEvents);
    if(numEvents != 0){
        for(counter = 0; counter < numEvents; counter++){
            CAEN_DGTZ_GetEventInfo(handle, buffer, bufferSize, counter, &eventInfo, &eventEncoded);
            char* singleEvent = new char[EVT_SIZE];
            memcpy(singleEvent, eventEncoded, EVT_SIZE);
            events.push_back(singleEvent);
            //CAEN_DGTZ_DecodeEvent(handle, eventEncoded, (void **) &eventDecoded);
        }
    }
    return false;
}

void CAEN743::afterPayload() {
    //std::cout << "after payload" << std::endl;
    ret = CAEN_DGTZ_SWStopAcquisition(handle);
    if(ret != 0){
        std::cout << "failed to stop ADC = " << ret << std::endl;
    }
    ret = CAEN_DGTZ_ClearData(handle);
    //std::cout << "stopped adc " << (int)address << " @ buffer cell # " << current_buffer << std::endl << std::flush;
    process();
}

void CAEN743::beforePayload() {
    //std::cout << "before payload" << std::endl;
    //ret = CAEN_DGTZ_ClearData(handle);
}


bool CAEN743::waitTillProcessed() {
    associatedThread.join();
    //std::cout << "thread " << (int)address << " joined" << std::endl;
    return false;
}

bool CAEN743::cyclicReadout() {
    associatedThread = std::thread([&](){
        run();
    });
    return false;
}

bool CAEN743::singleRead() {
    /*
    ret = CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer[current_buffer],&sizes[current_buffer]);

    if(sizes[current_buffer] != 0){
        current_buffer++;
    }
    if(current_buffer == MAX_BUFFER){
        std::cout << "not enough buffer!" << std::endl;
        return true;
    }
    return false;
     */
    return true;
}
