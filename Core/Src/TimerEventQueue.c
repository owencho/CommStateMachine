#include "TimerEventQueue.h"
#include "List.h"
#include "LinkedListCompare.h"
#include "EventCompare.h"
#include "Irq.h"

TimerEvent * currentTimerEventItem;
TimerEvent * timerEventItem;
void incTick(TimerEventQueue * timerEventQueue){
		disableIRQ();
		if(timerEventQueue == NULL)
				return;
		if(timerEventQueue->count != 0)
				timerEventQueue->relativeTick++;
		enableIRQ();
}

void resetTick(TimerEventQueue * timerEventQueue){
		disableIRQ();
		if(timerEventQueue == NULL)
				return;
		timerEventQueue->relativeTick = 0;
		enableIRQ();

}

void checkAndAddTimerEvent(TimerEventQueue * timerEventQueue,TimerEvent * event){
		if(timerEventQueue == NULL || event== NULL)
				return;
		event->time = event->time + timerEventQueue->relativeTick;
		resetCurrentListItem((List*)timerEventQueue);
		currentTimerEventItem =(TimerEvent*) getCurrentListItem((List*)timerEventQueue);
		if(currentTimerEventItem == NULL){
				return;
		}
		if(currentTimerEventItem->time > event->time){
				currentTimerEventItem->time = currentTimerEventItem->time - event->time;
				listAddItemToHead((List*)timerEventQueue,(ListItem*)event);
		}
		else{
				timerEventItem = (TimerEvent*)findListItem((List*)timerEventQueue,event
									,(LinkedListCompare)eventCompareForAddingTimeEvent);
				// new time delay for next Time Event is old time delay - previous delay time
				if(timerEventItem->next != NULL)
						timerEventItem->next->time = timerEventItem->next->time - event->time ;

				listAddItemToNext((List*)timerEventQueue,
													(ListItem*)timerEventItem,(ListItem*)event);
		}
}
void * timerEventDequeue(TimerEventQueue * timerEventQueue){
		TimerEvent * deletedTimerEvent;
		disableIRQ();
  	//disable interrupt put here to protect data from race condition
 		if(timerEventQueue->count ==0){
    		enableIRQ();
      	return NULL;
    }
    resetCurrentListItem((List*)timerEventQueue);
		deletedTimerEvent =(TimerEvent*)getCurrentListItem((List*)timerEventQueue);
    deleteHeadListItem((List*)timerEventQueue);
		resetTick(timerEventQueue);
    enableIRQ();
    return deletedTimerEvent;
}

void * timerEventDequeueSelectedEvent(TimerEventQueue * timerEventQueue,TimerEvent * deleteEvent){
		TimerEvent * deletedTimerEvent;
		int relativeTick;
		disableIRQ();
		if(timerEventQueue==NULL ||deleteEvent == NULL){
				enableIRQ();
				return NULL;
		}
		relativeTick = timerEventQueueGetRelativeTick(timerEventQueue);
		deletedTimerEvent = (TimerEvent*)findListItem((List*)timerEventQueue,(void*)deleteEvent
													,(LinkedListCompare)eventCompareSameTimeEvent);

		if(deletedTimerEvent == NULL){
				resetCurrentTimerEventQueue(timerEventQueue);
				enableIRQ();
				return NULL;
		}
		else if(deletedTimerEvent == timerEventQueue->head){
				deletedTimerEvent->next->time = deletedTimerEvent->next->time + deletedTimerEvent->time-relativeTick;
				resetTick(timerEventQueue);
		}
		else if(deletedTimerEvent != timerEventQueue->tail){
				deletedTimerEvent->next->time = deletedTimerEvent->next->time + deletedTimerEvent->time;
		}
		deletedTimerEvent = deleteSelectedListItem((List*)timerEventQueue,(void*)deletedTimerEvent,
													(LinkedListCompare)eventCompareSameTimeEvent);
    enableIRQ();
    return deletedTimerEvent;
}

void timerEventRequest (TimerEventQueue * timerEventQueue,TimerEvent * event,int expiryPeriod){
		if(event == NULL||timerEventQueue==NULL)
				return ;
		event->time =expiryPeriod ;
		timerEventEnqueue(timerEventQueue,event);
}

void timerEventEnqueue(TimerEventQueue * timerEventQueue,TimerEvent * event){
		disableIRQ();
		if(timerEventQueue == NULL || event == NULL){
				enableIRQ();
				return ;
		}
		if(timerEventQueue->count == 0){
				resetTick(timerEventQueue);
				listAddItemToTail((List*)timerEventQueue,(ListItem*)event);
		}
		else{
				checkAndAddTimerEvent(timerEventQueue,event);
		}
		enableIRQ();
}

int timerEventQueueGetRelativeTick(TimerEventQueue * timerEventQueue){
		disableIRQ();
		int relativeTick;
		if(timerEventQueue == NULL){
				enableIRQ();
				return 0;
		}
		relativeTick = timerEventQueue->relativeTick;
		enableIRQ();
		return relativeTick;
}

int timerEventQueueGetCount(TimerEventQueue * timerEventQueue){
		disableIRQ();
		int count;
		if(timerEventQueue == NULL){
				enableIRQ();
				return 0 ;
		}
		count = timerEventQueue->count;
		enableIRQ();
		return count;
}

TimerEvent * timerEventQueueGetCurrentEvent(TimerEventQueue * timerEventQueue){
		disableIRQ();
		TimerEvent*currentEvent;
		if(timerEventQueue == NULL){
				enableIRQ();
				return NULL ;
		}
		currentEvent = timerEventQueue->current;
		enableIRQ();
		return currentEvent;
}

TimerEvent * timerEventQueueGetNextEvent(TimerEventQueue * timerEventQueue){
		disableIRQ();
		TimerEvent*currentEvent;
		if(timerEventQueue == NULL){
				enableIRQ();
				return NULL ;
		}
		currentEvent = (TimerEvent * )getNextListItem((List*)timerEventQueue);
		enableIRQ();
		return currentEvent;
}

void resetCurrentTimerEventQueue(TimerEventQueue * timerEventQueue){
		disableIRQ();
		if(timerEventQueue == NULL){
				enableIRQ();
				return ;
		}
		resetCurrentListItem((List*)timerEventQueue);
		enableIRQ();
}
