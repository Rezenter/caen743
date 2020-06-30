//
// Created by user on 6/18/2020.
//

//debug
#include <iostream>
//debug

#include "CAEN743.h"

unsigned char CAEN743::caenCount = 0;

int CAEN743::init(Config& config) {
    ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_OpticalLink, address, 0, 0, &handle);
    if(ret != CAEN_DGTZ_Success) {
        printf("Can't open digitizer\n");
        return CAEN_Error_Connection;
    }
    ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
    printf("Connected to CAEN with address %d\n", address);

    //ret = CAEN_DGTZ_Reset(handle);                                               // Reset Digitizer
    ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);                                 // Get Board Info
    ret = CAEN_DGTZ_SetRecordLength(handle, MAX_RECORD_LENGTH);                       // Set the lenght of each waveform (in samples)
    ////ret = CAEN_DGTZ_SetChannelEnableMask(handle,1);                              // Enable channel 0
    //ret = CAEN_DGTZ_SetGroupEnableMask(handle, 0b11111111);

    /////ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,0,0x7fff); //+1.25V = 0x0000, 0V = 0x7FFF, -1.23 = 0xFFFF
    /////ret = CAEN_DGTZ_SetGroupTriggerThreshold(handle, 0, 32768);
    ret = CAEN_DGTZ_SetChannelSelfTrigger(handle,CAEN_DGTZ_TRGMODE_DISABLED,0b11111111);  // Set trigger on channel 0 to be ACQ_ONLY
    ret = CAEN_DGTZ_SetRunSynchronizationMode(handle, CAEN_DGTZ_RUN_SYNC_Disabled);
    ret = CAEN_DGTZ_SetIOLevel(handle, CAEN_DGTZ_IOLevel_NIM);
    //ret = CAEN_DGTZ_SetOutputSignalMode(handle, CAEN_DGTZ_TRIGGER);
    if(address == MASTER){
        //ret = CAEN_DGTZ_SetExtTriggerInputMode(handle, CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);
        ret = CAEN_DGTZ_SetSWTriggerMode(handle,CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);         // Set the behaviour when a SW tirgger arrives
    }else{
        ret = CAEN_DGTZ_SetSWTriggerMode(handle,CAEN_DGTZ_TRGMODE_DISABLED);         // Set the behaviour when a SW tirgger arrives
        ret = CAEN_DGTZ_SetExtTriggerInputMode(handle, CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);
    }
    ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle,MAX_TRANSFER);                                // Set the max number of events to transfer in a sigle readout
    ret = CAEN_DGTZ_SetSAMAcquisitionMode(handle, CAEN_DGTZ_AcquisitionMode_STANDARD);
    ret = CAEN_DGTZ_SetAcquisitionMode(handle,CAEN_DGTZ_SW_CONTROLLED);          // Set the acquisition mode
    if(ret != CAEN_DGTZ_Success) {
        std::cout << "ADC " << address << " initialisation error" << ret << std::endl;
        return 4;
    }

    //std::cout << "malloc buffer" << std::endl;
    uint32_t size;
    for(int i = 0; i < MAX_BUFFER; i++){
        ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &buffer[i], &size);
        sizes[i] = 0;
    }

    if(ret == CAEN_DGTZ_Success) {
        //std::cout << "success" << std::endl;
        return CAEN_Success;
    }
    std::cout << "ADC " << address << " initialisation error" << ret << std::endl;
    return 6;
}

bool CAEN743::isAlive(){
    return CAEN_DGTZ_GetInfo(handle, &BoardInfo) == CAEN_DGTZ_Success;
}

void CAEN743::run() {
    //std::cout << "run called" << std::endl;
    ret = CAEN_DGTZ_SWStartAcquisition(handle);
    while(!stopRequested()){
        if(address == MASTER){
            ret = CAEN_DGTZ_SendSWtrigger(handle); // Send a SW Trigger
        }
        ret = CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_POLLING_MBLT,buffer[current_buffer],&sizes[current_buffer]); // Read the buffer from the digitizer
        if(sizes[current_buffer] > 20){
            current_buffer++;
        }
        /*
        if(ret != 0){
            std::cout << "readData return" << ret << std::endl;
        }
         */
        //std::cout << sizes[current_buffer] << std::endl;
        if(current_buffer == MAX_BUFFER){
            std::cout << "not enougth buffer!" << std::endl;
            break;
        }
        /*
        if(address == MASTER){
            break;
        }
         */
    }
    ret = CAEN_DGTZ_SWStopAcquisition(handle);
    if(ret != 0){
        std::cout << "failed to stop ADC = " << ret << std::endl;
    }
    ret = CAEN_DGTZ_ClearData(handle);
    std::cout << "stopped adc " << (int)address << " @ buffer cell # " << current_buffer << std::endl << std::flush;
    process();
}

CAEN743::~CAEN743() {
    //associatedThread.join();
    //std::cout << "thread " << (int)address << " joined" << std::endl;
    //std::cout << "caen destructor...";
    if(buffer[0]){
        for(auto & i : buffer) {
            CAEN_DGTZ_FreeReadoutBuffer(&i);
        }
    }
    if(handle){
        CAEN_DGTZ_CloseDigitizer(handle);
    }
    //std::cout << "OK" << std::endl;
}

void CAEN743::process() {
    //std::cout << "processing" << std::endl;
    unsigned int count = 0;
    CAEN_DGTZ_EventInfo_t eventInfo;
    CAEN_DGTZ_X743_EVENT_t* evt = nullptr;
    ret = CAEN_DGTZ_AllocateEvent(handle, (void**)(&evt));
    uint32_t numEvents = 0;
    char *evtptr = nullptr;
    for(int i = 0; i < current_buffer; i++) {
        //std::cout << "buffer = " << sizes[i];
        if(sizes[i] > 16){
            ret = CAEN_DGTZ_GetNumEvents(handle, buffer[i], sizes[i], &numEvents);
            if(ret != CAEN_DGTZ_Success){
                printf("getNum error %d, adc %d, buffer %d\n", ret, address, i);
                break;
            }
            count += numEvents;
            //std::cout << ", events " << numEvents << std::endl;
            for (int event_index = 0; event_index < numEvents; event_index++) {
                //printf("adc %d, buffer %d, event %d/%d\n", address, i, event_index, numEvents-1);
                ret = CAEN_DGTZ_GetEventInfo(handle, buffer[i], sizes[i], event_index, &eventInfo, &evtptr);
                if(ret != CAEN_DGTZ_Success){
                    printf("einfo rror %d, adc %d, buffer %d, event %d/%d\n", ret, address, i, event_index, numEvents-1);
                    break;
                }
                ret = CAEN_DGTZ_DecodeEvent(handle, evtptr, (void **) &evt);
                if(ret != CAEN_DGTZ_Success){
                    printf("decode error %d, adc %d, buffer %d, event %d/%d\n", ret, address, i, event_index, numEvents-1);
                    break;
                }
            }
        }else{
            //std::cout << " skipped" << std::endl;
        }
        memset(buffer[i], 0, sizes[i] * (sizeof(char)));
        sizes[i] = 0;
    }
    current_buffer = 0;
    ret = CAEN_DGTZ_FreeEvent(handle, (void**)&evt);
    std::cout << "processed " << count << " events for ADC " << (int)address << std::endl;
}

bool CAEN743::arm() {
    //std::cout << "arm" << std::endl;

    associatedThread = std::thread([&](){
        run();
    });
    return true;

}

bool CAEN743::disarm() {
    stop();
    associatedThread.join(); //debug
    //std::cout << "thread " << (int)address << " joined" << std::endl;
    return true;
}

void CAEN743::trigger(unsigned int count) {
    while(!stopRequested()){

    }
}

bool CAEN743::armTrigger(unsigned int count) {
    return false;
}

bool CAEN743::disarmTrigger() {
    return false;
}

bool CAEN743::singleRead() {
    return false;
}
