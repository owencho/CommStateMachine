#ifndef USARTQUEUE_H
#define USARTQUEUE_H

#include "UsartEvent.h"
typedef struct UsartQueue UsartQueue ;
struct UsartQueue{
    UsartEvent * head ;
    UsartEvent * tail ;
    int count ;
    UsartEvent * previous;
    UsartEvent * current ;
};

void usartEventEnqueue(UsartQueue * queue,UsartEvent * event);
int usartEventDequeue(UsartQueue * queue,UsartEvent ** event);
#endif // USARTQUEUE_H
