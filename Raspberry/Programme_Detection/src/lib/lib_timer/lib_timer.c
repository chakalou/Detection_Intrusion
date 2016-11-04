/*ABC***********************************************************************
*                                
* NOM        : lib_timer.c
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
*        Contient les proc de gestion des timers.
*
***********************************************************************CBA*/
#include <stdlib.h>

/* Include systeme */
/* --------------- */
#include <lib_erreur.h>

#include <lib_mask.h>

#include "lib_timer.h"


/* constantes locales */
/* ------------------ */
#define UN_MILLION    1000000

/* types locaux */
/* ------------ */
typedef struct
{
    double    tv_sec;
    double    tv_usec;
    
} T_interval;

typedef struct
{
    T_interval    it_interval;
    T_interval    it_value;
    
} T_itimerval;

/* macros locales */
/* -------------- */
#define SET_BIT(a, i)   (a |= (1 << i))   /* met a '1' le bit 'i' de 'a' */
#define CLEAR_BIT(a, i) (a &= ~(1 << i))  /* met a '0' le bit 'i' de 'a' */


/* tableau des structures des timers logiques utilise sous Unix */
/*--------------------------------------------------------------*/
T_Timer          L_timer[LIB_TIMER_NB_MAX];

int              L_lib_timer_actif;
                    /*------------------------------------------------*/
                    /* regroupe les infos 'actif' de tous les timers  */
                    /* chaque bit correspond a un timer.              */
                    /* Sous Unix, cette variable indique que le timer */
                    /*   physique est actif.                          */
                    /*------------------------------------------------*/
                        
double           L_lib_timer_frequence_usec;
                    /*-------------------------------------------------*/
                    /* Sous Unix, frequence courante du timer physique */
                    /*              (nb de microsecondes)              */
                    /*-------------------------------------------------*/
            
double           L_lib_timer_duree_ecoulee_usec;
                    /*------------------------------------------------------*/
                    /* Sous Unix, micro secondes ecoulees (a l'expiration)  */
                    /*     depuis le dernier re-armement du timer physique. */
                    /*------------------------------------------------------*/
                            

/* prototypage des fonctions locales */
/* --------------------------------- */
static void            timer_handler_unix();
static void            timer_phys_valeur();
static void            timer_phys_lancer();
static void            timer_phys_stopper();
static void            timer_conv_usec_to_sec();
static void            timer_conv_sec_to_usec();
static void            timer_affiche();


#define LIB_TIMER_DEBUG              0


/*XYZ*****************************************************************
/ NOM        : void lib_timer_affiche( P_commentaire )
/ 
/ IN         : char *P_commentaire = commentaire
/
/ OUT         : Neant
/ INOUT         : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/     acces a timer_affiche en dehors de la librairie.
/     uniquement pour test.
*****************************************************************ZYX*/
void
lib_timer_affiche( P_commentaire )
char* P_commentaire;
{
    timer_affiche( P_commentaire );

} /* end lib_timer_affiche() */

/*XYZ*****************************************************************
/ NOM        : static void timer_affiche( P_commentaire )
/ 
/ IN         : char    *P_commentaire = commentaire
/
/ OUT         : Neant
/ INOUT         : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/     Affiche l'etat des timers quand on passe dans une des
/               procedures de ce fichier.
*****************************************************************ZYX*/
static void
timer_affiche( P_commentaire )
char    *P_commentaire;
{
int        i;
    
    if( strcmp(P_commentaire, "") != 0 ) printf( "%s\n", P_commentaire );
    
    printf( "\t T_Timer Unix physique : (F=%lf, D=%lf) \n",
            L_lib_timer_frequence_usec,
            L_lib_timer_duree_ecoulee_usec );
    
    for ( i=0; i<LIB_TIMER_NB_MAX; i++ )
    {
        printf( "\t timer No %d  %s", i,
                (L_timer[i].actif) ? "   ACTIF" : " INACTIF" );
                
        printf("  reste %7f sec/ f= %7f \n", L_timer[i].nusec,
                                             L_timer[i].freq );
    } 

    printf( "\n" );

} /* end timer_affiche() */


/*XYZ*****************************************************************
/ NOM        : int lib_timer_init()
/ 
/ IN         :    int P_timer = numero du timer a initialiser.
/ IN         :    int P_type = type du timer.
/                 LIB_TIMER_MONO
/                     le champ 'actif' ne peut avoir que les valeurs 0 et 1.
/                 LIB_TIMER_MULTI
/                     le champ actif est un compteur qui est incremente
/                   a chaque appel a
/                     'lib_timer_lancer()' et est decremente a chaque appel a
/                     'lib_timer_tester_et_stopper()'.
/ IN         :    int P_mode = mode de re-armement du timer.
/                 LIB_TIMER_SIMPLE :
/                     pas de re-armement automatique apres expiration.
/                 LIB_TIMER_REPETITION :
/                     re-armement automatique apres expiration.
/ IN         :    void (*P_proc)() 
/                    = fonction de traitement a appeler a chaque expiration.
/             
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : 
/        0 si tout s'est bien passe.
/        -1 si le numero de timer demande est trop grand.
/     
/ DESCRIPTION:
/     Initialise ou reinitialise un timer.
/     Cette fonction doit etre appelee autant de fois que l'on veut
/     de timers fonctionnant simultanement.
/     
/     NB: Une reinitialisation ne change rien au caractere actif ou non
/     d'un timer ni a sa frequence ou au temps avant sa prochaine
/     expiration.
/     Seuls sont modifies les champs 'type', 'mode' et la procedure de
/     traitement associee.
/
*****************************************************************ZYX*/
int
lib_timer_init( P_timer, P_type, P_mode, P_proc )
int            P_timer;
int            P_type;
int            P_mode;
void           (*P_proc)();
{
static int    premiere_fois = 1;
int           i;

    /* controle de la validite du parametre IN P_timer. */
    /*--------------------------------------------------*/
    if( P_timer >= LIB_TIMER_NB_MAX )
    {
        sprintf(    G_msgerr,
                    "lib_timer_init(): Err num timer %d > max (= %d)",
                    P_timer, LIB_TIMER_NB_MAX - 1);
        lib_erreur( 0, 0, G_msgerr );
        return( -1 );
    }
    
    /* Initialisation de toutes les variables locales */
    /* de la librairie des timers.                    */
    /*------------------------------------------------*/
    if( premiere_fois )
    {
        premiere_fois                    = 0;

        L_lib_timer_actif                = 0;
        L_lib_timer_frequence_usec       = 0.0;
        L_lib_timer_duree_ecoulee_usec   = 0.0;
        
        for( i=0; i<LIB_TIMER_NB_MAX; i++ )
        {
                L_timer[i].actif    =  0;
                L_timer[i].type     = -1;
                L_timer[i].mode     = -1;
                L_timer[i].proc     = (void*) NULL;
                L_timer[i].freq     = (float) 0.0;
                L_timer[i].nusec    = (float) 0.0;
        } 

        lib_erreur( 0, 0, 
            "lib_timer_init(): 1ere Init de T_Timer pour ce process" );
    } 
    
    /* Initialisation du timer specifique... */
    /*---------------------------------------*/
    L_timer[P_timer].type = P_type;
    L_timer[P_timer].mode = P_mode;
    L_timer[P_timer].proc = P_proc;

    return( 0 );
    
} /* end lib_timer_init() */


/*XYZ*********************************************************************
/ NOM        : int lib_timer_is_actif( P_timer )
/ 
/ IN         : int P_timer = indique le timer a tester.
/
/ OUT         : Neant
/ INOUT         : Neant
/ RETOUR     :
/    Renvoie le champ 'actif' du timer P_timer.
/
/ DESCRIPTION: 
/
*****************************************************************ZYX*/
int
lib_timer_is_actif( P_timer )
int P_timer;
{

    return( L_timer[P_timer].actif );
                
} /* end lib_timer_is_actif() */


/*XYZ*********************************************************************
/ NOM        : int lib_timer_get
/              ( P_timer, P_p_freq, P_p_nusec, P_p_type, P_p_mode, P_p_proc )
/ 
/ IN         : int P_timer = indique le timer a tester.
/ 
/ OUT        : double  *P_p_freq = pointeurs sur des variables a remplir
/              double *P_p_nusec ...
/              int     *P_p_type ...
/              int     *P_p_mode ...
/              void    (**P_p_proc)() ...
/                  Si le pointeur est non nul, le parametre demande est
/                  renvoye.
/ INOUT      : Neant    
/ RETOUR     :
/    Renvoie le champ 'actif' du timer P_timer.
/    Renvoie -1 si le timer n'est pas initialise ou n'existe pas.
/     
/ DESCRIPTION:
/     Renvoie les elements de la structure T_Timer du timer P_timer.
/     Rend accessible a un utilisateur des donnes internes de la
/         librairie lib_timer.
/
*****************************************************************ZYX*/
int
lib_timer_get( P_timer, P_p_freq, P_p_nusec, P_p_type, P_p_mode, P_p_proc )
int         P_timer;
double     *P_p_freq;
double     *P_p_nusec;
int        *P_p_type;
int        *P_p_mode;
void       (**P_p_proc)();
{

    /* controle de la validite du parametre IN P_timer. */
    /* controle de l'initialisation du timer P_timer    */
    /*     dont on demande des informations.            */
    /*--------------------------------------------------*/
    if ((P_timer >= LIB_TIMER_NB_MAX) || (L_timer[P_timer].type == -1))
    {
        sprintf( G_msgerr,
         "lib_timer_get(): acces a timer non initialise/inexistant: %d.",
                    P_timer );
        lib_erreur( 0, 0, G_msgerr );
        return( -1 );
    }
    
    if (P_p_freq)  *P_p_freq  = L_timer[P_timer].freq;
    if (P_p_nusec) *P_p_nusec = L_timer[P_timer].nusec;
    if (P_p_type)  *P_p_type  = L_timer[P_timer].type;
    if (P_p_mode)  *P_p_mode  = L_timer[P_timer].mode;
    if (P_p_proc)  *P_p_proc  = L_timer[P_timer].proc;

    return( L_timer[P_timer].actif );
    
} /* end lib_timer_get() */


/*XYZ*****************************************************************
/ NOM        :  void lib_timer_lancer( P_timer, P_seconde, P_useconde )
/ 
/ IN         : int P_timer = indique le timer a lancer.
/                 (de 0 a LIB_TIMER_NB_MAX).
/
/ IN         : int P_seconde = frequence du timer (en secondes).
/ IN         : int P_useconde = frequence du timer (en micro secondes).
/      (frequence = 1e6 * P_seconde + P_useconde (en micro_secondes))
/
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/    Sortie par exit(-1) en cas d'erreur
/ 
/ DESCRIPTION: Lance un timer Unix.
/
/ *****************************************************************ZYX*/
void
lib_timer_lancer( P_timer, P_seconde, P_useconde )
int P_timer;
int P_seconde;
int P_useconde;
{
T_itimerval     new_timer, old_timer;
int             i;
double          delta, n_usec, temps_qui_reste, temps_ecoule ;
sigset_t        old_mask, bid_mask;
    
    
    /* controle de la validite du parametre IN P_timer. */
    /* controle de l'initialisation du timer P_timer    */
    /*     dont on demande des informations.            */
    /*--------------------------------------------------*/
    if ((P_timer >= LIB_TIMER_NB_MAX) || (L_timer[P_timer].type == -1))
    {
        sprintf(G_msgerr,
                "lib_timer_lancer: acces non initialise/inexistant: %d",
                P_timer );
        lib_erreur( 0, 0, G_msgerr );
        exit( -1 );
    }
    
    /* on masque les signaux usuels pour ne pas etre interrompu */
    /*----------------------------------------------------------*/
    old_mask = lib_mask_masque_signal( (int) SIGALRM );
    bid_mask = lib_mask_masque_signal( (int) SIGIO );
    bid_mask = lib_mask_masque_signal( (int) SIGUSR1 );
    bid_mask = lib_mask_masque_signal( (int) SIGUSR2 );
    
    n_usec = (double) ( ((double )UN_MILLION * (double) P_seconde)
                        +
                        (double) P_useconde );
    
#ifdef LIB_TIMER_HANDLER_DEBUG    
    sprintf( G_msgerr, "DEBUT lib_timer_lancer(%i, %is + %ius)",
             P_timer, P_seconde, P_useconde );
    timer_affiche( G_msgerr );
#endif

    /* Il faut verifier avant de rearmer le timer physique que celui-ci */
    /* n'est pas deja actif , et si oui, si il ne l'est pas deja a une  */
    /* frequence inferieure a laquelle on veut le lancer maintenant ?   */
    /*-------------------------------------------------------------------*/
    if ( !L_lib_timer_actif )
    {        
        /* aucun timer n'est encore actif */
        /* on va lancer le 1er timer.     */
        /*--------------------------------*/
        L_lib_timer_frequence_usec      = n_usec;
        L_lib_timer_duree_ecoulee_usec  = n_usec;

        new_timer.it_interval.tv_sec    = 0.0;
        new_timer.it_interval.tv_usec   = n_usec;
        new_timer.it_value.tv_sec       = 0.0;
        new_timer.it_value.tv_usec      = n_usec;    
        
        timer_phys_lancer( &new_timer );
    }
    else
    {
        /* un timer est deja actif ! Il ne faut rearmer le timer que si */
        /* la nouvelle frequence demandee est plus petite !             */
        /*--------------------------------------------------------------*/
        if ( n_usec < L_lib_timer_frequence_usec )
        {
            /* la nvelle frequence devient la frequence physique */
            /*---------------------------------------------------*/
            L_lib_timer_frequence_usec = n_usec;
            
            /* on recupere timer en cours pour que la prochaine expiration */
            /* soit quand meme respectee malgre le changement de frequence */
            /*-------------------------------------------------------------*/
            timer_phys_valeur( &old_timer );
            
            temps_qui_reste = old_timer.it_value.tv_usec;
            temps_ecoule    = old_timer.it_interval.tv_usec
                            - old_timer.it_value.tv_usec;
            
            new_timer.it_interval.tv_sec  = 0.0;
            new_timer.it_interval.tv_usec = n_usec;
            
            /* on met a jour pour tous les timers qui etaient actifs */
            /* le temps qu'ils leur restent jusqu'a la prochaine     */
            /* expiration du timer physique.                         */
            /*-------------------------------------------------------*/
            for( i=0; i<LIB_TIMER_NB_MAX; i++ )
            {
                if( L_timer[i].actif && (P_timer != i) )
                {
                    /* pdt qu'on lance un nveau timer, il ne se passe */
                    /* pour un timer existant que le temps du traite- */
                    /* -ment. Si pour un timer il restait 30s avant   */
                    /* expiration, il restera toujours 30s moins epsi-*/
                    /* -lon apres qu'on en est lance un autre.        */
                    /*------------------------------------------------*/
                    L_timer[i].nusec -=
                       (L_lib_timer_duree_ecoulee_usec - temps_qui_reste);
                }
            }

            if ( n_usec < temps_qui_reste )
            {
                /* cas ou la nvelle frequence est inferieure */
                /* au temps qu'il reste jusqu'a la procahine */
                /* expiration.                               */
                /*-------------------------------------------*/
                new_timer.it_value.tv_sec       = 0.0;
                new_timer.it_value.tv_usec      = n_usec;
                L_lib_timer_duree_ecoulee_usec  = n_usec;
            }
            else
            {
                /* cas ou le temps qu'il reste jusqu'a la */
                /* la procahine expiration est inferieure */
                /* a la nouvell frequence.                */
                /*----------------------------------------*/
                new_timer.it_value.tv_sec       = temps_qui_reste;
                new_timer.it_value.tv_usec      = temps_qui_reste;
                L_lib_timer_duree_ecoulee_usec  = temps_qui_reste;
            }    
            
            timer_phys_lancer( &new_timer );
        }
        else
        {
            /* on est en train de lancer un nieme timer dont la frequence  */
            /* d'expiration est >= a la frequence en cours. (rien a faire) */
            /*-------------------------------------------------------------*/
        }

    } /* end if ( !L_lib_timer_actif ) */

    /* on arrive ici dans tous les cas :                            */
    /* - on est en train de lancer un 1er timer.                    */
    /* - on est en train de lancer un nieme timer dont la frequence */
    /*    d'expiration est >= a la frequence en cours.                */
    /* - on est en train de lancer un nieme timer dont la frequence */
    /*    d'expiration est < a la frequence en cours.                 */
    /*--------------------------------------------------------------*/
    
#ifdef LIB_TIMER_DEBUG_MSG
    sprintf( G_msgerr, "LANCEMENT du timer %d", P_timer );
    lib_erreur( 0, 0, G_msgerr );
#endif

    if( L_timer[P_timer].type == LIB_TIMER_MULTI )
    {
        ( L_timer[P_timer].actif )++;
    }
    else
    {
        L_timer[P_timer].actif = 1;    
    }
    
    SET_BIT( L_lib_timer_actif, P_timer );
    
    L_timer[P_timer].nusec = n_usec;
    L_timer[P_timer].freq  = n_usec;

#ifdef LIB_TIMER_HANDLER_DEBUG
    timer_affiche( "FIN   lib_timer_lancer()" );
    printf( "\n" );
#endif
    
    fin_lib_timer_lancer:;

    /* on repositionne l'ancien masque de signaux */
    /*--------------------------------------------*/
    lib_mask_demasque_signal( old_mask );

} /* end lib_timer_lancer() */


/*XYZ*****************************************************************
/ NOM        : void lib_timer_tester_et_stopper( P_timer )
/ 
/ IN         : int P_timer = indique le timer a stopper.
/
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/    Sortie par exit(-1) en cas d'erreur
/ 
/ DESCRIPTION:
/     Stoppe le timer si et seulement si il n'est plus necessaire:
/
*****************************************************************ZYX*/
void
lib_timer_tester_et_stopper( P_timer )
int P_timer;
{
T_itimerval       new_timer, current_timer;
double            current_frequence, new_frequence;
double            current_nusec, new_nusec;
double            temps_qui_reste, temps_ecoule;
int               i;
sigset_t          old_mask, bid_mask;


    if ((P_timer >= LIB_TIMER_NB_MAX) || (L_timer[P_timer].type == -1))
    {
        sprintf( G_msgerr,
                 "lib_timer...stopper: timer non initialise/inexistant: %d.",
                 P_timer );
        lib_erreur( 0, 0, G_msgerr );
        exit( -1 );
    }

    old_mask = lib_mask_masque_signal( (int) SIGALRM );
    bid_mask = lib_mask_masque_signal( (int) SIGIO );
    bid_mask = lib_mask_masque_signal( (int) SIGUSR1 );
    bid_mask = lib_mask_masque_signal( (int) SIGUSR2 );
    

#ifdef LIB_TIMER_HANDLER_DEBUG
    sprintf( G_msgerr, "DEBUT lib_timer_tester_et_stopper(%i)", P_timer );
    timer_affiche( G_msgerr );
#endif

    /*** on recupere la valeur actuelle du timer physique ***/
    /*------------------------------------------------------*/
    timer_phys_valeur( &current_timer );
    temps_qui_reste = current_timer.it_value.tv_usec;
    temps_ecoule    = current_timer.it_interval.tv_usec
                    - current_timer.it_value.tv_usec;
    
    /* on actualise les champs 'actif', 'nusec' */
    /* et 'freq' du timer a stopper.           */
    /*-----------------------------------------*/
    if( L_timer[P_timer].actif > 0 )
    {
        (L_timer[P_timer].actif)--;
        
        if( L_timer[P_timer].actif == 0 )
        {
            CLEAR_BIT(L_lib_timer_actif, P_timer);
            L_timer[P_timer].nusec = 0.0;
            L_timer[P_timer].freq  = 0.0;
        }
    }

    /*** on decremente le champ 'nusec' de tous les timers actifs ***/
    /*--------------------------------------------------------------*/
    for( i=0; i<LIB_TIMER_NB_MAX; i++ )
    {        
        if( L_timer[i].actif )
        {
            L_timer[i].nusec -=
                ( L_lib_timer_duree_ecoulee_usec - temps_qui_reste );
        }
    }

#ifdef LIB_TIMER_HANDLER_DEBUG
    timer_affiche( "Etape 1" );
#endif
    
    if ( !L_lib_timer_actif )
    {
        /* plus aucun timer n'est actif */
        /*------------------------------*/
        L_lib_timer_frequence_usec     = 0.0;
        L_lib_timer_duree_ecoulee_usec = 0.0;
        
        timer_phys_stopper();    
    }
    else
    {
        /* il reste des timers en cours !                  */
        /* il faut ajuster la frequence et la duree avant  */
        /* la prochaine expiration, en fonction des timers */
        /* qui restent actifs.                             */
        /*-------------------------------------------------*/
        current_frequence = 1e99 ;
        new_frequence     = 1e99;
        current_nusec     = temps_qui_reste;
        new_nusec         = temps_qui_reste;
        
        /*--------------------------------------------------------------*/
        /* On recherche (pour les timers actifs) :                      */
        /* - la plus petite frequence                                   */
        /* - la plus petite duree avant la prochaine expiration         */
        /*--------------------------------------------------------------*/
        for( i=0; i<LIB_TIMER_NB_MAX; i++ )
        {
            if( !(L_timer[i].actif) ) continue;
            
            if ( L_timer[i].freq < new_frequence )
            {
                new_frequence = L_timer[i].freq;
            }
            if ( L_timer[i].nusec < new_nusec )
            {
                new_nusec = L_timer[i].nusec;
            }
        } 
        
        if ( new_nusec <= 0.0 )     new_nusec     = 1e5;    /* 0.1s */
        if ( new_frequence <= 0.0 ) new_frequence = 1e5;    /* 0.1s */
        
        if ( ( new_frequence < current_frequence ) ||
             ( new_nusec < current_nusec ) )
        {
            /* un timer necessitant la diminution de la frequence   */
            /* reste actif !                                        */
            /*------------------------------------------------------*/
            L_lib_timer_frequence_usec      = new_frequence;
            L_lib_timer_duree_ecoulee_usec  = new_nusec;
    
            new_timer.it_interval.tv_sec    = 0.0;
            new_timer.it_interval.tv_usec   = new_frequence;
            new_timer.it_value.tv_sec       = 0.0;
            new_timer.it_value.tv_usec      = new_nusec;    
            
            timer_phys_lancer( &new_timer );
        }
    }

#ifdef LIB_TIMER_HANDLER_DEBUG
    timer_affiche( "FIN   lib_timer_tester_et_stopper()" );
    printf( "\n" );
#endif

    fin_lib_timer_tester_et_stopper:;

    lib_mask_demasque_signal( old_mask );

} /* end lib_timer_tester_et_stopper() */


/*XYZ******************************************************************
/ NOM        : timer_handler_unix()
/
/ IN         : Neant
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/     Fonction de reponse au signal SIGALRM genere par l'expiration
/     du timer cree par la fonction 'lib_timer_lancer()'.
/     SI L'ON N'EST PAS SOUS XVIEW.
/
*****************************************************************ZYX*/
static void
timer_handler_unix( P_signal )
int P_signal;
{
int		i, modif_flag, stop_flag;
double		nusec, nusec_min;
T_itimerval	new_timer;
sigset_t	old_mask, bid_mask;


    old_mask = lib_mask_masque_signal( (int) SIGALRM );
    bid_mask = lib_mask_masque_signal( (int) SIGIO );
    bid_mask = lib_mask_masque_signal( (int) SIGUSR1 );
    bid_mask = lib_mask_masque_signal( (int) SIGUSR2 );

#ifdef LIB_TIMER_HANDLER_DEBUG
    sprintf(    G_msgerr,
                "lib_timer_handler(): expiration apres %lf s.\n",
                L_lib_timer_duree_ecoulee_usec / (double)UN_MILLION );
    lib_erreur( 0, 0, G_msgerr );
#endif

#ifdef LIB_TIMER_HANDLER_DEBUG
    timer_affiche( "DEBUT lib_timer_handler()\n" );
#endif

    /* on decremente tous les timers du temps */
    /* qui vient de s'ecouler.                */
    /*----------------------------------------*/
    for ( i=0; i<LIB_TIMER_NB_MAX; i++ )
    {        
        if (L_timer[i].actif)
        {
            L_timer[i].nusec -= L_lib_timer_duree_ecoulee_usec;
        }
    } 
    
    /*-------------------------------------------------------*/
    /* car le timer est deja reparti tout seul pour          */
    /* un nb de secondes egal a 'L_lib_timer_frequence_usec' */
    /*-------------------------------------------------------*/
    L_lib_timer_duree_ecoulee_usec = L_lib_timer_frequence_usec;

    stop_flag = 0;
    for( i=0; i<LIB_TIMER_NB_MAX; i++ )
    {
        if ( (L_timer[i].actif) && (L_timer[i].nusec <= 0.0) )
        {
            /* passer la main a la procedure de traitement */
            /* d'expiration adequate                       */
            /*---------------------------------------------*/
            (*(L_timer[i].proc))(i);

            if (L_timer[i].mode == LIB_TIMER_REPETITION)
            {    
                /* si le timer est a re-armement automatique          */            
                /* rearmer le timer (au cas ou il ne sera pas arrete) */
                /*----------------------------------------------------*/
                L_timer[i].nusec += L_timer[i].freq;
            }
            else
            {
                lib_timer_tester_et_stopper(i);
                stop_flag = 1;
            }
        }
    } 

    if ( !stop_flag )
    {
        nusec_min = 1e99;

        modif_flag = 0;
        for( i=0; i<LIB_TIMER_NB_MAX; i++ )
        {
            if ( !(L_timer[i].actif) ) continue;
                
            /* nusec ne peut (ne doit) jamais etre egal a zero */
            /*-------------------------------------------------*/
            nusec = L_timer[i].nusec;
            if ( nusec < L_lib_timer_frequence_usec )
            {
                modif_flag = 1;
                if (nusec_min > nusec) nusec_min = nusec;
            }
        }
        
        if ( modif_flag )
        {
            /* modifier la prochaine expiration sans changer la frequence */
            /*------------------------------------------------------------*/
            new_timer.it_interval.tv_sec    = 0.0;
            new_timer.it_interval.tv_usec   = L_lib_timer_frequence_usec;
            new_timer.it_value.tv_sec       = 0.0;
            
            /* ---------------------------- */
            /* nusec_min == 0 bloque tout ! */
            /* ---------------------------- */
            if ( nusec_min <= 0.0 ) 
            {
                sprintf(G_msgerr, 
                        "timer_handler_unix(USEC TROP PETIT %lf a freq =%lf)",
                        nusec_min, L_lib_timer_frequence_usec );
                lib_erreur( 0, 0, G_msgerr );
                
                nusec_min = (double) 1.0 ;
            }
            /* ---------------------------- */
            
            new_timer.it_value.tv_usec     = nusec_min;
            L_lib_timer_duree_ecoulee_usec = nusec_min;
            
            timer_phys_lancer( &new_timer );
        }
        else
        {
            L_lib_timer_duree_ecoulee_usec = L_lib_timer_frequence_usec;
        }

    } /* end if */

#ifdef LIB_TIMER_HANDLER_DEBUG
    timer_affiche( "FIN   lib_timer_handler()\n" );
#endif

    lib_mask_demasque_signal( old_mask );

} /* end timer_handler_unix() */


/*XYZ*****************************************************************
/ NOM        : static void timer_phys_valeur( P_p_timer )
/ / 
/ OUT        : T_itimerval *P_p_timer = structure a remplir
/
/ OUT        : Neant
/ INOUT         : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/     Recupere la valeur courante du timer physique.
/ 
*****************************************************************ZYX*/
static void
timer_phys_valeur( P_p_timer )
T_itimerval    *P_p_timer;
{
struct itimerval    itval;
    
    getitimer( ITIMER_REAL, &itval );
    
    timer_conv_sec_to_usec( P_p_timer, &itval );

} /* end timer_phys_valeur() */


/*XYZ*****************************************************************
/ NOM        : static void timer_phys_lancer( P_p_timer )
/ 
/ IN         : T_itimerval *P_p_timer = valeur du timer
/
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/     Lance le timer physique.
/ 
*****************************************************************ZYX*/
static void
timer_phys_lancer( P_p_timer )
T_itimerval    *P_p_timer;
{
struct itimerval    itval;
    
    timer_conv_usec_to_sec( &itval, P_p_timer );

    setitimer( ITIMER_REAL, &itval, (struct itimerval *)NULL );
        
    if ( sigset( (int) SIGALRM, timer_handler_unix ) == SIG_ERR )
    {
    	lib_erreur( 0, 1, "Err timer_phys_lancer()" );
    	lib_erreur( 0, 0, "\t Err sigset(SIGALRM)" );
    }

} /* end timer_phys_lancer() */


/*XYZ*****************************************************************
/ NOM        : static void timer_phys_stopper()
/ 
/ IN         : Neant
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/     Stoppe le timer physique.
/
*****************************************************************ZYX*/
static void
timer_phys_stopper()
{
struct itimerval    timer;
    
   
    timer.it_interval.tv_sec  = 0;
    timer.it_interval.tv_usec = 0;
    timer.it_value.tv_sec     = 0;
    timer.it_value.tv_usec    = 0;
    
    setitimer( ITIMER_REAL, &timer, (struct itimerval *)NULL );

} /* end timer_phys_stopper() */


/*XYZ*****************************************************************
/ NOM        : static void timer_conv_usec_to_sec( P_p_itval, P_p_timer )
/ 
/ IN         : T_itimerval *P_p_timer = structure source
/
/ OUT        : struct itimerval    *P_p_itval = structure destination
/
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/     Convertit une structure itimerval ayant les champs ..._sec
/     a zero et les ..._usec remplis, en structures ayant les champs
/     ..._usec inferieurs a 1 million et les champs ..._sec remplis
/     en consequence.
/
*****************************************************************ZYX*/
static void
timer_conv_usec_to_sec( P_p_itval, P_p_timer )
struct itimerval    *P_p_itval;
T_itimerval         *P_p_timer;
{
int       sec;
double    usec;

    usec = P_p_timer->it_interval.tv_usec;
    sec  = (int)(usec / (double)UN_MILLION);

    P_p_itval->it_interval.tv_sec = sec;
    P_p_itval->it_interval.tv_usec
                = (int)(usec - ((double)sec * (double)UN_MILLION));
    
    usec = P_p_timer->it_value.tv_usec;
    sec  = (int)(usec / (double)UN_MILLION);

    P_p_itval->it_value.tv_sec = sec;
    P_p_itval->it_value.tv_usec
                = (int)(usec - ((double)sec * (double)UN_MILLION));

} /* end timer_conv_usec_to_sec() */


/*XYZ*****************************************************************
/ NOM        : static void timer_conv_sec_to_usec( P_p_timer, P_p_itval )
/ 
/ IN         : struct itimerval    *P_p_itval = structure source
/
/ OUT        : T_itimerval         *P_p_timer = structure destination
/
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/     Convertit une structure itimerval ayant les champs ..._sec
/     et les ..._usec remplis (une structure normale), en structure
/     n'ayant que les champs ..._usec remplis et les champs ..._sec
/     a zero.
/
*****************************************************************ZYX*/
static void
timer_conv_sec_to_usec( P_p_timer, P_p_itval )
T_itimerval         *P_p_timer;
struct itimerval    *P_p_itval;
{
double toto;
    
    P_p_timer->it_interval.tv_usec = 
        (double)(P_p_itval->it_interval.tv_usec)
            +
        (double)((double)(P_p_itval->it_interval.tv_sec) * (double)UN_MILLION);
    
    P_p_timer->it_interval.tv_sec = (double) 0.0;
    
    P_p_timer->it_value.tv_usec = 
        (double)(P_p_itval->it_value.tv_usec)
            +
        (double)((double)(P_p_itval->it_value.tv_sec) * (double)UN_MILLION);
    
    P_p_timer->it_value.tv_sec = (double)0.0;

} /* end timer_conv_sec_to_usec() */


/*XYZ*********************************************************************
/ NOM        : lib_timer_quit()
/ 
/ IN         : Neant
/ OUT        : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/     Stoppe tous les timers
/
*****************************************************************ZYX*/
void
lib_timer_quit( )
{
int	  i;
sigset_t  old_mask, bid_mask;


    old_mask = lib_mask_masque_signal( (int) SIGALRM );
    bid_mask = lib_mask_masque_signal( (int) SIGIO );
    bid_mask = lib_mask_masque_signal( (int) SIGUSR1 );
    bid_mask = lib_mask_masque_signal( (int) SIGUSR2 );
    bid_mask = lib_mask_masque_signal( (int) SIGCLD );

    for ( i=0; i<LIB_TIMER_NB_MAX; i++ )
    {
	if( L_timer[i].type == -1 ) continue;

	lib_timer_tester_et_stopper( i );
    }

    lib_mask_demasque_signal( old_mask );
    
    lib_erreur( 0, 0, "fin lib_timer_quit()" );

} /* end lib_timer_quit() */
