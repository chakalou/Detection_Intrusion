/***************************************************************************
/    A PLACER dans le programme principal qui utilise les sockets :
/
/    Detournement du signal SIGIO 
/ 
/    if ( sigset( SIGIO, sock_lecture ) == SIG_ERR )
/    {
/        lib_erreur( 0, 1, "Err sigset(SIGIO, sock_lecture)" );
/    }
/
/    include du fichier sock.c apres recopie eventuelle
/    #include <sock.c>
/
***************************************************************************/

/*ABC***********************************************************************
* NOM         : sock.c
*
* PROJET      : PLHP
* PROCESS     : tous les communicants
* TYPE        : Source C
*
* ECRIT PAR   : D. DELEFORTERIE               Le 18/05/2010
*
* MODIFS      :
* DESCRIPTION : 
*
***********************************************************************CBA*/

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


/*XYZ*******************************************************************
/ NOM         : static void sock_lecture( int ) 
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/
/******************************************************************ZYX*/
static void
sock_lecture( P_signal )
int P_signal;
{
sigset_t   old_mask, bidon;
 
   /* Masque certains signaux */
    /* ----------------------- */
    old_mask = lib_mask_masque_signal( SIGIO   );
    bidon    = lib_mask_masque_signal( SIGUSR1 );
    bidon    = lib_mask_masque_signal( SIGUSR2 );
    bidon    = lib_mask_masque_signal( SIGCLD  );
    bidon    = lib_mask_masque_signal( SIGALRM );

    /* lecture en tant que client  */
    /* --------------------------- */
    lib_sock_c_kill_sigio();
    
    /* lecture en tant que serveur */
    /* --------------------------- */
    lib_sock_s_kill_sigio();
 
    /* Detournement du signal SIGIO */
    /* ---------------------------- */
    if( sigset( SIGIO, sock_lecture ) == SIG_ERR )
    {
        lib_erreur( 0, 1, "Err sock_lecture: sigset(SIGIO)" );
    }

    /* Demasque certains signaux */
    /* ------------------------- */
    lib_mask_demasque_signal( old_mask );

} /* end sock_lecture() */
