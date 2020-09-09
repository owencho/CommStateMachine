#ifndef USARTDRIVER_H
#define USARTDRIVER_H
#include "Event.h"
#include "MemAlloc.h"
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

#define TRANSMITTER_ADDRESS 0
#define STATIC_BUFFER_SIZE 32

typedef enum{
    RX_IDLE,
    RX_ADDRESS_LENGTH,
    RX_RECEIVE_PAYLOAD_STATIC_BUFFER,
    RX_RECEIVE_PAYLOAD_MALLOC_BUFFER,
    RX_WAIT_CRC16_STATIC_BUFFER,
    RX_WAIT_CRC16_MALLOC_BUFFER,
    RX_WAIT_FOR_MALLOC_BUFFER,
    RX_WAIT_FOR_FREE_MALLOC_BUFFER,
} RxHandlerState;

typedef enum{
    TX_IDLE,
    TX_SEND_RECEIVER_ADDRESS,
    TX_SEND_TRANSMITTER_ADDRESS,
    TX_SEND_LENGTH,
    TX_SEND_FLAG,
    TX_SEND_BYTE,
    TX_SEND_CRC16,
} TxHandlerState;

typedef struct UsartDriverInfo UsartDriverInfo;
struct UsartDriverInfo {
    //transmit
    TxCallback txCallBack;
    UsartEvent * txUsartEvent;
    TxHandlerState txState;
    int requestTxPacket;
    int txCounter;
    int txLen;
    int txFlag;
    uint8_t * txBuffer;
    uint8_t txCRC16 [2];
    //receive
    RxCallback rxCallBack;
    UsartEvent * rxUsartEvent;
    RxHandlerState rxState;
    int requestRxPacket;
    int rxCounter;
    int rxLen;
    uint8_t * rxMallocBuffer;
    uint8_t rxStaticBuffer[STATIC_BUFFER_SIZE];
    uint8_t rxCRC16 [2];
    //SM_Common
    SystemEvent sysEvent;
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
