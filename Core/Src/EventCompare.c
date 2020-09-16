/*
 * EventCompare.c
 *
 *  Created on: Jun 28, 2020
 *      Author: owen
 */

#include "EventCompare.h"
int eventCompareForAddingTimeEvent(TimerEvent *currentEvent, TimerEvent * newEvent){
	if(currentEvent == NULL || newEvent == NULL)
		return 0;

	newEvent->time = newEvent->time - currentEvent->time;
	if(currentEvent->next == NULL){
		return 1 ;
	}

	else if(currentEvent->next->time > newEvent->time ){
		return 1;
	}
	else{
		return 0;
	}
}

int eventCompareSameTimeEvent(TimerEvent *currentEvent, Event * deleteEvent){
	Event * event = currentEvent->data;
	if(event == NULL || deleteEvent == NULL)
		return 0;

	if(event == deleteEvent){
		return 1 ;
	}
	else{
		return 0;
	}
}
