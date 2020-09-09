#ifndef MEMALLOC_H
#define MEMALLOC_H

#include "Event.h"

void allocMemForReceiver(Event * data);
void freeMemForReceiver(Event * data);
#define FREE_MEM_REQUEST_EVT 0x1
#define MALLOC_REQUEST_EVT 0x2

#endif // MEMALLOC_H
