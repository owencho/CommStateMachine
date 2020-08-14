#include "UsartDriver.h"
#include "UsartHardware.h"
#include "List.h"
#include "Irq.h"
#include "UsartEvent.h"
#include "UsartQueue.h"
#include "TimerEventQueue.h"
#include <stdlib.h>

static UsartQueue * usartQueue;
static UsartDriverInfo * usartDriverInfo;
static int isReceiveEvent = 0 ;
static int usartLength = 0 ;
static int usartAddress = 0 ;

// get UsartNumber from driver
// calloc sizeof(UsartDriverInfo)*UsartNumber
// call hardwareUsartReceive(port,activeBuffer,2) wait for the data
void usartInit(UsartPort port){ // and other config setting
    disableIRQ();
    //config
    usartHardwareInit();
    //usartSetBaudRate(UsartRegs* usart,uint32_t baudRate);
    usartDriverInfo = (UsartDriverInfo *)calloc(port+1,sizeof(UsartDriverInfo));
    __usartRxCompletionHandler(port);
    //hardwareUsartReceive(port,usartDriverInfo[port].activeBuffer,2);

    enableIRQ();
}

void usartDriverTransmit(UsartPort port, char * txData,UsartEvent * event){
    if(usartDriverInfo[port].txUsartEvent == NULL){
        hardwareUsartTransmit(port,txData);
        usartDriverInfo[port].txUsartEvent = event;
        //enqueue on normal EventQueue
    }
}

void usartDriverReceive(UsartPort port, char * rxBuffer,UsartEvent * event){
    disableIRQ();
    if(usartDriverInfo[port].isReceiveEvent){
        usartDriverInfo[port].rxUsartEvent->buffer = usartDriverInfo[port].activeRxBuffer;
        usartDriverInfo[port].activeRxBuffer = usartDriverInfo[port].spareRxBuffer;
        //eventEnQueue() on normal EventQueue
    }
    else if(usartDriverInfo[port].rxUsartEvent == NULL){
        hardwareUsartReceive(port,usartDriverInfo[port].activeRxBuffer,2);
        usartDriverInfo[port].spareRxBuffer = rxBuffer;
        usartDriverInfo[port].rxUsartEvent = event;
        //handleSM = WAIT_FOR_PACKET;
        //TimerEventRequest
    }else{
        lengthAddress = usartDriverInfo[port].activeRxBuffer+1;
        usartLength = *lengthAddress;
        hardwareUsartReceive(port,usartDriverInfo[port].activeRxBuffer+2,usartLength);
        usartDriverInfo[port].spareRxBuffer = rxBuffer;
    }
    enableIRQ();
}
// the data store is retrieved in the UsartEvent


void usartTxCompletionHandler(UsartPort port){
    //eventEnQueue()
    usartDriverInfo[port].txUsartEvent = NULL;
}
//StateMachine
void usartRxCompletionHandler(UsartPort port){
    UsartDriverInfo * info = usartDriverInfo[port];
    uint8_t * packet = info->activeRxBuffer;
    char * lengthAddress;
    disableIRQ();

    switch (info->rxHandlerState) {
      case WAIT_FOR_LENGTH:
          hardwareUsartReceive(port,packet,2);
          info->rxHandlerState = WAIT_FOR_PACKET;
      break;

      case WAIT_FOR_PACKET:
          hardwareUsartReceive(port,
                              getPacketPayloadAddress(packet),
                              getPacketLength(packet));

      case COMPLETE_RECEIVE:

          if(getPacketAddress(packet) == USART_ADDRESS)
              if(info->rxUsartEvent == NULL){
                  //rxUsartEvent is NULL where the receive function is not called
                  //we might need to store in a buffer
                  info->isReceiveEvent = 1;
              }
              else {
                  info->rxUsartEvent->buffer = packet;
                  info->activeRxBuffer = info->spareRxBuffer;
                  //eventEnQueue()
                  //enqueue event to the queue
              }


          info->rxUsartEvent = NULL;
          info->rxHandlerState = WAIT_FOR_LENGTH;
      break;

    }
    enableIRQ();
}
//this two function will call by interrupt request

void usartRemoveReceivefromQueue(UsartPort port , UsartEvent * event){
    //deleteSelectedListItem(List * linkedList,(void*)event,LinkedListCompare compare);
    usartDriverInfo[port].rxUsartEvent = NULL;
}

// set rxUsartEvent = NULL
void usartRemoveReceiveTimer(UsartPort port , TimerEvent * event){
    //timerEventDequeueSelectedEvent(TimerEventQueue * timerEventQueue,event);
    usartDriverInfo[port].rxUsartEvent = NULL;
}
