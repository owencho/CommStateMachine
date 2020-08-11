#ifndef USARTHARDWARE_H
#define USARTHARDWARE_H
#include "StateMachine.h"

typedef enum{
    LED_CONTROLLER,
    MAIN_CONTROLLER,
} UsartPort;

typedef struct UsartInfo UsartInfo;
struct UsartInfo {
    UsartReg * usart;
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

void hardwareUsartTransmit(UsartPort port,char * txData);
void hardwareUsartReceive(UsartPort port,char * rxBuffer,int length);

void hardwareUsartInit(UsartPort port);
//to init all the variable like stop bit ,usart length = 8bit , half duplex anot and enable usart

void hardwareUsartSetTxCompleteCallback(UsartPort port,Callback callback);
void hardwareUsartSetRxCompleteCallback(UsartPort port,Callback callback);
void hardwareUsartSetErrorCallback(UsartPort port,Callback callback);


#endif // USARTHARDWARE_H
