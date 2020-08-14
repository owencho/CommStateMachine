/*
 * Irq.c
 *
 *  Created on: Jun 27, 2020
 *      Author: owen
 */

#include "stm32f4xx_hal.h"
#include "Irq.h"

int disableIrqCount = 0 ;
void disableIRQ(){
    if(disableIrqCount == 0)
        __disable_irq();
    disableIrqCount++;
}
void enableIRQ(){
  disableIrqCount--;
  if(disableIrqCount == 0)
      __enable_irq();
}
