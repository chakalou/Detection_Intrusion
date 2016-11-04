/*ABC***********************************************************************
*                                
* NOM        : lib_timer.h
*
* PROJET     : Detection
*
* PROCESS    :
*
* TYPE       : Include Librairie
*
* ECRIT PAR  : LOLIO        
*
* MODIFS     : 
*
* DESCRIPTION:
*    Declarations necessaires a l'utilisation de lib_timer.c.
*
* ATTENTION
*    on ne peut pas lancer un timer pour plus de 30 minutes !! 
*
***********************************************************************CBA*/
#ifndef LIB_TIMER
#define LIB_TIMER

/* Include systeme */
/* --------------- */
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>


/* nombre max de timers utilisables simultanement */
/*------------------------------------------------*/
#define LIB_TIMER_NB_MAX      9

/* valeurs possibles du champ 'type' */
/*-----------------------------------*/
#define LIB_TIMER_MONO        1    /* le champ 'actif' a deux valeurs 0 et 1 */
#define LIB_TIMER_MULTI       2    /* le champ 'actif' est un compteur       */

/* valeurs possibles du champ 'mode' */
/*-----------------------------------*/
#define LIB_TIMER_SIMPLE      1    /* le timer ne sera pas rearme */
                                   /* automatiquement             */
                                   /* apres 1ere expiration       */
#define LIB_TIMER_REPETITION  2    /* le timer sera rearme automatiquement */
                                   /* apres  chaque expiration             */

/* types */
/* ----- */
typedef struct
{
    int     actif;        /* 0 ou 1 : timer actif ou a l'arret          */
    int     type;         /* LIB_TIMER_MONO ou LIB_TIMER_MULTI          */
    int     mode;         /* LIB_TIMER_SIMPLE ou LIB_TIMER_REPETITION   */
    int     gap;
    
    void    (*proc)();    /* proc declenchee sur expiration du timer    */
                          /*--------------------------------------------*/
    float   freq;         /* duree logique du timer en 10-6 secondes    */
    float   nusec;        /*--------------------------------------------*/
                          /* duree restante pour un timer jusqu'a la    */
                          /* prochaine expiration du timer physique.    */
                          /* ce champ est recalcule a chaque expiration */
                          /* du timer physique.                         */
                          /*--------------------------------------------*/
} T_Timer;


/* fonctions */
/* --------- */
extern int        lib_timer_init();
extern void       lib_timer_lancer();
extern void       lib_timer_tester_et_stopper();
extern int        lib_timer_is_actif();
extern int        lib_timer_get();
extern void       lib_timer_affiche();
extern void       lib_timer_quit();

#endif
