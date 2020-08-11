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
};

typedef enum{
    WAIT_FOR_LENGTH,
    WAIT_FOR_PACKET,
} rxHandlerState;

void usartInit(UsartPort port,...);
void usartTransmit(UsartPort port, char * txData,UsartEvent * event);
void usartReceive(UsartPort port, char * txData,UsartEvent * event);
// the data store is retrieved in the UsartEvent

void usartRemoveReceivefromQueue(UsartPort port , UsartEvent * event);
void usartRemoveReceiveTimer(UsartPort port , TimerEvent * event);

void __usartRxHandler();
void __usartTxHandler();
#endif // USARTDRIVER_H
