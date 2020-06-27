//
// Created by user on 6/18/2020.
//

//debug
#include <iostream>
//debug

#include "CAEN743.h"


void CAEN743::run() {
    std::cout << "Task Start" << std::endl;
    ret = CAEN_DGTZ_SWStartAcquisition(handle);
    //MDSplus::TreeNode* dataNode = ADCsNode->addNode("data", "NUMERIC");
    while(!stopRequested()){
        ret = CAEN_DGTZ_SendSWtrigger(handle); // Send a SW Trigger

        ret = CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer,&bsize); // Read the buffer from the digitizer

        // The buffer red from the digitizer is used in the other functions to get the event data
        //The following function returns the number of events in the buffer
        ret = CAEN_DGTZ_GetNumEvents(handle,buffer,bsize,&numEvents);
        //printf("events = %d\n", numEvents);
        count +=numEvents;

        for (event_index = 0; event_index < numEvents; event_index++) {
            //std::cout << "event" << std::endl;
            /* Get the Infos and pointer to the event */
            ret = CAEN_DGTZ_GetEventInfo(handle, buffer, bsize, event_index, &eventInfo, &evtptr);
            /* Decode the event to get the data */
            ret = CAEN_DGTZ_DecodeEvent(handle, evtptr, (void**)&Evt);
            //*************************************
            // Event Elaboration
            //*************************************
        }
        //std::cout << "alive" << std::endl;
    }
    auto data = new MDSplus::Uint16(count);
    ADCsNode->putData(data);
    tree->write();
    ret = CAEN_DGTZ_FreeEvent(handle, (void**)&Evt); //bad place
    ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer);
    ret = CAEN_DGTZ_CloseDigitizer(handle);

    std::cout << "Task End" << std::endl;
}

int CAEN743::init() {
        try{
        //std::cout << "env = " << getenv("short_path") << std::endl;
        tree = new MDSplus::Tree("short", 4, "NEW");
        try{
            MDSplus::TreeNode *outTopNode = tree->getNode("\\TOP");
            char fullname[14];
            sprintf(fullname, ".%s_%d", "eventCount", address);
            try {
                ADCsNode = outTopNode->addNode(fullname, "NUMERIC");
            } catch (MDSplus::MdsException &exc)
            {
                std::cout << "Error adding node: " << exc.what() << std::endl;
            }
            delete outTopNode;
        }catch(MDSplus::MdsException &exc){
//methode MdsException::what() return a description of the exception
            std::cout << "Cannot modify tree " << treeName  << " shot " << 1 << ": " << exc.what() << std::endl;
        }
    }catch(MDSplus::MdsException &exc){
//methode MdsException::what() return a description of the exception
        std::cout << "Cannot open tree " << treeName  << " shot " << 1 << ": " << exc.what() << std::endl;
    }

    //i = sizeof(CAEN_DGTZ_TriggerMode_t);


    ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_OpticalLink, address, 0, 0, &handle);
    if(ret != CAEN_DGTZ_Success) {
        printf("Can't open digitizer\n");
        return 2;
    }
    ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
    printf("\nConnected to CAEN Digitizer Model %s address %d\n", BoardInfo.ModelName, address);
    //printf("\tROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
    //printf("\tAMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);

    ret = CAEN_DGTZ_Reset(handle);                                               // Reset Digitizer
    ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);                                 // Get Board Info
    ret = CAEN_DGTZ_SetRecordLength(handle,RECORD_LENGTH);                       // Set the lenght of each waveform (in samples)
    //ret = CAEN_DGTZ_SetChannelEnableMask(handle,1);                              // Enable channel 0
    ret = CAEN_DGTZ_SetGroupEnableMask(handle, 0b11111111);
    ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,0,0x7fff); //+1.25V = 0x0000, 0V = 0x7FFF, -1.23 = 0xFFFF
    //ret = CAEN_DGTZ_SetGroupTriggerThreshold(handle, 0, 32768);
    ret = CAEN_DGTZ_SetChannelSelfTrigger(handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY,1);  // Set trigger on channel 0 to be ACQ_ONLY
    ret = CAEN_DGTZ_SetSWTriggerMode(handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY);         // Set the behaviour when a SW tirgger arrives
    ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle,5);                                // Set the max number of events to transfer in a sigle readout
    ret = CAEN_DGTZ_SetSAMAcquisitionMode(handle, CAEN_DGTZ_AcquisitionMode_STANDARD);
    ret = CAEN_DGTZ_SetAcquisitionMode(handle,CAEN_DGTZ_SW_CONTROLLED);          // Set the acquisition mode
    if(ret != CAEN_DGTZ_Success) {
        std::cout << "ADC " << address << " initialisation error" << ret << std::endl;
        return 4;
    }

    std::cout << "malloc buffer" << std::endl;
    ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &buffer, &size);

    std::cout << "malloc event" << std::endl;
    ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&Evt);

    if(ret == CAEN_DGTZ_Success) {
        std::cout << "success" << std::endl;
        return OK;
    }

    std::cout << "ADC " << address << " initialisation error" << ret << std::endl;
    return 6;
}

unsigned char arm(unsigned char address){
    std::cout << "arm" << std::endl;
    if(ADCs.size() < MAX_CONNECTIONS){
        for(auto & ADC : ADCs) {
            if(ADC.address == address){
                return 4;
            }
        }

        ADCs.emplace_back(address);
        int error = ADCs.back().init();
        if(error != OK){
            ADCs.pop_back();
            return error;
        }

        ADCs.back().associatedThread = std::thread([&](){
            ADCs.back().run();
        });
        std::cout << "main" <<std::endl;
        return OK;
    }
    return 2;
}

unsigned char disarm(unsigned char address){
    std::cout << "disarm" << std::endl;

    if(ADCs.empty()){
        return OK;
    }

    for(auto & adc : ADCs) {
        if (adc.address == address) {
            adc.stop();
            adc.associatedThread.join();
            std::cout << "thread " << address << " joined" << std::endl;
            return OK;
        }
    }
    return 2;
}

