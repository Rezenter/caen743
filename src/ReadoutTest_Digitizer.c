#include <stdio.h>
#include "CAENDigitizer.h"


#define CAEN_USE_DIGITIZERS
#define IGNORE_DPP_DEPRECATED

#define MAXNB 1 /* Number of connected boards */


int main(int argc, char* argv[])
{
    printf('alive\n');
    /* The following variable is the type returned from most of CAENDigitizer
    CAEN743 functions and is used to check if there was an error in function
    execution. For example:
    ret = CAEN_DGTZ_some_function(some_args);
    if(ret) printf("Some error"); */
	CAEN_DGTZ_ErrorCode ret;

    /* The following variable will be used to get an handler for the digitizer. The
    handler will be used for most of CAENDigitizer functions to identify the board */
	int	handle[MAXNB];

    CAEN_DGTZ_BoardInfo_t BoardInfo;
	CAEN_DGTZ_EventInfo_t eventInfo;
	CAEN_DGTZ_UINT16_EVENT_t *Evt = NULL;
	char *buffer = NULL;
	int MajorNumber;
	int i,b;
	int c = 0, count[MAXNB];
	char * evtptr = NULL;
	uint32_t size,bsize;
	uint32_t numEvents;
	i = sizeof(CAEN_DGTZ_TriggerMode_t);

    for(b=0; b<MAXNB; b++){
        ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_OpticalLink,2,0,0x20000000,&handle[b]);
        if(ret != CAEN_DGTZ_Success) {
            printf("Can't open digitizer\n");
            return 1;
        }
        /* Once we have the handler to the digitizer, we use it to call the other functions */
        ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo);
        printf("\nConnected to CAEN Digitizer Model %s, recognized as board %d\n", BoardInfo.ModelName, b);
        printf("\tROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
        printf("\tAMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);

        ret = CAEN_DGTZ_Reset(handle[b]);                                               /* Reset Digitizer */
	    ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo);                                 /* Get Board Info */
	    ret = CAEN_DGTZ_SetRecordLength(handle[b],4096);                                /* Set the lenght of each waveform (in samples) */
	    ret = CAEN_DGTZ_SetChannelEnableMask(handle[b],1);                              /* Enable channel 0 */
	    ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle[b],0,32768);                  /* Set selfTrigger threshold */
	    ret = CAEN_DGTZ_SetChannelSelfTrigger(handle[b],CAEN_DGTZ_TRGMODE_ACQ_ONLY,1);  /* Set trigger on channel 0 to be ACQ_ONLY */
	    ret = CAEN_DGTZ_SetSWTriggerMode(handle[b],CAEN_DGTZ_TRGMODE_ACQ_ONLY);         /* Set the behaviour when a SW tirgger arrives */
	    ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle[b],3);                                /* Set the max number of events to transfer in a sigle readout */
        ret = CAEN_DGTZ_SetAcquisitionMode(handle[b],CAEN_DGTZ_SW_CONTROLLED);          /* Set the acquisition mode */
        if(ret != CAEN_DGTZ_Success) {
            printf("Errors during Digitizer Configuration.\n");
            return 2;
        }
    }

    printf("Init complete\n");

    // Malloc Readout Buffer. The mallocs must be done AFTER digitizer's configuration!
    ret = CAEN_DGTZ_MallocReadoutBuffer(handle[0],&buffer,&size);

    
    for(b=0; b<MAXNB; b++) {
        // Start Acquisition
        ret = CAEN_DGTZ_SWStartAcquisition(handle[b]);
    }

    printf("Start acquisition\n");

	while(count[0] < 10) {
        for(b=0; b<MAXNB; b++) {
            printf("trigger");
		    ret = CAEN_DGTZ_SendSWtrigger(handle[b]); /* Send a SW Trigger */
            printf(" - ok\n");
		    ret = CAEN_DGTZ_ReadData(handle[b],CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer,&bsize); /* Read the buffer from the digitizer */
            printf("read");
            /* The buffer red from the digitizer is used in the other functions to get the event data
            The following function returns the number of events in the buffer */
		    ret = CAEN_DGTZ_GetNumEvents(handle[b],buffer,bsize,&numEvents);
            printf(" - ok\n");
            printf("events = %d\n", numEvents);
		    count[b] +=numEvents;
            printf("wtf\n", numEvents);
		    for (i=0; i<numEvents; i++) {
                printf("event");
                /* Get the Infos and pointer to the event */
			    //ret = CAEN_DGTZ_GetEventInfo(handle[b],buffer,bsize,i,&eventInfo,&evtptr);

                /* Decode the event to get the data */
			    //ret = CAEN_DGTZ_DecodeEvent(handle[b],evtptr,&Evt);
			    //*************************************
			    // Event Elaboration
			    //*************************************
			    //ret = CAEN_DGTZ_FreeEvent(handle[b],&Evt);
                printf(" - ok\n");
		    }

		    printf("here\n");
        } // end of loop on boards
    } // end of readout loop


    printf("there\n");
    for(b=0; b<MAXNB; b++)
        printf("\nBoard %d: Retrieved %d Events\n",b, count[b]);

    // Free the buffers and close the digitizers
	ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer);
    for(b=0; b<MAXNB; b++) {
        printf("Closing board\n");
        ret = CAEN_DGTZ_CloseDigitizer(handle[b]);
    }
	printf("Press 'Enter' key to exit\n");
	c = getchar();
	return 0;
}

