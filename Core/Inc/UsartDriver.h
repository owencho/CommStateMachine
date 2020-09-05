#ifndef USARTDRIVER_H
#define USARTDRIVER_H
#include "UsartEvent.h"
#include "UsartHardware.h"
#include "TimerEvent.h"
#include "Usart.h"
#include "SM_Common.h"
/*
typedef enum{
    WAIT_FOR_PACKET_HEADER,
    WAIT_FOR_PACKET_PAYLOAD,
} RxHandlerState;
*/
typedef enum{
    RX_IDLE,
    RX_START_DELIMITETER,
    RX_RECEIVE_ADDRESS,
    RX_RECEIVE_LENGTH,
    RX_RECEIVE_PACKET,
    RX_RECEIVED_DELIMETER_PACKET,
    RX_SKIP_PACKET,
} RxHandlerState;

typedef enum{
    TX_IDLE,
    TX_SEND_DELIMITER,
    TX_SEND_ADDRESS,
    TX_SEND_LENGTH,
    TX_SEND_PACKET,
    TX_SEND_CRC,
} TxHandlerState;

typedef struct UsartDriverInfo UsartDriverInfo;
struct UsartDriverInfo {
    UsartEvent * rxUsartEvent;
    UsartEvent * txUsartEvent;
    char * activeRxBuffer;
    char * spareRxBuffer;
    int rxCounter;
    int txCounter;
    int rxLen;
    int txLen;
    RxHandlerState rxState;
    TxHandlerState txState;
    int requestTxPacket;
    int requestRxPacket;
};

STATIC char * getRxPacket(UsartDriverInfo *info);
STATIC int getPacketLength(char * txData);
STATIC int isCorrectAddress(UsartDriverInfo *info);

void usartInit();
void usartConfig(UsartPort port,int baudRate,OversampMode overSampMode,ParityMode parityMode,
               WordLength length,StopBit sBitMode,EnableDisable halfDuplex);
//need to add more config inside
void usartDriverTransmit(UsartPort port, char * txData,UsartEvent * event);
void usartDriverReceive(UsartPort port, char * txData,UsartEvent * event);
// the data store is retrieved in the UsartEvent

void usartRemoveReceivefromQueue(UsartPort port , UsartEvent * event);
void usartRemoveReceiveTimer(UsartPort port , TimerEvent * event);
// hardwareHandler
void usartReceiveHandler(UsartPort port,char rxByte);
char usartTransmissionHandler(UsartPort port);
/*
void usartRxCompletionHandler(UsartPort port);
void usartTxCompletionHandler(UsartPort port);
*/
void usartHandleRxByte(char rx);
#endif // USARTDRIVER_H
