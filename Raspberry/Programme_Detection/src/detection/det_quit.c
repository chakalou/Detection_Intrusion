/*ABC***********************************************************************
*
* NOM         : det_quit.c
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
*    - procedure d'arret de la tache 
*
***********************************************************************CBA*/
#include "det_incl.h"
#include "det_const.h"
#include "det_type.h"
#include "det_var.h"
#include "det_proto.h"

/* pour connaitre lib_erreur().. */
/* ----------------------------- */
#include <lib_erreur.h>

/* pour connaitre lib_timer...() */
/* ----------------------------- */
#include <lib_timer.h>



/*XYZ*********************************************************************
/ NOM         : det_quit()
/
/ IN          : Neant
/ OUT         : Neant
/ INOUT       : Neant
/ RETOUR      :
/    Sortie par exit(-1)
/ DESCRIPTION :
/    procedure d'arret obligatoire avant d'arreter la tache.
/
*********************************************************************ZYX*/
void
det_quit()
{
static int    first_time = 1;
static int    cpt_recursif = 0;

    if( first_time == 1 )
    {
        first_time = 0;
    }
    else
    {
        /* Appel recursif de det_quit() .... */
        /* --------------------------------- */
        if( cpt_recursif < 100 )
        {
            cpt_recursif++;
            sprintf( G_msgerr, "det_quit(%d) appel recursif", cpt_recursif );
            lib_erreur( 0, 0, G_msgerr );
            return;
        }
        else
        {
            lib_erreur( 0, 0, "exit( -90 ) en dur" );
            exit( -90 );
        }
    }
    lib_erreur( 0, 0, "deb det_quit()" );
	
	/*fermeture de la liaison serie*/
	/*-----------------------------*/
	lib_serial_close();
	
    /* Arrete tous les timers */
    /* ---------------------- */
    lib_timer_quit();
    
	lib_kartoz_speak("Arret detection intrusion");
    /* arret de la tache */
    /*-------------------*/
    lib_erreur( 0, 0, "fin det_quit() => exit" );
    exit( 0 );

} /* end det_quit() */

/*XYZ*********************************************************************
/ NOM        : det_quit_SIGINT()
/
/ IN         : P_signal = parametre standart Unix.
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/
/ DESCRIPTION: Cette procedure 
/        - intercepte signal SIGINT.
*********************************************************************ZYX*/
void
det_quit_SIGINT( P_signal )
int P_signal;
{
    lib_erreur( 0, 0, "det_quit_SIGINT()" );
    det_quit();

} /* end det_quit_SIGINT() */


/*XYZ*********************************************************************
/ NOM        : det_quit_SIGBUS()
/
/ IN         : P_signal = parametre standart Unix.
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
*********************************************************************ZYX*/
void
det_quit_SIGBUS( P_signal )
int P_signal;
{
    lib_erreur( 0, 0, "det_quit_SIGBUS()" );
    det_quit();

} /* end det_quit_SIGBUS() */

/*XYZ*********************************************************************
/ NOM        : det_quit_SIGBUS()
/
/ IN         : P_signal = parametre standart Unix.
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
*********************************************************************ZYX*/
void
det_quit_SIGTERM( P_signal )
int P_signal;
{
    lib_erreur( 0, 0, "det_quit_SIGTERM()" );
    det_quit();

} /* end det_quit_SIGBUS() */

