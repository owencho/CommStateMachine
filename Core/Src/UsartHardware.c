#include "UsartHardware.h"
#include "EventQueue.h"
#include "Event.h"
#include "List.h"
#include "Gpio.h"
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
STATIC void initUsartHardwareInfo(UsartPort port ,UsartRegs * usart){
    UsartInfo * info = &usartInfo[port];
    info->usart = usart;
    info->txBuffer = malloc(sizeof(char)*64);
    info->rxBuffer = malloc(sizeof(char)*64);
    info->txlen = 0;
    info->rxlen = 0;
}

void usartHardwareConfig(UsartPort port,int baudRate,OversampMode overSampMode,ParityMode parityMode,
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

void usartHardwareInit(){
	disableIRQ();
    memset(&usartInfo[1],0,sizeof(UsartInfo));
    initUsartHardwareInfo(LED_CONTROLLER,usart1);
    initUsartHardwareInfo(MAIN_CONTROLLER,uart5);
    configureGpio();
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

void configureGpio(){
	  //button
	  enableGpioA();
	  gpioSetMode(gpioA, PIN_0, GPIO_IN);
	  gpioSetPinSpeed(gpioA,PIN_0,HIGH_SPEED);

	  //set GpioA as alternate mode
	  gpioSetMode(gpioA, PIN_8, GPIO_ALT);
	  gpioSetMode(gpioA, PIN_9, GPIO_ALT);
	  gpioSetMode(gpioA, PIN_10, GPIO_ALT);
	  gpioSetPinSpeed(gpioA,PIN_8,HIGH_SPEED);
	  gpioSetPinSpeed(gpioA,PIN_9,HIGH_SPEED);
	  gpioSetPinSpeed(gpioA,PIN_10,HIGH_SPEED);

	  //set alternate function
	  gpioSetAlternateFunction(gpioA ,PIN_8 ,AF7); //set PA8 as USART1_CK
	  gpioSetAlternateFunction(gpioA ,PIN_9 ,AF7); //set PA9 as USART1_TX
	  gpioSetAlternateFunction(gpioA ,PIN_10 ,AF7); //set PA10 as USART1_RX

	  enableGpio(PORT_C);
	  gpioSetMode(gpioC, PIN_12, GPIO_ALT);  //set GpioC as alternate mode
	  gpioSetPinSpeed(gpioC,PIN_12,HIGH_SPEED);

	  enableGpio(PORT_D);
	  gpioSetMode(gpioD, PIN_2, GPIO_ALT);  //set GpioC as alternate mode
	  gpioSetPinSpeed(gpioD,PIN_2,HIGH_SPEED);

	  enableGpio(PORT_G);
	  gpioSetMode(gpioG, PIN_13, GPIO_OUT);  //set GpioC as alternate mode
	  gpioSetPinSpeed(gpioG,PIN_13,HIGH_SPEED);

	  //set alternate function
	  gpioSetAlternateFunction(gpioC ,PIN_12 ,AF8); //set PC12 as UART5_TX
	  gpioSetAlternateFunction(gpioD ,PIN_2 ,AF8); //set PD2 as UART5_RX

	  //enable interrupt uart5
	  nvicEnableInterrupt(53);
	 //enable interrupt usart1
	  nvicEnableInterrupt(37);

	  enableUSART1();
	  enableUART5();

}

