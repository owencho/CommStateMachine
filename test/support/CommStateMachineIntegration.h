#ifndef COMMSTATEMACHINEINTEGRATION_H
#define COMMSTATEMACHINEINTEGRATION_H
#include "Event.h"
#include "EventQueue.h"
#include "UsartHardware.h"

typedef struct FakeInfo FakeInfo;
struct FakeInfo {
		uintptr_t  funcPtr;
    	int shouldCallFunc;
		void * inArgs;
};

typedef struct FakeSequence FakeSequence;
struct FakeSequence{
		FakeInfo *fakeInfo;
		uintptr_t retVal;
};

typedef void (*Handler)(void*);
void freeFakeInfo(FakeInfo * info);
void freeExpectedArgs(void * data);
void fakeCommStateMachineSequence(FakeSequence sequence[]);
int triggerSequence();
uintptr_t CommStateMachineIntegrationSeqeunce(uintptr_t funcPtr , void *data);
FakeInfo * createFakeInfo(void* funcName,int shouldCallFunc,int argCount,...);
FakeInfo * simulate_usartIrqHandler(UsartPort port);
FakeInfo * simulate_mainExecutiveLoop();
//expect
FakeInfo * expect_usartReceive(UsartRegs * usart);
FakeInfo * expect_usartSend(UsartRegs * usart,int data);
//fake
void fake_mainExecutiveLoop();
void fake_usartIrqHandler(UsartPort port, int callNumber);
void fake_usartSend(UsartRegs * usart,int data, int callNumber);
int fake_usartReceive(UsartRegs * usart,int callNumber);
void fake_setTimerTick(int value);
//void fake_doNothing(ExtiRegs *extiLoc , int pin,RequestMasked mode, int callNumber);
#endif // COMMSTATEMACHINEINTEGRATION_H
