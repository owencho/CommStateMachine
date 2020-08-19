#include "UsartHardware.h"
#include "EventQueue.h"
#include "Event.h"
#include "List.h"
#include "Irq.h"
#include "UsartEvent.h"
#include "UsartDriver.h"
#include "TimerEventQueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

UsartInfo usartInfo[] = {
  [LED_CONTROLLER]={NULL},
  [MAIN_CONTROLLER]={NULL},
};
STATIC int findPacketLength(char* data){
    return (sizeof(data)/sizeof(char));
}
void initUsartHardwareReg(UsartPort port ,UsartRegs * usart){
    UsartInfo * info = &usartInfo[port];
    info->usart = usart;
    //info->txBuffer = malloc(sizeof(char)*64);
    //info->rxBuffer = malloc(sizeof(char)*64);
    info->txlen = 0;
    info->rxlen = 0;
}

void usartHardwareInit(UsartPort port,OversampMode overSampMode,ParityMode parityMode,
                       WordLength length,StopBit sBitMode,EnableDisable halfDuplex){
    memset(&usartInfo[1],0,sizeof(UsartInfo));
    initUsartHardwareReg(LED_CONTROLLER,usart1);
    setUsartOversamplingMode(usart1,overSampMode);
    usartEnableParityControl(usart1);
    setUsartParityMode(usart1,parityMode);
    setUsartWordLength(usart1,length);
    usartSetStopBit(usart1,sBitMode);
    usartSetHalfDuplexMode(usart1,halfDuplex);
}

void hardwareUsartTransmit(UsartPort port,char * txData){
    disableIRQ();
    UsartInfo * hardwareInfo =&usartInfo[port];
    hardwareInfo->txlen=findPacketLength(txData);
    hardwareInfo->txBuffer = txData;
    hardwareUsartSetTxCompleteCallback(port,(UsartCallback)usartTxCompletionHandler);
    enableIRQ();
}

void hardwareUsartReceive(UsartPort port,char * rxBuffer,int length){
    disableIRQ();
    UsartInfo * hardwareInfo =&usartInfo[port];
    hardwareInfo->rxlen=length;
    hardwareInfo->rxBuffer = rxBuffer;
    hardwareUsartSetRxCompleteCallback(port,(UsartCallback)usartRxCompletionHandler);
    enableIRQ();
}

void hardwareUsartSetTxCompleteCallback(UsartPort port,UsartCallback callback){
    disableIRQ();
    UsartInfo * hardwareInfo =&usartInfo[port];
    hardwareInfo->txCompleteCallBack = callback;
    enableIRQ();
}
void hardwareUsartSetRxCompleteCallback(UsartPort port,UsartCallback callback){
    disableIRQ();
    UsartInfo * hardwareInfo =&usartInfo[port];
    hardwareInfo->rxCompleteCallBack = callback;
    enableIRQ();
}
void hardwareUsartSetErrorCallback(UsartPort port,UsartCallback callback){
    disableIRQ();
    UsartInfo * hardwareInfo =&usartInfo[port];
    hardwareInfo->errorCallBack = callback;
    enableIRQ();
}
