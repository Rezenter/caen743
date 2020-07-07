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

    ret = CAEN_DGTZ_Reset(handle);                                               // Reset Digitizer
    ret = CAEN_DGTZ_SetRecordLength(handle,1024);
    ret = CAEN_DGTZ_SetChannelEnableMask(handle,1);
    ret = CAEN_DGTZ_SetChannelSelfTrigger(handle,CAEN_DGTZ_TRGMODE_DISABLED,0b11111111);  // Set trigger on channel 0 to be ACQ_ONLY
    ret = CAEN_DGTZ_SetSWTriggerMode(handle,CAEN_DGTZ_TRGMODE_DISABLED);
    ret = CAEN_DGTZ_SetExtTriggerInputMode(handle, CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);

    ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle, MAX_TRANSFER);

    //ret = CAEN_DGTZ_SetRunSynchronizationMode(handle, CAEN_DGTZ_RUN_SYNC_Disabled);
    ret = CAEN_DGTZ_SetIOLevel(handle, CAEN_DGTZ_IOLevel_NIM);
    ret = CAEN_DGTZ_SetAcquisitionMode(handle,CAEN_DGTZ_SW_CONTROLLED);          // Set the acquisition mode

    /*
    if(address == MASTER){
        ret = CAEN_DGTZ_SetInterruptConfig (handle, CAEN_DGTZ_DISABLE, 1, 0, INTERRUPTION_THRESHOLD, CAEN_DGTZ_IRQ_MODE_RORA);
    }
*/

    if(ret != CAEN_DGTZ_Success) {
        std::cout << "ADC " << (int)address << " initialisation error " << ret << std::endl;
        return 4;
    }

    //std::cout << "malloc buffer" << std::endl;
    /*
    uint32_t size;
    for(int i = 0; i < MAX_BUFFER; i++){
        ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &buffer[i], &size);
        if(ret != CAEN_DGTZ_Success){
            std::cout << "ADC " << (int)address << " allocation error " << ret << std::endl;
            return 6;
        }
        sizes[i] = 0;
    }
     */
    ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &singleBuf, &singleSize);

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
        for(int i = 0; i < MAX_BUFFER; i++){
            //CAEN_DGTZ_FreeReadoutBuffer(&buffer[i]); // some shit
        }
        CAEN_DGTZ_FreeReadoutBuffer(&singleBuf); // some shit
    }
    if(handle){
        CAEN_DGTZ_CloseDigitizer(handle);
    }
    //delete[] buffer;
    //delete[] sizes;
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
        /*
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
         */
    }
    current_buffer = 0;
    ret = CAEN_DGTZ_FreeEvent(handle, (void**)&evt);
    std::cout << "processed " << count << " events for ADC " << (int)address << std::endl;
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
    //ret = CAEN_DGTZ_IRQWait(handle, 1000);
    //std::cout << "interrupt " << ret << std::endl;
    //ret = CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer[current_buffer],&sizes[current_buffer]);
    //ret = CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_POLLING_MBLT,buffer[current_buffer],&sizes[current_buffer]);
    ret = CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_POLLING_MBLT,singleBuf,&singleSize);
    /*
    if(ret == CAEN_DGTZ_Success){
        //ret = CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_POLLING_MBLT,buffer[current_buffer],&sizes[current_buffer]);
        ret = CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer[current_buffer],&sizes[current_buffer]);
    }
     */
    //ret = CAEN_DGTZ_SetInterruptConfig (handle, CAEN_DGTZ_DISABLE, 1, 0, INTERRUPTION_THRESHOLD, CAEN_DGTZ_IRQ_MODE_RORA);
    //ret = CAEN_DGTZ_SetInterruptConfig (handle, CAEN_DGTZ_ENABLE, 1, 0, INTERRUPTION_THRESHOLD, CAEN_DGTZ_IRQ_MODE_RORA);
    if(address != 0){
        //std::cout << int(address) << " ret " << ret << " got " << sizes[current_buffer] << std::endl;
        std::cout << int(address) << " ret " << ret << " got " << singleSize << std::endl;
    }
    /*
    if(sizes[current_buffer] > 0){
        current_buffer++;
    }
    if(current_buffer == MAX_BUFFER){
        std::cout << "not enough buffer!" << std::endl;
        return true;
    }
     */
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
    //ret = CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer[current_buffer],&sizes[current_buffer]);
    ret = CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_POLLING_MBLT,singleBuf,&singleSize);

    /*
    if(sizes[current_buffer] != 0){
        std::cout << int(address) << " got " << sizes[current_buffer] << std::endl;
        current_buffer++;
    }
    if(current_buffer == MAX_BUFFER){
        std::cout << "not enough buffer!" << std::endl;
        return true;
    }
     */
    return false;
}
