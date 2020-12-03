/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: to control led through EINT and joystick buttons
 * Note(s):
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2017 Politecnico di Torino. All rights reserved.
 *----------------------------------------------------------------------------*/
                  
#include <stdio.h>
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "led/led.h"
#include "button_EXINT/button.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include "Elevator/ElevatorControl.h"
#include "Panel/RequPan.h"
#include "dac/dac.h"
#include "GLCD/GLCD.h"
#include "adc/adc.h"
#include "TouchPanel/TouchPanel.h"
#define FREQ_POLLING_JOYSTICK20Hz 0x4C4B40
#define FREQ_LAMPEGGIO 0x2625A0  // A 5 hz -> 5 volte si accende e 5 volte si spegne!
/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
 

int main (void) {
  
	SystemInit();	/* System Initialization (i.e., PLL)  */
	LED_init();   	/* LED Initialization                 */							
	joystick_init();											/* Joystick Initialization            */
   
	DAC_init();
	   ADC_init();	
	  LCD_Initialization();
    TP_Init();
    TouchPanel_Calibrate();
	  InitElevator();
	   LED_Out(0);
	  InitPanel();
	   
	 	init_RIT(FREQ_POLLING_JOYSTICK20Hz);							/* RIT Initialization 50 msec       	*/
	  enable_RIT(); 
  // RIT enabled	  
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						

   while(1){  __ASM("wfi"); 
	
	   }   /*Loop forever                       */	
		
			
	          
 

}
