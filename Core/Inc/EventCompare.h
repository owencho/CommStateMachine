/*
 * EventCompare.h
 *
 *  Created on: Jun 28, 2020
 *      Author: owen
 */

#ifndef INC_EVENTCOMPARE_H_
#define INC_EVENTCOMPARE_H_
#include "Event.h"
#include <stdlib.h>
#include "TimerEvent.h"
int eventCompareForAddingTimeEvent (TimerEvent *currentEvent, TimerEvent * newEvent);
int eventCompareSameTimeEvent(TimerEvent *currentEvent, TimerEvent * deleteEvent);
#endif /* INC_EVENTCOMPARE_H_ */
