/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           lib_EC.h
** Last modified Date:  20/12/2019
** Last Version:        V1.00
** Descriptions:        Atomic EC init functions
** Correlated files:    funct_ElevatorControl.c, ElevatorControl.h ,lib_EC.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/
#include "lpc17xx.h"
#include "ElevatorControl.h"
#include "../led/led.h"
#include "../button_EXINT/button.h"
#include "../joystick/joystick.h"
#include "../timer/timer.h"
#define FREQ_LED4HZ 0x2FAF08
#define FREQ_LAMPEGGIO 0x2625A0  // A 5 hz -> 5 volte si accende e 5 volte si spegne!
#define FREQ_TOUCH 0x4E2   // 50us * 25MHz = 1.25*10^3 = 0x4E2 
/*----------------------------------------------------------------------------
  Function that initializes Elevator Control
 *----------------------------------------------------------------------------*/
typedef struct note Tiponote;
extern unsigned int PathCompleted;
extern unsigned char Floor;
extern unsigned char PressSelect;
extern unsigned char Arrivato;
extern unsigned char Automatico;
extern unsigned char Reserved;
extern unsigned char FaseLampeggio;
extern unsigned char Emergency;
extern unsigned char BottonePressato;
extern unsigned char Maintenance;
extern unsigned char Menu;
extern unsigned char PaginaDef;
extern unsigned char Save;
extern unsigned char ModifNota;
extern unsigned char Ascolto;
extern unsigned int Nota1Default;
extern unsigned int Nota2Default;
extern unsigned char FasediAscolto;
extern unsigned char UscitaMenu;
extern unsigned char FaseSceltaNota;
void InitElevator(void)
{ Arrivato=1;
	Floor=0;
	PathCompleted=0;
	PressSelect=0;
	Reserved=0;
	FaseLampeggio=0;
	BottonePressato=0;
	Emergency=0;
	Maintenance=0;
	Menu=0;
	Ascolto=0;
	Save=0;
	UscitaMenu=0;
	FaseSceltaNota=0;
	PaginaDef=0;
	ModifNota=0;
	Nota1Default=3;
	Nota2Default=3;
	FasediAscolto=0;
   ChangeFrequency(0,Nota1Default);
	 ChangeFrequency(1,Nota2Default); // Frequenze di default ( Sirena Ambulanza )
	
	
	init_timer(0,FREQ_LAMPEGGIO);	/* 25 Mhz * ( 1/ 5 ) = 5,000,000 = 0x4C4B40*/
  init_timer(1,FREQ_TOUCH); 
	init_timer(3,FREQ_LED4HZ);
	TurnOnButtonMaintenance();

}

void ActiveElevator(int num)
{		
	  SetReserved(1);
	  SetFaseLampeggio(0);
	if(num!=0)               // Select Automatico se chiamato da button in modalità normale o emergenza 
		PressSelect=1; 
	switch(num){
		case 0:
			Automatico=0;
		//TurnOff
	    break;
		case 1: 
			Automatico=1;  	// Nel caso di chiamata da button
		//  TurnOffButtonMaintenance();
		  break;
		case 2:   // CASI IN EMERGENCY MODE
			if(!StatusFloor()) // Se piano di partenza 0
			Automatico=2;  // Data Emergenza richiamato al piano da dove è partito
			else 
			Automatico=1; // Data Emergenza richiamato al piano di destinazione -> Piano 1}
		  break;
		case 3:
			if(StatusFloor()) // Se piano di partenza 0
			Automatico=2;  //Data Emergenza richiamato al piano da dove è partito
			else 
			Automatico=1; //Data Emergenza richiamato al piano di destinazione -> Piano 0
		  break;
				
}
		
 }   
   		


void DeActiveElevator(void)
{ 
   	PathCompleted=0;
		PressSelect=0;
	  Arrivato=1; 
	  SetFaseLampeggio(0);
	  NVIC_EnableIRQ(EINT2_IRQn);             /* enable irq in nvic                 */
	 	LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */			 
    NVIC_EnableIRQ(EINT1_IRQn);              /* enable irq in nvic                 */
	  LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
	 
}
/******************************************************************************
**                            End Of File
******************************************************************************/

