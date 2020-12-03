/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           ElevatorControl.h
** Last modified Date:  2019-12-20
** Last Version:        V1.00
** Descriptions:        Prototypes of functions included in the ElevatorControl .c files
** Correlated files:    ElevatorControl.c
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "../GLCD/GLCD.h"
#define DURATA_PATHCOMPLETO 144  //Rit per completare deve entrare 144 volte -> 144 x 50 ms = 7.2 secondi*/

/* Status Elevator*/

/* lib Elevator Control */
void InitElevator(void);

void ActiveElevator(int num);

void DeActiveElevator(void);

/* Funct ElevatorControl*/   
int SelectElevator(void);       /*Restituisce Select*/

int StatusElevator(void);
/* Arrivato =0 -> Elevatore tra i piani  */ /* Arrivato=1-> Elevatore al piano */

int ControlPathElevator(void);     /* Controlla se ha raggiunto l'altro piano*/

void SelectPressed(int num);      /*Settare Select*/


int GetReserved(void);     /* Restituisce Riservato*/

void SetReserved(int num); /*Settare Riservato*/

int GetFaseLampeggio(void);  /* Restituisce FaseLampeggio*/

void SetFaseLampeggio(int num);

void MoveElevator(int dir);
// Muove elevatore ( Cambia Path)  &  Cambia stato " Arrivato " dell'elevatore 

int ButtonElevator(int dir); //Controlla movimento joistick

int StatusLedOut(void);  // 1 ledStatus Acceso 0 Spento*/

void ElevatorAlarm(void );  

void ChangeFloor(void); // Cambia Piano quando si è completato il path

int StatusFloor(void);  // Restituisce il piano

int EmergencyMode(void);

void ChangeFrequency(int num , int Frequency); // Setta frequenze

// Funzionalità di emergenza

void SetEmergencyMode(int num);

int InterruptButton(void);  // Quando il bottone di emergenza viene pressato ( accidentalmente o meno)

void SetInterruptButton(int num);

int ButtonEmergencyPressed(void);  // Controlla Se il bottone è pressato*/

// Funzionalità Maintenance

int MaintenanceMode(void);

int TimerNota(int num);

void RestituisciNota(int num);

void SetMaintenanceMode(int num);

int GetUscitaMenu(void); 

void TurnOnButtonMaintenance(void);

int ReturnButtonTouched(void);
// Varie schermate sul LCD 
void ShowMenu(void);

void StampoScritta(char*string1,char*string2,uint16_t Color);

void SetUscitaMenu(int num);
	
void StampoFreccia(uint16_t Color);

void AttivaAudio(void);

void DisattivaAudio(void);

void SetAudio(int num);

int GetAudio(void);

void ShowModificaNota(int num);

void Default(void);

void ShowDisplaySave(void);

int GetFaseSceltaNota(void);

void SetFaseSceltaNota(int num);


/*****************************************************************************
**                            End Of File
******************************************************************************/
