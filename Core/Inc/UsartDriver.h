#ifndef USARTDRIVER_H
#define USARTDRIVER_H
#include "UsartEvent.h"
#include "UsartHardware.h"
#include "TimerEvent.h"

typedef struct UsartDriverInfo UsartDriverInfo;
struct UsartDriverInfo {
    UsartEvent * rxUsartEvent;
    UsartEvent * txUsartEvent;
    char * activeRxBuffer;
    char * spareRxBuffer;
    int rxHandlerState;
    int isReceiveEvent;
};

typedef enum{
    WAIT_FOR_LENGTH,
    WAIT_FOR_PACKET,
} rxHandlerState;

void usartInit(UsartPort port);
//need to add more config inside
void usartDriverTransmit(UsartPort port, char * txData,UsartEvent * event);
void usartDriverReceive(UsartPort port, char * txData,UsartEvent * event);
// the data store is retrieved in the UsartEvent

void usartRemoveReceivefromQueue(UsartPort port , UsartEvent * event);
void usartRemoveReceiveTimer(UsartPort port , TimerEvent * event);

void __usartRxCompletionHandler(UsartPort port);
void __usartTxCompletionHandler(UsartPort port);
#endif // USARTDRIVER_H
