#include "UsartDriver.h"
#include "UsartHardware.h"
#include <stdlib.h>
static UsartDriverInfo * usartDriverInfo;
static char * lengthAddress;
static int usartLength = 0 ;
static rxHandleState handleSM = WAIT_FOR_LENGTH ;

// get UsartNumber from driver
// calloc sizeof(UsartDriverInfo)*UsartNumber
// call hardwareUsartReceive(port,activeBuffer,2) wait for the data
void usartInit(UsartPort port){ // and other config setting
    disableIRQ();
    //config
    usartDriverInfo = (UsartDriverInfo *)calloc(sizeof(UsartDriverInfo)*port);
    hardwareUsartReceive(port,usartDriverInfo[port].activeBuffer,2);

    enableIRQ();
}

void usartTransmit(UsartPort port, char * txData,UsartEvent * event){
    hardwareUsartTransmit(port,txData);
    usartDriverInfo[port].txUsartEvent = event;
    //enqueue
}

void usartReceive(UsartPort port, char * rxBuffer,UsartEvent * event){
    //disable irq
    if(usartDriverInfo[port].rxUsartEvent == NULL){
        hardwareUsartReceive(port,usartDriverInfo[port].activeRxBuffer,2);
        usartDriverInfo[port].spareRxBuffer = rxBuffer;
        usartDriverInfo[port].rxUsartEvent = event;
        handleSM = WAIT_FOR_PACKET;
        //TimerEventRequest
    }else{
        lengthAddress = usartDriverInfo[port].activeRxBuffer+1;
        usartLength = *lengthAddress;
        hardwareUsartReceive(port,usartDriverInfo[port].activeRxBuffer+2,usartLength);
        usartDriverInfo[port].spareRxBuffer = rxBuffer;
    }
    //enable irq
}
// the data store is retrieved in the UsartEvent

void usartRemoveReceivefromQueue(UsartPort port , UsartEvent * event);

// set rxUsartEvent = NULL
void usartRemoveReceiveTimer(UsartPort port , TimerEvent * event);

//this two function will call by interrupt request

void __usartTxCompletionHandler(UsartPort port){
  // txUsartEvent in the queue
}
//StateMachine
void __usartRxCompletionHandler(UsartPort port){
    disableIRQ();
    switch (handleSM) {
      case WAIT_FOR_LENGTH:
          hardwareUsartReceive(port,usartDriverInfo[port].activeRxBuffer,2);
          handleSM = WAIT_FOR_PACKET;
      break;

      case WAIT_FOR_PACKET:
          lengthAddress = usartDriverInfo[port].activeRxBuffer+1;
          usartLength = *lengthAddress;
          hardwareUsartReceive(port,usartDriverInfo[port].activeRxBuffer+2,usartLength);
          /*
          if(address same)
              if(rxUsartEvent is NULL where the receive function is not called)
                  //we might need to store in a buffer
              usartDriverInfo[port].rxUsartEvent->buffer = usartDriverInfo[port].activeRxBuffer;
              usartDriverInfo[port].activeRxBuffer = usartDriverInfo[port].spareRxBuffer;
              enqueue event to the queue
          */

          usartDriverInfo[port].rxUsartEvent = NULL;
          handleSM = WAIT_FOR_LENGTH;
      break;

    }
    enableIRQ();
}
/*
//disableIrq
// check rxUsartEvent is NULL anot (if NULL we dont enqueue it)
// globalEvent in the queue
//wait for two byte , get the length
// then
if(usartDriverInfo[port].rxUsartEvent ==NULL){
    // dont enqueue
}
hardwareUsartReceive(port,usartDriverInfo[port].activeRxBuffer+2,length);

//after we received all the data
//if correct then we enqueue
//if wrong then go back wait for length

// if the address is wrong
// we will start to count until the length ends
// we will read but not write

//after we receive the whole packet
//we will return activeBuffer by putting into the eventQueue where the event hold the activeBuffer
usartDriverInfo[port].rxUsartEvent->buffer = usartDriverInfo[port].activeRxBuffer;
usartDriverInfo[port].activeRxBuffer = usartDriverInfo[port].spareRxBuffer;
//call hardwareUsartReceive(port,activeBuffer,2)
//spareRxBuffer will be activeRxBuffer
*/
