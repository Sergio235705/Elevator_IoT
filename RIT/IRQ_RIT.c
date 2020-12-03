/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../timer/timer.h"
#include "../Panel/RequPan.h"
#include "../Elevator/ElevatorControl.h"
#include "../adc/adc.h"
#include "../GLCD/GLCD.h"
#define DURATA_UNO_MINUTO 1200
#define FREQ_LAMPEGGIO2HZ 5 // 2 volte si accende e 2 volte si spegne 
#define DURATA_DUE_MINUTI 2*DURATA_UNO_MINUTO
#define FREQ_LAMPEGGIO4HZ FREQ_LAMPEGGIO2HZ  // Frequenza per i timer delle due note dello speaker-> Si cambia Frequenza quando Led si accende!
#define DURATA_1_SECONDO DURATA_UNO_MINUTO/60
#define DURATA_2_SECONDI 2*DURATA_1_SECONDO
                                             // Frequenza Led 4 hz gestita da timer 3 
/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern unsigned char Menu;

void RIT_IRQHandler (void)
{	
	
	static int Blinking2hz=FREQ_LAMPEGGIO2HZ;
	static int Blinking4hz=FREQ_LAMPEGGIO4HZ;
  static int TimeAlarm=0;
  static int TimeReserved=0;
	static int TimeButton=0;
	static int Alarm=0;
  int TastoPremuto=0;
	static int TimerAttivoOne=1;
  char stringCount[10]={'0','1','2','3','4','5','6','7','8','9'};
	char string[2];
if(!MaintenanceMode())	
{
 
   if(!GetReserved())
	{if(ReturnButtonTouched())               // Fase per entrare in Modalita MaintenanceMode();
   	{SetMaintenanceMode(1);}
	 else if((LPC_GPIO2->FIOPIN & (1<<11)) == 0)  // Gestisco bottoni in polling ( per entrare in riservato) 
	 {PressButton(1);}                            // in seguito tramite interrupt
	 else if((LPC_GPIO2->FIOPIN & (1<<12)) == 0)
	 {PressButton(0);}
	   return;}
	
	if(GetReserved() && !SelectElevator() && ButtonElevator(0) ){	  /* Select selezionato*/
		 /* Joytick Select pressed */
				SelectPressed(1);
	}
	
	
	  /* Comando Up per muoversi in direzione dell'altro piano*/
	if (SelectElevator() && ButtonElevator(-1) /*&& GetFaseLampeggio()==0 && !EmergencyMode() */){	/* Joystick down*/
		/* Elevator Actived */
			MoveElevator(-1);			
			TastoPremuto=1;
		  if(Blinking2hz==FREQ_LAMPEGGIO2HZ)            /*Lampeggio STATUS LED*/
	   { 	
			  if(!StatusLedOut())
			  {LED_On(7);}
				else 
				{LED_Off(7);}
			  Blinking2hz=0; 
				
		 }
		else 
			{Blinking2hz++;}
	}                               //Controllo tornare indietro
	
	/* Comando Down per tornare indietro al piano di partenza*/
	
    /*Elevator in movimento */
		if(TastoPremuto==0 && SelectElevator() && ButtonElevator(1) && GetFaseLampeggio()==0 && !EmergencyMode() ){	 /*Joystick Up */
		/* Elevator Actived */
			MoveElevator(1);
			TastoPremuto=1;                   /*Lampeggio STATUS LED*/
		  if(Blinking2hz==FREQ_LAMPEGGIO2HZ)                    /* Uso COSTANTE in quanto sfrutto Frequenza RIT ( 20 Hz) per generarmi una sotto frequenza di 2 Hz */
	   { 	
			if(!StatusLedOut())
			  {LED_On(7);}
				else 
				{LED_Off(7);}
			  Blinking2hz=0; 
				
		 }
		else 
			{Blinking2hz++;}
	}
	if(InterruptButton() && !EmergencyMode() && !StatusElevator()) //StatusElevator ( 1 al piano ,0 tra i piani )
	{	if(ButtonEmergencyPressed())
		TimeButton++;
		else 
		TimeButton=0; }

		
		
		
		/*Elevator al piano ma non selezionato*/
	if(StatusElevator() && !SelectElevator() && GetReserved())   /*Elevatore fermo , led status spento*/
	{TimeReserved++;} /*Conto il tempo nella quale è al piano e devo mantenere Led Panel acceso*/
	else {TimeReserved=0;}                    /* StatusElevator -> Arrivato al Piano o meno*/
	                                          /*SelectElevator-> Select Selezionato o meno*/
                                            /* Ritorna Riservato -> Variabile per tempo riservato all'arrivo */
		                                        /* Riservato indica fase Post Lampeggio*/
		// Elevator non in movimento e non al piano
	if(!StatusElevator() && TastoPremuto==0 && !Alarm && !EmergencyMode())
	   {TimeAlarm++;               /* Entra qui durante la fase di lampeggio ma dura solo 3 secondi quindi non fa scattare l'allarme*/
      LED_On(7);
			Blinking2hz=FREQ_LAMPEGGIO2HZ;  /* Nel caso elevatore fermo LED STATUS ACCESO FISSO , nel caso di movimenti BLINKING=20 ,LED STATUS subito spento*/
	}	 	              /* e torna ad eseguire il conteggio per il lampeggio a 2 hz */
  else { TimeAlarm=0;}
			
	
	// Controlli Stati Elevator
	
	if(TimeButton==DURATA_2_SECONDI)
	{SetEmergencyMode(1); // initTimer Freq Diverse
		TimeButton=0;
		LedAlarmOn();
	  enable_timer(3);
		Blinking2hz=FREQ_LAMPEGGIO2HZ;
	}
	
  
	if(TimeAlarm==DURATA_UNO_MINUTO) // 1200 x 50 ms = 1 minuto Costante
	{  ElevatorAlarm();
				Alarm=1;}      
			 
	if(TimeReserved==DURATA_UNO_MINUTO)    

	{TurnOffPanel();        //SpegniPanel dopo 1 minuto di inattività al piano ( Select non selezionato) 
	  SetReserved(0);
		enable_timer(1);
		}

	if(EmergencyMode())
	{  
		
	    if(Blinking4hz==FREQ_LAMPEGGIO4HZ)                    /* Uso COSTANTE in quanto sfrutto Frequenza RIT ( 20 Hz) per generarmi una sotto frequenza di 4 Hz */
	   { 	
			if(TimerAttivoOne==0)
			  {
				disable_timer(2);
				init_timer(2,TimerNota(1));
				reset_timer(2);
			  enable_timer(2);
				TimerAttivoOne=1;}
				else 
				{
				disable_timer(2);
				init_timer(2,TimerNota(2));
				reset_timer(2);
			  enable_timer(2);
				TimerAttivoOne=0;}
				
			  Blinking4hz=0; 
				
		 }
		else 
			{Blinking4hz++;}
			
		if(InterruptButton())
		{SetEmergencyMode(0);
			LedAlarmOff();
		}
	}
		
		
		
	if(ControlPathElevator() && GetFaseLampeggio()==0)      
	{ 
		SetFaseLampeggio(1);                      /*Fase di lampeggio a 3 hz in corso*/
		ChangeFloor();	
	//	Lampeggio; 
		enable_timer(0);
    Blinking2hz=FREQ_LAMPEGGIO2HZ;
		}
	
	}
else 
{ 
   
	 // funzionalità speaker in maintenance
	  if(GetAudio())
		{ static int k=100; // 5 secondi =50 ms*100 
			if(Blinking4hz==FREQ_LAMPEGGIO4HZ)             //        Uso COSTANTE in quanto sfrutto Frequenza RIT ( 20 Hz) per generarmi una sotto frequenza di 4 Hz 
	   { 
			if(TimerAttivoOne==0)
			  {
				disable_timer(2);
				init_timer(2,TimerNota(1));
				reset_timer(2);
			  enable_timer(2);
				TimerAttivoOne=1;}
				else 
				{
				disable_timer(2);
				init_timer(2,TimerNota(2));
				reset_timer(2);
			  enable_timer(2);
				TimerAttivoOne=0;}
				
			  Blinking4hz=0; 
				
		 }
		else 
			{Blinking4hz++;}
    k--;
		 string[0]=stringCount[k/20];
	   string[1]='\0';
		 GUI_Text(150,210,(uint8_t*)string,White,Green);
	   if(k==0)  
		 { 	k=100;
		SetAudio(0);} 
		}
	                                           
	    if(GetUscitaMenu()==1)               //Esco da maintenance                         
		 {  SetMaintenanceMode(0);}
	  
}   
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
