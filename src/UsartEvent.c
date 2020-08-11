#include "UsartEvent.h"

void usartInit(UsartPort port,...);
void usartTransmit(UsartPort port, char * txData,UsartEvent * event);
void usartReceive(UsartPort port, char * txData,UsartEvent * event);
// the data store is retrieved in the UsartEvent

void usartRemoveReceivefromQueue(UsartPort port , UsartEvent * event);
void usartRemoveReceiveTimer(UsartPort port , TimerEvent * event);
