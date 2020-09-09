/*
 * StateMachine.h
 *
 *  Created on: Jun 25, 2020
 *      Author: academic
 */

#ifndef INC_STATEMACHINE_H_
#define INC_STATEMACHINE_H_

//#include "Event.h"
typedef void (*Callback)(void * data);
typedef void (*UsartCallback)(UsartPort port);
typedef uint8_t (*TxCallback)(UsartPort port);
typedef void (*RxCallback)(UsartPort port,uint8_t rxByte);

typedef struct GenericStateMachine GenericStateMachine;
// all state machines should have this generic data struct
struct GenericStateMachine {
	Callback callback;
	// specifics data of the state machine should be written here
	// all the state machine like blinky should follow generic state machine
	// after callback will have data for specific stateMachine
};

#endif /* INC_STATEMACHINE_H_ */
