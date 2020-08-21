#ifndef USARTHARDWARE_H
#define USARTHARDWARE_H
#include "StateMachine.h"
#include "Usart.h"
#include "SM_Common.h"
//This is user define configuration
#define USART_ADDRESS 0x34
#define PACKET_HEADER_SIZE 2
#define PACKET_ADDRESS_OFFSET 0
#define LENGTH_ADDRESS_OFFSET 1

typedef enum{
    LED_CONTROLLER,
    MAIN_CONTROLLER,
} UsartPort;

typedef void (*UsartCallback)(UsartPort port);

typedef struct UsartInfo UsartInfo;
struct UsartInfo {
    UsartRegs * usart;
    UsartCallback txCompleteCallBack;
    UsartCallback rxCompleteCallBack;
    UsartCallback errorCallBack;
    int txTurn;
    char * txBuffer;
    char  txlen;
    char * rxBuffer;
    char  rxlen;
};

#define getUsartNumber() (sizeof(usartInfo)/sizeof(UsartInfo))
STATIC int findPacketLength(char* data);
STATIC void initUsartHardwareInfo(UsartPort port ,UsartRegs * usart);
void usartHardwareInit();
void usartHardwareConfig(UsartPort port,int baudRate,OversampMode overSampMode,ParityMode parityMode,
                       WordLength length,StopBit sBitMode,EnableDisable halfDuplex);
void hardwareUsartTransmit(UsartPort port,char * txData);
void hardwareUsartReceive(UsartPort port,char * rxBuffer,int length);
//to init all the variable like stop bit ,usart length = 8bit , half duplex anot and enable usart

void hardwareUsartSetTxCompleteCallback(UsartPort port,UsartCallback callback);
void hardwareUsartSetRxCompleteCallback(UsartPort port,UsartCallback callback);
void hardwareUsartSetErrorCallback(UsartPort port,UsartCallback callback);
void configureGpio();


#endif // USARTHARDWARE_H
