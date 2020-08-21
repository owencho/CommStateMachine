#ifndef TIMEREVENTISR_H
#define TIMEREVENTISR_H
#include "EventQueue.h"
#include "TimerEventQueue.h"
#include "TimerEvent.h"
void timerEventISR(EventQueue * readyQueue,TimerEventQueue *timerEventQueue);
void checkAndDequeueIfNextEventTimerIsZero(EventQueue * readyQueue,TimerEventQueue *timerEventQueue);
#endif // TIMEREVENTISR_H
