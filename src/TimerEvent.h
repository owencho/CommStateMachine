/*
 * TimerEvent.h
 *
 *  Created on: Jun 25, 2020
 *      Author: academic
 */

#ifndef INC_TIMEREVENT_H_
#define INC_TIMEREVENT_H_

#include <stdint.h>
#include <stddef.h>

typedef struct TimerEvent TimerEvent;
struct TimerEvent {
		TimerEvent * next;
		void * data;
};

#endif /* INC_TIMEREVENT_H_ */
