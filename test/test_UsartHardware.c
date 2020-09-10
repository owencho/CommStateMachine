#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include "unity.h"
#include "unity_internals.h"
#include "UsartHardware.h"
#include "mock_UsartDriver.h"
#include "mock_Irq.h"
#include "mock_EventQueue.h"
#include "mock_Rcc.h"
#include "mock_Gpio.h"
#include "mock_Nvic.h"
#include "Event.h"
#include "EventCompare.h"
#include "List.h"
#include "Crc.h"
#include "ListItemCompare.h"
#include "TimerEventQueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SM_Common.h"
#include "FakeIRQ.h"
#include "CommStateMachine.h"
#include "CommEventQueue.h"
#include "Common.h"
#include "mock_Usart.h"
extern UsartInfo usartInfo[];
extern UsartDriverInfo usartDriverInfo[] ;
void setUp(void){}

void tearDown(void){}

void initFakeUsartDriverAndHardware(){
    memset(&usartInfo[1],0,sizeof(UsartInfo));
    initUsartHardwareInfo(LED_CONTROLLER,usart1);
}

void expectEndOfUsartTx(UsartPort port){
    UsartInfo * fakeInfo = &usartInfo[LED_CONTROLLER];
    usartDisableInterrupt_Expect(fakeInfo->usart,TRANS_COMPLETE);
    usartDisableTransmission_Expect(fakeInfo->usart);
    usartEnableReceiver_Expect(fakeInfo->usart);
}
void test_usartTransmitHardwareHandler_from_idle_to_send_normal_byte(void){
    int transmitByte;
    initFakeUsartDriverAndHardware();
    UsartInfo * fakeInfo = &usartInfo[LED_CONTROLLER];
    //IDLE state
    TEST_ASSERT_EQUAL(fakeInfo->hwTxState,HW_TX_IDLE);
    TEST_ASSERT_EQUAL(usartTransmitHardwareHandler(LED_CONTROLLER),0x7E);
    //Send Delimiter State
    TEST_ASSERT_EQUAL(fakeInfo->hwTxState,HW_TX_SEND_DELIMITER);
    TEST_ASSERT_EQUAL(usartTransmitHardwareHandler(LED_CONTROLLER),0x81);
    //Send Byte State
    TEST_ASSERT_EQUAL(fakeInfo->hwTxState,HW_TX_SEND_BYTE);
    usartTransmissionHandler_ExpectAndReturn(LED_CONTROLLER,0x11);
    TEST_ASSERT_EQUAL(usartTransmitHardwareHandler(LED_CONTROLLER),0x11);
    //Send Byte State
    TEST_ASSERT_EQUAL(fakeInfo->hwTxState,HW_TX_SEND_BYTE);
    usartTransmissionHandler_ExpectAndReturn(LED_CONTROLLER,0x20);
    TEST_ASSERT_EQUAL(usartTransmitHardwareHandler(LED_CONTROLLER),0x20);
}

void test_usartTransmitHardwareHandler_from_send_to_0x7E_send(void){
    UsartInfo * fakeInfo = &usartInfo[LED_CONTROLLER];
    initUsartHardwareInfo(LED_CONTROLLER,usart1);
    //fake hardware Transmission on send byte state
    fakeInfo->hwTxState = HW_TX_SEND_BYTE;
    //Send 0x7E Byte State
    TEST_ASSERT_EQUAL(fakeInfo->hwTxState,HW_TX_SEND_BYTE);
    usartTransmissionHandler_ExpectAndReturn(LED_CONTROLLER,0x7E);
    TEST_ASSERT_EQUAL(usartTransmitHardwareHandler(LED_CONTROLLER),0x7E);
    //Send 0xE7 Byte State
    TEST_ASSERT_EQUAL(fakeInfo->hwTxState,HW_TX_SEND_7E_BYTE);
    TEST_ASSERT_EQUAL(usartTransmitHardwareHandler(LED_CONTROLLER),0xE7);
    //Send next Byte State
    TEST_ASSERT_EQUAL(fakeInfo->hwTxState,HW_TX_SEND_BYTE);
    usartTransmissionHandler_ExpectAndReturn(LED_CONTROLLER,0x12);
    TEST_ASSERT_EQUAL(usartTransmitHardwareHandler(LED_CONTROLLER),0x12);
}

void test_usartTransmitHardwareHandler_from_send_to_IDLE(void){
    UsartInfo * fakeInfo = &usartInfo[LED_CONTROLLER];
    initUsartHardwareInfo(LED_CONTROLLER,usart1);
    //fake hardware Transmission on send byte state
    fakeInfo->hwTxState = HW_TX_SEND_BYTE;
    //Send 0x7E Byte State
    TEST_ASSERT_EQUAL(fakeInfo->hwTxState,HW_TX_SEND_BYTE);
    usartTransmissionHandler_ExpectAndReturn(LED_CONTROLLER,0x12);
    expectEndOfUsartTx(LED_CONTROLLER);
    setHardwareTxLastByte(LED_CONTROLLER);
    TEST_ASSERT_EQUAL(usartTransmitHardwareHandler(LED_CONTROLLER),0x12);
    //IDLE state
    TEST_ASSERT_EQUAL(fakeInfo->hwTxState,HW_TX_IDLE);
    TEST_ASSERT_EQUAL(usartTransmitHardwareHandler(LED_CONTROLLER),0x7E);
}

void test_usartTransmitHardwareHandler_from_send_0x7E_to_IDLE(void){
    UsartInfo * fakeInfo = &usartInfo[LED_CONTROLLER];
    initUsartHardwareInfo(LED_CONTROLLER,usart1);
    //fake hardware Transmission on send byte state
    fakeInfo->hwTxState = HW_TX_SEND_7E_BYTE;
    //Send 0x7E Byte State
    TEST_ASSERT_EQUAL(fakeInfo->hwTxState,HW_TX_SEND_7E_BYTE);
    expectEndOfUsartTx(LED_CONTROLLER);
    setHardwareTxLastByte(LED_CONTROLLER);
    TEST_ASSERT_EQUAL(usartTransmitHardwareHandler(LED_CONTROLLER),0xE7);
    //IDLE state
    TEST_ASSERT_EQUAL(fakeInfo->hwTxState,HW_TX_IDLE);
    TEST_ASSERT_EQUAL(usartTransmitHardwareHandler(LED_CONTROLLER),0x7E);
}

void test_usartReceiveHardwareHandler_from_IDLE_to_RECEIVED_DELIMITER(void){
    int transmitByte;
    initUsartHardwareInfo(LED_CONTROLLER,usart1);
    UsartInfo * fakeInfo = &usartInfo[LED_CONTROLLER];
    //IDLE state
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_IDLE);
    usartReceiveHardwareHandler(LED_CONTROLLER,0x81);
    //remain IDLE state as input was not 0x7E
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_IDLE);
    usartReceiveHardwareHandler(LED_CONTROLLER,0x7E);
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_RECEIVED_DELIMITER);
}

void test_usartReceiveHardwareHandler_from_RECEIVED_DELIMITER_to_IDLE(void){
    int transmitByte;
    initUsartHardwareInfo(LED_CONTROLLER,usart1);
    UsartInfo * fakeInfo = &usartInfo[LED_CONTROLLER];
    fakeInfo->hwRxState = HW_RX_RECEIVED_DELIMITER;
    //Receive delimiter state
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_RECEIVED_DELIMITER);
    usartReceiveHardwareHandler(LED_CONTROLLER,0x11);
    //return back to IDLE state as input was not 0x81
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_IDLE);
}

void test_usartReceiveHardwareHandler_from_RECEIVED_DELIMITER_to_RECEIVE_BYTE(void){
    int transmitByte;
    initUsartHardwareInfo(LED_CONTROLLER,usart1);
    UsartInfo * fakeInfo = &usartInfo[LED_CONTROLLER];
    fakeInfo->hwRxState = HW_RX_RECEIVED_DELIMITER;
    //Receive delimiter state
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_RECEIVED_DELIMITER);
    usartReceiveHandler_Expect(LED_CONTROLLER,RX_PACKET_START<<8);
    usartReceiveHardwareHandler(LED_CONTROLLER,0x81);
    //return back to IDLE state as input was not 0x81
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_RECEIVE_BYTE);
}

void test_usartReceiveHardwareHandler_from_RECEIVE_BYTE_to_RECEIVE_7E_BYTE(void){
    int transmitByte;
    initUsartHardwareInfo(LED_CONTROLLER,usart1);
    UsartInfo * fakeInfo = &usartInfo[LED_CONTROLLER];
    fakeInfo->hwRxState = HW_RX_RECEIVE_BYTE;
    //Receive state
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_RECEIVE_BYTE);
    usartReceiveHardwareHandler(LED_CONTROLLER,0x7E);
    //transition receive 7e byte
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_RECEIVE_7E_BYTE);
}

void test_usartReceiveHardwareHandler_RECEIVE_BYTE(void){
    int transmitByte;
    initUsartHardwareInfo(LED_CONTROLLER,usart1);
    UsartInfo * fakeInfo = &usartInfo[LED_CONTROLLER];
    fakeInfo->hwRxState = HW_RX_RECEIVE_BYTE;
    //Receive byte state
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_RECEIVE_BYTE);
    usartReceiveHandler_Expect(LED_CONTROLLER,0x22);
    usartReceiveHardwareHandler(LED_CONTROLLER,0x22);
    //remain on Receive byte state
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_RECEIVE_BYTE);
}

void test_usartReceiveHardwareHandler_RECEIVE_7E_BYTE_received_0xE7(void){
    int transmitByte;
    initUsartHardwareInfo(LED_CONTROLLER,usart1);
    UsartInfo * fakeInfo = &usartInfo[LED_CONTROLLER];
    fakeInfo->hwRxState = HW_RX_RECEIVE_7E_BYTE;
    //Receive byte 7E state
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_RECEIVE_7E_BYTE);
    usartReceiveHandler_Expect(LED_CONTROLLER,0x7E);
    usartReceiveHardwareHandler(LED_CONTROLLER,0xE7);
    //return to  Receive byte state
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_RECEIVE_BYTE);
}

void test_usartReceiveHardwareHandler_RECEIVE_7E_BYTE_received_0x81(void){
    int transmitByte;
    initUsartHardwareInfo(LED_CONTROLLER,usart1);
    UsartInfo * fakeInfo = &usartInfo[LED_CONTROLLER];
    fakeInfo->hwRxState = HW_RX_RECEIVE_7E_BYTE;
    //Receive byte 7E state
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_RECEIVE_7E_BYTE);
    usartReceiveHandler_Expect(LED_CONTROLLER,(RX_PACKET_START<<8));
    usartReceiveHardwareHandler(LED_CONTROLLER,0x81);
    //return to Receive byte state
    // with signal generated for the usartDriver
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_RECEIVE_BYTE);
}

void test_usartReceiveHardwareHandler_RECEIVE_7E_BYTE_received_other_value(void){
    int transmitByte;
    initUsartHardwareInfo(LED_CONTROLLER,usart1);
    UsartInfo * fakeInfo = &usartInfo[LED_CONTROLLER];
    fakeInfo->hwRxState = HW_RX_RECEIVE_7E_BYTE;
    //Receive byte 7E state
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_RECEIVE_7E_BYTE);
    usartReceiveHardwareHandler(LED_CONTROLLER,0x21);
    //return to idle states as error happened
    TEST_ASSERT_EQUAL(fakeInfo->hwRxState,HW_RX_IDLE);
}
