/*ABC***********************************************************************
*                                
* NOM        : lib_sleep.c
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
*        Librairie soporifique...
* 
***********************************************************************CBA*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Include systeme */
/* --------------- */
#include <time.h>
#include <sys/time.h>

/* pour connaitre G_msgerr */
/* ----------------------- */
#include <lib_erreur.h>

/* pour connaitre errno */
/* -------------------- */
#include <errno.h>

#include "lib_sleep.h"


/*XYZ***********************************************************************
/ NOM        : lib_sleep()
/
/ IN         : P_ms = Nb de milli-secondes = 10-3 sec
/
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/    Cette routine endort le process pendant le nb de milli secondes
/        passe en argument.
/    P_ms ne doit pas depasser 31 jours (=contrainte systeme)
/
/ **********************************************************************ZYX*/
void
lib_sleep( P_ms )
unsigned int P_ms; 
{
int                ret;
struct timeval    timeout;

    timeout.tv_sec = (unsigned long) (P_ms/1000);    /* secondes */
    timeout.tv_usec = (long)((P_ms%1000)*1000);    /* micro-secondes */
    
    ret = select( 3, 0, 0, 0, &timeout );

} /* end lib_sleep() */

/*XYZ***************************************************************************
/ NOM        : lib_sleep_milli_sec()
/
/ IN         : P_ms = Nb de milli-secondes = 10-3 sec
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/    Cette routine dort pendant N milli secondes
/
/ *************************************************************************ZYX*/
void
lib_sleep_milli_sec( P_ms )
int P_ms;
{
int              cpt = 0;
int              ret;
unsigned int     nb_millisec = P_ms;
struct timespec  rqtp, rmtp;


    rqtp.tv_sec = (time_t) ( nb_millisec/1000 );
    rqtp.tv_nsec = (long) ( (nb_millisec%1000) * 1000000 );

    while( cpt < 100 )
    {
        ret = nanosleep( &rqtp, &rmtp );
        if( ret == 0 )
        {
            /* le process a ete endormi le temps demande */
            /* ----------------------------------------- */
            return;
        }
        else if( ret == 1 )
        {
            /* nanosleep() interrompu par la reception d'un signal. */
            /* il reste a dormir un temps defini dans rmtp.         */
            /* ---------------------------------------------------- */
            cpt++;
            if( cpt > 25 )
            {
                sprintf( G_msgerr,
                     "Err lib_sleep(%d ms)=%d erno=%d cpt=%d reste %d,%09d s",
                            P_ms,
                            ret,
                            errno,
                            cpt,
                            (int) rmtp.tv_sec,
                            (int) rmtp.tv_nsec );
                /* lib_erreur( 0, 0, G_msgerr ); */
            }

            memcpy( (char*) &rqtp,
                    (char*) &rmtp,
                    sizeof( struct timespec ) );
        }
        else if( ret == -1 && errno == EINTR )
        {
            /* nanosleep() interrompu par la reception d'un signal. */
            /* il reste a dormir un temps defini dans rmtp.         */
            /* ---------------------------------------------------- */
            cpt++;
            if( cpt > 25 )
            {
                sprintf( G_msgerr,
                "Err EINTR lib_sleep(%d ms)=%d erno=%d cpt=%d reste %d,%09d s",
                            P_ms,
                            ret,
                            errno,
                            cpt,
                            (int) rmtp.tv_sec,
                            (int) rmtp.tv_nsec );
                /* lib_erreur( 0, 0, G_msgerr ); */
            }

            memcpy( (char*) &rqtp,
                    (char*) &rmtp,
                    sizeof( struct timespec ) );
        }
        else
        {
            /* Autre chose qu'une interruption de nanosleep() */
            /* par la reception d'un signal. (ret=-1)         */
            /* ---------------------------------------------- */
            sprintf( G_msgerr,
                        "Err lib_sleep(%d)=%d erno=%d rqtp=%d,%09d",
                        P_ms,
                        ret,
                        errno,
                        (int) rqtp.tv_sec,
                        (int) rqtp.tv_nsec );
            lib_erreur( 0, 1, G_msgerr );
            return;
        }

    } /* end while */

} /* end lib_sleep_milli_sec() */


/*XYZ***************************************************************************
/ NOM        : lib_sleep_sec()
/
/ IN         : P_secondes = Nb de secondes
/ IN         : P_centiemes_secondes = Nb de centimemes secondes = 10-2 sec
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/    Cette routine dort pendant N milli secondes
/
/ *************************************************************************ZYX*/
void
lib_sleep_sec( P_secondes, P_centiemes_secondes )
int P_secondes;
int P_centiemes_secondes;
{
int nb_millisec;

    nb_millisec = (P_secondes * 1000) + (P_centiemes_secondes * 10);
    lib_sleep_milli_sec( nb_millisec );

} /* end lib_sleep_sec() */


/*XYZ***************************************************************************
/ NOM        : lib_sleep_cent_sec()
/
/ IN         : P_c_sec = Nb de centimemes secondes = 10-2 sec
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/    Cette routine dort pendant N centiemes de secondes
/
/ *************************************************************************ZYX*/
void
lib_sleep_cent_sec( P_c_sec )
int P_c_sec;
{
int nb_millisec;

    nb_millisec = P_c_sec * 10;
    lib_sleep_milli_sec( nb_millisec );

} /* end lib_sleep_cent_sec() */
