/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           funct_ElevatorControl.h
** Last modified Date:  2018-12-30
** Last Version:        V1.00
** Descriptions:        High level ElevatorControl management functions
** Correlated files:    lib_joystick.c, funct_joystick.c
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "../joystick/joystick.h"
#include "../led/led.h"
#include "ElevatorControl.h"
#include "../timer/timer.h"
#include "../GLCD/GLCD.h"
#include "../TouchPanel/TouchPanel.h"
#include "math.h"
#include "string.h"
#include "../Panel/RequPan.h"
#include "../DAC/DAC.h"
#include "../ADC/ADC.h"
#define FREQ_LED4HZ 0x2FAF08
#define FREQ_LAMPEGGIO 0x2625A0  // A 5 hz -> 5 volte si accende e 5 volte si spegne!

 typedef struct note{
 char nota[5]; 
 unsigned int freq;
 unsigned int CountTimer;
 } Tiponote;
 unsigned int PathCompleted;    // Tanti Flag di stato 
 unsigned char Floor;
 unsigned char PressSelect;
 unsigned char Arrivato;
 unsigned char Automatico;
 unsigned char FaseLampeggio;
 unsigned char Reserved;
 unsigned char Emergency;
 unsigned char Maintenance;
 unsigned char BottonePressato;
 unsigned char FaseSceltaNota;
 unsigned int NotaScelta;
 unsigned int Count_NOTA1;         // Variabili di default
 unsigned int Count_NOTA2;
 unsigned int NOTA1;
 unsigned int NOTA2;
 unsigned int NotaProvvisoria1;
 unsigned int NotaProvvisoria2;
 unsigned int Nota1Default;
 unsigned int Nota2Default;
 unsigned char Menu;                 //Flag di stato dello schermo
 unsigned char PaginaDef;
 unsigned char Save;
 unsigned char ModifNota;
 unsigned char Ascolto;
 unsigned char FasediAscolto;
 unsigned char UscitaMenu;
 unsigned char Button;

                /*C: *523 Hz --> k=25M/(523*45)= 1062
                B: 494Hz --> k=25M/(494*45)= 1125
                Bb: 466 Hz --> k=25M/(466*45)= 1192
                A: 440 Hz --> k=25M/(440*45)= 1263
                Ab: 415 Hz --> k=25M/(415*45)= 1339
                G: 392 Hz --> k=25M/(392*45)= 1417
                Gb: 370 Hz --> k=25M/(370*45)= 1502
                F: 349 Hz --> k=25M/(349*45)= 1592
                E: 330 Hz --> k=25M/(330*45)= 1684
                Eb: 311 Hz --> k=25M/(311*45)= 1786
                D: 294 Hz --> k=25M/(294*45)= 1890
                Db: 277 Hz --> k=25M/(277*45)= 2006
                C: 262 Hz --> k=25M/(262*45)= 2120*/
        // 45 è il numero dei campioni della sinusoide

 Tiponote note[14]={{"DO5",523,1062},
													 {"SI",494,1125},
                           {"SIb",466,1192},
													{"LA",440,1263},
													 {"LAb",415,1339},
													{"SOL",392,1417},
													 {"FA#",370,1502},
														{"FA",349,1592},
													{"MI",330,1684},
													 {"RE#",311,1786},
													 {"RE",294,1890},
													 {"DO#",277,2006},
                           {"DO4",262,2120},
                            {"RE#5",622,893}  // Aggiunta come ultima nota ma non considerata ( non segue l'ordine da frequenze piu alte a piu basse!)
  };                                          // utile per riprodurre sirena bitonale ambulanza RE#5-SIB4
 

/*----------------------------------------------------------------------------
  Functions
 *----------------------------------------------------------------------------*/
int SelectElevator(void)
{ return PressSelect;
}

int StatusElevator(void)
{  return Arrivato;}

void SelectPressed(int num)
{  PressSelect=num;
   Automatico=0;}

int ControlPathElevator(void)
{   if(PathCompleted==DURATA_PATHCOMPLETO || (PathCompleted==0 && Arrivato==0))  /* Caso Particolare nella quale prima si va avanti e poi si torna indietro al piano*/
	     { return 1;}
	  else return 0;}

void MoveElevator(int dir)
{    
	   if(dir==-1 && PathCompleted>0)
	   PathCompleted--;
	   else if (dir==1)
		 {PathCompleted++;
			 if(Arrivato==1)
	   {Arrivato=0;}}
}
int GetReserved(void)
{return Reserved;}

void SetReserved(int num)
{Reserved=num;
	if(num==0)
	{NVIC_EnableIRQ(EINT2_IRQn);              /* enable irq in nvic                 */
	 	LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */			   // Nel caso di Riservato ma non partito
    NVIC_EnableIRQ(EINT1_IRQn);              /* enable irq in nvic                 */        // dopo un minuto non è piu riservato quindi timer riattivati
	  LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
  }
	else 
	{ disable_timer(1);}
}
int GetFaseLampeggio(void)
{	return FaseLampeggio;}

void SetFaseLampeggio(int num)
{FaseLampeggio=num;}

void ChangeFloor(void)
{	if(PathCompleted==DURATA_PATHCOMPLETO)
	{if(Floor==0)
		   {Floor=1;}
		else {
			  Floor=0;} 
     	}
}
int StatusLedOut(void)
{  if(IsLedOn(7))
	    return 1;
	else return 0;
}
void ElevatorAlarm(void )
{SetEmergencyMode(1); // Allarme dopo 1 minuto di inattività tra i piani ( Almeno 1 up )
				LedAlarmOn();}
int ButtonElevator(int dir)
	{ int StatusButton=0;
		switch (dir)
		{ case 0:
			if((LPC_GPIO1->FIOPIN & (1<<25)) == 0)
				StatusButton=1;
			break;
		case 1:
			if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 || Automatico==1)
				StatusButton=1;
			break;
		case -1:
			if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 || Automatico==2 )
				StatusButton=1;
		  break; 
		}
	return StatusButton; 
		
}
int StatusFloor(void)
{ return Floor;}

int EmergencyMode(void)
{  
   return Emergency;
}
void SetEmergencyMode(int num)
{
   if(num==0)
	 {Emergency=0;
		reset_timer(2);
		disable_timer(2);
	  disable_timer(3);}
	 else
	 { Emergency=1;
	 init_timer(2,Count_NOTA1);
	 BottonePressato=0;
	 }
}
int InterruptButton(void)
{ return BottonePressato;
}
void SetInterruptButton(int num)
{if( num == 1)
		BottonePressato=1;
	else
		BottonePressato=0;}
int ButtonEmergencyPressed(void)
{  if((LPC_GPIO2->FIOPIN & (1<<10)) == 0)
	   return 1;
	else 
	{ SetInterruptButton(0);
		return 0;}

}
void ChangeFrequency(int num,int Frequency)
{   if(num==0)
	   {Count_NOTA1=note[Frequency].CountTimer;
		   NOTA1=Frequency;
		   NotaProvvisoria1=Frequency;}
	  else
		{Count_NOTA2=note[Frequency].CountTimer;
		NOTA2=Frequency;
		NotaProvvisoria2=Frequency;}
}
int TimerNota(int num)
{ if(!MaintenanceMode())
	{if(num==1)
		return Count_NOTA1;
	if(num==2)
		return Count_NOTA2;}
	else 
	{if(num==1)
	return note[NotaProvvisoria1].CountTimer;
	if(num==2)
		return note[NotaProvvisoria2].CountTimer;	 }
	}

void SetAudio(int num)        // Fase di ascolto durante maintenance 
{	FasediAscolto=num;
 if(num==0)
	{	ShowMenu();
		disable_timer(2);
		Menu=1;
	 enable_timer(1);	}
	else 
	{ disable_timer(1);}
}
int GetAudio(void)
{return FasediAscolto;}

int MaintenanceMode(void){
  return Maintenance;}

int GetFaseSceltaNota(void)
{return FaseSceltaNota;}
void SetFaseSceltaNota(int num)
{FaseSceltaNota=num;}

int GetUscitaMenu(void)
{	return UscitaMenu;}
void SetUscitaMenu(int num)
{ UscitaMenu=num;}

void SetMaintenanceMode(int num)
	{ if(num==1)
	  	{UscitaMenu=0;
	  Maintenance=1;
	  LED_deinit();
		FaseSceltaNota=0;
	  FasediAscolto=0;
		NotaProvvisoria1=NOTA1;
		NotaProvvisoria2=NOTA2;
			init_timer(3,2*FREQ_LED4HZ);
			Save=0;
			Button=0;
			PaginaDef=0;
			ModifNota=0;
			Ascolto=0;
      		}						  
		else {
			Maintenance=0;
		 init_timer(3,FREQ_LED4HZ);
		 LED_init();
		 InitPanel();
	}}


	
void TurnOnButtonMaintenance(void)
  { 	int k,a;
		  float LMax=3.3,Lmin=1;
		  char string[60];
	  
		 
		 
		 enable_timer(1);
		 
		LCD_Clear(Green);
	
		strcpy(string,"Modalita' riservata ad");
	  GUI_Text(30,70,(uint8_t*)string,Black,Green);
		strcpy(string," un operatore qualificato");
	  GUI_Text(20,90,(uint8_t*)string,Black,Green);
		for(k=21;k<32;k++)
	  for(a=0;a<720;a++)
		{LCD_SetPoint(k*cos(a)+120,k*sin(a)+160,Black);}
	  
		for(k=0;k<15;k++)
		for(a=0;a<720;a++)
		{LCD_SetPoint(k*cos(a)+120,k*sin(a)+160,Black);}
	
			for(k=130;k<160;k++)
		LCD_DrawLine(112,k,127,k,Black);
		
		for(k=1;k<5;k++)
		for(a=0;a<720;a++)
		{LCD_SetPoint(k*(Lmin)*cos(a)+120,(int)k*(LMax)*sin(a)+150,Green);}
		
		
	
	  
	strcpy(string,"Tocca il bottone sovrastante ");
	  GUI_Text(10,200,(uint8_t*)string,Black,Green);
			strcpy(string,"per entrare in ");
	  GUI_Text(70,220,(uint8_t*)string,Black,Green);
		strcpy(string,"Maintenance Mode ");
	  GUI_Text(60,240,(uint8_t*)string,Black,Green);
		strcpy(string,"quando tutti i led ");
	  GUI_Text(50,260,(uint8_t*)string,Black,Green);
		strcpy(string," saranno spenti ");
	  GUI_Text(60,280,(uint8_t*)string,Black,Green);
		Button=1;
		
		
	}
int ReturnButtonTouched(void)
{  if(Button==0)
     return 1;
  else 
     return 0;	}

void ShowMenu(void)
{ 
	char string[60];
	int i;
	 
	UscitaMenu=0;
	for(i=0;i<10;i++)  // Ritardo la composizione del menu
	LCD_Clear(White);
	
	LCD_DrawLine(0,60,239,60,Black);
	LCD_DrawLine(0,150,239,150,Black);

	
	for(i=280;i<320;i++)
	{LCD_DrawLine(0,i,239,i,Green);
	}
	for(i=236;i<280;i++)
	{LCD_DrawLine(0,i,239,i,Red);
	}
StampoFreccia(Green);
	if(Ascolto==1)
	{for(i=125;i<175;i++)
	{LCD_DrawLine(170,i,239,i,Green);
	}
  	strcpy(string,"Ascolta " );
	GUI_Text(175,132,(uint8_t*)string,White,Green);
	strcpy(string,"per 5 s" );
	GUI_Text(175,155,(uint8_t*)string,White,Green);}
	
	strcpy(string,"    Note Speaker   " );
	GUI_Text(30,240,(uint8_t*)string,White,Red);
	strcpy(string,"(Default)");
	GUI_Text(80,255,(uint8_t*)string,White,Red);
	
	strcpy(string,"Adesso puoi modificare ");
	GUI_Text(5,20,(uint8_t*)string,Black,White);
	
	strcpy(string," le note dello speaker");	
	GUI_Text(4,40,(uint8_t*)string,Black,White);
	
	strcpy(string,"Modifica qui");
  GUI_Text(70,100,(uint8_t*)string,Black,White);
	
	strcpy(string,"SPEAKER NOTA 1:");
  GUI_Text(30,80,(uint8_t*)string,Black,White);
	strcpy(string,note[NotaProvvisoria1].nota);
  GUI_Text(160,80,(uint8_t*)string,Black,White);
	strcpy(string,"Modifica qui");
	GUI_Text(70,210,(uint8_t*)string,Black,White);
	strcpy(string,"SPEAKER NOTA 2:");
  GUI_Text(30,190,(uint8_t*)string,Black,White);
		strcpy(string,note[NotaProvvisoria2].nota);
  GUI_Text(160,190,(uint8_t*)string,Black,White);
	strcpy(string,"Salva impostazioni");
	GUI_Text(50,300,(uint8_t*)string,Black,Green);
	DelayUS(400);
	Menu=1;
		}

void RestituisciNota(int num) // chiamata a frequenze regolari
{  
  char string[20];int i;
	
 	
	  for(i=170;i<210;i++)
	 LCD_DrawLine(i,130,i,160,White);
	 strcpy(string,note[num].nota);
	if(NotaScelta==1)
	{NotaProvvisoria1=num;
		
	}		
	else
	{NotaProvvisoria2=num;
	 }
   // Stampa Nota 
    GUI_Text(170,140,(uint8_t*)string,Black,White);
	 
  }
		

void ShowModificaNota(int num)
{  
	
	char string[20];
	ADC_start_conversion();
	ModifNota=1;
	LCD_Clear(White);
	FaseSceltaNota=1;
	NotaScelta=num;
	LCD_DrawLine(0,100,239,100,Black);
	LCD_DrawLine(0,200,239,200,Black);
	if(num==1)
	strcpy(string,"SPEAKER NOTA 1:");
	else
	strcpy(string,"SPEAKER NOTA 2:");
  GUI_Text(30,140,(uint8_t*)string,Black,White);
	
  StampoFreccia(Green);
	
	
	
		return;     } 


void ShowDisplaySave()
	{  
		char string1[25];
		char string2[15]; 
		Menu=0;
    	Save=1;
		
		ChangeFrequency(0,NotaProvvisoria1);
		ChangeFrequency(1,NotaProvvisoria2);
			strcpy(string1," impostazioni salvate");
       strcpy(string2,"con successo");
  	
	  StampoScritta(string1,string2,Green);
    StampoFreccia(Green);

	return;
		
	}
void Default()
{ 
	char string1[20];char string2[15];
		PaginaDef=1;
	NotaProvvisoria1=Nota1Default;
	NotaProvvisoria2=Nota2Default;
	ChangeFrequency(0,Nota1Default);
		 ChangeFrequency(1,Nota2Default);

		strcpy(string1,"   Note di Default");
   	strcpy(string2,"ripristinate");
	 StampoScritta(string1,string2,Red);
 
  
	StampoFreccia(Red);

			return ;
}



	
     
	
	                                                 // Se non dovesse funzionare
	                                                  // Si tocca quando è muto -> Spuntano Circonferenze -> suona
	                                   //al timer 3 si auto disabilita dopo un tot ( solo in questa modalità)
	                         // Torna ad essere muto  -> si torna su

	  

		
void AttivaAudio(void){
			int k,a,i;

	for(i=100;i<250;i++)
		LCD_DrawLine(0,i,239,i,Green);
		

	
	for(k=21;k<26;k++)
	  for(a=0;a<720;a++)
		{LCD_SetPoint(k*cos(a)+130,k*sin(a)+170,Yellow);}
	for(k=28;k<33;k++)
	  for(a=0;a<720;a++)
		{LCD_SetPoint(k*cos(a)+130,k*sin(a)+170,Yellow);}
	for(k=14;k<19;k++)
	  for(a=0;a<720;a++)
		{LCD_SetPoint(k*cos(a)+130,k*sin(a)+170,Yellow);}
 
		for(i=120;i<220;i++)
		LCD_DrawLine(95,i,137,i,Green);
		
	  for(i=155;i<185;i++)
	{LCD_DrawLine(85,i,130,i,Yellow);  
	}
	for(i=154,k=0;k<25;i--,k++)
	{LCD_DrawLine(105+k,i,130,i,Yellow);
	}
	for(i=185,k=0;k<25;i++,k++)
	{LCD_DrawLine(105+k,i,130,i,Yellow);
	}
	
	  SetAudio(1);
			return;
		}
	
	
	
				
		
void StampoFreccia(uint16_t Color)  
	{	int i,k;
		for(i=0;i<40;i++)         //Freccia Bianca , Colore di sfondo a scelta
	{LCD_DrawLine(198,i,239,i,Color);
	}
	for(i=13;i<28;i++)
	{LCD_DrawLine(220,i,235,i,White);
	}
	for(i=5,k=0;i<20;i++,k++)
	{LCD_DrawLine(220-k,i,220,i,White);
	}
	for(i=20,k=0;i<35;i++,k++)
	{LCD_DrawLine(205+k,i,220,i,White);                   
	}        
}	
void StampoScritta(char*string1,char*string2,uint16_t Color)
{ int i;
	LCD_Clear(White);
	for(i=100;i<200;i++)
		LCD_DrawLine(0,i,239,i,Color);
		
  GUI_Text(30,140,(uint8_t*)string1,Black,Color);
  GUI_Text(60,160,(uint8_t*)string2,Black,Color);
	
}
/*****************************************************************************
**                            End Of File
******************************************************************************/
