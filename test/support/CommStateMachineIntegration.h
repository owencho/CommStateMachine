#ifndef COMMSTATEMACHINEINTEGRATION_H
#define COMMSTATEMACHINEINTEGRATION_H
#include "Event.h"
#include "EventQueue.h"
#include "TimerEventQueue.h"
#include "TimerEvent.h"
#include "Led.h"
#include "Exti.h"
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
void fakeButtonBlinkySequence(FakeSequence sequence[]);
uintptr_t blinkyButtonSequenceStateMachine(uintptr_t funcPtr , void *data);
FakeInfo * createFakeInfo(void* funcName,int shouldCallFunc,int argCount,...);
FakeInfo * call_handleButtonStateMachine(Event * event);
FakeInfo * call_handleBlinkyStateMachine(Event * event);
FakeInfo * simulate_mainExecutiveLoop();
FakeInfo * simulate_setTimerTick(int tick);
FakeInfo * simulate_buttonInterrupt();
FakeInfo * simulate_rawButtonEventRequest(Event * event , EventType state);
FakeInfo * expect_turnLed(OnOffState state);
//FakeInfo * expect_rawButtonEventRequest(Event * event , EventType state);
FakeInfo * expect_eventEnqueue(EventQueue * queue,Event * event);
FakeInfo * expect_timerEventRequest(TimerEventQueue * timerEventQueue,TimerEvent * event,int expiryPeriod);
FakeInfo * expect_readPhysicalButton();
FakeInfo * expect_extiSetInterruptMaskRegister(ExtiRegs *extiLoc , int pin,RequestMasked mode);
int triggerSequence();
void fake_mainExecutiveLoop();
void fake_buttonInterrupt();
void fake_setTimerTick(int value);
void fake_rawButtonEventRequest(Event * event , EventType state);
//void fake_rawButtonEventRequest(Event * event , EventType state, int callNumber);
void fake_turnLed(OnOffState state, int callNumber);
void fake_eventEnqueue(EventQueue * queue,Event * event, int callNumber);
void fake_timerEventRequest(TimerEventQueue * timerEventQueue,TimerEvent * event,int expiryPeriod, int callNumber);
void fake_extiSetInterruptMaskRegister(ExtiRegs *extiLoc , int pin,RequestMasked mode, int callNumber);
int fake_readPhysicalButton(int callNumber);
void fake_doNothing(ExtiRegs *extiLoc , int pin,RequestMasked mode, int callNumber);
#endif // COMMSTATEMACHINEINTEGRATION_H
