#include "unity.h"
#include "UsartDriver.h"
#include "mock_UsartHardware.h"
#include "mock_Irq.h"
#include "mock_Gpio.h"
#include "mock_Rcc.h"
#include "mock_EventQueue.h"
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
#include "Common.h"
extern UsartDriverInfo usartDriverInfo[];
UsartEvent * txEvent;
UsartEvent rxEvent;
UsartEvent * rxEvent2;
char * rxPacket;
void setUp(void){
}

void tearDown(void){

}

void fake_emptyFunction(){

}

void test_UsartDriver_usartConfig(void){
    disableIRQ_StubWithCallback(fake_disableIRQ);
    enableIRQ_StubWithCallback(fake_enableIRQ);
    usartHardwareConfig_Expect(LED_CONTROLLER,115200,OVER_8,EVEN_PARITY,DATA_8_BITS,STOP_BIT_2,ENABLE_MODE);
    usartConfig(LED_CONTROLLER,115200,OVER_8,EVEN_PARITY,DATA_8_BITS,STOP_BIT_2,ENABLE_MODE);
    fakeCheckIRQ(__LINE__);
}

void test_usartDriverTransmit(void){
    disableIRQ_StubWithCallback(fake_disableIRQ);
    enableIRQ_StubWithCallback(fake_enableIRQ);
    UsartDriverInfo * info =&usartDriverInfo[LED_CONTROLLER];
    uint8_t * array="abc";
    uint16_t crc16;
    uint8_t txCRC16[2];
    hardwareUsartTransmit_Expect(LED_CONTROLLER);
    usartDriverTransmit(LED_CONTROLLER,0x23,array,txEvent);
    TEST_ASSERT_EQUAL(info->txLen,4);
    TEST_ASSERT_EQUAL(info->receiverAddress,0x23);
    TEST_ASSERT_EQUAL(info->txUsartEvent,txEvent);
    TEST_ASSERT_EQUAL(info->requestTxPacket,1);
    TEST_ASSERT_EQUAL(info->txBuffer,array);
    crc16 = generateCrc16(array, 4);
    *(uint16_t*)&txCRC16[0] = crc16;
    TEST_ASSERT_EQUAL(info->txCRC16,txCRC16);
    fakeCheckIRQ(__LINE__);
}

void test_usartTransmissionHandler_idle_to_send_rxAddress(){
    disableIRQ_StubWithCallback(fake_disableIRQ);
    enableIRQ_StubWithCallback(fake_enableIRQ);
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
    uint8_t * array="abc";
    //initialization
    usartDriverInit(LED_CONTROLLER);
    hardwareUsartTransmit_Expect(LED_CONTROLLER);
    usartDriverTransmit(LED_CONTROLLER,0x23,array,txEvent);
    TEST_ASSERT_EQUAL(fakeInfo->txState,TX_IDLE);
    //(send receiverAddress)
    TEST_ASSERT_EQUAL(usartTransmissionHandler(LED_CONTROLLER),0x23);
    TEST_ASSERT_EQUAL(fakeInfo->txState,TX_SEND_RECEIVER_ADDRESS);
    //(send transmitter address)
    TEST_ASSERT_EQUAL(usartTransmissionHandler(LED_CONTROLLER),USART_ADDRESS);
    TEST_ASSERT_EQUAL(fakeInfo->txState,TX_SEND_TRANSMITTER_ADDRESS);
    //(send length)
    TEST_ASSERT_EQUAL(usartTransmissionHandler(LED_CONTROLLER),8);
    TEST_ASSERT_EQUAL(fakeInfo->txState,TX_SEND_LENGTH);
    //(send flag)
    TEST_ASSERT_EQUAL(usartTransmissionHandler(LED_CONTROLLER),0);
    TEST_ASSERT_EQUAL(fakeInfo->txState,TX_SEND_FLAG);
    //(send byte)
    TEST_ASSERT_EQUAL(usartTransmissionHandler(LED_CONTROLLER),'a');
    TEST_ASSERT_EQUAL(fakeInfo->txState,TX_SEND_BYTE);
    //(send byte)
    TEST_ASSERT_EQUAL(usartTransmissionHandler(LED_CONTROLLER),'b');
    TEST_ASSERT_EQUAL(fakeInfo->txState,TX_SEND_BYTE);
    //(send byte)
    //fakeInfo->lastByte
    TEST_ASSERT_EQUAL(usartTransmissionHandler(LED_CONTROLLER),'c');
    TEST_ASSERT_EQUAL(fakeInfo->txState,TX_SEND_BYTE);
	fakeCheckIRQ(__LINE__);
}
//////Receive Handler//////////////////////////////////////////////////////
//RX_PACKET_START state
void test_handleRxAddressAndLength_correct_address(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxState = RX_ADDRESS_LENGTH;
    //main
    handleRxAddressAndLength(LED_CONTROLLER,USART_ADDRESS);
    TEST_ASSERT_EQUAL(fakeInfo->rxStaticBuffer[0],USART_ADDRESS);
    TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_ADDRESS_LENGTH);

    handleRxAddressAndLength(LED_CONTROLLER,0x12);
    TEST_ASSERT_EQUAL(fakeInfo->rxStaticBuffer[1],0x12);
    TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_ADDRESS_LENGTH);

    eventEnqueue_Expect(&sysQueue,(Event*)&fakeInfo->sysEvent);
    handleRxAddressAndLength(LED_CONTROLLER,0x3);
    TEST_ASSERT_EQUAL(fakeInfo->rxStaticBuffer[2],0x3);
    TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_RECEIVE_PAYLOAD_STATIC_BUFFER);
}

void test_handleRxAddressAndLength_wrong_address(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxState = RX_ADDRESS_LENGTH;
    //main
    handleRxAddressAndLength(LED_CONTROLLER,0x11);
    TEST_ASSERT_EQUAL(fakeInfo->rxStaticBuffer[0],0x11);
    TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_ADDRESS_LENGTH);

    handleRxAddressAndLength(LED_CONTROLLER,0x12);
    TEST_ASSERT_EQUAL(fakeInfo->rxStaticBuffer[1],0x12);
    TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_ADDRESS_LENGTH);

    handleRxAddressAndLength(LED_CONTROLLER,0x3);
    TEST_ASSERT_EQUAL(fakeInfo->rxStaticBuffer[2],0x3);
    TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_IDLE);
}

void test_handleRxAddressAndLength_rx_packet_start(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxState = RX_ADDRESS_LENGTH;
    //main
    handleRxAddressAndLength(LED_CONTROLLER,(RX_PACKET_START<<8));
    TEST_ASSERT_EQUAL(fakeInfo->rxCounter,0);
    TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_ADDRESS_LENGTH);
}

//RX_PACKET_START state
void test_handleRxStaticBufferPayload_rx_data_receive(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxCounter = 3;
    fakeInfo->rxLen = 10;
    fakeInfo->rxState = RX_RECEIVE_PAYLOAD_STATIC_BUFFER;
    //main
    handleRxStaticBufferPayload(LED_CONTROLLER,0x12);
    TEST_ASSERT_EQUAL(fakeInfo->rxStaticBuffer[3],0x12);
    TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_RECEIVE_PAYLOAD_STATIC_BUFFER);

    handleRxStaticBufferPayload(LED_CONTROLLER,0x55);
    TEST_ASSERT_EQUAL(fakeInfo->rxStaticBuffer[4],0x55);
    TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_RECEIVE_PAYLOAD_STATIC_BUFFER);
}

void test_handleRxStaticBufferPayload_rx_data_packet_start(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxCounter = 3;
    fakeInfo->rxState = RX_RECEIVE_PAYLOAD_STATIC_BUFFER;
    //main
    handleRxStaticBufferPayload(LED_CONTROLLER,(RX_PACKET_START<<8));
    TEST_ASSERT_EQUAL(fakeInfo->rxCounter,0);
    TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_ADDRESS_LENGTH);
}

void test_handleRxStaticBufferPayload_malloc_request_evt(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
	uint8_t * staticBuffer = fakeInfo->rxStaticBuffer;
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxCounter = 3;
    fakeInfo->rxLen = 10;
    fakeInfo->rxState = RX_RECEIVE_PAYLOAD_STATIC_BUFFER;
	staticBuffer[0] ='a';
	staticBuffer[1] ='b';
    //main
    fakeInfo->rxMallocBuffer = (uint8_t*) malloc(13 * sizeof(uint8_t));
    handleRxStaticBufferPayload(LED_CONTROLLER,(MALLOC_REQUEST_EVT << 8));
    TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_RECEIVE_PAYLOAD_MALLOC_BUFFER);
	TEST_ASSERT_EQUAL(fakeInfo->rxMallocBuffer[0],'a');
	TEST_ASSERT_EQUAL(fakeInfo->rxMallocBuffer[1],'b');
	free(fakeInfo->rxMallocBuffer);
}

void test_handleRxStaticBufferPayload_last_packet(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxCounter = 7;
    fakeInfo->rxLen = 3;
    fakeInfo->rxState = RX_RECEIVE_PAYLOAD_STATIC_BUFFER;

    //main
    handleRxStaticBufferPayload(LED_CONTROLLER,0x12);
    TEST_ASSERT_EQUAL(fakeInfo->rxCRC16[0],0x12);
	TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_WAIT_CRC16_STATIC_BUFFER);
}

void test_handleRxMallocBufferPayload_addPacket(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
	uint8_t * staticBuffer = fakeInfo->rxStaticBuffer;
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxCounter = 3;
    fakeInfo->rxLen = 10;
    fakeInfo->rxState = RX_RECEIVE_PAYLOAD_MALLOC_BUFFER;
    fakeInfo->rxMallocBuffer = (uint8_t*) malloc(13 * sizeof(uint8_t));
    //main
    handleRxMallocBufferPayload(LED_CONTROLLER,0x22);
	TEST_ASSERT_EQUAL(fakeInfo->rxMallocBuffer[3],0x22);
    TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_RECEIVE_PAYLOAD_MALLOC_BUFFER);

	handleRxMallocBufferPayload(LED_CONTROLLER,0x23);
	TEST_ASSERT_EQUAL(fakeInfo->rxMallocBuffer[4],0x23);
	TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_RECEIVE_PAYLOAD_MALLOC_BUFFER);

	free(fakeInfo->rxMallocBuffer);
}

void test_handleRxMallocBufferPayload_packet_start(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
	uint8_t * staticBuffer = fakeInfo->rxStaticBuffer;
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxCounter = 3;
    fakeInfo->rxLen = 10;
    fakeInfo->rxState = RX_RECEIVE_PAYLOAD_MALLOC_BUFFER;
	eventEnqueue_Expect(&sysQueue,(Event*)&fakeInfo->sysEvent);
    //main
    handleRxMallocBufferPayload(LED_CONTROLLER,(RX_PACKET_START<<8));
	TEST_ASSERT_EQUAL(fakeInfo->rxCounter,0);
    TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_WAIT_FOR_FREE_MALLOC_BUFFER);
}

void test_handleRxMallocBufferPayload_last_packet(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxCounter = 7;
    fakeInfo->rxLen = 3;
    fakeInfo->rxState = RX_RECEIVE_PAYLOAD_MALLOC_BUFFER;

    //main
    handleRxMallocBufferPayload(LED_CONTROLLER,0x12);
    TEST_ASSERT_EQUAL(fakeInfo->rxCRC16[0],0x12);
	TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_WAIT_CRC16_MALLOC_BUFFER);
}

void test_handleCRC16WithStaticBuffer_rx_data_packet_start(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxState = RX_WAIT_CRC16_STATIC_BUFFER;
    //main
    handleCRC16WithStaticBuffer(LED_CONTROLLER,(RX_PACKET_START<<8));
    TEST_ASSERT_EQUAL(fakeInfo->rxCounter,0);
    TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_ADDRESS_LENGTH);
}

void test_handleCRC16WithStaticBuffer_malloc_request_evt(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
	uint8_t * staticBuffer = fakeInfo->rxStaticBuffer;
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxCounter = 3;
    fakeInfo->rxLen = 10;
    fakeInfo->rxState = RX_WAIT_CRC16_STATIC_BUFFER;
	staticBuffer[0] ='a';
	staticBuffer[1] ='b';
    //main
    fakeInfo->rxMallocBuffer = (uint8_t*) malloc(13 * sizeof(uint8_t));
    handleCRC16WithStaticBuffer(LED_CONTROLLER,(MALLOC_REQUEST_EVT << 8));
    TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_WAIT_CRC16_MALLOC_BUFFER);
	TEST_ASSERT_EQUAL(fakeInfo->rxMallocBuffer[0],'a');
	TEST_ASSERT_EQUAL(fakeInfo->rxMallocBuffer[1],'b');
	free(fakeInfo->rxMallocBuffer);
}

void test_handleCRC16WithStaticBuffer_CRC_packet_received(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxState = RX_WAIT_CRC16_STATIC_BUFFER;

    //main
    handleCRC16WithStaticBuffer(LED_CONTROLLER,0x55);
    TEST_ASSERT_EQUAL(fakeInfo->rxCRC16[1],0x55);
	TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_WAIT_FOR_MALLOC_BUFFER);
}

void test_handleCRC16WithMallocBuffer_rx_data_packet_start(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxState = RX_WAIT_CRC16_STATIC_BUFFER;
	eventEnqueue_Expect(&sysQueue,(Event*)&fakeInfo->sysEvent);
    //main
    handleCRC16WithMallocBuffer(LED_CONTROLLER,(RX_PACKET_START<<8));
    TEST_ASSERT_EQUAL(fakeInfo->rxCounter,0);
    TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_WAIT_FOR_FREE_MALLOC_BUFFER);
}


void test_handleCRC16WithMallocBuffer_CRC_packet_received(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxState = RX_WAIT_CRC16_STATIC_BUFFER;
	fakeInfo->rxMallocBuffer = (uint8_t*) malloc(13 * sizeof(uint8_t));
	fakeInfo->rxUsartEvent = (UsartEvent*) malloc(sizeof(UsartEvent));
	eventEnqueue_Expect(&evtQueue,(Event*)fakeInfo->rxUsartEvent);
    //main

    handleCRC16WithMallocBuffer(LED_CONTROLLER,0x55);
    TEST_ASSERT_EQUAL(fakeInfo->rxCRC16[1],0x55);
	TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_IDLE);
	TEST_ASSERT_EQUAL(fakeInfo->rxUsartEvent->type,RX_CRC_ERROR_EVENT);
	TEST_ASSERT_EQUAL(fakeInfo->rxUsartEvent->buffer,fakeInfo->rxMallocBuffer);
}

void test_usartReceiveHandler_wait_for_malloc_with_malloc_evt(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
	uint8_t * staticBuffer = fakeInfo->rxStaticBuffer;
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxState = RX_WAIT_FOR_MALLOC_BUFFER;
	fakeInfo->rxMallocBuffer = (uint8_t*) malloc(13 * sizeof(uint8_t));
	fakeInfo->rxUsartEvent = (UsartEvent*) malloc(sizeof(UsartEvent));
	staticBuffer[0] ='a';
	staticBuffer[1] ='b';
	eventEnqueue_Expect(&evtQueue,(Event*)fakeInfo->rxUsartEvent);
    //main
    usartReceiveHandler(LED_CONTROLLER,(MALLOC_REQUEST_EVT << 8));
	TEST_ASSERT_EQUAL(fakeInfo->rxMallocBuffer[0],'a');
	TEST_ASSERT_EQUAL(fakeInfo->rxMallocBuffer[1],'b');
	TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_IDLE);
	TEST_ASSERT_EQUAL(fakeInfo->rxUsartEvent->type,RX_CRC_ERROR_EVENT);
	TEST_ASSERT_EQUAL(fakeInfo->rxUsartEvent->buffer,fakeInfo->rxMallocBuffer);
	free(fakeInfo->rxMallocBuffer);
	free(fakeInfo->rxUsartEvent);
}

void test_usartReceiveHandler_wait_for_malloc_without_malloc_evt(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
	uint8_t * staticBuffer = fakeInfo->rxStaticBuffer;
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxState = RX_WAIT_FOR_MALLOC_BUFFER;
    //main
    usartReceiveHandler(LED_CONTROLLER,(FREE_MALLOC_EVT << 8));
	TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_WAIT_FOR_MALLOC_BUFFER);
}

void test_usartReceiveHandler_wait_for_free_memory_with_free_malloc_evt(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
	uint8_t * staticBuffer = fakeInfo->rxStaticBuffer;
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxState = RX_WAIT_FOR_FREE_MALLOC_BUFFER;
    //main
    usartReceiveHandler(LED_CONTROLLER,(FREE_MALLOC_EVT << 8));
	TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_IDLE);
}

void test_usartReceiveHandler_wait_for_free_memory_without_free_malloc_evt(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
	uint8_t * staticBuffer = fakeInfo->rxStaticBuffer;
    //initialization
    usartDriverInit(LED_CONTROLLER);
    fakeInfo->rxState = RX_WAIT_FOR_FREE_MALLOC_BUFFER;
    //main
    usartReceiveHandler(LED_CONTROLLER,(MALLOC_REQUEST_EVT << 8));
	TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_WAIT_FOR_FREE_MALLOC_BUFFER);
}

void test_checkRxPacketCRC_pass(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
	uint8_t fakeBuffer[] = {0x12,0x13,0x3,0xA,0xB,0xC};
	uint8_t * rxCRC16 = fakeInfo->rxCRC16;
	uint16_t crc16;
    //initialization
    usartDriverInit(LED_CONTROLLER);
	fakeInfo->rxLen = 3;
	crc16 = generateCrc16(&fakeBuffer[PAYLOAD_OFFSET], 3);
	*(uint16_t*)&rxCRC16[0] = crc16;
	fakeInfo->rxMallocBuffer = &fakeBuffer[0];
    //main
	TEST_ASSERT_EQUAL(checkRxPacketCRC(LED_CONTROLLER),1);
}

void test_checkRxPacketCRC_fail(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
	uint8_t fakeBuffer[] = {0x12,0x13,0x3,0xA,0xB,0xC};
	uint8_t * rxCRC16 = fakeInfo->rxCRC16;
	uint16_t crc16;
    //initialization
    usartDriverInit(LED_CONTROLLER);
	fakeInfo->rxLen = 2;
	crc16 = generateCrc16(&fakeBuffer[PAYLOAD_OFFSET], 3);
	*(uint16_t*)&rxCRC16[0] = crc16;
	fakeInfo->rxMallocBuffer = &fakeBuffer[0];
    //main
	TEST_ASSERT_EQUAL(checkRxPacketCRC(LED_CONTROLLER),0);
}

void test_generateEventForReceiveComplete_pass(){
    UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
	uint8_t fakeBuffer[] = {0x12,0x13,0x3,0xA,0xB,0xC};
	uint8_t * rxCRC16 = fakeInfo->rxCRC16;
	uint16_t crc16;
    //initialization
    usartDriverInit(LED_CONTROLLER);
	fakeInfo->rxLen = 3;
	crc16 = generateCrc16(&fakeBuffer[PAYLOAD_OFFSET], 3);
	*(uint16_t*)&rxCRC16[0] = crc16;
	fakeInfo->rxMallocBuffer = &fakeBuffer[0];
	fakeInfo->rxUsartEvent = &rxEvent;
	eventEnqueue_Expect(&evtQueue,(Event*)fakeInfo->rxUsartEvent);
    //main
	generateEventForReceiveComplete(LED_CONTROLLER);
	TEST_ASSERT_EQUAL(fakeInfo->rxUsartEvent->type,PACKET_RX_EVENT);
	TEST_ASSERT_EQUAL(fakeInfo->rxUsartEvent->buffer,fakeInfo->rxMallocBuffer);
}

void test_generateEventForReceiveComplete_fail(){
	UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
	uint8_t fakeBuffer[] = {0x12,0x13,0x3,0xA,0xB,0xC};
	uint8_t * rxCRC16 = fakeInfo->rxCRC16;
	uint16_t crc16;
    //initialization
    usartDriverInit(LED_CONTROLLER);
	fakeInfo->rxLen = 2;
	crc16 = generateCrc16(&fakeBuffer[PAYLOAD_OFFSET], 3);
	*(uint16_t*)&rxCRC16[0] = crc16;
	fakeInfo->rxMallocBuffer = &fakeBuffer[0];
	fakeInfo->rxUsartEvent = &rxEvent;
	eventEnqueue_Expect(&evtQueue,(Event*)fakeInfo->rxUsartEvent);
    //main
	generateEventForReceiveComplete(LED_CONTROLLER);
	TEST_ASSERT_EQUAL(fakeInfo->rxUsartEvent->type,RX_CRC_ERROR_EVENT);
	TEST_ASSERT_EQUAL(fakeInfo->rxUsartEvent->buffer,fakeInfo->rxMallocBuffer);
}

void test_resetUsartDriverReceive(){
	UsartDriverInfo * fakeInfo =&usartDriverInfo[LED_CONTROLLER];
    //initialization
    usartDriverInit(LED_CONTROLLER);
    //main
	resetUsartDriverReceive(LED_CONTROLLER);
	TEST_ASSERT_EQUAL(fakeInfo->rxState,RX_IDLE);
	TEST_ASSERT_EQUAL(fakeInfo->rxCounter,0);
	TEST_ASSERT_EQUAL(fakeInfo->rxLen,0);
}
/*
void test_UsartDriver_getRxPacket(void){
    UsartDriverInfo * info =&usartDriverInfo[LED_CONTROLLER];
    info->activeRxBuffer = "abc";

    TEST_ASSERT_EQUAL(info->activeRxBuffer,getRxPacket(info));
}

void test_UsartDriver_getPacketLength(void){
    char * packet = "123";
    TEST_ASSERT_EQUAL('2',getPacketLength(packet));
}

void test_UsartDriver_isCorrectAddress_true(void){
    UsartDriverInfo * info =&usartDriverInfo[LED_CONTROLLER];
    info->activeRxBuffer = "41";
    //address is ASCII 4 which is 0x34
    // 1 is the length size
    // it is same as the usart_address
    TEST_ASSERT_EQUAL(1,isCorrectAddress(info));
}

void test_UsartDriver_isCorrectAddress_wrong(void){
    UsartDriverInfo * info =&usartDriverInfo[LED_CONTROLLER];
    info->activeRxBuffer = "31";
    //address is ASCII 3 which is 0x33
    //different with the usartAddress
    // 1 is the length size
    // it is different as the usart_address
    TEST_ASSERT_EQUAL(0,isCorrectAddress(info));
}

void test_usartDriverTransmit(void){
    disableIRQ_StubWithCallback(fake_disableIRQ);
    enableIRQ_StubWithCallback(fake_enableIRQ);
    UsartDriverInfo * info =&usartDriverInfo[LED_CONTROLLER];
    //test run first time where there is no Tx Packet
    char * transmitPacket = "1234";
    hardwareUsartTransmit_Expect(LED_CONTROLLER,transmitPacket);
    usartDriverTransmit(LED_CONTROLLER,transmitPacket,txEvent);
    TEST_ASSERT_EQUAL(1,info->requestTxPacket);
    // when txPacket is transmitting there is no more hardwareUsartTransmit
    usartDriverTransmit(LED_CONTROLLER,transmitPacket,txEvent);
    TEST_ASSERT_EQUAL(1,info->requestTxPacket);
    fakeCheckIRQ(__LINE__);
}

void test_usartDriverReceive(void){
    disableIRQ_StubWithCallback(fake_disableIRQ);
    enableIRQ_StubWithCallback(fake_enableIRQ);
    UsartDriverInfo * info =&usartDriverInfo[LED_CONTROLLER];
    //test run first time where there is no rx Packet
    TEST_ASSERT_EQUAL(0,info->requestRxPacket);
    TEST_ASSERT_NULL(info->spareRxBuffer);
    usartDriverReceive(LED_CONTROLLER,rxPacket,&rxEvent);
    TEST_ASSERT_EQUAL(1,info->requestRxPacket);
    TEST_ASSERT_EQUAL(&rxEvent,info->rxUsartEvent);
    // when rxPacket is transmitting the event wont change
    usartDriverReceive(LED_CONTROLLER,rxPacket,rxEvent2);
    TEST_ASSERT_EQUAL(&rxEvent,info->rxUsartEvent);
    fakeCheckIRQ(__LINE__);
}


void test_usartTxCompletionHandler(void){
    disableIRQ_StubWithCallback(fake_disableIRQ);
    enableIRQ_StubWithCallback(fake_enableIRQ);
    UsartDriverInfo * info =&usartDriverInfo[LED_CONTROLLER];
    info->txUsartEvent = txEvent; //there is event
    TEST_ASSERT_EQUAL(1,info->requestRxPacket);
    TEST_ASSERT_EQUAL(txEvent,info->txUsartEvent);
    eventEnqueue_Expect(&evtQueue,(Event*)info->txUsartEvent);
    usartTxCompletionHandler(LED_CONTROLLER);

    fakeCheckIRQ(__LINE__);
}

void test_usartRxCompletionHandler_correct_address(void){
    disableIRQ_StubWithCallback(fake_disableIRQ);
    enableIRQ_StubWithCallback(fake_enableIRQ);
    gpioToggleBit_StubWithCallback(fake_emptyFunction);
    UsartDriverInfo * info =&usartDriverInfo[LED_CONTROLLER];
    info->activeRxBuffer = "41";
    info->requestRxPacket = 1; //data is requested to receive
    info->rxUsartEvent = &rxEvent; //there is event
    char * packet = info->activeRxBuffer;

    //wait for the packet header
    TEST_ASSERT_EQUAL(WAIT_FOR_PACKET_HEADER,info->state);
    hardwareUsartReceive_Expect(LED_CONTROLLER,info->activeRxBuffer+PACKET_HEADER_SIZE,getPacketLength(packet));
    usartRxCompletionHandler(LED_CONTROLLER);
    // when Rx packet is requested
    TEST_ASSERT_EQUAL(WAIT_FOR_PACKET_PAYLOAD,info->state);
    eventEnqueue_Expect(&evtQueue,(Event*)info->rxUsartEvent);
    hardwareUsartReceive_Expect(LED_CONTROLLER,usartDriverInfo[LED_CONTROLLER].spareRxBuffer,PACKET_HEADER_SIZE);
    usartRxCompletionHandler(LED_CONTROLLER);
    //now request the usart receive again
    TEST_ASSERT_EQUAL(WAIT_FOR_PACKET_HEADER,info->state);
    fakeCheckIRQ(__LINE__);
}

void test_usartRxCompletionHandler_wrong_address(void){
    disableIRQ_StubWithCallback(fake_disableIRQ);
    enableIRQ_StubWithCallback(fake_enableIRQ);
    UsartDriverInfo * info =&usartDriverInfo[LED_CONTROLLER];
    info->activeRxBuffer = "51"; // 5 is the address
    info->requestRxPacket = 1; //data is requested to receive
    info->rxUsartEvent = &rxEvent; //there is event
    info->state = WAIT_FOR_PACKET_PAYLOAD;
    char * packet = info->activeRxBuffer;

    TEST_ASSERT_EQUAL(WAIT_FOR_PACKET_PAYLOAD,info->state);
    hardwareUsartReceive_Expect(LED_CONTROLLER,usartDriverInfo[LED_CONTROLLER].activeRxBuffer,PACKET_HEADER_SIZE);
    usartRxCompletionHandler(LED_CONTROLLER);
    //now request the usart receive again
    TEST_ASSERT_EQUAL(WAIT_FOR_PACKET_HEADER,info->state);
    fakeCheckIRQ(__LINE__);
}
*/
