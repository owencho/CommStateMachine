#include "UsartHardware.h"
#include "EventQueue.h"
#include "Event.h"
#include "List.h"
#include "Gpio.h"
#include "Irq.h"
#include "UsartEvent.h"
#include "UsartDriver.h"
#include "TimerEventQueue.h"
#include "InitUsart.h"
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
STATIC void initUsartHardwareInfo(UsartPort port ,UsartRegs * usart){
    UsartInfo * info = &usartInfo[port];
    info->usart = usart;
    info->txBuffer = malloc(sizeof(char)*64);
    info->rxBuffer = malloc(sizeof(char)*64);
    info->txlen = 0;
    info->rxlen = 0;
}

STATIC void usartHardwareConfig(UsartPort port,int baudRate,OversampMode overSampMode,ParityMode parityMode,
                       	   	   WordLength length,StopBit sBitMode,EnableDisable halfDuplex){
	disableIRQ();
	UsartInfo * info = &usartInfo[port];
	UsartRegs * usart = info->usart;
	usartSetBaudRate(usart,baudRate);
    setUsartOversamplingMode(usart,overSampMode);
    usartEnableParityControl(usart );
    setUsartParityMode(usart ,parityMode);
    setUsartWordLength(usart ,length);
    usartSetStopBit(usart,sBitMode);
    usartSetHalfDuplexMode(usart,halfDuplex);
    enableUsart(usart);
    enableIRQ();
}

void usartHardwareInit(UsartPort port,int baudRate,OversampMode overSampMode,ParityMode parityMode,
                       WordLength length,StopBit sBitMode,EnableDisable halfDuplex){
	disableIRQ();
    memset(&usartInfo[1],0,sizeof(UsartInfo));
    initUsartHardwareInfo(LED_CONTROLLER,usart1);
    initUsartHardwareInfo(MAIN_CONTROLLER,uart5);
    configureGpio();
    usartHardwareConfig(port,baudRate,overSampMode,parityMode,length,sBitMode,halfDuplex);
    enableIRQ();
}

void hardwareUsartTransmit(UsartPort port,char * txData){
    disableIRQ();
    UsartInfo * info =&usartInfo[port];
    info->txlen=findPacketLength(txData);
    info->txBuffer = txData;
    info->txTurn = 1;
    hardwareUsartSetTxCompleteCallback(port,(UsartCallback)usartTxCompletionHandler);
    usartEnableTransmission(info->usart);
    usartEnableInterrupt(info->usart,TRANS_COMPLETE);
    usartDisableReceiver(info->usart);
    enableIRQ();
}

void hardwareUsartReceive(UsartPort port,char * rxBuffer,int length){
    disableIRQ();
    UsartInfo * info =&usartInfo[port];
    info->rxlen=length;
    info->rxBuffer = rxBuffer;
    hardwareUsartSetRxCompleteCallback(port,(UsartCallback)usartRxCompletionHandler);
    usartEnableReceiver(info->usart);
    usartEnableInterrupt(info->usart,RXNE_INTERRUPT);
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
