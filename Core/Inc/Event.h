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

#endif // EVENT_H
