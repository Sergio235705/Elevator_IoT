#include "button.h"
#include "lpc17xx.h"
#include "../Elevator/ElevatorControl.h"
#include "../Panel/RequPan.h"
#include "../timer/timer.h"

unsigned int semaforo=1;
void EINT0_IRQHandler (void)	  	/* INT0														 */
{		
	SetInterruptButton(1); // Come spiegato nel PressButton() anche in questo bottone 
                         // non ho implementato il debouncing , interessa la pressione del bottone in generale e non la singola pressione ( in questo caso per 2 secondi)
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  	/* KEY1		Request from floor 1		 */     
{if(semaforo>0)                                                               // Semaforo per gestire la sincronizzazione quando in 
	{ 		semaforo--; 		// Emergency Mode viene chiamato da entrambi i piani ( Si potrebbe anche sfruttare la priorità dei bottoni)
		if(PressButton(1)==1)
	{	NVIC_DisableIRQ(EINT1_IRQn);	//disable Button interrupts			 
	LPC_PINCON->PINSEL4    &= ~(1 << 22);}  // GPIO pin selection 
	 semaforo++;}
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
	
}

void EINT2_IRQHandler (void)	  	/* KEY2		 Request from floor 0										 */
{
if(semaforo>0)             
	{semaforo--;
		if(PressButton(0)==1)
	{NVIC_DisableIRQ(EINT2_IRQn);		//disable Button interrupts			 
    LPC_PINCON->PINSEL4    &= ~(1 << 24);}   	// GPIO pin selection 
		semaforo++;}
	LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
}
