/*
 * InitUsart.c
 *
 *  Created on: Aug 18, 2020
 *      Author: academic
 */
#include "InitUsart.h"
#include "Gpio.h"
#include "Clock.h"
#include "Nvic.h"
#include "Syscfg.h"
#include "Exti.h"
#include "Rcc.h"
#include "Usart.h"
#include "Common.h"
#include "BaseAddress.h"

void configureGpio(){
	  //button
	  enableGpioA();
	  gpioSetMode(gpioA, PIN_0, GPIO_IN);
	  gpioSetPinSpeed(gpioA,PIN_0,HIGH_SPEED);

	  //set GpioA as alternate mode
	  gpioSetMode(gpioA, PIN_8, GPIO_ALT);
	  gpioSetMode(gpioA, PIN_9, GPIO_ALT);
	  gpioSetMode(gpioA, PIN_10, GPIO_ALT);
	  gpioSetPinSpeed(gpioA,PIN_8,HIGH_SPEED);
	  gpioSetPinSpeed(gpioA,PIN_9,HIGH_SPEED);
	  gpioSetPinSpeed(gpioA,PIN_10,HIGH_SPEED);

	  //set alternate function
	  gpioSetAlternateFunction(gpioA ,PIN_8 ,AF7); //set PA8 as USART1_CK
	  gpioSetAlternateFunction(gpioA ,PIN_9 ,AF7); //set PA9 as USART1_TX
	  gpioSetAlternateFunction(gpioA ,PIN_10 ,AF7); //set PA10 as USART1_RX

	  enableGpio(PORT_C);
	  gpioSetMode(gpioC, PIN_12, GPIO_ALT);  //set GpioC as alternate mode
	  gpioSetPinSpeed(gpioC,PIN_12,HIGH_SPEED);

	  enableGpio(PORT_D);
	  gpioSetMode(gpioD, PIN_2, GPIO_ALT);  //set GpioC as alternate mode
	  gpioSetPinSpeed(gpioD,PIN_2,HIGH_SPEED);

	  enableGpio(PORT_G);
	  gpioSetMode(gpioG, PIN_13, GPIO_OUT);  //set GpioC as alternate mode
	  gpioSetPinSpeed(gpioG,PIN_13,HIGH_SPEED);

	  //set alternate function
	  gpioSetAlternateFunction(gpioC ,PIN_12 ,AF8); //set PC12 as UART5_TX
	  gpioSetAlternateFunction(gpioD ,PIN_2 ,AF8); //set PD2 as UART5_RX

	  //enable interrupt uart5
	  nvicEnableInterrupt(53);
	 //enable interrupt usart1
	  nvicEnableInterrupt(37);

	  enableUSART1();
	  enableUART5();

}

