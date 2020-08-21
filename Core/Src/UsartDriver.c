#include "UsartDriver.h"
#include "UsartHardware.h"
#include "EventQueue.h"
#include "Event.h"
#include "Gpio.h"
#include "List.h"
#include "Irq.h"
#include "UsartEvent.h"
#include "TimerEventQueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

UsartDriverInfo usartDriverInfo[] = {
  [LED_CONTROLLER]={NULL},
  [MAIN_CONTROLLER]={NULL},
};

extern EventQueue evtQueue;

#define peepRxPacket(info) ((info)->activeRxBuffer)
#define hasRequestedTxPacket(info) ((info)->requestTxPacket)
#define hasRequestedRxPacket(info) ((info)->requestRxPacket)
#define getPacketPayloadAddress(packet) (packet + PACKET_HEADER_SIZE)

STATIC char * getRxPacket(UsartDriverInfo *info){
    char * packet = info->activeRxBuffer;
    if(hasRequestedRxPacket(info)){
        info->activeRxBuffer = info->spareRxBuffer;
        info->spareRxBuffer = NULL;
    }
    return packet;
}
STATIC int getPacketLength(char * txData){
    int packetLength = txData[LENGTH_ADDRESS_OFFSET];
    return packetLength;
}

STATIC int isCorrectAddress(UsartDriverInfo *info){
    char * packet = info->activeRxBuffer;
    char usartAddress = *(packet + PACKET_ADDRESS_OFFSET);

    if((int)usartAddress == USART_ADDRESS)
        return 1;
    else
        return 0;
}

void usartInit(){
    memset(&usartDriverInfo[1],0,sizeof(UsartDriverInfo));
    usartHardwareInit();
}
void usartConfig(UsartPort port,int baudRate,OversampMode overSampMode,ParityMode parityMode,
               WordLength length,StopBit sBitMode,EnableDisable halfDuplex){

    disableIRQ();
    UsartDriverInfo * info =&usartDriverInfo[port];
    info->activeRxBuffer = malloc(sizeof(char)*64);
    char * packet = info->activeRxBuffer;
    info->state = WAIT_FOR_PACKET_HEADER;
    usartHardwareConfig(port,baudRate,overSampMode,parityMode,length,sBitMode,halfDuplex);
    hardwareUsartReceive(port,packet,PACKET_HEADER_SIZE);
    enableIRQ();
}

void usartDriverTransmit(UsartPort port, char * txData,UsartEvent * event){
    disableIRQ();
    UsartDriverInfo * info =&usartDriverInfo[port];

    if(!hasRequestedTxPacket(info)){
        info->txUsartEvent = event;
        info->requestTxPacket = 1;
        hardwareUsartTransmit(port,txData);
    }
    enableIRQ();
}

void usartDriverReceive(UsartPort port, char * rxBuffer,UsartEvent * event){
    disableIRQ();
    UsartDriverInfo * info =&usartDriverInfo[port];

    if(!hasRequestedRxPacket(info)){
        info->rxUsartEvent = event;
        info->spareRxBuffer = rxBuffer;
        info->requestRxPacket = 1;
    }
    enableIRQ();
}

void usartTxCompletionHandler(UsartPort port){
    disableIRQ();
    UsartDriverInfo * info =&usartDriverInfo[port];
    UsartEvent * event = info->txUsartEvent;
    eventEnqueue(&evtQueue,(Event*)event);
    info->txUsartEvent = NULL;
    info->requestTxPacket = 0;
    enableIRQ();
}

void usartRxCompletionHandler(UsartPort port){
    disableIRQ();
    UsartDriverInfo * info =&usartDriverInfo[port];
    char * packet = info->activeRxBuffer;
    UsartEvent * event = info->rxUsartEvent;

    switch(info->state){
        case WAIT_FOR_PACKET_HEADER :
            hardwareUsartReceive(port,
                                 getPacketPayloadAddress(packet),
                                 getPacketLength(packet));
            info->state = WAIT_FOR_PACKET_PAYLOAD;
            break;

        case WAIT_FOR_PACKET_PAYLOAD :
            if(hasRequestedRxPacket(info) && isCorrectAddress(info)){
            	gpioToggleBit(gpioG,PIN_13);
                event->buffer = getRxPacket(info);
                info->requestRxPacket = 0;
                eventEnqueue(&evtQueue,(Event*)event);
            }
            packet = peepRxPacket(info);
            hardwareUsartReceive(port,packet,PACKET_HEADER_SIZE);
            info->state = WAIT_FOR_PACKET_HEADER;
            break;
    }
    enableIRQ();
}
