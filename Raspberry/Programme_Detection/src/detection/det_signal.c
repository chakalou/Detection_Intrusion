/*ABC***********************************************************************
*                                
* NOM         : det_signal.c
*
* PROJET      : Detection
* PROCESS     :
* TYPE        : Source C
*
* ECRIT PAR   : LOLIO                   25/06/2013
*
* MODIFS      :
*
* DESCRIPTION :
*
***********************************************************************CBA*/
#include "det_incl.h"
#include "det_const.h"
#include "det_type.h"
#include "det_var.h"
#include "det_proto.h"

/* pour connaitre lib_erreur() */
/* --------------------------- */
#include <lib_erreur.h>

/* pour connaitre lib_mask() */
/* ------------------------- */
#include <lib_mask.h>

/* pour connaitre lib_sock_.... */
/* ---------------------------- */
#include <lib_sock_incl.h>
#include <lib_sock_const.h>
#include <lib_sock_type.h>

#include <lib_sock_c.h>
#include <lib_sock_s.h>

/* Fonctions locales au fichier...      */
/* ------------------------------------ */
static void    attrape_signaux();
static void    debug_signal_recu();


/*XYZ******************************************************************
/ NOM         : det_signal_init()
/
/ IN          : Neant
/ OUT         : Neant
/ INOUT       : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/
*******************************************************************ZYX*/
void
det_signal_init()
{

    /* on ignore SIGHUP pour pouvoir lancer l'application   */
    /* a la main sans que la reception de SIGHUP n'arrete   */
    /* le process.                                          */
    /*------------------------------------------------------*/
    if( sigset( SIGHUP, SIG_IGN ) == SIG_ERR )
    {
        lib_erreur( 0, 0, "Err sigset(SIGHUP) => exit" );
        det_quit();
    }

    /* pour ne pas etre perturbe par l'affectation    */
    /* d'un double au format Motorola.                */
    /* ---------------------------------------------- */
    if( sigset( SIGFPE, SIG_IGN ) == SIG_ERR )
    {
        lib_erreur( 0, 0, "Err sigset(SIGHFP) => exit" );
        det_quit();
    }

    /* on ignore SIGINT pour qu'un ^C arretant la tache GP */
    /* ne tue pas le process.                              */
    /*-----------------------------------------------------*/
    /*if ( sigset( SIGINT, SIG_IGN ) == SIG_ERR )*/
	 if( sigset( SIGINT, det_quit_SIGINT ) == SIG_ERR ) 
    {
        lib_erreur( 0, 0, "Err sigset(SIGINT) => exit" );
        det_quit();
    }

	/*-------------------------------------------------------------*/
    /* on attrape SIGTERM qui peut etre utilise par arretprog pour */
    /*-------------------------------------------------------------*/
    if ( sigset( SIGTERM, det_quit_SIGTERM ) == SIG_ERR )
    {
        lib_erreur( 0, 0, "Err sigset(SIGTERM) => exit" );
        det_quit();
    }
	

    /* on intercepte SIGIO pour lire les sockets */
    /*-------------------------------------------*/
    if ( sigset( SIGIO, det_signal_SIGIO ) == SIG_ERR )
    {
        lib_erreur( 0, 0, "Err sigset(SIGIO) => exit" );
        det_quit();
    }
    
    /* traitement par defaut pour les autres signaux */
    /* --------------------------------------------- */
    attrape_signaux();
	
	lib_erreur( 0, 0, "Fin det_signal_init()" );
    
} /* end det_signal_init() */


/*XYZ*******************************************************************
/ NOM         : det_signal_SIGIO() 
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/    on intercepte SIGIO pour lire les sockets
/
******************************************************************ZYX*/
void
det_signal_SIGIO( P_signal )
int P_signal;
{
sigset_t   old_mask, bidon;
 
    
    /* Masque certains signaux */
    /* ----------------------- */
  /*  old_mask = lib_mask_masque_signal( SIGIO   );
    bidon    = lib_mask_masque_signal( SIGUSR1 );
    bidon    = lib_mask_masque_signal( SIGUSR2 );
    bidon    = lib_mask_masque_signal( SIGALRM );*/

    /* lecture en tant que client  */
    /* --------------------------- */
   lib_sock_c_kill_sigio();
    
    /* lecture en tant que serveur */
    /* --------------------------- */
    lib_sock_s_kill_sigio();
 
    /* Detournement du signal SIGIO */
    /* ---------------------------- */
    if( sigset( SIGIO, det_signal_SIGIO ) == SIG_ERR )
    {
        lib_erreur( 0, 0, "Err sigset(SIGIO) => exit" );
        lib_mask_demasque_signal( old_mask );
        det_quit();
    }

    /* Demasque certains signaux */
    /* ------------------------- */
   /* lib_mask_demasque_signal( old_mask );*/

} /* end det_signal_SIGIO() */


/*XYZ*********************************************************************
/ NOM        : static void attrape_signaux()
/
/ IN         : Neant
/ OUT        : Neant    
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/
*********************************************************************ZYX*/
static void
attrape_signaux()
{
void     (*retsigset)();
	
		
    retsigset = sigset( SIGILL, debug_signal_recu );
    if( retsigset == SIG_ERR )
        lib_erreur( 0, 0, "Err sigset(SIGILL)" );

    retsigset = sigset( SIGTRAP, debug_signal_recu );
    if( retsigset == SIG_ERR )
        lib_erreur( 0, 0, "Err sigset(SIGTRAP)" );

    /*retsigset = sigset( SIGIOT, debug_signal_recu );
    if( retsigset == SIG_ERR )
        lib_erreur( 0, 0, "Err sigset(SIGIOT)" );*/

    retsigset = sigset( SIGABRT, debug_signal_recu );
    if( retsigset == SIG_ERR )
        lib_erreur( 0, 0, "Err sigset(SIGABRT)" );

    retsigset = sigset( SIGFPE, debug_signal_recu );
    if( retsigset == SIG_ERR )
        lib_erreur( 0, 0, "Err sigset(SIGFPE)" );


    /*retsigset = sigset( SIGKILL, debug_signal_recu );
    if( retsigset == SIG_ERR )
        lib_erreur( 0, 0, "Err sigset(SIGKILL)" );*/

    
    retsigset = sigset( SIGSYS, debug_signal_recu );
    if( retsigset == SIG_ERR )
        lib_erreur( 0, 0, "Err sigset(SIGSYS)" );

    retsigset = sigset( SIGPIPE, debug_signal_recu );
    if( retsigset == SIG_ERR )
        lib_erreur( 0, 0, "Err sigset(SIGPIPE)" );


    /*retsigset = sigset( SIGTERM, det_quit_SIGTERM );
    if( retsigset == SIG_ERR )
        lib_erreur( 0, 0, "Err sigset(SIGTERM)" );*/


    retsigset = sigset( SIGPWR, debug_signal_recu );
    if( retsigset == SIG_ERR )
        lib_erreur( 0, 0, "Err sigset(SIGPWR)" );

    retsigset = sigset( SIGURG, debug_signal_recu );
    if( retsigset == SIG_ERR )
        lib_erreur( 0, 0, "Err sigset(SIGURG)" );

/*
    retsigset = sigset( SIGPOLL, debug_signal_recu );
    if( retsigset == SIG_ERR )
        lib_erreur( 0, 0, "Err sigset(SIGPOLL)" );
*/

    retsigset = sigset( SIGWINCH, debug_signal_recu );
    if( retsigset == SIG_ERR )
        lib_erreur( 0, 0, "Err sigset(SIGWINCH)" );

/*
    retsigset = sigset( SIGIO, debug_signal_recu );
    if( retsigset == SIG_ERR )
        lib_erreur( 0, 0, "Err sigset(SIGIO)" );
*/

    retsigset = sigset( SIGPROF, debug_signal_recu );
    if( retsigset == SIG_ERR )
        lib_erreur( 0, 0, "Err sigset(SIGPROF)" );


} /* end attrape_signaux() */


/*XYZ*********************************************************************
/ NOM        : static void debug_signal_recu( P_signal )
/
/ IN         : int P_signal = parametre standart Unix.
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/
/ DESCRIPTION:  
/
*********************************************************************ZYX*/
static void
debug_signal_recu( P_signal )
int   P_signal;
{
    sprintf( G_msgerr, "Reception signal %d ???", P_signal );
    lib_erreur( 0, 0, G_msgerr );

} /* end debug_signal_recu() */


