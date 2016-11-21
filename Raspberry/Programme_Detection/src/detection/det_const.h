/*ABC***********************************************************************
*
* NOM         : det_const.h
*
* PROJET      : PSCL
*
* PROCESS     :
*
* TYPE        : Include C
*
* ECRIT PAR   : D. DELEFORTERIE                   25/06/2013
*
* MODIFS      :
*
* DESCRIPTION :
*
***********************************************************************CBA*/
#ifndef _DET_CONST_
#define _DET_CONST_

/* numero de timer */
/* --------------- */
#define TIMER_CONNECT_GP     1
#define TIMER_SERVICE_GP     2
#define TIMER_TRAITEMENT     3
#define TIMER_ACCEPT         4
#define TIMER_SERVICE        5
#define MAX_DETECTEUR		 10
#define MAX_CONTACT			 10
/*Port série de l'arduino*/
#define PORT_NAME "/dev/ttyACM0"

/*Définition des constantes propres à detection.c*/
#define NB_MIN_ATTENTE		 1


#endif
