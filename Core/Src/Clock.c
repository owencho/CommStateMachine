/*
 * Clock.c
 *
 *  Created on: Jul 16, 2020
 *      Author: academic
 */

#include "Clock.h"
#include "stm32f4xx_hal.h"

uint32_t getPCLK2Clock(){
	return HAL_RCC_GetPCLK2Freq();
}
uint32_t getPCLK1Clock(){
	return HAL_RCC_GetPCLK1Freq();
}
