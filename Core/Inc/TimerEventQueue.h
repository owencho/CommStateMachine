#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H
#include "TimerEvent.h"
typedef struct TimerEventQueue TimerEventQueue ;
struct TimerEventQueue{
    TimerEvent * head ;
    TimerEvent * tail ;
    int count ;
    TimerEvent * previous;
    TimerEvent * current ;
    int relativeTick;
};

void timerEventRequest (TimerEventQueue * timerEventQueue,TimerEvent * event,int expiryPeriod);
void resetTick(TimerEventQueue * timerEventQueue);
void incTick(TimerEventQueue * timerEventQueue);
void * timerEventDequeue(TimerEventQueue * timerEventQueue);
void checkAndAddTimerEvent(TimerEventQueue * timerEventQueue,TimerEvent * event);
void timerEventEnqueue(TimerEventQueue * timerEventQueue,TimerEvent * event);
void * timerEventDequeueSelectedEvent(TimerEventQueue * timerEventQueue,TimerEvent * deleteEvent);
int timerEventQueueGetRelativeTick(TimerEventQueue * timerEventQueue);
int timerEventQueueGetCount(TimerEventQueue * timerEventQueue);
TimerEvent * timerEventQueueGetCurrentEvent(TimerEventQueue * timerEventQueue);
void resetCurrentTimerEventQueue(TimerEventQueue * timerEventQueue);
TimerEvent * timerEventQueueGetNextEvent(TimerEventQueue * timerEventQueue);
#endif // TIMERQUEUE_H
