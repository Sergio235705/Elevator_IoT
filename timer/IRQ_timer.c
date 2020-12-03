/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "timer.h"
#include "../led/led.h"
#include "../Elevator/ElevatorControl.h"
#include "../Panel/RequPan.h"
#include "../TouchPanel/TouchPanel.h"
#include "../dac/dac.h"
/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern unsigned char Menu;
extern unsigned char PaginaDef;
extern unsigned char Save;
extern unsigned char ModifNota;
extern unsigned char Ascolto;
extern unsigned char FasediAscolto;
extern unsigned char Button;

 
void TIMER0_IRQHandler (void)  //Timer lampeggio a 5 hz
{ 
	static int i=0;
	if(i%2==0)
  LED_On(7);
	else
	LED_Off(7);
  i++;
	if(i==30)              /*Lampeggio 3 secondi , 10 volte al secondo -> 30*/
	{disable_timer(0);
		reset_timer(0);
		LED_Off(7);
		i=0;
		DeActiveElevator();  /* Fase di lampeggio a 3 Hz spenta*/
	}
		
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
uint16_t SinTable[45] =                                       /* ÕýÏÒ±í                       */
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};
uint16_t value;
void TIMER1_IRQHandler (void)            //Timer Touch Screen
{ 
	getDisplayPoint(&display, Read_Ads7846(), &matrix ) ;
  
	if(Button)
	{if(TouchedArea(90,150,130,190))
		 { DelayUS(10000);
			 Ascolto=0;
			 ShowMenu();
			
			 return;}}
		 
	if(Menu )
	{
		if(TouchedArea(198,239,0,40)) //freccia
		{  SetUscitaMenu(1);
			 Button=1;
			 SetFaseSceltaNota(0);
			Menu=0;
			TurnOnButtonMaintenance();
		return;}
		if(TouchedArea(160,239,120,180) && Ascolto==1) //Ascolto
		{ SetFaseSceltaNota(0);
			AttivaAudio();	
 			Menu=0;	
		//  SetFaseSceltaNota(0);
			 return;}
		if ((TouchedArea(0,239,60,150) && Ascolto==0) || ((TouchedArea(0,160,120,150) || TouchedArea(0,239,60,120)) && Ascolto==1))//nota1	
		{SetFaseSceltaNota(1);
			ShowModificaNota(1);	
			
			
			Menu=0;

		return;}
		if((TouchedArea(0,239,151,234) && Ascolto==0) || ((TouchedArea(0,160,150,180) || TouchedArea(0,239,180,234)) && Ascolto==1))
		{ SetFaseSceltaNota(1);// nota2
			ShowModificaNota(2); 
				  
			Menu=0;
      
		return;}
		if(TouchedArea(0,239,280,320)) // impostazioni
		{ ShowDisplaySave();
			 SetFaseSceltaNota(0);
		   Menu=0;
		
		return;} 
		if(TouchedArea(0,239,235,289)) // default
		{Default();
		SetFaseSceltaNota(0);
		Menu=0;

		return;}
		
	}

	
		
	if(PaginaDef  || ModifNota )
	{if(TouchedArea(198,239,0,40)) //freccia
		 { 
			 SetFaseSceltaNota(0);
			 Ascolto=1;
			 PaginaDef=0;
		 Button=0;
		 ModifNota=0;
			Menu=1;
			
		 ShowMenu();
		 return;}
		}
	if(Save && TouchedArea(198,239,0,40))
	{Save=0;
		Ascolto=1;
	ShowMenu();
		Menu=1;
		
		 return;}
	 
   LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
	}
void TIMER2_IRQHandler (void)   // speaker ( Modalità emergnecy o in maintenance
{ 
static int ticks=0;
	/* DAC management */	
	value=(uint16_t)SinTable[ticks]*0.3;
	DAC_convert(value<<6);
	ticks++;
	if(ticks==45) ticks=0;

	
  LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}
void TIMER3_IRQHandler (void)   // Lampeggio a 4 hz
{ 
	static int i=0;
	if(i%2==0)
  LED_On(7);
	else
  LED_Off(7);
  i++;
	if(i==50)             
	{i=0;}
  LPC_TIM3->IR = 1;			/* clear interrupt flag */
  return;
}
				
int TouchedArea(int x1,int x2,int y1,int y2)  // Controllo touch
{
	if(display.x>x1 && display.x<x2 && display.y>y1 && display.y<y2)
	 {return 1; }
 else
	 return 0;
 }

/******************************************************************************
**                            End Of File
******************************************************************************/
