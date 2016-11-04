/*ABC***********************************************************************
*
* NOM         : det_timer.c
*
* PROJET      : Detection
* PROCESS     :
* TYPE        : Include C
*
* ECRIT PAR   : LOLIO                   25/06/2013
*
* MODIFS      :
*
* DESCRIPTION :
*    - Fonctions timer relatives au process 
*
***********************************************************************CBA*/
#include "det_incl.h"
#include "det_const.h"
#include "det_type.h"
#include "det_var.h"
#include "det_proto.h"

/* pour connaitre Seconde */
/* ---------------------- */
#include <lib_date.h>

/* pour connaitre lib_erreur() et G_msgerr */
/* --------------------------------------- */
#include <lib_erreur.h>
#include <time.h>

/* pour connaitre les constantes timer */
/* ----------------------------------- */
#include <lib_timer.h>

/* Fonctions locales a ce fichier */
/* ------------------------------ */
static void timer_traitement();


/*XYZ*********************************************************************
/ NOM        : det_timer_init()
/
/ IN         : Neant
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/    - Initialisation des timers necessaires.
*****************************************************************ZYX*/
void
det_timer_init()
{

    if( lib_timer_init( TIMER_TRAITEMENT,
                        LIB_TIMER_MONO, 
                        LIB_TIMER_REPETITION,
                        timer_traitement ) == -1 )
    {
        sprintf( G_msgerr, "Err lib_timer_init(%d) => exit",
                 TIMER_TRAITEMENT );
        lib_erreur( 0, 0, G_msgerr );
        det_quit();
    }

} /* det_timer_init() */


/*XYZ*********************************************************************
/ NOM        : timer_traitement()
/
/ IN         : P_timer
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
*****************************************************************ZYX*/
static void
timer_traitement( P_timer )
int  P_timer;
{
	
	

} /* end timer_traitement() */
    
