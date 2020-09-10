#ifndef USARTHARDWARE_H
#define USARTHARDWARE_H
#include "StateMachine.h"
#include "Usart.h"
#include <stdint.h>
#include "SM_Common.h"
//This is user define configuration
#define USART_ADDRESS 0x34
#define PACKET_HEADER_SIZE 2
#define RECEIVER_ADDRESS_OFFSET 0
#define SENDER_ADDRESS_OFFSET 1
#define LENGTH_OFFSET 2
#define PAYLOAD_OFFSET 3

#define RX_PACKET_START 0x11

typedef enum{
    LED_CONTROLLER,
    MAIN_CONTROLLER,
} UsartPort;

typedef enum{
    HW_TX_IDLE,
    HW_TX_SEND_DELIMITER,
    HW_TX_SEND_BYTE,
    HW_TX_SEND_7E_BYTE,
} UsartHardwareTxState;

typedef enum{
    HW_RX_IDLE,
    HW_RX_RECEIVED_DELIMITER,
    HW_RX_RECEIVE_BYTE,
    HW_RX_RECEIVE_7E_BYTE,
} UsartHardwareRxState;

typedef void (*UsartCallback)(UsartPort port);
typedef uint8_t (*TxCallback)(UsartPort port);
typedef void (*RxCallback)(UsartPort port,uint16_t rxByte);

typedef struct UsartInfo UsartInfo;
struct UsartInfo {
    UsartRegs * usart;
    TxCallback txCallBack;
    RxCallback rxCallBack;
    UsartCallback errorCallBack;
    UsartHardwareTxState hwTxState;
    UsartHardwareRxState hwRxState;
    int txTurn;
    int lastByte;
};

#define getUsartNumber() (sizeof(usartInfo)/sizeof(UsartInfo))
STATIC int findPacketLength(char* data);
STATIC void initUsartHardwareInfo(UsartPort port ,UsartRegs * usart);
void usartHardwareInit();
void usartHardwareConfig(UsartPort port,int baudRate,OversampMode overSampMode,ParityMode parityMode,
                       WordLength length,StopBit sBitMode,EnableDisable halfDuplex);

void hardwareUsartTransmit(UsartPort port);
void hardwareUsartReceive(UsartPort port);
void usartIrqHandler(UsartPort port);
uint8_t usartTransmitHardwareHandler(UsartPort port);
void usartReceiveHardwareHandler(UsartPort port,uint8_t rxByte);
void setHardwareTxLastByte(UsartPort port);
void endOfUsartTxHandler(UsartPort port);
void configureGpio();


#endif // USARTHARDWARE_H
