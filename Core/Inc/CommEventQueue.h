#ifndef COMMEVENTQUEUE_H
#define COMMEVENTQUEUE_H
#include "EventQueue.h"
#include "TimerEventQueue.h"
#include "CmdNode.h"

extern EventQueue evtQueue;
extern EventQueue sysQueue;
extern TimerEventQueue timerQueue;
extern CmdNode * rootCmdNode;
#endif // COMMEVENTQUEUE_H
