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
#define PAYLOAD_OFFSET 2

typedef enum{
    LED_CONTROLLER,
    MAIN_CONTROLLER,
} UsartPort;
typedef void (*UsartCallback)(UsartPort port);
typedef char (*TxCallback)(UsartPort port);
typedef void (*RxCallback)(UsartPort port,Char rxByte);
typedef struct UsartInfo UsartInfo;
struct UsartInfo {
    UsartRegs * usart;
    TxCallback txCallBack;
    RxCallback rxCallBack;
    UsartCallback errorCallBack;
    int rxSkip;
    int txTurn;
};

#define getUsartNumber() (sizeof(usartInfo)/sizeof(UsartInfo))
STATIC int findPacketLength(char* data);
STATIC void initUsartHardwareInfo(UsartPort port ,UsartRegs * usart);
void usartHardwareInit();
void usartHardwareConfig(UsartPort port,int baudRate,OversampMode overSampMode,ParityMode parityMode,
                       WordLength length,StopBit sBitMode,EnableDisable halfDuplex);
void hardwareUsartTransmit(UsartPort port);
void hardwareUsartReceive(UsartPort port);
void hardwareUsartSkipReceive(UsartPort port);
void hardwareUsartResetSkipReceive(UsartPort port);
//to init all the variable like stop bit ,usart length = 8bit , half duplex anot and enable usart

void configureGpio();


#endif // USARTHARDWARE_H
