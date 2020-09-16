#include "CommStateMachineIntegration.h"
#include "Usart.h"
#include "CustomAssert.h"
#include "ExceptionTestSupport.h"
#include "Exception.h"
#include "CException.h"
#include "CExceptionConfig.h"
#include "CommEventQueue.h"
#include <stdlib.h>
#include <stdarg.h>

FakeSequence * seqPtr;
int nextSeqIndex = 0 ;
int seqIndex = 0;

void fakeCommStateMachineSequence(FakeSequence sequence[]){
    seqPtr = sequence;
    seqIndex = 0 ;
    nextSeqIndex = 0 ;
}
int triggerSequence(){
    FakeSequence *seq = &seqPtr[nextSeqIndex];
    FakeInfo * info = seq->fakeInfo;
    if(seq->fakeInfo == NULL){
        return 0;
    }
    else if(info->shouldCallFunc){
        return CommStateMachineIntegrationSeqeunce(info->funcPtr , NULL);
    }
    else{
        testReportFailure("Sequence stop prematurely at seq %d",nextSeqIndex);
    }

}

uintptr_t CommStateMachineIntegrationSeqeunce(uintptr_t funcPtr , void *data){
    FakeSequence *seq = &seqPtr[nextSeqIndex];
    FakeInfo * info = seq->fakeInfo;

    seqIndex=nextSeqIndex++;
    if(info->shouldCallFunc){
        uintptr_t * expectedArgs = (uintptr_t *)seq->fakeInfo->inArgs;
        if(funcPtr == (uintptr_t)usartTransmitHardwareHandler){
            usartTransmitHardwareHandler((UsartPort)expectedArgs[0]);
        }
        else if(funcPtr == (uintptr_t)usartReceiveHardwareHandler){
            usartReceiveHardwareHandler((UsartPort)expectedArgs[0],(uint16_t)expectedArgs[1]);
        }
        else if(funcPtr == (uintptr_t)fake_mainExecutiveLoop){
            fake_mainExecutiveLoop();
        }
		else if(funcPtr == (uintptr_t)fake_usartIrqHandler){
			usartIrqHandler((UsartPort)expectedArgs[0]);
		}
        else if(funcPtr == (uintptr_t)fake_setTimerTick){
            fake_setTimerTick((int)expectedArgs[0]);
        }
        else{
            testReportFailure("should call func is triggered at seq %d but the funcPtr is not to be called"
                              ,seqIndex);
        }
        return 1 ;
    }

    if(info->funcPtr != funcPtr){
        testReportFailure("expected function called is different as actual function at sequence %d"
                         ,seqIndex);
    }
    else if (funcPtr == (uintptr_t)fake_usartSend){
        uintptr_t * expectedArgs = (uintptr_t *)info->inArgs;
        uintptr_t * actualArgs = (uintptr_t *)data;
        for(int i=0 ; i<2 ; i++){
            if(actualArgs[i]!=expectedArgs[i]){
                testReportFailure(" expected input parameter [%d] in usartSend is different as actual input parameter at sequence %d"
                              ,i,seqIndex);
            }
        }
    }
    else if (funcPtr == (uintptr_t)fake_usartReceive){
        uintptr_t * expectedArgs = (uintptr_t *)info->inArgs;
        uintptr_t * actualArgs = (uintptr_t *)data;
        for(int i=0 ; i<1 ; i++){
            if(actualArgs[i]!=expectedArgs[i]){
                testReportFailure(" expected input parameter [%d] in usartReceive is different as actual input parameter at sequence %d"
                              ,i,seqIndex);
            }
        }
    }
    freeExpectedArgs(data);
    freeFakeInfo(info);
    return seq->retVal;
}


void freeFakeInfo(FakeInfo * info){
    if(info->inArgs){
        free(info->inArgs);
    }
    free(info);
}

void freeExpectedArgs(void * data){
    if(data){
        free(data);
    }
}

// this is what we expect in the test
FakeInfo * createFakeInfo(void* funcName,int shouldCallFunc,int argCount,...){
    FakeInfo * info = (FakeInfo *)malloc(sizeof(FakeInfo));
    uintptr_t *inArgs = malloc(sizeof(uintptr_t)*argCount);
    va_list args;
    va_start(args, argCount);

    for(int i=0 ; i < argCount ; i++){
        inArgs[i]=va_arg(args,uintptr_t);
    }

    va_end(args);
    info->funcPtr = (uintptr_t)funcName;
    info->shouldCallFunc = shouldCallFunc ;
    info->inArgs = inArgs;
    return info;
}

FakeInfo * simulate_usartIrqHandler(UsartPort port){
    return createFakeInfo(fake_usartIrqHandler,1,1,(uintptr_t)port);
}

FakeInfo * simulate_mainExecutiveLoop(){
    return createFakeInfo(fake_mainExecutiveLoop,1,0);
}

FakeInfo * expect_usartSend(UsartRegs * usart,int data){
    return createFakeInfo(fake_usartSend,0,2,(uintptr_t)usart,
                          (uintptr_t)data);
}

FakeInfo * expect_usartReceive(UsartRegs * usart){
    return createFakeInfo(fake_usartReceive,0,1,(uintptr_t)usart);
}

// called when the function is called  the main function
void fake_mainExecutiveLoop(){
    Event * event;
	if(eventDequeue(&sysQueue,&event))
		event->stateMachine->callback(event);
	else if(eventDequeue(&evtQueue,&event))
        event->stateMachine->callback(event);
}

void fake_usartIrqHandler(UsartPort port, int callNumber){
    uintptr_t * args = malloc(sizeof(uintptr_t)*2);
    args[0] = (uintptr_t)port;
    CommStateMachineIntegrationSeqeunce((uintptr_t)fake_usartIrqHandler,(void*)args);
}

void fake_usartSend(UsartRegs * usart,int data, int callNumber){
    uintptr_t * args = malloc(sizeof(uintptr_t)*2);
    args[0] = (uintptr_t)usart;
    args[1] = (uintptr_t)data;
    CommStateMachineIntegrationSeqeunce((uintptr_t)fake_usartSend,(void*)args);
}

int fake_usartReceive(UsartRegs * usart,int callNumber){
    uintptr_t * args = malloc(sizeof(uintptr_t)*1);
    args[0] = (uintptr_t)usart;
    return CommStateMachineIntegrationSeqeunce((uintptr_t)fake_usartReceive,(void*)args);
}

void fake_setTimerTick(int value){
    for(int i = 0 ; i < value ;i++){
        incTick(&timerQueue);
        timerEventISR(&evtQueue,&timerQueue);
    }
}

/*
void fake_doNothing(ExtiRegs *extiLoc , int pin,RequestMasked mode, int callNumber){
}
*/
