#ifndef TIMEREVENTISR_H
#define TIMEREVENTISR_H
#include "TcbQueue.h"
#include "ThreadContext.h"
#include "TimerEventQueue.h"
#include "TimerEvent.h"
void timerEventISR(TcbQueue * readyQueue,TimerEventQueue *timerEventQueue);
void checkAndDequeueIfNextEventTimerIsZero(TcbQueue * readyQueue,TimerEventQueue *timerEventQueue);
#endif // TIMEREVENTISR_H
