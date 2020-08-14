#ifndef USARTEVENT_H
#define USARTEVENT_H

#include "EventType.h"
#include "StateMachine.h"
typedef struct UsartEvent UsartEvent;
struct UsartEvent {
    UsartEvent * next;
    EventType type;
    GenericStateMachine *sm;
    char * buffer;
};

#endif // USARTEVENT_H
