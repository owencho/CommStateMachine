#include "CommStateMachineIntegration.h"
#include "ButtonSM.h"
#include "Blinky.h"
#include "Event.h"
#include "Button.h"
#include "EventQueue.h"
#include "TimerEvent.h"
#include "TimerEventQueue.h"
#include "Led.h"
#include "CustomAssert.h"
#include "Exception.h"
#include "CException.h"
#include "CExceptionConfig.h"
#include "BlinkyButtonError.h"
#include "TimerEventISR.h"
#include "ButtonAndBlinkyQueue.h"
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
        if(funcPtr == (uintptr_t)usartTransmissionHandler){
            usartTransmissionHandler((UsartPort)expectedArgs[0]);
        }
        else if(funcPtr == (uintptr_t)handleBlinkyStateMachine){
            usartReceiveHandler((UsartPort)expectedArgs[0],(uint16_t)expectedArgs[1]);
        }
        else if(funcPtr == (uintptr_t)fake_mainExecutiveLoop){
            fake_mainExecutiveLoop();
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
    else if (funcPtr == (uintptr_t)fake_eventEnqueue){
        uintptr_t * expectedArgs = (uintptr_t *)info->inArgs;
        uintptr_t * actualArgs = (uintptr_t *)data;
        for(int i=0 ; i<2 ; i++){
            if(actualArgs[i]!=expectedArgs[i]){
                testReportFailure(" expected input parameter [%d] in eventEnqueue is different as actual input parameter for rawButtonEventRequest at sequence %d"
                              ,i,seqIndex);
            }
        }
    }
    else if (funcPtr == (uintptr_t)fake_timerEventRequest){
        uintptr_t * expectedArgs = (uintptr_t *)info->inArgs;
        uintptr_t * actualArgs = (uintptr_t *)data;
        for(int i=0 ; i<3 ; i++){
            if(actualArgs[i]!=expectedArgs[i]){
                testReportFailure(" expected input parameter [%d] in timerEventRequest is different as actual input parameter for rawButtonEventRequest at sequence %d"
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

FakeInfo * call_handleButtonStateMachine(Event * event){
    return createFakeInfo(handleButtonStateMachine,1,1,(uintptr_t)event);
}

FakeInfo * call_handleBlinkyStateMachine(Event * event){
    return createFakeInfo(handleBlinkyStateMachine,1,1,(uintptr_t)event);
}

FakeInfo * simulate_rawButtonEventRequest(Event * event , EventType state){
    return createFakeInfo(fake_rawButtonEventRequest,1,2,(uintptr_t)event,
                          (uintptr_t)state);
}

FakeInfo * simulate_mainExecutiveLoop(){
    return createFakeInfo(fake_mainExecutiveLoop,1,0);
}

FakeInfo * simulate_setTimerTick(int tick){
    return createFakeInfo(fake_setTimerTick,1,1,(uintptr_t)tick);
}

FakeInfo * simulate_buttonInterrupt(){
    return createFakeInfo(fake_buttonInterrupt,1,0);
}

FakeInfo * expect_turnLed(OnOffState state){
    return createFakeInfo(fake_turnLed,0,1,(uintptr_t)state);
}
/*
FakeInfo * expect_rawButtonEventRequest(Event * event , EventType state){
    return createFakeInfo(fake_rawButtonEventRequest,0,2,(uintptr_t)event,
                          (uintptr_t)state);
}
*/
FakeInfo * expect_eventEnqueue(EventQueue * queue,Event * event){
    return createFakeInfo(fake_eventEnqueue,0,2,(uintptr_t)queue,
                          (uintptr_t)event);
}

FakeInfo * expect_timerEventRequest(TimerEventQueue * timerEventQueue,
                                    TimerEvent * event,int expiryPeriod){
    return createFakeInfo(fake_timerEventRequest,0,3,(uintptr_t)timerEventQueue,
                         (uintptr_t)event,(uintptr_t)expiryPeriod);
}
FakeInfo * expect_extiSetInterruptMaskRegister(ExtiRegs *extiLoc , int pin,RequestMasked mode){
    return createFakeInfo(fake_extiSetInterruptMaskRegister,0,3,(uintptr_t)extiLoc,
                          (uintptr_t)pin,(uintptr_t)mode);
}
FakeInfo * expect_readPhysicalButton(){
    return createFakeInfo(fake_readPhysicalButton,0,0);
}


// called when the function is called  the main function

void fake_buttonInterrupt(){
    buttonEventISR();
}

void fake_mainExecutiveLoop(){
    Event * event;
    if(eventDequeue(&buttonBlinkyEventQueue,&event))
        event->stateMachine->callback(event);
}

void fake_setTimerTick(int value){
    for(int i = 0 ; i < value ;i++){
        incTick(&buttonBlinkyTimerEventQueue);
        timerEventISR(&buttonBlinkyEventQueue,&buttonBlinkyTimerEventQueue);
    }
}

void fake_turnLed(OnOffState state, int callNumber){
    uintptr_t * args = malloc(sizeof(uintptr_t));
    args[0] = (uintptr_t)state;
    blinkyButtonSequenceStateMachine((uintptr_t)fake_turnLed,(void*)args);
}
void fake_rawButtonEventRequest(Event * event , EventType state){
    rawButtonEventRequest(event ,state);
}

void fake_eventEnqueue(EventQueue * queue,Event * event, int callNumber){
    uintptr_t * args = malloc(sizeof(uintptr_t)*2);
    args[0] = (uintptr_t)queue;
    args[1] = (uintptr_t)event;
    blinkyButtonSequenceStateMachine((uintptr_t)fake_eventEnqueue,(void*)args);
}

void fake_timerEventRequest(TimerEventQueue * timerEventQueue,TimerEvent * event,int expiryPeriod, int callNumber){
    uintptr_t * args = malloc(sizeof(uintptr_t)*3);
    args[0] = (uintptr_t)timerEventQueue;
    args[1] = (uintptr_t)event;
    args[2] = (uintptr_t)expiryPeriod;
    blinkyButtonSequenceStateMachine((uintptr_t)fake_timerEventRequest,(void*)args);
}
void fake_extiSetInterruptMaskRegister(ExtiRegs *extiLoc , int pin,RequestMasked mode, int callNumber){
    uintptr_t * args = malloc(sizeof(uintptr_t)*3);
    args[0] = (uintptr_t)extiLoc;
    args[1] = (uintptr_t)pin;
    args[2] = (uintptr_t)mode;
    blinkyButtonSequenceStateMachine((uintptr_t)fake_extiSetInterruptMaskRegister,(void*)args);
}
void fake_doNothing(ExtiRegs *extiLoc , int pin,RequestMasked mode, int callNumber){
}
int fake_readPhysicalButton(int callNumber){
    return blinkyButtonSequenceStateMachine((uintptr_t)fake_readPhysicalButton,NULL);
}
