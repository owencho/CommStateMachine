#include "UsartHardware.h"
#include "EventQueue.h"
#include "Event.h"
#include "List.h"
#include "Gpio.h"
#include "Rcc.h"
#include "Nvic.h"
#include "Irq.h"
#include "UsartDriver.h"
#include "TimerEventQueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define isLastByte(info) ((info)->lastByte)

UsartInfo usartInfo[] = {
  [LED_CONTROLLER]={NULL},
  [MAIN_CONTROLLER]={NULL},
};

STATIC void initUsartHardwareInfo(UsartPort port ,UsartRegs * usart){
    UsartInfo * info = &usartInfo[port];
    info->usart = usart;
    info->rxCallBack = (RxCallback)usartReceiveHandler;
    info->txCallBack = (TxCallback)usartTransmissionHandler;
    info->hwTxState = HW_TX_IDLE;
    info->hwRxState = HW_RX_IDLE;
    info->txTurn =0;
    info->lastByte =0;
}

void usartHardwareConfig(UsartPort port,int baudRate,OversampMode overSampMode,ParityMode parityMode,
                       	   	   WordLength length,StopBit sBitMode,EnableDisable halfDuplex){
	disableIRQ();
	UsartInfo * info = &usartInfo[port];
	UsartRegs * usart = info->usart;
	usartSetBaudRate(usart,baudRate);
    setUsartOversamplingMode(usart,overSampMode);
    usartEnableParityControl(usart);
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

void hardwareUsartTransmit(UsartPort port){
    disableIRQ();
    UsartInfo * info =&usartInfo[port];
    info->txTurn = 1;
    usartEnableTransmission(info->usart);
    usartEnableInterrupt(info->usart,TRANS_COMPLETE);
    usartDisableReceiver(info->usart);
    enableIRQ();
}

void hardwareUsartReceive(UsartPort port){
    disableIRQ();
    UsartInfo * info =&usartInfo[port];
    info->txTurn = 0;
    usartEnableReceiver(info->usart);
    usartEnableInterrupt(info->usart,RXNE_INTERRUPT);
    enableIRQ();
}

void usartIrqHandler(UsartPort port){
    UsartInfo * info = &usartInfo[port];
    UsartRegs * usart = info->usart;
    char rxByte;
    char txByte;

    if(info->txTurn){
        txByte = usartTransmitHardwareHandler(port);
        usartSend(usart,txByte);
    }
    else{
        rxByte = usartReceive(usart);
        usartReceiveHardwareHandler(port,rxByte);
    }
}

uint8_t usartTransmitHardwareHandler(UsartPort port){
    UsartInfo * info = &usartInfo[port];
    uint8_t transmitByte;
    switch(info->hwTxState){
        case HW_TX_IDLE :
            info->hwTxState = HW_TX_SEND_DELIMITER;
            transmitByte = 0x7E;
            break;
        case HW_TX_SEND_DELIMITER :
            info->hwTxState = HW_TX_SEND_BYTE;
            transmitByte = 0x81;
            break;
        case HW_TX_SEND_BYTE :
            transmitByte = info->txCallBack(port);
            if(transmitByte == 0x7E){
                info->hwTxState = HW_TX_SEND_7E_BYTE;
            }
            else if(isLastByte(info)){
                endOfUsartTxHandler(port);
                info->hwTxState = HW_TX_IDLE;
                info->lastByte = 0;
            }
            break;
        case HW_TX_SEND_7E_BYTE :
            if(isLastByte(info)){
                endOfUsartTxHandler(port);
                info->hwTxState = HW_TX_IDLE;
                info->lastByte = 0;
            }
            else{
                info->hwTxState = HW_TX_SEND_BYTE;
            }
            transmitByte = 0xE7;
            break;
    }
    return transmitByte;
}

void usartReceiveHardwareHandler(UsartPort port,uint8_t rxByte){
    UsartInfo * info = &usartInfo[port];
    switch(info->hwRxState){
        case HW_RX_IDLE :
            if(rxByte == 0x7E){
                info->hwRxState = HW_RX_RECEIVED_DELIMITER;
            }
            break;
        case HW_RX_RECEIVED_DELIMITER :
            if(rxByte == 0x81){
                info->rxCallBack(port,RX_PACKET_START<<8);
                info->hwRxState = HW_RX_RECEIVE_BYTE;
            }
            else{
                info->hwRxState = HW_RX_IDLE;
            }
            break;
        case HW_RX_RECEIVE_BYTE :
            if(rxByte == 0x7E){
                info->hwRxState = HW_RX_RECEIVE_7E_BYTE;
            }
            else{
                info->rxCallBack(port,rxByte);
            }
            break;
        case HW_RX_RECEIVE_7E_BYTE :
            if(rxByte == 0x81){
                info->rxCallBack(port,(RX_PACKET_START<<8));
                info->hwRxState = HW_RX_RECEIVE_BYTE;
            }
            else if (rxByte == 0xE7){
                info->rxCallBack(port,0x7E);
                info->hwRxState = HW_RX_RECEIVE_BYTE;
            }
            else{
                info->hwRxState = HW_RX_IDLE;
            }
            break;
    }
}

void setHardwareTxLastByte(UsartPort port){
    UsartInfo * info = &usartInfo[port];
    info->lastByte = 1;
}

void endOfUsartTxHandler(UsartPort port){
    UsartInfo * info = &usartInfo[port];
    UsartRegs * usart = info->usart;
    usartDisableInterrupt(usart,TRANS_COMPLETE);
    usartDisableTransmission(usart);
    usartEnableReceiver(usart);
    info->txTurn = 0;
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
