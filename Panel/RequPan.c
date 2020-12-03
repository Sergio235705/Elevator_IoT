/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           RequPanel.c
** Last modified Date:  2020/01/02
** Last Version:        V1.00
** Descriptions:        High level Panel management functions
** Correlated files:    RequPan.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "RequPan.h"
#include "../led/led.h"
#include "../button_EXINT/button.h"
#include "../Elevator/ElevatorControl.h"
#include "../GLCD/GLCD.h"

/*----------------------------------------------------------------------------
  Function that Turn On/Off Led on Panel
 *----------------------------------------------------------------------------*/
int PressButton(int num)  // Gestisco i vari casi dei bottoni
{
	if(( StatusElevator()|| EmergencyMode()) && !MaintenanceMode())
	{	
		if(EmergencyMode())    // Non ho implementato il debouncing in quanto 
		    {SetEmergencyMode(0);// non interessa la quantità di volte in cui un bottone è pressato 
				 ActiveElevator(num+2);// ma semplicemente se è stato pressato almeno una volta 
				 LedAlarmOff();
				return 1;}
		else if ( ((!num && StatusFloor())||(num && !StatusFloor())) && StatusElevator() && !GetReserved())	// Chiamato da altro piano
		{ActiveElevator(1);  			//Attivo Automatico    		
	 TurnOnPanel();
		return 1;}
		else if ( num == StatusFloor() && StatusElevator() && !GetReserved()) //Prenoto dal mio stesso piano
		{ ActiveElevator(0);
			TurnOnPanel();
			return 1;
		}
	}
	  return 0;
	 }  
 

void InitPanel(void)
{ BUTTON_init();}
void TurnOnPanel(void)
{		LED_Out(0);
	  LED_On(0);                                 
		LED_On(2);
	  LedAlarmOff();
}
void TurnOffPanel(void )
	{ LED_Off(0);
		LED_Off(2);
		 LedAlarmOff();
   }
void LedAlarmOn(void)
	{ LED_On(1);
		LED_On(3);
}
void LedAlarmOff(void)
	{ LED_Off(1);
		LED_Off(3);
}
/******************************************************************************
                           End Of File                                *****/ 