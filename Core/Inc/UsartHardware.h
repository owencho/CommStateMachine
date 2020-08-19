#ifndef USARTHARDWARE_H
#define USARTHARDWARE_H
#include "StateMachine.h"
#include "Usart.h"
//This is user define configuration
#define USART_ADDRESS 0x34
#define PACKET_HEADER_SIZE 2
#define PACKET_ADDRESS_OFFSET 0
#define LENGTH_ADDRESS_OFFSET 1

typedef enum{
    LED_CONTROLLER,
    MAIN_CONTROLLER,
} UsartPort;

typedef struct UsartInfo UsartInfo;
struct UsartInfo {
    UsartRegs * usart;
    //FuncPtr apbclk
    Callback txCompleteCallBack;
    Callback rxCompleteCallBack;
    Callback errorCallBack;
    char * txBuffer;
    char  txlen;
    char * rxBuffer;
    char  rxlen;
};

#define getUsartNumber() (sizeof(usartInfo)/sizeof(UsartInfo))
void usartHardwareInit(UsartPort port,OversampMode overSampMode,ParityMode parityMode,WordLength length,StopBit sBitMode);
void hardwareUsartTransmit(UsartPort port,char * txData);
void hardwareUsartReceive(UsartPort port,char * rxBuffer,int length);
//to init all the variable like stop bit ,usart length = 8bit , half duplex anot and enable usart

void hardwareUsartSetTxCompleteCallback(UsartPort port,Callback callback);
void hardwareUsartSetRxCompleteCallback(UsartPort port,Callback callback);
void hardwareUsartSetErrorCallback(UsartPort port,Callback callback);


#endif // USARTHARDWARE_H
