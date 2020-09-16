#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include "unity.h"
#include "unity_internals.h"
#include "Exception.h"
#include "UsartHardware.h"
#include "UsartDriver.h"
#include "mock_Irq.h"
#include "EventQueue.h"
#include "mock_Rcc.h"
#include "mock_Gpio.h"
#include "mock_Nvic.h"
#include "Event.h"
#include "EventCompare.h"
#include "CustomAssert.h"
#include "List.h"
#include "Crc.h"
#include "CExceptionConfig.h"
#include "ListItemCompare.h"
#include "TimerEventQueue.h"
#include "TimerEventISR.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SM_Common.h"
#include "FakeIRQ.h"
#include "CommStateMachine.h"
#include "CommEventQueue.h"
#include "Common.h"
#include "ExceptionTestSupport.h"
#include "mock_Usart.h"
#include "CommStateMachineIntegration.h"
#include "CmdCompareForAVL.h"
#include "Avl.h"
#include "Node.h"
#include "Balance.h"
#include "Rotate.h"
#include "AvlError.h"
#include "Common.h"
CEXCEPTION_T ex;
extern UsartInfo usartInfo[];
extern UsartDriverInfo usartDriverInfo[];
void clearEventQueue(EventQueue* queue);
void setUp(void){}
void tearDown(void){
    clearEventQueue(&evtQueue);
	clearEventQueue(&sysQueue);
}


void clearEventQueue(EventQueue* queue){
    queue->head = NULL;
    queue->tail = NULL;
    queue->count = 0;
    queue->previous = NULL;
    queue->current = NULL;
}

#define RETURN(x) ((uintptr_t)(x))
#define NOTHING ((uintptr_t)(NULL))
#define NONE ((uintptr_t)(NULL))

void initFakeSequence(FakeSequence sequence[]){
    usartSend_StubWithCallback(fake_usartSend);
    usartReceive_StubWithCallback(fake_usartReceive);
    enableIRQ_StubWithCallback(fake_enableIRQ);
    disableIRQ_StubWithCallback(fake_disableIRQ);
    fakeCommStateMachineSequence(sequence);
}


void test_receive_packet_to_check_mainExecutiveLoop(void){
	UsartDriverInfo * info =&usartDriverInfo[LED_CONTROLLER];
	FakeSequence sequence[] ={
		//StartDelimiter
		//rawButtonEventRequest from buttonStartStateMachine
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x7E)},
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x81)},
		//RX address and Tx address
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(USART_ADDRESS)},
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x23)},
		//length
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x1)},
		//payload
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x55)},
		{simulate_mainExecutiveLoop(),NONE},
		{NULL,RETURN(NOTHING)}
	};
	initFakeSequence(sequence);
	initUsartHardwareInfo(LED_CONTROLLER,usart1);
	initUsartHardwareInfo(MAIN_CONTROLLER,uart5);
	usartDriverInit(LED_CONTROLLER);
	Try{
	    while(triggerSequence()){

	    }
	    fakeCheckIRQ(__LINE__);
	}
	Catch(ex){
	    dumpException(ex);
	    TEST_FAIL_MESSAGE("Do not expect any exception to be thrown");
	}
	TEST_ASSERT_EQUAL(RX_RECEIVE_PAYLOAD_MALLOC_BUFFER,info->rxState);
}

void test_receive_packet_to_check_malloc_on_CRC(void){
	UsartDriverInfo * info =&usartDriverInfo[LED_CONTROLLER];
	FakeSequence sequence[] ={
		//StartDelimiter
		//rawButtonEventRequest from buttonStartStateMachine
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x7E)},
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x81)},
		//RX address and Tx address
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(USART_ADDRESS)},
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x23)},
		//length
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x1)},
		//payload
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x55)},
		//CRC
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0xA)},
		{simulate_mainExecutiveLoop(),NONE},
		{NULL,RETURN(NOTHING)}
	};
	initFakeSequence(sequence);
	initUsartHardwareInfo(LED_CONTROLLER,usart1);
	initUsartHardwareInfo(MAIN_CONTROLLER,uart5);
	usartDriverInit(LED_CONTROLLER);
	Try{
	    while(triggerSequence()){

	    }
	    fakeCheckIRQ(__LINE__);
	}
	Catch(ex){
	    dumpException(ex);
	    TEST_FAIL_MESSAGE("Do not expect any exception to be thrown");
	}
	TEST_ASSERT_EQUAL(RX_WAIT_CRC16_MALLOC_BUFFER,info->rxState);
}

void test_packet_start_on_middle_of_malloc(void){
	UsartDriverInfo * info =&usartDriverInfo[LED_CONTROLLER];
	FakeSequence sequence[] ={
		//StartDelimiter
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x7E)},
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x81)},
		//RX address and Tx address
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(USART_ADDRESS)},
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x23)},
		//length
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x1)},
		//payload
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x55)},
		{simulate_mainExecutiveLoop(),NONE},
		//start delimter received again
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x7E)},
		{simulate_usartIrqHandler(LED_CONTROLLER),RETURN(NOTHING)},
		{expect_usartReceive(usart1),RETURN(0x81)},
		//run free mem stateMachine
		{simulate_mainExecutiveLoop(),NONE},
		{NULL,RETURN(NOTHING)}
	};
	initFakeSequence(sequence);
	initUsartHardwareInfo(LED_CONTROLLER,usart1);
	initUsartHardwareInfo(MAIN_CONTROLLER,uart5);
	usartDriverInit(LED_CONTROLLER);
	Try{
	    while(triggerSequence()){

	    }
	    fakeCheckIRQ(__LINE__);
	}
	Catch(ex){
	    dumpException(ex);
	    TEST_FAIL_MESSAGE("Do not expect any exception to be thrown");
	}
	TEST_ASSERT_EQUAL(RX_IDLE,info->rxState);
}

/*
// this test is to test BUTTON RELEASE EVENT FOR LED_ON to BLINK_ON
// when the button was pressed
void test_handleBlinkyStateMachine_from_LED_ON_to_BLINK_ON(void){
    setUpBlinkySM(&blinkySM, (Callback)handleBlinkyStateMachine,LED_ON,0,&buttonSM);
    setUpButtonSM(&buttonSM, (Callback)handleButtonStateMachine,BUTTON_RELEASED,RELEASE);
    setUpEvent(&evt,NULL,BUTTON_RELEASED_EVENT,(GenericStateMachine*)&blinkySM,NULL);
    setUpEvent(&evt2,NULL,TIMEOUT_EVENT,(GenericStateMachine*)&buttonSM,NULL);
    FakeSequence sequence[] ={
      //simulate the rawButtonEventRequest to press
      {call_handleBlinkyStateMachine(&evt)},
      {simulate_rawButtonEventRequest(&evt2,PRESS)},
      //rawButtonEventRequest is requested on handleButtonStateMachine
      {expect_readPhysicalButton(),RETURN(RELEASE)},
      // mockup where the event just
      // now trying to simulate the buttonInterrupt
      {simulate_buttonInterrupt(),NONE},
      {expect_readPhysicalButton(),RETURN(PRESS)},
      // where at this point the event was queued into the normal queue
      // to call the button stateMachine
      {simulate_mainExecutiveLoop(),NONE},
      // at this point the timerEventRequest was called to queue into the timerEventQueue
      //set timerTick (this will call TimerEventISR)
      {simulate_setTimerTick(101),NONE},
      //after event is dequeue from the timerEventQueue
      //expected button event is called again for BUTTON_PRESSED_DEBOUNCING with TIMEOUT_EVENT
      {simulate_mainExecutiveLoop(),NONE},
      //button Event is enqueued into the eventQueue and rawButtonEventRequest is called
      {expect_readPhysicalButton(),RETURN(PRESS)},
      // and now the current state of Button stateMachine is BUTTON_PRESSED
      // main executive loop is run again
      {simulate_mainExecutiveLoop(),NONE},
      // blinkyEvent was dequeued and handleBlinkyStateMachine was called
      // the led still turn ON as it goes from BLINK_ON to LED_ON
      {NULL,RETURN(NOTHING)}
      };
      initFakeSequence(sequence);
      extiSetInterruptMaskRegister_StubWithCallback(fake_doNothing);
      Try{
          while(triggerSequence()){

          }
          fakeCheckIRQ(__LINE__);
      }
      Catch(ex){
          dumpException(ex);
          TEST_FAIL_MESSAGE("Do not expect any exception to be thrown");
      }
}
*/
/*
// this test is to test BUTTON PRESSED EVENT FOR BLINK_ON to LED_OFF
// when the button was pressed
void test_handleBlinkyStateMachine_from_BLINK_ON_to_LED_OFF(void){
  setUpBlinkySM(&blinkySM, (Callback)handleBlinkyStateMachine,BLINK_ON,0,&buttonSM);
  setUpButtonSM(&buttonSM, (Callback)handleButtonStateMachine,BUTTON_RELEASED,RELEASE);
  setUpEvent(&evt,NULL,BUTTON_RELEASED_EVENT,(GenericStateMachine*)&blinkySM,NULL);
  setUpEvent(&evt2,NULL,TIMEOUT_EVENT,(GenericStateMachine*)&buttonSM,NULL);
  FakeSequence sequence[] ={
    //simulate the rawButtonEventRequest to press
    {call_handleBlinkyStateMachine(&evt)},
    {simulate_rawButtonEventRequest(&evt2,PRESS)},
    //rawButtonEventRequest is requested on handleButtonStateMachine
    {expect_readPhysicalButton(),RETURN(RELEASE)},
    // mockup where the event just
    // now trying to simulate the buttonInterrupt
    {simulate_buttonInterrupt(),NONE},
    {expect_readPhysicalButton(),RETURN(PRESS)},
    // where at this point the event was queued into the normal queue
    // to call the button stateMachine
    {simulate_mainExecutiveLoop(),NONE},
    // at this point the timerEventRequest was called to queue into the timerEventQueue
    //set timerTick (this will call TimerEventISR)
    {simulate_setTimerTick(101),NONE},
    //after event is dequeue from the timerEventQueue
    //expected button event is called again for BUTTON_PRESSED_DEBOUNCING with TIMEOUT_EVENT
    {simulate_mainExecutiveLoop(),NONE},
    //button Event is enqueued into the eventQueue and rawButtonEventRequest is called
    {expect_readPhysicalButton(),RETURN(PRESS)},
    // and now the current state of Button stateMachine is BUTTON_PRESSED
    // main executive loop is run again
    {simulate_mainExecutiveLoop(),NONE},
    // blinkyEvent was dequeued and handleBlinkyStateMachine was called
    // the led will turn OFF as it goes from BLINK_ON to LED_OFF
    {expect_turnLed(OFF),RETURN(NOTHING)},
    {NULL,RETURN(NOTHING)}
    };
    initFakeSequence(sequence);
    extiSetInterruptMaskRegister_StubWithCallback(fake_doNothing);
    Try{
        while(triggerSequence()){

        }
        fakeCheckIRQ(__LINE__);
    }
    Catch(ex){
        dumpException(ex);
        TEST_FAIL_MESSAGE("Do not expect any exception to be thrown");
    }
}
*/
/*
// this test is to test TIMEOUT_EVENT FOR BLINK_ON to BLINK_OFF
void test_handleBlinkyStateMachine_from_BLINK_ON_to_BLINK_OFF(void){
  setUpBlinkySM(&blinkySM, (Callback)handleBlinkyStateMachine,LED_ON,1,&buttonSM);
  setUpButtonSM(&buttonSM, (Callback)handleButtonStateMachine,BUTTON_RELEASED,RELEASE);
  setUpEvent(&evt,NULL,BUTTON_PRESSED_EVENT,(GenericStateMachine*)&blinkySM,NULL);
  setUpEvent(&evt2,NULL,TIMEOUT_EVENT,(GenericStateMachine*)&buttonSM,NULL);
  FakeSequence sequence[] ={
    //simulate the rawButtonEventRequest to press
    {call_handleBlinkyStateMachine(&evt)},
    // simulate the main executive loop see is there any event to be called
    {simulate_mainExecutiveLoop(),NONE},
    // expected to have no event as timer is not expired yet
    {simulate_setTimerTick(30),NONE},
    // event in timerEventQueue are still remain inside as it is not expired yet
    {simulate_mainExecutiveLoop(),NONE},
    // we set timerTick again to expire the timerEvent
    {simulate_setTimerTick(100),NONE},
    // event in timerEventQueue are dequeued and it is queued into main EventQueue
    {simulate_mainExecutiveLoop(),NONE},
    // handleBlinkyStateMachine will be called in this Sequence
    // timerEventRequest is called again in the stateMachine as the stateMachine is transitioning
    // into blink_OFF where it also required timer to calculate the clock.
    // and there we expect the Led to turn off as it is BlinkOff
    {expect_turnLed(OFF),RETURN(NOTHING)},
    {NULL,RETURN(NOTHING)}
    };
    initFakeSequence(sequence);
    extiSetInterruptMaskRegister_StubWithCallback(fake_doNothing);
    Try{
        while(triggerSequence()){

        }
        fakeCheckIRQ(__LINE__);
    }
    Catch(ex){
        dumpException(ex);
        TEST_FAIL_MESSAGE("Do not expect any exception to be thrown");
    }
}
*/
