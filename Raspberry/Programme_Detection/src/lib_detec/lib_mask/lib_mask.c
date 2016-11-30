/*ABC***********************************************************************
*                                
* NOM        : lib_mask.c
*
* PROJET     : Detection
*
* PROCESS    :
*
* TYPE       : Code Librairie
*
* ECRIT PAR  : LOLIO        
*
* MODIFS     :
*
* DESCRIPTION: 
*    Contient des fonctions de masquage / demasquage des signaux UNIX
*
* 
***********************************************************************CBA*/
#include "lib_mask.h"

/* pour connaitre lib_erreur() et G_msgerr */
/* --------------------------------------- */
#include <string.h>
#include <lib_erreur.h>

/* Fonctions locales a ce fichier */
/* ------------------------------ */
static void   affiche_int_bits();
static int    is_bit();


/*XYZ*********************************************************************
/ NOM        : lib_mask_masque_signal()
/ 
/ IN         : P_signal = signal a masquer
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     :
/     Masque de signaux courant avant l'ajout de P_signal.
/ 
/ DESCRIPTION:
/     Masque (interdit la reception d') UN signal UNIX
/     NB: Si un signal arrive alors qu'il est masque, il est memorise
/     par le systeme et sera re-declenche par UNIX apres son demasquage.
/ 
*********************************************************************ZYX*/
sigset_t
lib_mask_masque_signal( P_signal )
int P_signal;
{
int         ret, int_bidon, how;
sigset_t    old_set, mask_set;
/* le sizeof d'un sigset_t = 32 */

/************************
    switch( P_signal )
    {
        case SIGIO :
            sprintf( G_msgerr, "mask(SIGIO=%d)", P_signal );
            lib_erreur( 0, 0, G_msgerr );
            break;
        case SIGALRM :
            sprintf( G_msgerr, "mask(SIGALRM=%d)", P_signal );
            lib_erreur( 0, 0, G_msgerr );
            break;
        case SIGUSR1 :
            sprintf( G_msgerr, "mask(SIGUSR1=%d)", P_signal );
            lib_erreur( 0, 0, G_msgerr );
            break;
        case SIGUSR2 :
            sprintf( G_msgerr, "mask(SIGUSR2=%d)", P_signal );
            lib_erreur( 0, 0, G_msgerr );
            break;
        case SIGCHLD :
            sprintf( G_msgerr, "mask(SIGCHLD=%d)", P_signal );
            lib_erreur( 0, 0, G_msgerr );
            break;
        default :
            sprintf( G_msgerr, "mask(%d)", P_signal );
            lib_erreur( 0, 0, G_msgerr );
            break;
    }
    lib_mask_aff_masque( "deb lib_mask_masque_signal()" ); 
    
*************************/
    
    /* ------------------------------------ */
    /* on ajoute le signal P_signal.        */
    /* on initialise un set de signaux et   */
    /* on y positionne le signal a masquer. */
    /* ------------------------------------ */
    ret = sigemptyset( (sigset_t*) &mask_set );
    if( ret != 0 )
    {
        lib_erreur( 0, 1, "lib_mask_masque_signal() Err sigemptyset()" );
    }

    ret = sigaddset(    (sigset_t*) &mask_set,
                        (int) P_signal );
    if( ret != 0 )
    {
        lib_erreur( 0, 1, "lib_mask_masque_signal() Err sigaddset()" );
    }
    
    /* mise en place du nouveau masque. */
    /* Le nouveau masque est l'union du */
    /* masque courant et du masque que  */
    /*  l'on vient de fabriquer !       */
    /* -------------------------------- */
    how = (int) SIG_BLOCK;
    ret = sigemptyset( (sigset_t*) &old_set );
    if( ret != 0 )
    {
        lib_erreur( 0, 1, "lib_mask_masque_signal() Err sigemptyset()" );
    }

    ret = sigprocmask(    (int) how,
                        (sigset_t*) &mask_set,
                        (sigset_t*) &old_set );
    if( ret != 0 )
    {
        lib_erreur( 0, 1, "lib_mask_masque_signal() Err sigprocmask()" );
    }
    
    /*
    affiche_int_bits( mask_set, "fin 1 lib_mask_masque_signal()" );
    affiche_int_bits( old_set, "fin 2 lib_mask_masque_signal()" );
    lib_mask_aff_masque( "fin lib_mask_masque_signal()" );
    */

    /* on renvoie l'ancien masque */
    /* -------------------------- */
    return( old_set );

} /* end lib_mask_masque_signal() */


/*XYZ*********************************************************************
/ NOM        : lib_mask_get_masque()
/ 
/ IN         : Neant
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     :
/     Masque de signaux courant.
/ 
/ DESCRIPTION:
/ 
*********************************************************************ZYX*/
sigset_t
lib_mask_get_masque( )
{
int         ret, int_bidon, how;
sigset_t    old_set, mask_set;

    /* ------------------------------------ */
    /* on initialise un set de signaux      */
    /* ------------------------------------ */
    ret = sigemptyset( (sigset_t*) &mask_set );
    if( ret != 0 )
    {
        lib_erreur( 0, 1, "lib_mask_masque_signal() Err sigemptyset()" );
    }
    
    /* on recupere le mask en cours !     */
    /* en faisant l'union du mask courant */
    /* avec un mask vide .                */
    /* ---------------------------------- */
    how = (int) SIG_BLOCK;
    ret = sigemptyset( (sigset_t*) &old_set );
    if( ret != 0 )
    {
        lib_erreur( 0, 1, "lib_mask_masque_signal() Err sigemptyset()" );
    }

    ret = sigprocmask(    (int) how,
                        (sigset_t*) &mask_set,
                        (sigset_t*) &old_set );
    if( ret != 0 )
    {
        lib_erreur( 0, 1, "lib_mask_masque_signal() Err sigprocmask()" );
    }
    
    /* on renvoie l'ancien masque */
    /* -------------------------- */
    return( old_set );

} /* end lib_mask_get_masque() */


/*XYZ*********************************************************************
/ NOM        : lib_mask_aff_masque()
/ 
/ IN         : Neant
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     :
/ DESCRIPTION:
/ 
*********************************************************************ZYX*/
void
lib_mask_aff_masque( P_blabla )
char* P_blabla;
{
sigset_t set;

    set = lib_mask_get_masque();
    affiche_int_bits( set, P_blabla );

} /* end lib_mask_aff_masque() */


/*XYZ*********************************************************************
/ NOM        : lib_mask_demasque_signal()
/ 
/ IN         : P_oldset = masque a repositionner
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/     Positionne un masque de signaux passe en parametre.
/ 
*********************************************************************ZYX*/
void
lib_mask_demasque_signal( P_oldset )
sigset_t P_oldset;
{
    int ret, how;
    
    /*
    affiche_int_bits( P_oldset, "deb lib_mask_demasque_signal()" );
    */

    how = (int) SIG_SETMASK;
    ret = sigprocmask(    (int) how,
                        (sigset_t*) &P_oldset,
                        (sigset_t*) NULL );
    if( ret != 0 )
    {
        lib_erreur( 0, 1, "lib_mask_demasque_signal() Err sigprocmask()" );
    }

    /*
    affiche_int_bits( P_oldset, "fin lib_mask_demasque_signal()" );
    */

} /* end lib_mask_demasque_signal() */


/*XYZ*********************************************************************
/ NOM        : lib_mask_rend_signal()
/ 
/ IN         : P_signal = signal a demasquer
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     :
/         0 si OK
/        -1 si erreur systeme
/ 
/ DESCRIPTION:
/     Demasque (rajoute la reception de) UN signal UNIX.
/    Necessaire car GPX lance l'application a l'aide de lib_timer, ce
/        qui signifie qu'a ce moment la, les signaux SIGALRM, SIGIO,
/        SIGUSR1 et SIGUSR2 sont masques.
/ 
*********************************************************************ZYX*/
int
lib_mask_rend_signal( P_signal )
int P_signal;
{
int ret;

/********************
    switch( P_signal )
    {
        case SIGIO :
            sprintf( G_msgerr, "rend(SIGIO=%d)", P_signal );
            lib_erreur( 0, 0, G_msgerr );
            break;
        case SIGALRM :
            sprintf( G_msgerr, "rend(SIGALRM=%d)", P_signal );
            lib_erreur( 0, 0, G_msgerr );
            break;
        case SIGUSR1 :
            sprintf( G_msgerr, "rend(SIGUSR1=%d)", P_signal );
            lib_erreur( 0, 0, G_msgerr );
            break;
        case SIGUSR2 :
            sprintf( G_msgerr, "rend(SIGUSR2=%d)", P_signal );
            lib_erreur( 0, 0, G_msgerr );
            break;
        case SIGCHLD :
            sprintf( G_msgerr, "rend(SIGCHLD=%d)", P_signal );
            lib_erreur( 0, 0, G_msgerr );
            break;
        default :
            sprintf( G_msgerr, "rend(%d)", P_signal );
            lib_erreur( 0, 0, G_msgerr );
            break;
    }
********************/

    ret = sigrelse( (int) P_signal );
    if( ret != 0 )
    {
        sprintf( G_msgerr,
                    "lib_mask_demasque_signal() : sigrelse(%i) = -1 !!",
                        P_signal );
        lib_erreur( 0, 1, G_msgerr );
    }
    
    return( ret );

} /* end lib_mask_rend_signal() */


/*XYZ*********************************************************************
/ NOM        : is_bit()
/
/ IN         : Neant 
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     :
/     Renvoie 0 ou 1
/
/ DESCRIPTION:
/    Renvoie la valeur du P_i eme bit de P_int.
*********************************************************************ZYX*/
static int
is_bit( P_int, P_i )
int  P_int;
int  P_i;
{
    return( (int) (P_int >> P_i) & (int) 1 );
}

/*XYZ*********************************************************************
/ NOM        : affiche_int_bits()
/
/ IN         : Neant
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/    Affiche tous les bits d'un integer precedes d'un message.
*********************************************************************ZYX*/
static void
affiche_int_bits( P_sigset, P_message )
sigset_t  P_sigset;
char*     P_message;
{
int  i, *pi;
char strerr[256];
char strtmp[80];

    pi = (int*) &P_sigset;

    sprintf( strerr, "%s : ", P_message );
    for( i=0; i<sizeof(sigset_t); i++ )
    {
        if( !(i % (int) 8) )
            strcat( strerr, "." );
        sprintf( strtmp, "%1d", is_bit( (int) *pi, (int) i ) );
        strcat( strerr, strtmp );
    }
    strcat( strerr, "." );
    lib_erreur( 0, 0, strerr );

} /* end affiche_int_bits() */
