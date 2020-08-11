#ifndef USARTEVENT_H
#define USARTEVENT_H

typedef struct UsartEvent UsartEvent;
struct UsartEvent {
    UsartEvent * next;
    EventType type;
    GenericStateMachine *sm;
    char * buffer;
};

#endif // USARTEVENT_H
