#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H
#include "Event.h"
typedef struct EventQueue EventQueue ;
struct EventQueue{
    Event * head ;
    Event * tail ;
    int count ;
    Event * previous;
    Event * current ;
};

void eventEnqueue(EventQueue * queue,Event * event);
int  eventDequeue(EventQueue * queue,Event ** event);
#endif // EVENTQUEUE_H
