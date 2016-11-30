/*ABC***********************************************************************
*
* NOM         : lib_sock_c.c
*
* PROJET      : PSCL
*
* PROCESS     : 
*
* TYPE        : Source C
*
* ECRIT PAR   : D. DELEFORTERIE               Le 18/05/2012
*
* MODIFS      :
*
* DESCRIPTION : 
*
***********************************************************************CBA*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/file.h>


/* pour connaitre lib_erreur() */
/* --------------------------- */
#include <lib_sleep.h>

#include <lib_erreur.h>

/* pour connaitre lib_timer() */
/* -------------------------- */
#include <lib_timer.h>

/* pour connaitre LBSWAP */
/* --------------------- */
#include <lib_swap.h>

/* pour connaitre lib_mask() */
/* ------------------------- */
#include <lib_mask.h>

/* pour connaitre lib_sock_.... */
/* ---------------------------- */
#include <lib_sock_incl.h>
#include <lib_sock_const.h>
#include <lib_sock_type.h>

#include <lib_sock_c.h>


/* ------------------------------- */
/* Variables globales a ce fichier */
/* ------------------------------- */

int                L_lib_sock_c_init_ok = 0;

char               L_deb_trame[SOCK_LG_DEB_TRAME+1];
char               L_fin_trame[SOCK_LG_FIN_TRAME+1];

int                L_nb_malloc;
int                L_nb_free;

LT_socket_serveur  L_socket_client[NB_MAX_SERVEURS];

void               (*L_fct_quit)();

int                L_num_timer_connect;
int                L_nb_sec_timer_connect;

int                L_num_timer_service;
int                L_nb_sec_timer_service;

/* Fonctions contenues dans ce fichier */
/* ----------------------------------- */

static void        close_socket_client();
static void        connect_socket_client();
static void        sock_c_read();
static void        socket_init();

static void        timer_init_et_lancer();
static void        timer_connect();
static void        timer_service();


/* ------------------------------------------ */
/* un peu de code place dans un autre fichier */
/* ------------------------------------------ */
#include "lib_sock_c_timer.c"

#define DEBUG_CONNECT     0

/*XYZ***********************************************************************
/ NOM         : lib_sock_c_init() 
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/
/ *********************************************************************ZYX*/
void
lib_sock_c_init( P_fct_quit,
                 P_num_timer_connect, P_nb_sec_timer_connect,
                 P_num_timer_service, P_nb_sec_timer_service )
void       (*P_fct_quit)();
int        P_num_timer_connect;
int        P_nb_sec_timer_connect;
int        P_num_timer_service;
int        P_nb_sec_timer_service;
{
int  i, j;

    if( L_lib_sock_c_init_ok == 1 )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_init()" );
        lib_erreur( 0, 0, "2ieme init => exit en dur" );
        exit( -1 );
    }

    /* Init des variables de debut et fin de trame */
    /* ------------------------------------------- */
    strncpy( L_deb_trame, SOCK_DEB_TRAME, SOCK_LG_DEB_TRAME );
    L_deb_trame[SOCK_LG_DEB_TRAME] = '\0';

    strncpy( L_fin_trame, SOCK_FIN_TRAME, SOCK_LG_FIN_TRAME );
    L_fin_trame[SOCK_LG_FIN_TRAME] = '\0';

    /* Init des variables d'allocation memoire */
    /* --------------------------------------- */
    L_nb_malloc = 0;
    L_nb_free = 0;

    /* Init des structures socket client ... */
    /* ------------------------------------- */
    for( i=0; i<NB_MAX_SERVEURS; i++ ) socket_init( i );

    if( P_fct_quit == NULL )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_init() => exit en dur" );
        lib_erreur( 0, 0, "\t Err P_fct_quit() NULL" );
        exit( -1 );
    }

    L_fct_quit = P_fct_quit;
    L_num_timer_connect = P_num_timer_connect;
    L_nb_sec_timer_connect = P_nb_sec_timer_connect;
    L_num_timer_service = P_num_timer_service;
    L_nb_sec_timer_service = P_nb_sec_timer_service;

    timer_init_et_lancer(); /* ds lib_sock_c_timer.c */

    /* Fin Init ...                            */
    /* --------------------------------------- */
    L_lib_sock_c_init_ok = 1;

    lib_erreur( 0, 0, "Fin lib_sock_c_init()" );

} /* end lib_sock_c_init() */


/*XYZ***********************************************************************
/ NOM         : lib_sock_c_is_init_ok() 
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : L_lib_sock_c_init_ok (0 ou 1)
/ DESCRIPTION :
/
/ *********************************************************************ZYX*/
int
lib_sock_c_is_init_ok()
{
    return( L_lib_sock_c_init_ok );
}


/*XYZ***********************************************************************
/ NOM         : lib_sock_c_init_socket_client() 
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : 1 si init OK
/               0 si erreur
/ DESCRIPTION :
/
/ *********************************************************************ZYX*/
int
lib_sock_c_init_socket_client( P_num_session,
                               P_serveur, P_service,
                               P_fct_trame, P_fct_connect, P_fct_close,
                               P_typ_emet, P_flipflop )
int      P_num_session;
char    *P_serveur;
char    *P_service;
void    (*P_fct_trame)();
void    (*P_fct_connect)();
void    (*P_fct_close)();
int      P_typ_emet;
int      P_flipflop;
{
struct hostent     *serveur;
struct servent     *service;
LT_socket_serveur  *p_socket;

    if( L_lib_sock_c_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_init_socket_client()" );
        lib_erreur( 0, 0, "sans init => exit en dur" );
        exit( -1 );
    }

    if( P_num_session < 0 || P_num_session >= NB_MAX_SERVEURS )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_init_socket_client()" );
        sprintf( G_msgerr, "ses %d Err <0 ou >=%d => exit",
                 P_num_session, NB_MAX_SERVEURS );
        lib_erreur( 0, 0, G_msgerr );
        (*L_fct_quit)();
    }

    p_socket = (LT_socket_serveur*) &( L_socket_client[P_num_session] );

    if( p_socket->init_ok == 1 )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_init_socket_client()" );
        sprintf( G_msgerr, "ses %d deja initialisee", P_num_session );
        lib_erreur( 0, 0, G_msgerr );
        return( 0 );
    }

    if( P_fct_trame == NULL )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_init_socket_client()" );
        lib_erreur( 0, 0, "\t Err P_fct_trame() NULL" );
        return( 0 );
    }

    memset( (char*) &( p_socket->sock ), '\0', sizeof(T_sockaddr_in) );
    
    /* Recuperation infos sur machine distante */
    /* --------------------------------------- */
    if (P_flipflop == 1)
        sprintf(p_socket->host, "%s%d", P_serveur, P_flipflop );
    else
        sprintf(p_socket->host, "%s", P_serveur );
    serveur = (struct hostent*) gethostbyname( p_socket->host );
    if( serveur == NULL )
    {
        lib_erreur( 0, 1, "Err lib_sock_c_init_socket_client()" );
        sprintf( G_msgerr, "ses %d Err gethostbyname( %s )",
                 P_num_session, p_socket->host );
        lib_erreur( 0, 0, G_msgerr );
        return( 0 );
    }
    memcpy( (char*) &( p_socket->sock.sin_addr ),
            (char*) serveur->h_addr,
            (int) serveur->h_length );
    p_socket->sock.sin_family = AF_INET ;
    
    sprintf( G_msgerr, "ses %d OK gethostbyname( %s )",
                 P_num_session, p_socket->host );
    lib_erreur( 0, 0, G_msgerr );

    /* Recuperation numero du port du service */
    /* -------------------------------------- */
    service = (struct servent*) getservbyname( P_service, "tcp" );
    if( service == NULL )
    {
        lib_erreur( 0, 1, "Err lib_sock_c_init_socket_client()" );
        sprintf( G_msgerr, "ses %d Err getservbyname( %s )",
                 P_num_session, P_service );
        lib_erreur( 0, 0, G_msgerr );
        return( 0 );
    }
    p_socket->sock.sin_port = (u_short) service->s_port;
    
    sprintf( G_msgerr, "ses %d OK getservbyname( %s )",
             P_num_session, P_service );
    lib_erreur( 0, 0, G_msgerr );

    /* Allocation du buffer de lecture */
    /* ------------------------------- */
    p_socket->buf = (char*) malloc( (unsigned) LNG_BUFFER );
    if( p_socket->buf == NULL )
    {
        lib_erreur( 0, 1, "Err lib_sock_c_init_socket_client()" );
        sprintf( G_msgerr, "ses %d Err malloc( %d )",
                 P_num_session, LNG_BUFFER );
        lib_erreur( 0, 0, G_msgerr );
        return( 0 );
    }
    L_nb_malloc++;

    sprintf( G_msgerr, "malloc en %lx de %d octets",
              p_socket->buf, LNG_BUFFER );
    lib_erreur( 0, 0, G_msgerr );
    sprintf( G_msgerr, "malloc = %d \tfree = %d ",
             L_nb_malloc, L_nb_free );
    lib_erreur( 0, 0, G_msgerr );

    strcpy( p_socket->str_serveur, P_serveur );
    strcpy( p_socket->str_service, P_service );
    
    p_socket->fct_trame = P_fct_trame;
    p_socket->fct_connect = P_fct_connect;
    p_socket->fct_close = P_fct_close;
    
    p_socket->typ_emet = P_typ_emet;
    if (P_flipflop == 1)
        p_socket->flipflop = 2;
    else
        p_socket->flipflop = 0;
        
    p_socket->tentative = 0;
    p_socket->init_ok = 1;

    sprintf( G_msgerr, "Fin lib_sock_c_init_socket_client(%d,%s,%s)",
             P_num_session, P_serveur, P_service );
    lib_erreur( 0, 0, G_msgerr );
    
    return( 1 );

} /* end lib_sock_c_init_socket_client() */


/*XYZ**********************************************************************
/ NOM         : lib_sock_c_quit() 
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/
/ *********************************************************************ZYX*/
void
lib_sock_c_quit()
{
int  i, ret;

    if( L_lib_sock_c_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_quit() avant lib_sock_c_init()" );
        return;
    }
    lib_erreur( 0, 0, "deb lib_sock_c_quit()" );

    /* ------------------------------------------ */
    /* fermeture de toutes les sockets connectees */
    /* ------------------------------------------ */
    for ( i=0; i<NB_MAX_SERVEURS; i++ )
    {
        if ( (L_socket_client[i].connectee == 1)
          || (L_socket_client[i].connect_en_cours == 1) )
        {
            close_socket_client( i, 0 );
            if ( L_socket_client[i].buf != (char*) NULL )
            {
                free( L_socket_client[i].buf );
                L_nb_free++;
            }
        }
    }

    lib_erreur( 0, 0, "lib_sock_c_quit()" );
    lib_erreur( 0, 0, "\t L_lib_sock_c_init_ok=0" );
    lib_erreur( 0, 0, "\t lib_sock_c n'est plus utilisable" );

    sprintf( G_msgerr, "L_nb_malloc = %d", L_nb_malloc );
    lib_erreur( 0, 0, G_msgerr );
    sprintf( G_msgerr, "L_nb_free   = %d", L_nb_free );
    lib_erreur( 0, 0, G_msgerr );

    L_lib_sock_c_init_ok = 0;
    lib_erreur( 0, 0, "fin lib_sock_c_quit()" );

} /* end lib_sock_c_quit() */


/*XYZ**********************************************************************
/ NOM         : lib_sock_c_kill_sigio() 
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/
/ *********************************************************************ZYX*/
void
lib_sock_c_kill_sigio()
{
int                   i, ret, bytes_avail;
int                   y_a_qqch_a_lire = 0;
LT_socket_serveur    *p_socket;

#ifdef DAODEBUG
    lib_erreur( 0, 0, "deb lib_sock_c_kill_sigio()" );
#endif
 
    /* pas de sockets clients , on passe */
    /* --------------------------------- */
    if ( L_lib_sock_c_init_ok == 0 )
    {
#ifdef DAODEBUG
        lib_erreur( 0, 0, "\t init non faite , return" );
#endif
        return;
    }

    /* quelque chose a lire ?? */
    /* ----------------------- */
    for ( i=0; i<NB_MAX_SERVEURS; i++ )
    {
        p_socket = (LT_socket_serveur*) &( L_socket_client[i] );
        if( p_socket->init_ok == 0 ) continue;

        if( p_socket->connect_en_cours == 0 && p_socket->connectee == 0 )
            continue;

        ret = ioctl( p_socket->id, FIONREAD, &bytes_avail );
        if( ret < 0 )
        {
            lib_erreur( 0, 0, "Err lib_sock_c_kill_sigio()" );
            lib_erreur( 0, 1, "\t Err ioctl(FIONREAD)" );
            continue;
        }
        if( bytes_avail == 0 ) continue;
        
        y_a_qqch_a_lire = 1;
    }

    /* non, on verra plus tard */
    /* ----------------------- */
    if ( y_a_qqch_a_lire == 0 )
    {
#ifdef DAODEBUG
        lib_erreur( 0, 0, "-->rien a lire" );
#endif
        return;
    }
    else
    {
#ifdef DAODEBUG
        lib_erreur( 0, 0, "on appelle sock_c_read" );
#endif
    }

    /* oui, allons y pour la lecture */
    /* ----------------------------- */
    sock_c_read( 0 );
    
#ifdef DAODEBUG
    lib_erreur( 0, 0, "fin lib_sock_c_kill_sigio()" );
#endif

} /* end lib_sock_c_kill_sigio() */


/*XYZ***********************************************************************
/ NOM         : lib_sock_c_is_client_connecte() 
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/
/ *********************************************************************ZYX*/
int
lib_sock_c_is_client_connecte( P_num_session )
int P_num_session;
{
LT_socket_serveur  *p_socket;

    if( L_lib_sock_c_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_is_client_connecte()" );
        lib_erreur( 0, 0, "sans init => exit en dur" );
        exit( -1 );
    }

    if( P_num_session < 0 || P_num_session >= NB_MAX_SERVEURS )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_is_client_connecte()" );
        sprintf( G_msgerr, "ses %d Err <0 ou >=%d => exit",
                 P_num_session, NB_MAX_SERVEURS );
        lib_erreur( 0, 0, G_msgerr );
        (*L_fct_quit)();
    }

    p_socket = (LT_socket_serveur*) &( L_socket_client[P_num_session] );
    return( p_socket->connectee );

} /* end lib_sock_c_is_client_connecte() */


/*XYZ***********************************************************************
/ NOM         : lib_sock_c_write_socket_client() 
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : 1 si init OK
/               0 si erreur
/ DESCRIPTION :
/
/ *********************************************************************ZYX*/
int
lib_sock_c_write_socket_client( P_num_session, P_typ_tram, P_typ_dest,
                                P_lng_data, P_data )
int         P_num_session;
int         P_typ_tram;
int         P_typ_dest;
int         P_lng_data;
char        *P_data;
{
int                ret, ret_value = 1;
char               trame[LNG_BUFFER];
int                idx, nb_oct;
int                nb_essai_write, nb_max_essai_write = 5;
int                nb_a_ecrire, nb_ecrits;
int                sleep_write = 500;
sigset_t           old_mask, bidon;
T_entete           entete;
LT_socket_serveur  *p_socket;


    if( L_lib_sock_c_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_write_socket_client()" );
        lib_erreur( 0, 0, "sans init => exit en dur" );
        exit( -1 );
    }

    if( P_num_session < 0 || P_num_session >= NB_MAX_SERVEURS )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_write_socket_client()" );
        sprintf( G_msgerr, "ses %d Err <0 ou >=%d => exit",
                 P_num_session, NB_MAX_SERVEURS );
        lib_erreur( 0, 0, G_msgerr );
        (*L_fct_quit)();
    }

    p_socket = (LT_socket_serveur*) &( L_socket_client[P_num_session] );

    /* On masque des signaux */
    /* --------------------- */
    old_mask = lib_mask_masque_signal( SIGIO );
    bidon    = lib_mask_masque_signal( SIGALRM );
    bidon    = lib_mask_masque_signal( SIGUSR1 );
    bidon    = lib_mask_masque_signal( SIGUSR2 );
    bidon    = lib_mask_masque_signal( SIGCLD );

    if( p_socket->init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_write_socket_client()" );
        sprintf( G_msgerr, "ses %d Err init_ok", P_num_session );
        lib_erreur( 0, 0, G_msgerr );
        ret_value = 0;
        goto fin_proc;
    }

    if( p_socket->connectee == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_write_socket_client()" );
        sprintf( G_msgerr, "ses %d Err connectee", P_num_session );
        lib_erreur( 0, 0, G_msgerr );
        ret_value = 0;
        goto fin_proc;
    }

    entete.typ_tram = P_typ_tram;
    entete.lng_data = P_lng_data;
    entete.typ_emet = p_socket->typ_emet;
    entete.pid_emet = getpid();
    entete.typ_dest = P_typ_dest;

    idx = SOCK_LG_DEB_TRAME + sizeof(T_entete)
                            + entete.lng_data
                            + SOCK_LG_FIN_TRAME;
    if( idx > LNG_BUFFER )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_write_socket_client()" );
        sprintf( G_msgerr, "ses %d idx = %d > %d",
                 P_num_session, idx, LNG_BUFFER );
        lib_erreur( 0, 0, G_msgerr );
        ret_value = 0;
        goto fin_proc;
    }

    if( P_lng_data < 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_write_socket_client()" );
        sprintf( G_msgerr, "ses %d P_lng_data = %d < 0",
                 P_num_session, P_lng_data );
        lib_erreur( 0, 0, G_msgerr );
        ret_value = 0;
        goto fin_proc;
    }

    /* Formatage de la trame */
    /* --------------------- */
    idx = 0;
    memcpy( trame, L_deb_trame, strlen(L_deb_trame) );
    idx = idx + (int) SOCK_LG_DEB_TRAME;
    
    memcpy( &trame[idx], &entete, sizeof(T_entete) );
    idx = idx + sizeof(T_entete);
    
    memcpy( &trame[idx], P_data, entete.lng_data );
    idx = idx + entete.lng_data;
    
    memcpy( &trame[idx], L_fin_trame, strlen(L_fin_trame) );
    idx = idx + (int) SOCK_LG_FIN_TRAME;

    /* Ecriture de la trame sur la socket specifiee */
    /* -------------------------------------------- */
    nb_a_ecrire      = idx;
    nb_ecrits        = 0;
    nb_essai_write   = 0;

    /* Boucle pour ecrire la totalite de la trame */
    /* ------------------------------------------ */
    do
    {
        ret = write( p_socket->id, (char*) &trame[nb_ecrits],
                     (nb_a_ecrire - nb_ecrits) );
        if( ret < 0 )
        {
            switch( errno )
            {
                /* case EWOULDBLOCK : */
                case EAGAIN :
                    nb_essai_write++;
                    if( nb_essai_write > nb_max_essai_write )
                    {
                        lib_erreur(0,0,"Err lib_sock_c_write_socket_client()");
                        lib_erreur(0,1,"\t Err EW/EG" );

                        close_socket_client( P_num_session, 1 );
                        ret_value = 0;
                        goto fin_proc;
                    }
                    else
                    {
                        lib_sleep_milli_sec( sleep_write );
                    }
                break;
                
                case EPIPE :
                    lib_erreur(0,0,"Err lib_sock_c_write_socket_client()" );
                    lib_erreur(0,1,"\t Err write()" );

                    close_socket_client( P_num_session, 1 );
                    ret_value = 0;
                    goto fin_proc;
                break;
                
                default :
                    /* Dans tous les autres cas : On ferme la socket */
                    /* et on appelle la fonction d'arret de la tache */
                    /* --------------------------------------------- */
                    lib_erreur(0,0,"Err lib_sock_c_write_socket_client()" );
                    lib_erreur(0,1,"\t Err write()" );

                    close_socket_client( P_num_session, 0 );
                    (*L_fct_quit)(); /* ARRET DE LA TACHE */
                                     /* NE REVIENT JAMAIS */
                break;

            } /* end switch errno */
        }
        else
        {
            nb_ecrits += ret;
        }
    
    } while ( nb_ecrits < nb_a_ecrire );

    fin_proc :;
    
    /* On demasque les signaux */
    /* ----------------------- */
    lib_mask_demasque_signal( old_mask );

    return ( ret_value );

} /* end lib_sock_c_write_socket_client() */


/*XYZ***********************************************************************
/ NOM         : lib_sock_c_close_socket_client() 
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/
/ *********************************************************************ZYX*/
void
lib_sock_c_close_socket_client( P_num_session )
int P_num_session;
{
int                ret;
LT_socket_serveur  *p_socket;

    if( L_lib_sock_c_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_close_socket_client()" );
        lib_erreur( 0, 0, "sans init => exit en dur" );
        exit( -1 );
    }

    if( P_num_session < 0 || P_num_session >= NB_MAX_SERVEURS )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_close_socket_client()" );
        sprintf( G_msgerr, "ses %d Err <0 ou >=%d => exit",
                 P_num_session, NB_MAX_SERVEURS );
        lib_erreur( 0, 0, G_msgerr );
        (*L_fct_quit)();
    }

    p_socket = (LT_socket_serveur*) &( L_socket_client[P_num_session] );

    if( p_socket->init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_close_socket_client()" );
        sprintf( G_msgerr, "ses %d Err init_ok", P_num_session );
        lib_erreur( 0, 0, G_msgerr );
        return;
    }

    if ( p_socket->connectee == 0 && p_socket->connect_en_cours == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_c_close_socket_client()" );
        sprintf( G_msgerr, "ses %d Err connectee", P_num_session );
        lib_erreur( 0, 0, G_msgerr );
        return;
    }

    close_socket_client( P_num_session, 1 );

} /* end lib_sock_c_close_socket_client() */


/*XYZ***********************************************************************
/ NOM         : close_socket_client() 
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/
/ *********************************************************************ZYX*/
static void
close_socket_client( P_num_session, P_call_user )
int P_num_session;
int P_call_user;
{
int                ret;
LT_socket_serveur  *p_socket;

    p_socket = (LT_socket_serveur*) &( L_socket_client[P_num_session] );

    /* Fermeture de la socket */
    /* ---------------------- */
    ret = close( p_socket->id );
    if( ret != 0 )
    {
        lib_erreur( 0, 1, "Err close_socket_client()" );
        sprintf( G_msgerr, "ses %d Err close()", P_num_session );
        lib_erreur( 0, 0, G_msgerr );
    }

    /* Liberation du buffer de reception */
    /* --------------------------------- */
/****
    free( p_socket->buf );
    L_nb_free++;
    sprintf(G_msgerr, "malloc = %d \tfree = %d ", L_nb_malloc, L_nb_free );
    lib_erreur( 0, 0, G_msgerr );
****/

    /* La session est obligatoirement deconnectee */
    /* ------------------------------------------ */
    p_socket->connect_en_cours = 0;
    p_socket->connectee = 0;

    p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
    p_socket->alire = 1;
    p_socket->idx = 0;
    p_socket->lus = 0;

    sprintf( G_msgerr, "Close ses=%d sur '%s,%s'", P_num_session,
             p_socket->str_serveur, p_socket->str_service );
    lib_erreur( 0, 0, G_msgerr );

    /* On previent la tache avec le numero de session */
    /* ---------------------------------------------- */
    if( p_socket->fct_close != NULL && P_call_user == 1 )
    {
        (*p_socket->fct_close)( P_num_session,
                                p_socket->str_serveur,
                                p_socket->str_service );
    }

} /* end close_socket_client() */


/*XYZ***************************************************************************
/ NOM         : connect_socket_client() 
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/
/ *************************************************************************ZYX*/
static void
connect_socket_client( P_num_session )
int P_num_session;
{
int                ret;
int                pid, flag;
LT_socket_serveur  *p_socket;
struct hostent     *serveur;


    p_socket = (LT_socket_serveur*) &( L_socket_client[P_num_session] );

    if( p_socket->init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err connect_socket_client()" );
        sprintf( G_msgerr, "ses %d Err init_ok", P_num_session );
        lib_erreur( 0, 0, G_msgerr );
        return;
    }

    if( p_socket->connect_en_cours == 1 || p_socket->connectee == 1 )
    {
        lib_erreur( 0, 0, "Err connect_socket_client()" );
        sprintf( G_msgerr, "ses %d Err connectee", P_num_session );
        lib_erreur( 0, 0, G_msgerr );
        return;
    }

    if (p_socket->tentative > 0)
    {  
#if DEBUG_CONNECT
        lib_erreur( 0, 0, "on retente le connect !!!" );
#endif
        goto connect_tentative;
    }
#if DEBUG_CONNECT
    else
    {
        lib_erreur( 0, 0, "on gere flipflop eventuel" );
    }
#endif
    
    /* gestion du flipflop si necessaire */
    /* --------------------------------- */
    if ((p_socket->flipflop == 1) || (p_socket->flipflop == 2))
    {
        /* recuperation infos sur machine distante */
        p_socket->flipflop = (p_socket->flipflop % 2) + 1;
        sprintf(p_socket->host, "%s%d",
                p_socket->str_serveur, p_socket->flipflop );
        serveur = (struct hostent*) gethostbyname( p_socket->host );
        if( serveur == NULL )
        {
            lib_erreur( 0, 1, "Err connect_socket_client()" );
            sprintf( G_msgerr, "ses %d Err gethostbyname( %s )",
                 P_num_session, p_socket->host );
            lib_erreur( 0, 0, G_msgerr );
            return;
        }
        memcpy( (char*) &( p_socket->sock.sin_addr ),
                (char*) serveur->h_addr,
                (int) serveur->h_length );
        p_socket->sock.sin_family = AF_INET;
    }
    else
        sprintf(p_socket->host, "%s ", p_socket->str_serveur );
    
    /* Initialisation de la socket */
    /* --------------------------- */
#if DEBUG_CONNECT
    lib_erreur( 0, 0, "socket(AF_INET)" );
#endif
    p_socket->id = socket( AF_INET, SOCK_STREAM, 0 );
    if( p_socket->id < 0 )
    {
        lib_erreur( 0, 1, "Err connect_socket_client()" );
        sprintf( G_msgerr, "ses %d Err socket()", P_num_session );
        lib_erreur( 0, 0, G_msgerr );
        return;
    }

    /* Socket declaree non blocante */
    /* ---------------------------- */
#if DEBUG_CONNECT
    lib_erreur( 0, 0, "ioctl(FIONBIO)" );
#endif
    flag = 1;
    ret = ioctl( p_socket->id, FIONBIO, &flag );
    if( ret < 0 )
    {
        lib_erreur( 0, 1, "Err connect_socket_client()" );
        sprintf( G_msgerr, "ses %d Err ioctl(FIONBIO)", P_num_session );
        lib_erreur( 0, 0, G_msgerr );
        goto connect_error;
    }
    
connect_tentative:;

#if DEBUG_CONNECT
    sprintf( G_msgerr, "deb connect(%d/%s)", P_num_session, p_socket->host );
    lib_erreur( 0, 0, G_msgerr );
#endif

    /* Tentatives de connexion */
    /* ----------------------- */
    ret = connect( p_socket->id,
                   (struct sockaddr*) &( p_socket->sock ),
                   sizeof(T_sockaddr_in) );
    if( ret < 0 )
    {
        switch( errno )
        {
            case EALREADY :
            case EINPROGRESS :
            case EINTR :
                p_socket->tentative += 1 ;
                if ( p_socket->tentative >= 5 )
                {
                    lib_erreur( 0, 1, "CONNECT(+ de 5 essais) => TIMEOUT" );
                    p_socket->tentative = 0 ;
                    goto connect_error;
                }
                else
                {
#if DEBUG_CONNECT
                    sprintf( G_msgerr, "connect(%d essais)",
                             p_socket->tentative );
                    lib_erreur( 0, 1, G_msgerr );
#endif
                    return ;
                }
            break;
            
            case EISCONN :
#if DEBUG_CONNECT
                lib_erreur( 0, 1, "deja connectee ??" );
#endif
                p_socket->tentative = 0 ;
            break;
            
            default :
                /* Dans tous les autres cas : On ferme la socket */
                /* --------------------------------------------- */
                lib_erreur( 0, 1, "Err connect_socket_client()" );
                sprintf( G_msgerr, "ses %d Err connect()", P_num_session );
                lib_erreur( 0, 0, G_msgerr );    
                p_socket->tentative = 0 ;
                goto connect_error;
            break;
        }
    }
    else p_socket->tentative = 0 ;
    
    /* Definition du proprietaire de la socket */
    /* --------------------------------------- */
#if DEBUG_CONNECT
    lib_erreur( 0, 0, "ioctl(SIOCSPGRP)" );
#endif
    pid = getpid();
    ret = ioctl( p_socket->id, SIOCSPGRP, &pid );
    if( ret < 0 )
    {
        lib_erreur( 0, 1, "Err connect_socket_client()" );
        sprintf( G_msgerr, "ses %d Err ioctl(SIOCSPGRP)", P_num_session );
        lib_erreur( 0, 0, G_msgerr );
        goto connect_error;
    }

    /* Socket declaree non blocante */
    /* ---------------------------- */
#if DEBUG_CONNECT
    lib_erreur( 0, 0, "ioctl(FIONBIO)" );
#endif
    flag = 1;
    ret = ioctl( p_socket->id, FIONBIO, &flag );
    if( ret < 0 )
    {
        lib_erreur( 0, 1, "Err connect_socket_client()" );
        sprintf( G_msgerr, "ses %d Err ioctl(FIONBIO)", P_num_session );
        lib_erreur( 0, 0, G_msgerr );
        goto connect_error;
    }

    /* Socket declaree asynchrone */
    /* -------------------------- */
#if DEBUG_CONNECT
    lib_erreur( 0, 0, "ioctl(FIOASYNC)" );
#endif
    flag = 1;
    ret = ioctl( p_socket->id, FIOASYNC, &flag );
    if( ret < 0 )
    {
        lib_erreur( 0, 1, "Err connect_socket_client()" );
        sprintf( G_msgerr, "ses %d Err ioctl(FIOASYNC)", P_num_session );
        lib_erreur( 0, 0, G_msgerr );
        goto connect_error;
    }

    p_socket->connect_en_cours = 1;
    p_socket->connectee = 0;

    /* Initialisation des variables de gestion de lecture */
    /* -------------------------------------------------- */
    p_socket->etat_read = SOCK_ETAT_READ_WAIT_INIT;
    p_socket->alire = 1;
    p_socket->lus = 0;
    p_socket->idx = 0;

    sprintf( G_msgerr, "CONNECTION OK ses %d sur serveur='%s' service='%s'",
             P_num_session,
             p_socket->host, p_socket->str_service );
    lib_erreur( 0, 0, G_msgerr );
    return;

connect_error:;

    sprintf( G_msgerr, "=> PAS DE CONNECTION sur serveur='%s' service='%s'",
             p_socket->host, p_socket->str_service );
    lib_erreur( 0, 0, G_msgerr );
    
    ret = close( p_socket->id );
    if( ret != 0 )
    {
        lib_erreur( 0, 1, "Err connect_socket_client()" );
        sprintf( G_msgerr, "ses %d Err close()", P_num_session );
        lib_erreur( 0, 0, G_msgerr );
    }

} /* end connect_socket_client() */


/*XYZ***************************************************************************
/ NOM         : sock_c_read() 
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/
/ *************************************************************************ZYX*/
static void
sock_c_read( P_signal )
int P_signal;
{
int                  i;
int                  stop;
int                  nb_oct;
sigset_t             old_mask, bidon;
T_entete             *p_entete;
LT_socket_serveur    *p_socket;
char                 *adress_data ;

    /* Masque certains signaux */
    /* ----------------------- */
    old_mask = lib_mask_masque_signal( SIGIO );
    bidon    = lib_mask_masque_signal( SIGALRM );
    bidon    = lib_mask_masque_signal( SIGUSR1 );
    bidon    = lib_mask_masque_signal( SIGUSR2 );
    bidon    = lib_mask_masque_signal( SIGCLD );

    /* Parcours du tableau de session */
    /* ------------------------------ */
    for ( i=0; i<NB_MAX_SERVEURS; i++ )
    {
        p_socket = (LT_socket_serveur*) &( L_socket_client[i] );

        if( p_socket->init_ok == 0 )
            continue;

        if( p_socket->connect_en_cours == 0 && p_socket->connectee == 0 )
            continue;

        stop = 0;
        do
        {
            if( p_socket->idx >= LNG_BUFFER )
            {
                lib_erreur( 0, 0, "Err sock_c_read() => exit" );
                 sprintf( G_msgerr,
                          "\t Err L_socket_client[%d].idx = %d >= LNG_BUFFER",
                          i, p_socket->idx );
                lib_erreur( 0, 0, G_msgerr );
                (*L_fct_quit)();
            }

            nb_oct = read( p_socket->id,
                           (char*) (p_socket->buf + p_socket->idx),
                           p_socket->alire - p_socket->lus );

            if( nb_oct > 0 )
            {
                /* Des caracteres ont ete lus */
                /* -------------------------- */
                if( p_socket->lus + nb_oct == p_socket->alire )
                {
                    /* On vient de lire le bon nombre de caracteres */
                    /* pour terminer l'etat courant de la lecture.  */
                    /* -------------------------------------------- */
                    switch( p_socket->etat_read )
                    {

/************************************************************************/
/* Pourquoi l'etat SOCK_ETAT_READ_WAIT_INIT ?                           */
/* ----------------------------------------                             */
/* En l'absence d'erreur, un client peut ecrire sur une socket apres    */
/* un "connect" meme si celui-ci n'a pas ete "accepte" par le serveur.  */
/* Afin d'empecher cette ecriture sans que la session soit completement */
/* etablie, le premier etat de la lecture est l'attente du premier      */
/* caractere envoye par le serveur apres un "accept".                   */
/************************************************************************/

                        case SOCK_ETAT_READ_WAIT_INIT :
                            /* Test si on a lu le 1er caractere */
                            if( *(p_socket->buf) == SOCK_START_CHAR )
                            {
                                p_socket->connectee = 1;
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                                p_socket->alire = 1;
    
                                if( p_socket->fct_connect != NULL )
                                {
                                    (*p_socket->fct_connect)( i,
                                            p_socket->str_serveur,
                                            p_socket->str_service );
                                }
                            }
                        break;

                        case SOCK_ETAT_READ_WAIT_STX1 :
                            /* Test 1er caractere de debut de trame */
                            if( *(p_socket->buf) == L_deb_trame[0] )
                            {
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX2;
                                p_socket->alire = 1;
                            }
                            else
                            {
                                sprintf( G_msgerr,
                                     "\t\t tr perdue STX1 : %d!=%d .%c.!=.%c.",
                                     L_deb_trame[0], *(p_socket->buf),
                                     L_deb_trame[0], *(p_socket->buf) );
                                lib_erreur( 0, 0, G_msgerr );
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                                p_socket->alire = 1;
                            }
                        break;

                        case SOCK_ETAT_READ_WAIT_STX2 :
                            /* Test 2eme caractere de debut de trame */
                            if( *(p_socket->buf) == L_deb_trame[1])
                            {
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX3;
                                p_socket->alire = 1;
                            }
                            else
                            {
                                sprintf( G_msgerr,
                                     "\t\t tr perdue STX2 : %d!=%d .%c.!=.%c.",
                                     L_deb_trame[1], *(p_socket->buf),
                                     L_deb_trame[1], *(p_socket->buf) );
                                lib_erreur( 0, 0, G_msgerr );
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                                p_socket->alire = 1;
                            }
                        break;

                        case SOCK_ETAT_READ_WAIT_STX3 :
                            /* Test 3eme caractere de debut de trame */
                            if( *(p_socket->buf) == L_deb_trame[2])
                            {
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX4;
                                p_socket->alire = 1;
                            }
                            else
                            {
                                sprintf( G_msgerr,
                                     "\t\t tr perdue STX3 : %d!=%d .%c.!=.%c.",
                                     L_deb_trame[2], *(p_socket->buf),
                                     L_deb_trame[2], *(p_socket->buf) );
                                lib_erreur( 0, 0, G_msgerr );
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                                p_socket->alire = 1;
                            }
                        break;

                        case SOCK_ETAT_READ_WAIT_STX4 :
                            /* Test 4eme caractere de debut de trame */
                            if( *(p_socket->buf) == L_deb_trame[3])
                            {
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_HEAD;
                                p_socket->alire = sizeof(T_entete);
                                /* On stocke en debut les prochains octets */
                                p_socket->idx = 0;
                            }
                            else
                            {
                                sprintf( G_msgerr,
                                     "\t\t tr perdue STX4 : %d!=%d .%c.!=.%c.",
                                     L_deb_trame[3], *(p_socket->buf),
                                     L_deb_trame[3], *(p_socket->buf) );
                                lib_erreur( 0, 0, G_msgerr );
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                                p_socket->alire = 1;
                            }
                        break;

                        case SOCK_ETAT_READ_WAIT_HEAD :
                            p_entete = (T_entete*) &( p_socket->entete );

                            memcpy( (char*) p_entete,
                                    p_socket->buf,
                                    sizeof(T_entete) );

                            /* -------------------------- */
                            /* Test si trame sans data    */
                            /* Si oui -> on saute un etat */
                            /* -------------------------- */
                            if( p_entete->lng_data == 0 )
                            {
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_EOT1;
                                p_socket->alire = 1;
                            }
                            else
                            {
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_DATA;
                                p_socket->alire = p_entete->lng_data;
                            }

                            /* On stocke l'entete dans buffer de reception */
                            /* -> on indexe le buffer de reception pour la */
                            /* prochaine lecture                           */
                            /* ------------------------------------------- */
                            p_socket->idx += nb_oct;
                        break;

                        case SOCK_ETAT_READ_WAIT_DATA :
                            p_socket->etat_read = SOCK_ETAT_READ_WAIT_EOT1;
                            p_socket->alire = 1;
                            p_socket->idx += nb_oct;
                        break;

                        case SOCK_ETAT_READ_WAIT_EOT1 :
                            if(    (char) *(p_socket->buf+p_socket->idx)
                                ==
                                (char) L_fin_trame[0] )
                            {
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_EOT2;
                                p_socket->alire = 1;
                            }
                            else
                            {
                                sprintf( G_msgerr,
                                     "\t\t tr perdue EOT1 : %d!=%d .%c.!=.%c.",
                                     L_fin_trame[0],
                                     *(p_socket->buf+p_socket->idx),
                                     L_fin_trame[0],
                                     *(p_socket->buf+p_socket->idx) );
                                lib_erreur( 0, 0, G_msgerr );
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                                p_socket->alire = 1;
                                p_socket->idx = 0;
                            }
                        break;

                        case SOCK_ETAT_READ_WAIT_EOT2 :
                            if(*(p_socket->buf+p_socket->idx) == L_fin_trame[1])
                            {
                                p_socket->alire = 1;
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_EOT3;
                            }
                            else
                            {
                                sprintf( G_msgerr,
                                     "\t\t tr perdue EOT2 : %d!=%d .%c.!=.%c.",
                                     L_fin_trame[1],
                                     *(p_socket->buf+p_socket->idx),
                                     L_fin_trame[1],
                                     *(p_socket->buf+p_socket->idx) );
                                lib_erreur( 0, 0, G_msgerr );
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                                p_socket->alire = 1;
                                p_socket->idx = 0;
                            }
                        break;

                        case SOCK_ETAT_READ_WAIT_EOT3 :
                            if(*(p_socket->buf+p_socket->idx) == L_fin_trame[2])
                            {
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_EOT4;
                                p_socket->alire = 1;
                            }
                            else
                            {
                                sprintf( G_msgerr,
                                     "\t\t tr perdue EOT3 : %d!=%d .%c.!=.%c.",
                                     L_fin_trame[2],
                                     *(p_socket->buf+p_socket->idx),
                                     L_fin_trame[2],
                                     *(p_socket->buf+p_socket->idx) );
                                lib_erreur( 0, 0, G_msgerr );
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                                p_socket->alire = 1;
                                p_socket->idx = 0;
                            }
                        break;

                        case SOCK_ETAT_READ_WAIT_EOT4 :
                            if(*(p_socket->buf+p_socket->idx) == L_fin_trame[3])
                            {
                                adress_data = (char*) ( p_socket->buf +
                                                        sizeof(T_entete) );
                                /*                        
                                sprintf( G_msgerr, "buf %x data %x",
                                         p_socket->buf, adress_data );
                                lib_erreur( 0, 0, G_msgerr );
                                */
                                
                                (*p_socket->fct_trame)
                                      ( i,
                                        p_socket->str_serveur,
                                        p_socket->str_service,
                                        &(p_socket->entete),
                                        adress_data ,
                                        p_socket->entete.lng_data
                                      );
                            }
                            else
                            {
                                sprintf( G_msgerr,
                                     "\t\t tr perdue EOT4 : %d!=%d .%c.!=.%c.",
                                     L_fin_trame[3],
                                     *(p_socket->buf+p_socket->idx),
                                     L_fin_trame[3],
                                     *(p_socket->buf+p_socket->idx) );
                                lib_erreur( 0, 0, G_msgerr );
                            }
                            p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                            p_socket->alire = 1;
                            p_socket->idx = 0;
                        break;

                        default :
                            /* Cas normalement impossible */
                            sprintf( G_msgerr,
                                 "\t Err L_socket_client[%d].etat_read = %d",
                                 i, p_socket->etat_read );
                            lib_erreur( 0, 0, G_msgerr );

                            p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                            p_socket->alire = 1;
                            p_socket->idx = 0;
                            break;

                    } /* end switch */

                    /* Pour l'etat courant nous avons lus tous les octets */
                    /* -> Remise a zero du compteur d'octets deja lus     */
                    /* -------------------------------------------------- */
                    p_socket->lus = 0;
                }
                else
                {
                    /* On n' a pas encore lu suffisament de caracteres pour */
                    /* pour terminer l'etat courant de la lecture.          */
                    /* ---------------------------------------------------- */
                    p_socket->lus += nb_oct;
                    p_socket->idx += nb_oct;

                    /* Controle du depassement de taille de buffer */
                    /* ------------------------------------------- */
                    if( p_socket->idx >= LNG_BUFFER )
                    {
                        lib_erreur( 0, 0, "Err sock_c_read() => exit" );
                         sprintf( G_msgerr,
                         "\t Err 2 L_socket_client[%d].idx =%d >= LNG_BUFFER",
                               i, p_socket->idx );
                        lib_erreur( 0, 0, G_msgerr );
                        (*L_fct_quit)();
                    }
                }
            }
            else if (nb_oct == 0)
            {
                /* Le READ a renvoye une valeur egale a zero */
                /* ----------------------------------------- */
                close_socket_client( i, 1 );
                stop = 1;
            }
            else if ( nb_oct < 0 )
            {
                /* Le READ a renvoye une valeur negative */
                /* ------------------------------------- */
                switch( errno )
                {
                    /* case EWOULDBLOCK : */
                    case EAGAIN :
                    break;

                    case ECONNRESET :
                        /* Connexion Reset by Peer */
                        /* ----------------------- */

                    default :
                        /* Erreur non gerable => deconnexion ! */
                        /* ----------------------------------- */
                        close_socket_client( i, 1 );
                    break;
                } 
                stop = 1;
            }
    
        } while( nb_oct > 0 && stop == 0 );

    } /* end for i */

    /* Demasque certains signaux */
    /* ------------------------- */
    lib_mask_demasque_signal( old_mask );

} /* end sock_c_read() */


/*XYZ***************************************************************************
/ NOM         : socket_init() 
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/
/ *************************************************************************ZYX*/
static void
socket_init( P_num_session )
int P_num_session;
{
LT_socket_serveur    *p_socket;

    p_socket = (LT_socket_serveur*) &( L_socket_client[P_num_session] );
    p_socket->init_ok = 0;

    p_socket->connect_en_cours = 0;
    p_socket->connectee = 0;

    p_socket->etat_read = SOCK_ETAT_READ_WAIT_INIT;
    p_socket->alire = 1;
    p_socket->lus = 0;
    p_socket->idx = 0;
    p_socket->buf = (char*) NULL;

    p_socket->entete.lng_data = 0;
    p_socket->entete.typ_tram = 0;
    p_socket->entete.typ_emet = 0;
    p_socket->entete.pid_emet = 0;
    p_socket->entete.typ_dest = 0;

    p_socket->fct_trame = NULL;
    p_socket->fct_connect = NULL;
    p_socket->fct_close = NULL;

    strcpy( p_socket->str_serveur, "" );
    strcpy( p_socket->str_service, "" );
    strcpy( p_socket->host, "" );

    p_socket->typ_emet = TED_INCONNU;
    p_socket->flipflop = 0;
    p_socket->tentative = 0;

} /* end socket_init() */
