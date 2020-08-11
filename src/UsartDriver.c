#include "UsartDriver.h"
#include "UsartHardware.h"
static UsartDriverInfo * usartDriverInfo;
// can use usartDriverInfo[0] to access the value
//rxHandlerState; // enum
// 0 w for length
// 1 w for packet
void usartInit(UsartPort port,...);
// get UsartNumber from driver
// calloc sizeof(UsartDriverInfo)*UsartNumber
// call hardwareUsartReceive(port,activeBuffer,2) wait for the data


void usartTransmit(UsartPort port, char * txData,UsartEvent * event){
    hardwareUsartTransmit(port,txData);
    txUsartEvent = event;
}

void usartReceive(UsartPort port, char * rxBuffer,UsartEvent * event){
    //disable irq
    if(activeRxBuffer == NULL){
      hardwareUsartReceive(port,rxBuffer,2);
      usartDriverInfo[port].spareRxBuffer = rxBuffer;
      rxUsartEvent = event;
    }else{

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
  // check rxUsartEvent is NULL anot (if NULL we dont enqueue it)
  // globalEvent in the queue
  //wait for two byte , get the length
  // then
  hardwareUsartReceive(port,activeRxBuffer+2,length);

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
}
