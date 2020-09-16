#include "unity.h"
#include "Exception.h"
#include "UsartDriver.h"
#include "mock_UsartHardware.h"
#include "mock_Irq.h"
#include "mock_Gpio.h"
#include "mock_Rcc.h"
#include "EventQueue.h"
#include "Event.h"
#include "EventCompare.h"
#include "List.h"
#include "ListItemCompare.h"
#include "TimerEventQueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SM_Common.h"
#include "Crc.h"
#include "FakeIRQ.h"
#include "CommStateMachine.h"
#include "CommEventQueue.h"
#include "CmdCompareForAVL.h"
#include "Avl.h"
#include "Node.h"
#include "Balance.h"
#include "CmdNode.h"
#include "Rotate.h"
#include "AvlError.h"
#include "Common.h"
extern CmdNode* rootCmdNode;
extern UsartDriverInfo usartDriverInfo[];
UsartEvent txEvent;
UsartEvent rxEvent;
TimerEvent timerEvent;
UsartEvent * rxEvent2;
char * rxPacket;
CmdNode node;
GenericStateMachine fakeSM;
void setUp(void){}
void tearDown(void){}


void test_removeTimerEventFromQueue(){
	disableIRQ_StubWithCallback(fake_disableIRQ);
	enableIRQ_StubWithCallback(fake_enableIRQ);
	eventEnqueue(&evtQueue,(Event*)&rxEvent);
	TEST_ASSERT_EQUAL(&rxEvent,evtQueue.head);
	removeTimerEventFromQueue((Event*)&rxEvent);
	fakeCheckIRQ(__LINE__);
	TEST_ASSERT_NULL(evtQueue.current);
	TEST_ASSERT_NULL(evtQueue.previous);
	TEST_ASSERT_NULL(evtQueue.head);
	TEST_ASSERT_NULL(evtQueue.tail);
	TEST_ASSERT_EQUAL(0,evtQueue.count);
}

void test_removeTimerEventFromQueue_inTimerEVENTQUEUE(){
	disableIRQ_StubWithCallback(fake_disableIRQ);
	enableIRQ_StubWithCallback(fake_enableIRQ);
	timerEvent.data = (void*)&rxEvent;
	timerEventRequest (&timerQueue,&timerEvent,1);
	removeTimerEventFromQueue((Event*)&rxEvent);
	fakeCheckIRQ(__LINE__);
	TEST_ASSERT_NULL(timerQueue.current);
	TEST_ASSERT_NULL(timerQueue.previous);
	TEST_ASSERT_NULL(timerQueue.head);
	TEST_ASSERT_NULL(timerQueue.tail);
	TEST_ASSERT_EQUAL(0,timerQueue.count);
}
