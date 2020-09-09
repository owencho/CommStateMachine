#ifndef EVENT_H
#define EVENT_H
#include <stdint.h>
#include <stddef.h>
#include "StateMachine.h"
#include "EventType.h"

typedef struct Event Event;

struct Event {
  	Event * next;
  	EventType type;
  	GenericStateMachine * stateMachine;
    void * data;
};

typedef struct UsartEvent UsartEvent;
struct UsartEvent {
    UsartEvent * next;
    EventType type;
    GenericStateMachine *sm;
    uint8_t * buffer;
};

typedef struct SystemEvent SystemEvent;
struct SystemEvent {
    SystemEvent * next;
    EventType type;
    GenericStateMachine * stateMachineInfo;
    void * data;
};

#endif // EVENT_H
