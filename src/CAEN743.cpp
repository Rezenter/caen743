//
// Created by user on 6/18/2020.
//

//debug
#include <iostream>
//debug

#include "CAEN743.h"

void CAEN743::run() {
    std::cout << "Task Start" << std::endl;
    while(!stopRequested()){
        ret = CAEN_DGTZ_SendSWtrigger(handle); /* Send a SW Trigger */

        ret = CAEN_DGTZ_ReadData(handle,CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer,&bsize); /* Read the buffer from the digitizer */

        /* The buffer red from the digitizer is used in the other functions to get the event data
        The following function returns the number of events in the buffer */
        ret = CAEN_DGTZ_GetNumEvents(handle,buffer,bsize,&numEvents);
        printf("events = %d\n", numEvents);
        count +=numEvents;

        try{
            MDSplus::Tree tree("test_tree", 1, "NEW");
            MDSplus::TreeNode *outTopNode = tree.getNode("\\TOP");
            MDSplus::TreeNode *newNode;
            char fullname[14];
            sprintf(fullname, ".%s", "itsALIVE");
            try {
                newNode = outTopNode->addNode(fullname, (const char *) TreeUSAGE_TEXT);
            } catch (MDSplus::MdsException &exc)
            {
                std::cout << "Error adding node: " << exc.what() << std::endl;
                exit(0);
            }

            tree.write();
            delete outTopNode;
        }catch(MDSplus::MdsException &exc){
//methode MdsException::what() return a description of the exception
            std::cout << "Cannot open tree " << "tset_tree"  << " shot " << 1 << ": " << exc.what() << std::endl;
            exit(0);
        }

        for (event_index = 0; event_index < numEvents; event_index++) {
            /* Get the Infos and pointer to the event */
            //ret = CAEN_DGTZ_GetEventInfo(handle[b],buffer,bsize,i,&eventInfo,&evtptr);

            /* Decode the event to get the data */
            //ret = CAEN_DGTZ_DecodeEvent(handle[b],evtptr,&Evt);
            //*************************************
            // Event Elaboration
            //*************************************
            //ret = CAEN_DGTZ_FreeEvent(handle[b],&Evt);
        }
    }
    std::cout << "Task End" << std::endl;
}

int CAEN743::init() {
    //i = sizeof(CAEN_DGTZ_TriggerMode_t);

    ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_OpticalLink, this->address, 0,
                                  this->address, &handle); //works only for 0
    if(ret != CAEN_DGTZ_Success) {
        printf("Can't open digitizer\n");
        return 2;
    }
    ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
    printf("\nConnected to CAEN Digitizer Model %s\n", BoardInfo.ModelName);
    printf("\tROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
    printf("\tAMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);

    ret = CAEN_DGTZ_Reset(handle);                                               /* Reset Digitizer */
    ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);                                 /* Get Board Info */
    ret = CAEN_DGTZ_SetRecordLength(handle,RECORD_LENGTH);                                /* Set the lenght of each waveform (in samples) */
    ret = CAEN_DGTZ_SetChannelEnableMask(handle,1);                              /* Enable channel 0 */
    ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle,0,32768);                  /* Set selfTrigger threshold */
    ret = CAEN_DGTZ_SetChannelSelfTrigger(handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY,1);  /* Set trigger on channel 0 to be ACQ_ONLY */
    ret = CAEN_DGTZ_SetSWTriggerMode(handle,CAEN_DGTZ_TRGMODE_ACQ_ONLY);         /* Set the behaviour when a SW tirgger arrives */
    ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle,5);                                /* Set the max number of events to transfer in a sigle readout */
    ret = CAEN_DGTZ_SetAcquisitionMode(handle,CAEN_DGTZ_SW_CONTROLLED);          /* Set the acquisition mode */
    if(ret != CAEN_DGTZ_Success) {
        return 4;
    }

    ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &buffer, &size);

    ret = CAEN_DGTZ_SWStartAcquisition(handle);
    if(ret == CAEN_DGTZ_Success) {
        return OK;
    }
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

    for(auto & adc : ADCs) {
        if (adc.address == address) {
            adc.stop();
            adc.associatedThread.join();
            return OK;
        }
    }
    return 2;
}

