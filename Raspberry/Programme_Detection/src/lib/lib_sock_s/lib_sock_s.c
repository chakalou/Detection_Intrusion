/*ABC***********************************************************************
*
* NOM         : lib_sock_s.c
*
* PROJET      : PSCL
*
* PROCESS     : 
*
* TYPE        : Source C
*
* ECRIT PAR   : D. DELEFORTERIE                     Le 22/05/2012
*
* MODIFS      :
*
* DESCRIPTION : 
*
***********************************************************************CBA*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* pour le prototypage de inet_ntoa() */
/* ---------------------------------- */
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/* pour connaitre lib_mask() */
/* -------------------------- */
#include <lib_mask.h>

/* pour connaitre G_msgerr et lib_erreur() */
/* --------------------------------------- */
#include <lib_erreur.h>

/* pour connaitre LBSWAP */
/* --------------------- */
#include <lib_swap.h>

/* pour connaitre lib_timer() */
/* -------------------------- */
#include <lib_timer.h>
#include <lib_sleep.h>

/* pour connaitre sockaddr_in   */
/* pour connaitre SOCK_LNG_INET */
/* ---------------------------- */
#include <lib_sock_incl.h>
#include <lib_sock_const.h>
#include <lib_sock_type.h>

#include <lib_sock_s.h>

/* ------------------------------ */
/* Variables locales a ce fichier */
/* ------------------------------ */

int                 L_lib_sock_s_init_ok = 0;

LT_socket_ecoute    L_sock_ecoute;
LT_socket_client    L_sock_client[SOCK_NB_MAX_CLIENTS];

char                L_deb_trame[SOCK_LG_DEB_TRAME+1];
char                L_fin_trame[SOCK_LG_FIN_TRAME+1];

char               *L_trame;
int                 L_longueur_SNDBUF = LNG_BUFFER;
int                 L_longueur_RCVBUF = LNG_BUFFER;

int                 L_nb_max_essai_write;
int                 L_nb_max_essai_write_debug;
int                 L_sleep_write_nb_ms;
int                 L_flag_debug_write;
int                 L_flag_debug_ew;

void                (*L_fct_trt_trame)();
void                (*L_fct_accept)();
void                (*L_fct_close)();
void                (*L_fct_quit)();

LT_machine_autor    L_machine[NB_MAX_MACHINES];

int                 L_num_timer_accept;
int                 L_nb_sec_timer_accept;

int                 L_num_timer_service;
int                 L_nb_sec_timer_service;

int                 L_nb_malloc;
int                 L_nb_free;

int                 L_nb_max_connections;

/* --------------------------------- */
/* Fonctions locales dans ce fichier */
/* --------------------------------- */
static void        sock_s_read();
static void        sock_s_accept();
static void        sock_s_sigusr2();
static int         nb_socket_client();
static void        aff_socket_client();

static int         machine_nb_max_client();
static int         machine_flag_swap();
static int         machine_origine();
static void        machine_lit_fichier();
static void        machine_init();

static void        timer_init_et_lancer();
static void        timer_accept();
static void        timer_service();

static int         write_socket_client();
static void        close_socket_client();


/* ------------------------------------------ */
/* un peu de code place dans un autre fichier */
/* ------------------------------------------ */
#include "lib_sock_s_machine.c"
#include "lib_sock_s_timer.c"


/*XYZ***********************************************************************
/
/ NOM:      lib_sock_s_init() 
/
/ IN:       P_service             Nom du service
/           P_fct_trame           Pointeur sur fonction traitement trame
/           P_fct_quit           Pointeur sur fonction arret tache 
/           P_nb_max_essai_write  Nbre max d'essai successifs de write
/                                    sur Err EWOULDBLOCK.
/           P_sleep_nb_ms         Duree du sleep entre 2 essais
/                                    consecutifs.
/           P_nb_max_connections  Nbre max de connections
/            
/ OUT:      Neant
/ RETOUR:   1 si init OK
/            0 si erreur
/ DESCRIPTION:
/
/ *********************************************************************ZYX*/
int
lib_sock_s_init( P_service, P_fct_trame, P_fct_accept, P_fct_close,
                 P_fct_quit, 
                 P_nb_max_essai_write, P_nb_max_essai_write_debug,
                 P_sleep_write_nb_ms, P_flag_debug_write, P_flag_debug_ew,
                 P_nb_max_connections, P_num_timer_accept,
                 P_nb_sec_timer_accept, P_num_timer_service,
                 P_nb_sec_timer_service, P_typ_emet )
 
char            *P_service;
void            (*P_fct_trame)();
void            (*P_fct_accept)();
void            (*P_fct_close)();
void            (*P_fct_quit)();
int             P_nb_max_essai_write;
int             P_nb_max_essai_write_debug;
unsigned int    P_sleep_write_nb_ms;
int             P_flag_debug_write;
int             P_flag_debug_ew;
int             P_nb_max_connections;
int             P_num_timer_accept;
int             P_nb_sec_timer_accept;
int             P_num_timer_service;
int             P_nb_sec_timer_service;
int             P_typ_emet;
{
int         i;
int         ret, retour = 1;
int         pid;
int         sock_flag;
sigset_t    old_mask, bidon;


    lib_erreur( 0, 0, "deb lib_sock_s_init()" );

    /* On masque les signaux */
    /* --------------------- */
    old_mask = lib_mask_masque_signal( (int) SIGIO );
    bidon    = lib_mask_masque_signal( (int) SIGALRM );
    bidon    = lib_mask_masque_signal( (int) SIGUSR1 );
    bidon    = lib_mask_masque_signal( (int) SIGUSR2 );
    bidon    = lib_mask_masque_signal( (int) SIGCLD );

    if( L_lib_sock_s_init_ok == 1 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_init()" );
        lib_erreur( 0, 0, "2ieme init => exit en dur" );
        exit( -1 );
    }

    /* Init des variables de debut et fin de trame */
    /* ------------------------------------------- */
    strncpy( L_deb_trame, SOCK_DEB_TRAME, SOCK_LG_DEB_TRAME );
    L_deb_trame[SOCK_LG_DEB_TRAME] = '\0';

    strncpy( L_fin_trame, SOCK_FIN_TRAME, SOCK_LG_FIN_TRAME );
    L_deb_trame[SOCK_LG_FIN_TRAME] = '\0';

    L_trame = (char*) malloc( (unsigned) LNG_BUFFER );

    /* Init a l'etat libre des sockets d'ecoute */
    /* ---------------------------------------- */
    for( i=0; i<SOCK_NB_MAX_CLIENTS; i++ )
    {
        L_sock_client[i].connectee = 0;
        L_sock_client[i].typ_emet = P_typ_emet;
    }

    for( i=0; i<NB_MAX_MACHINES; i++ )
    {
        machine_init( (LT_machine_autor*) &( L_machine[i] ) );
    }

    /* Init des pointeurs de fonctions */
    /* ------------------------------- */
    if( P_fct_trame == NULL )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_init()" );
        lib_erreur( 0, 0, "\t Err P_fct_trame() NULL" );
        retour = 0;
        goto fin_proc;
    }

    if( P_fct_quit == NULL )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_init() => exit en dur" );
        lib_erreur( 0, 0, "\t Err P_fct_quit() NULL" );
        exit( -1 );
    }

    L_fct_trt_trame = P_fct_trame;
    L_fct_accept = P_fct_accept;
    L_fct_close = P_fct_close;
    L_fct_quit = P_fct_quit;

    L_nb_max_essai_write = P_nb_max_essai_write;

    L_nb_max_essai_write_debug = P_nb_max_essai_write_debug;
    if( L_nb_max_essai_write_debug < 1 )
        L_nb_max_essai_write_debug = 1;
    if( L_nb_max_essai_write_debug > L_nb_max_essai_write )
        L_nb_max_essai_write_debug = L_nb_max_essai_write;

    L_sleep_write_nb_ms = (unsigned int) P_sleep_write_nb_ms;
    L_flag_debug_write = P_flag_debug_write;
    L_flag_debug_ew = P_flag_debug_ew;

    sprintf( G_msgerr,
                "\t nb_max_essai_write = %d (debug=%d)",
                L_nb_max_essai_write,
                P_nb_max_essai_write_debug );
    lib_erreur( 0, 0, G_msgerr );
    sprintf( G_msgerr,
                "\t sleep_write_nb_ms = %d ms",
                L_sleep_write_nb_ms );
    lib_erreur( 0, 0, G_msgerr );
    sprintf( G_msgerr,
                "\t flag_debug_write = %d",
                L_flag_debug_write );
    lib_erreur( 0, 0, G_msgerr );

    /* Init structure socket */
    /* --------------------- */
    L_sock_ecoute.sock.sin_addr.s_addr = (unsigned long) INADDR_ANY;
    L_sock_ecoute.sock.sin_family = AF_INET;

    /* Recuperation numero du port du service */
    /* -------------------------------------- */
    {
        struct servent    *service;

        service = (struct servent*) getservbyname( P_service, "tcp" );
        if( service == NULL )
        {
            lib_erreur( 0, 0, "Err lib_sock_s_init()" );
            sprintf( G_msgerr, "\t Err getservbyname(%s)", P_service );
            lib_erreur( 0, 0, G_msgerr );
            retour = 0;
            goto fin_proc;
        }
        L_sock_ecoute.sock.sin_port = (u_short) service->s_port;
    }

    /* Creation de la socket d'ecoute */
    /* ------------------------------ */
    L_sock_ecoute.id = socket( AF_INET, SOCK_STREAM, 0 );
    if( L_sock_ecoute.id < 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_init()" );
        lib_erreur( 0, 1, "\t Err socket()" );
        retour = 0;
        goto fin_proc;
    }

    /* Positionnement flag SO_REUSEADDR */
    /* -------------------------------- */
    sock_flag = 1;
    ret = setsockopt(   L_sock_ecoute.id,
                        (int) SOL_SOCKET,
                        (int) SO_REUSEADDR,
                        &sock_flag,
                        (int) 4 );
    if( ret < 0 ) 
    {
        lib_erreur( 0, 0, "Err lib_sock_s_init()" );
        lib_erreur( 0, 1, "\t Err setsockopt()" );
        retour = 0;
        goto fin_proc;
    }

    /* Bind de la socket */
    /* ----------------- */
    ret = bind(  L_sock_ecoute.id,
                (struct sockaddr*) &L_sock_ecoute.sock,
                sizeof(T_sockaddr_in) );
    if( ret < 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_init()" );
        lib_erreur( 0, 1, "\t Err bind()" );
        retour = 0;
        goto fin_proc;
    }

    /* ---------------------------------------------------- */
    /* Les options de ioctl() utilisees ici sont dans :     */
    /* FIONBIO dans "man 5 ioctl", "man 7 socket".          */
    /* FIOASYNC dans "man 7 socket | more".                 */
    /* SIOCSPGRP dans "man 7 socket | more".                */
    /* ---------------------------------------------------- */

    /* Definition du proprietaire de la socket */
    /* --------------------------------------- */
    pid = (int) getpid();
    ret = ioctl( L_sock_ecoute.id, (int) SIOCSPGRP, (int*) &pid );
    if( ret < 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_init()" );
        lib_erreur( 0, 1, "\t Err ioctl(SIOCSPGRP)" );
        retour = 0;
        goto fin_proc;
    }

    /* Socket declaree non blocante */
    /* ---------------------------- */
    sock_flag = (int) 1;
    ret = ioctl( L_sock_ecoute.id, (int) FIONBIO, (int*) &sock_flag );
    if( ret < 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_init()" );
        lib_erreur( 0, 1, "\t Err ioctl(FIONBIO)" );
        retour = 0;
        goto fin_proc;
    }

    /* Socket declaree asynchrone */
    /* -------------------------- */
    sock_flag = (int) 1;
    ret = ioctl( L_sock_ecoute.id, (int) FIOASYNC, (int*) &sock_flag );
    if( ret < 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_init()" );
        lib_erreur( 0, 1, "\t Err ioctl(FIOASYNC)" );
        retour = 0;
        goto fin_proc;
    }

    /* Ecoute sur la socket */
    /* -------------------- */
    if( P_nb_max_connections > SOCK_NB_MAX_CLIENTS )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_init()" );
        sprintf( G_msgerr,
                    "\t Err P_nb_max_connections = %d > %d",
                    P_nb_max_connections,
                    SOCK_NB_MAX_CLIENTS );
        lib_erreur( 0, 0, G_msgerr );
        retour = 0;
        goto fin_proc;
    }

    ret = listen( L_sock_ecoute.id, P_nb_max_connections );
    if( ret < 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_init()" );
        lib_erreur( 0, 1, "\t Err listen()" );
        retour = 0;
        goto fin_proc;
    }
    L_nb_max_connections = P_nb_max_connections ;

    machine_lit_fichier();

    L_num_timer_accept = P_num_timer_accept;
    L_nb_sec_timer_accept = P_nb_sec_timer_accept;

    L_num_timer_service = P_num_timer_service;
    L_nb_sec_timer_service = P_nb_sec_timer_service;
    
    sprintf( G_msgerr, "\t appel timer init: accept %d service %d",
             L_num_timer_accept, L_num_timer_service );
    lib_erreur( 0, 0, G_msgerr );
    
    timer_init_et_lancer();

    L_nb_malloc = 0;
    L_nb_free = 0;
    L_lib_sock_s_init_ok = 1;

    /* Detournement du signal SIGUSR2 */
    /* ---------------------------- */
    if( sigset( (int) SIGUSR2, sock_s_sigusr2 ) == SIG_ERR )
    {
        lib_erreur( 0, 1, "Err lib_sock_s_init()" );
        lib_erreur( 0, 0, "\t Err sigset(SIGUSR2)" );
        retour = 0;
        goto fin_proc;
    }

fin_proc:;
    
    lib_erreur( 0, 0, "fin lib_sock_s_init()" );
    
    /* On demasque les signaux */
    /* ----------------------- */
    lib_mask_demasque_signal( old_mask );

    return( retour );

} /* end lib_sock_s_init() */


/*XYZ***********************************************************************
/ NOM         : lib_sock_s_set_SNDBUF( P_taille ) 
/
/ IN          : P_taille, nouvelle taille pour le buffer
/ OUT         : Neant
/ RETOUR      : 1 ok , 0 sinon
/ DESCRIPTION :
/
/ **********************************************************************ZYX*/
int
lib_sock_s_set_SNDBUF( P_taille )
int   P_taille;
{

    if ((P_taille > LNG_BUFFER) && (P_taille <= MAX_SND_BUFFER))
    {
        L_longueur_SNDBUF  = P_taille;
        L_trame = (char*) realloc( (void *) L_trame, (size_t) P_taille );
        return(1);
    }
    else return(0);

} /* end lib_sock_s_set_SNDBUF() */


/*XYZ***********************************************************************
/ NOM         : lib_sock_s_set_RCVBUF( P_taille ) 
/
/ IN          : P_taille, nouvelle taille pour le buffer
/ OUT         : Neant
/ RETOUR      : 1 ok , 0 sinon
/ DESCRIPTION :
/
/ **********************************************************************ZYX*/
int
lib_sock_s_set_RCVBUF( P_taille )
int   P_taille;
{

    if ((P_taille > LNG_BUFFER) && (P_taille <= MAX_SND_BUFFER))
    {
        L_longueur_RCVBUF = P_taille;
        return(1);
    }
    else return(0);

} /* end lib_sock_s_set_RCVBUF() */


/*XYZ***********************************************************************
/ NOM         : lib_sock_s_is_init_ok() 
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : L_lib_sock_s_init_ok (0 ou 1)
/ DESCRIPTION :
/
/ *********************************************************************ZYX*/
int
lib_sock_s_is_init_ok()
{
    return( L_lib_sock_s_init_ok );
}


/*XYZ***********************************************************************
/
/ NOM:      lib_sock_s_write_socket_client()
/
/ IN:       Neant
/ OUT:      Neant 
/ RETOUR:   nb EW si write OK
/           -1 si erreur
/ DESCRIPTION:
/    Envoi une trame sur la socket
/
/ *********************************************************************ZYX*/
int
lib_sock_s_write_socket_client( P_session, P_typ_tram, P_typ_dest,
                                P_lng_data, P_data )
int        P_session;
int        P_typ_tram;
int        P_typ_dest;
int        P_lng_data;
char       *P_data;
{
int                 lng;
int                 ret_value;
sigset_t            old_mask, bidon;
LT_socket_client    *p_socket;


    /* On masque les signaux */
    /* --------------------- */
    old_mask = lib_mask_masque_signal( (int) SIGIO );
    bidon    = lib_mask_masque_signal( (int) SIGALRM );
    bidon    = lib_mask_masque_signal( (int) SIGUSR1 );
    bidon    = lib_mask_masque_signal( (int) SIGUSR2 );
    bidon    = lib_mask_masque_signal( (int) SIGCLD );

    if( L_lib_sock_s_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_write_socket_client()" );
        lib_erreur( 0, 0, "Avant lib_sock_s_init() => exit en dur" );
        exit( -1 );
    }

    /* Test numero de session */
    /* ---------------------- */
    if( P_session >= (int) SOCK_NB_MAX_CLIENTS || P_session < (int) 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_write_socket_client() => exit" );
        sprintf( G_msgerr, "\t Err P_session = %d", P_session );
        lib_erreur( 0, 0, G_msgerr );
        (*L_fct_quit)();
    }

    /* Test taille a ecrire */
    /* -------------------- */
    lng = (int) SOCK_LG_DEB_TRAME
            + (int) sizeof(T_entete)
            + (int) P_lng_data
            + (int) SOCK_LG_FIN_TRAME;

    if( lng > (int) L_longueur_SNDBUF )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_write_socket_client()" );
        sprintf( G_msgerr, "ses %d lng = %d > %d",
                 P_session, lng, L_longueur_SNDBUF );
        lib_erreur( 0, 0, G_msgerr );
        ret_value = -1;
        goto fin_proc;
    }

    if( P_lng_data < (int) 0 )
    {
        lib_erreur( 0, 0, "Err write_socket_client()" );
        sprintf( G_msgerr, "ses %d P_lng_data < 0", P_session );
        lib_erreur( 0, 0, G_msgerr );
        ret_value = -1;
        goto fin_proc;
    }

    p_socket = (LT_socket_client*) &( L_sock_client[P_session] );

    /* Test si session connectee */
    /* ------------------------- */
    if( p_socket->connectee == (int) 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_write_socket_client()" );
        sprintf( G_msgerr, "\t Err L_sock_client[%d].connectee = %d",
                 P_session, p_socket->connectee );
        lib_erreur( 0, 0, G_msgerr );
        ret_value = -1;
        goto fin_proc;
    }

    ret_value = write_socket_client( (int) P_session,
                                     (int) P_typ_tram,
                                     (int) P_typ_dest,
                                     (int) P_lng_data,
                                     (char*) P_data );
    fin_proc:;

    /* On demasque les signaux */
    /* ----------------------- */
    lib_mask_demasque_signal( old_mask );

    return( ret_value );

} /* end lib_sock_s_write_socket_client() */


/*XYZ***********************************************************************
/
/ NOM:      lib_sock_s_write_socket_all_clients()
/
/ IN:       P_entete = Structure entete
/           P_data = Donnees a transmettre
/ OUT:      Neant 
/ RETOUR:   Neant
/ DESCRIPTION:
/    Envoi une trame sur la socket
/
/ *********************************************************************ZYX*/
char*
lib_sock_s_write_socket_all_clients( P_typ_tram, P_typ_dest,
                                     P_lng_data, P_data )
int        P_typ_tram;
int        P_typ_dest;
int        P_lng_data;
char       *P_data;
{
int              i;
int              ret;
int              lng;
int              nb_stations_devlpt;
static char      str_liste[256];    /* permet de fabriquer la liste  */
                                    /* des adresses inet actives ... */
sigset_t         old_mask, bidon;
LT_socket_client *p_socket;


    /* On masque les signaux */
    /* --------------------- */
    old_mask = lib_mask_masque_signal( (int) SIGIO );
    bidon    = lib_mask_masque_signal( (int) SIGALRM );
    bidon    = lib_mask_masque_signal( (int) SIGUSR1 );
    bidon    = lib_mask_masque_signal( (int) SIGUSR2 );
    bidon    = lib_mask_masque_signal( (int) SIGCLD );

    if( L_lib_sock_s_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_write_socket_all_clients()" );
        lib_erreur( 0, 0, "Avant lib_sock_s_init() => exit en dur" );
        exit( -1 );
    }

    /* Test taille a ecrire */
    /* -------------------- */
    lng = (int) SOCK_LG_DEB_TRAME
            + (int) sizeof(T_entete)
            + (int) P_lng_data
            + (int) SOCK_LG_FIN_TRAME;

    if( lng > (int) L_longueur_SNDBUF )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_write_socket_all_clients()" );
        sprintf( G_msgerr, "lng = %d > %d", lng, (int) L_longueur_SNDBUF );
        lib_erreur( 0, 0, G_msgerr );
        goto fin_proc;
    }

    if( P_lng_data < (int) 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_write_socket_all_clients()" );
        sprintf( G_msgerr, "P_lng_data < 0" );
        lib_erreur( 0, 0, G_msgerr );
        goto fin_proc;
    }

    strcpy( str_liste, "" );
    nb_stations_devlpt = 0;
    for( i=0; i<SOCK_NB_MAX_CLIENTS; i++ )
    {
        p_socket = (LT_socket_client*) &( L_sock_client[i] );

        /* Test si session connectee */
        /* ------------------------- */
        if( p_socket->connectee == 0 ) continue;

        if( strncmp( p_socket->str_inet, "192.9.202", 9 ) == 0 )
        {
            nb_stations_devlpt++;
        }

        ret = write_socket_client(  i,
                                    P_typ_tram,
                                    P_typ_dest,
                                    P_lng_data,
                                    P_data );
        if( ret >= 0 )
        {
            /* ecriture OK */
            /* on ne garde que le dernier numero de l'adresse */
            /* ---------------------------------------------- */
            strcat( str_liste, (char*) &(p_socket->str_inet[10]) );
            strcat( str_liste, " " );
        }
    }

    /* ------------------------------------------ */
    /* on traite ensuite le fait que des machines */
    /* du reseau de devlpt sont connectees...     */
    /* ------------------------------------------ */
    if( nb_stations_devlpt > 0 )
    {
        /* interrogation en devlpt depuis Paris ...   */
        /* => on ralentit un rien le debit de donnees */
        /*    renvoye.                                */
        /* ------------------------------------------ */
        lib_sleep_milli_sec( 150 );
    }

   fin_proc:;

    /* On demasque les signaux */
    /* ----------------------- */
    lib_mask_demasque_signal( old_mask );
    
    return( str_liste );

} /* end lib_sock_s_write_socket_all_clients() */


/*XYZ***********************************************************************
/
/ NOM:      lib_sock_s_write_socket_orig_clients()
/
/ IN:       P_origine = Numero de l origine concernee
/           P_entete = Structure entete
/           P_data = Donnees a transmettre
/ OUT:      Neant 
/ RETOUR:   Neant
/ DESCRIPTION:
/    Envoi une trame sur la socket
/
/ *********************************************************************ZYX*/
void
lib_sock_s_write_socket_orig_clients( P_origine, P_typ_tram, P_typ_dest,
                                      P_lng_data, P_data )
int        P_origine;
int        P_typ_tram;
int        P_typ_dest;
int        P_lng_data;
char       *P_data;
{
int                i;
int                ret;
int                lng;
sigset_t           old_mask, bidon;
LT_socket_client   *p_socket;


    /* On masque les signaux */
    /* --------------------- */
    old_mask = lib_mask_masque_signal( (int) SIGIO );
    bidon    = lib_mask_masque_signal( (int) SIGALRM );
    bidon    = lib_mask_masque_signal( (int) SIGUSR1 );
    bidon    = lib_mask_masque_signal( (int) SIGUSR2 );
    bidon    = lib_mask_masque_signal( (int) SIGCLD );

    if( L_lib_sock_s_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_write_socket_all_clients()" );
        lib_erreur( 0, 0, "Avant lib_sock_s_init() => exit en dur" );
        exit( -1 );
    }

    /* Test taille a ecrire */
    /* -------------------- */
    lng = (int) SOCK_LG_DEB_TRAME
            + (int) sizeof(T_entete)
            + (int) P_lng_data
            + (int) SOCK_LG_FIN_TRAME;

    if( lng > (int) L_longueur_SNDBUF )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_write_socket_all_clients()" );
        sprintf( G_msgerr, "lng = %d > %d", lng, L_longueur_SNDBUF );
        lib_erreur( 0, 0, G_msgerr );
        goto fin_proc;
    }
    if( P_lng_data < 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_write_socket_all_clients()" );
        sprintf( G_msgerr, "P_lng_data < 0" );
        lib_erreur( 0, 0, G_msgerr );
        goto fin_proc;
    }

    for( i=0; i<SOCK_NB_MAX_CLIENTS; i++ )
    {
        p_socket = (LT_socket_client*) &( L_sock_client[i] );

        /* Test si session connectee */
        /* ------------------------- */
        if (p_socket->connectee == 0) continue;
        if (machine_origine(p_socket->str_inet) != P_origine) continue;

        ret = write_socket_client( i, P_typ_tram, P_typ_dest,
                                      P_lng_data, P_data );
    }

    fin_proc:;

    /* On demasque les signaux */
    /* ----------------------- */
    lib_mask_demasque_signal( old_mask );

} /* end lib_sock_s_write_socket_orig_clients() */


/*XYZ***********************************************************************
/
/ NOM:      write_socket_client()
/
/ IN:       P_session = Numero de la session
/           P_entete = Structure entete
/           P_data = Donnees a transmettre
/ OUT:      Neant 
/ RETOUR:   nb EW si write OK
/           -1 si erreur
/ DESCRIPTION:
/    Envoi une trame sur la socket
/
/ *************************************************************************ZYX*/
static int write_socket_client( P_session, P_typ_tram, P_typ_dest,
                                P_lng_data, P_data )
int        P_session;
int        P_typ_tram;
int        P_typ_dest;
int        P_lng_data;
char       *P_data;
{
int                 ret;
int                 idx;
int                 lng;
int                 ret_value = 0;
int                 nb_essai_write, tempo;
int                 nb_a_ecrire;
int                 nb_ecrits;
int                 flag_swap;
T_entete            tmp_entete, tmp2_entete;
LT_socket_client    *p_socket;


    p_socket = (LT_socket_client*) &( L_sock_client[P_session] );

    tmp_entete.typ_tram = P_typ_tram;
    tmp_entete.lng_data = P_lng_data;
    tmp_entete.typ_emet = p_socket->typ_emet;
    tmp_entete.pid_emet = getpid();
    tmp_entete.typ_dest = P_typ_dest;

    if( L_flag_debug_write >= 2 )
    {
        sprintf( G_msgerr,
                    "ses %d write_s( lng=%d.%d.%d.%d.%d )",
                    P_session,
                    tmp_entete.lng_data,
                    tmp_entete.typ_tram,
                    tmp_entete.typ_emet,
                    tmp_entete.pid_emet,
                    tmp_entete.typ_dest );
        lib_erreur( 0, 0, G_msgerr );
    }

    flag_swap = machine_flag_swap( p_socket->str_inet );

    /* Formatage de la trame */
    /* --------------------- */
    if( flag_swap == 1 )
    {
        tmp2_entete.lng_data = LBSWAP( tmp_entete.lng_data );
        tmp2_entete.typ_tram = LBSWAP( tmp_entete.typ_tram );
        tmp2_entete.typ_emet = LBSWAP( tmp_entete.typ_emet );
        tmp2_entete.pid_emet = LBSWAP( tmp_entete.pid_emet );
        tmp2_entete.typ_dest = LBSWAP( tmp_entete.typ_dest );    
    }
    else
    {
        tmp2_entete.lng_data = tmp_entete.lng_data;
        tmp2_entete.typ_tram = tmp_entete.typ_tram;
        tmp2_entete.typ_emet = tmp_entete.typ_emet;
        tmp2_entete.pid_emet = tmp_entete.pid_emet;
        tmp2_entete.typ_dest = tmp_entete.typ_dest;    
    }
    
    /*
    if( L_flag_debug_write == 1 )
    {
        sprintf( G_msgerr,
                    "swap write_s( %d.%d.%d.%d.%d )",
                    tmp2_entete.lng_data,
                    tmp2_entete.typ_tram,
                    tmp2_entete.typ_emet,
                    tmp2_entete.pid_emet,
                    tmp2_entete.typ_dest );
        lib_erreur( 0, 0, G_msgerr );
    }
    */

    idx = 0;
    memcpy(    (char*) &(L_trame[idx]),
            (char*) L_deb_trame,
            (int) strlen(L_deb_trame) );
    idx += (int) SOCK_LG_DEB_TRAME;
    
    memcpy(    (char*) &(L_trame[idx]),
            (char*) &tmp2_entete,
            (int) sizeof(T_entete) );
    idx += (int) sizeof (T_entete);
    
    memcpy(    (char*) &(L_trame[idx]),
            (char*) P_data,
            (int) tmp_entete.lng_data );
    idx += (int) tmp_entete.lng_data;
    
    memcpy(    (char*) &(L_trame[idx]),
            (char*) L_fin_trame,
            (int) strlen(L_fin_trame) );
    idx = idx + (int) SOCK_LG_FIN_TRAME;

    /* Ecriture de la trame sur la socket specifiee */
    /* -------------------------------------------- */
    nb_a_ecrire    = (int) idx;
    nb_ecrits      = (int) 0;
    nb_essai_write = (int) 0;

    /* Boucle pour ecrire la totalite de la trame */
    /* ------------------------------------------ */
    do
    {
        if( L_flag_debug_write == 1 )
        {
            sprintf( G_msgerr,
                        "deb write( %d, %d-%d lng=%d.%d.%d.%d.%d ) ses=%d",
                         (int) p_socket->id, nb_a_ecrire, nb_ecrits,
                         tmp_entete.lng_data,
                         tmp_entete.typ_tram,
                         tmp_entete.typ_emet,
                         tmp_entete.pid_emet,
                         tmp_entete.typ_dest,
                         P_session );
            lib_erreur( 0, 0, G_msgerr );
        }

        ret = write(    (int) (p_socket->id),
                        (char*) &( L_trame[nb_ecrits] ),
                        (int) (nb_a_ecrire - nb_ecrits ) );

        if( L_flag_debug_write == 1 )
        {
            lib_erreur( 0, 0, "fin" );
        }
        
        /* ----------------------------------- */
        /* Un write sur socket non blocante    */
        /* deconnectee renvoit un code -1      */
        /* ----------------------------------- */
        if( ret < 0 )
        {
            switch( errno )
            {
                case EAGAIN :
                /* case EWOULDBLOCK : */
                    nb_essai_write++;
                    tempo = L_sleep_write_nb_ms ;
                    
                    if( L_flag_debug_ew == 1 )
                    {
                        sprintf( G_msgerr, "EW %d/%d tempo %d",
                                nb_essai_write, L_nb_max_essai_write, tempo );
                        lib_erreur( 0, 0, G_msgerr );
                    }
                    if( nb_essai_write > L_nb_max_essai_write )
                    {
                        sprintf( G_msgerr,
                                "ses %d write(%d.%d.%d.%d.%d) Err EW => close",
                                        P_session,
                                        tmp_entete.lng_data,
                                        tmp_entete.typ_tram,
                                        tmp_entete.typ_emet,
                                        tmp_entete.pid_emet,
                                        tmp_entete.typ_dest );
                        lib_erreur( 0, 0, G_msgerr );

                        close_socket_client( P_session, 1 );
                        ret_value = -1;
                        goto fin_proc;
                    }
                    else
                    {
                        if ( (nb_essai_write == 1) ||
                             (nb_essai_write == L_nb_max_essai_write_debug) )
                        {
                            if( L_flag_debug_write == 1 )
                            {
                                sprintf( G_msgerr,
                                        "ses %d write(%d.%d.%d.%d.%d) EW %d/%d",
                                            P_session,
                                            tmp_entete.lng_data,
                                            tmp_entete.typ_tram,
                                            tmp_entete.typ_emet,
                                            tmp_entete.pid_emet,
                                            tmp_entete.typ_dest,
                                            nb_essai_write,
                                            L_nb_max_essai_write );
                                lib_erreur( 0, 0, G_msgerr );
                            }
                        }
                        lib_sleep( tempo );
                    }
                break;
                
/**********************************************************************
                case EAGAIN :
                    nb_essai_write++;
                    if( nb_essai_write > L_nb_max_essai_write )
                    {
                        sprintf( G_msgerr,
                                "ses %d write(%d.%d.%d.%d.%d) Err EG => close",
                                        P_session,
                                        tmp_entete.lng_data,
                                        tmp_entete.typ_tram,
                                        tmp_entete.typ_emet,
                                        tmp_entete.pid_emet,
                                        tmp_entete.typ_dest );
                        lib_erreur( 0, 0, G_msgerr );
                        
                        close_socket_client( P_session, 1 );
                        ret_value = -1;
                        goto fin_proc;
                    }
                    else
                    {
                        if( nb_essai_write >= L_nb_max_essai_write_debug )
                        {
                            sprintf( G_msgerr, "EG %d/%d",
                                            nb_essai_write,
                                            L_nb_max_essai_write );
                            lib_erreur( 0, 0, G_msgerr );
                        }
                        lib_sleep( L_sleep_write_nb_ms );
                    }
                    break;
**********************************************************************/
            
                case EPIPE :
                    sprintf( G_msgerr,
                            "ses %d write(%d.%d.%d.%d.%d) Err EPIPE => close",
                                    P_session,
                                    tmp_entete.lng_data,
                                    tmp_entete.typ_tram,
                                    tmp_entete.typ_emet,
                                    tmp_entete.pid_emet,
                                    tmp_entete.typ_dest );
                    lib_erreur( 0, 0, G_msgerr );

                    close_socket_client( P_session, 1 );
                    ret_value = -1;
                    goto fin_proc;
                    break;
				/* LOL - Ajout du cas traitement du signal connection reset by peers (errono 104)*/
				case ECONNRESET :
                    sprintf( G_msgerr,
                            "ses %d write(%d.%d.%d.%d.%d) Err ECONNRESET => close",
                                    P_session,
                                    tmp_entete.lng_data,
                                    tmp_entete.typ_tram,
                                    tmp_entete.typ_emet,
                                    tmp_entete.pid_emet,
                                    tmp_entete.typ_dest );
                    lib_erreur( 0, 0, G_msgerr );

                    close_socket_client( P_session, 1 );
                    ret_value = -1;
                    goto fin_proc;
                break;
                default :
                    /* Dans tous les autres cas : On ferme la socket */
                    /* et on appelle la fonction d'arret de la tache */
                    /* --------------------------------------------- */
                    sprintf( G_msgerr,
                            "ses %d write(%d.%d.%d.%d.%d) Err => exit (errno=%d)",
                                    P_session,
                                    tmp_entete.lng_data,
                                    tmp_entete.typ_tram,
                                    tmp_entete.typ_emet,
                                    tmp_entete.pid_emet,
                                    tmp_entete.typ_dest, errno );
                    lib_erreur( 0, 1, G_msgerr );

                    close_socket_client( P_session, 0 );
                    (*L_fct_quit)();
                                /* ARRET DU PROCESS !!!! */
                                /* <----- INUTILE !!!!!! */
                    break;
            }
        }
        else
        {
            /* Pas d'erreur : on met a jour le nb octets ecrits */
            nb_ecrits += ret;
        }
    
    } while( nb_ecrits < nb_a_ecrire );

    fin_proc :;

    if( nb_essai_write > 0 )
    {        
        if( ret_value == 0 )
            ret_value = nb_essai_write;
    }

    return( ret_value );

} /* end write_socket_client() */

/*XYZ***************************************************************************
/
/ NOM:         close_socket_client()
/
/ IN:          P_session  Numero de la session a fermer
/ OUT:         Neant
/ RETOUR:      Neant
/ DESCRIPTION:
/
/ *************************************************************************ZYX*/
static void close_socket_client( P_session, P_call_user )
int P_session;
int P_call_user;
{
int                 ret;
LT_socket_client    *p_socket;

    p_socket = (LT_socket_client*) &( L_sock_client[P_session] );

    /* Test si session initialisee */
    /* --------------------------- */
    if( p_socket->connectee == 0 )
    {
        lib_erreur( 0, 0, "Err close_socket_client() => exit" );
        sprintf( G_msgerr,
                    "\t Err L_sock_client[%d].connectee = %d",
                    P_session,
                    p_socket->connectee );
        lib_erreur( 0, 0, G_msgerr );
        (*L_fct_quit)();
    }

    /* Fermeture de la socket */
    /* ---------------------- */
    ret = close( p_socket->id );
    if( ret != 0 )
    {
        lib_erreur( 0, 0, "Err close_socket_client()" );
         sprintf( G_msgerr,
                    "\t Err close() L_sock_client[%d].id = %d",
                    P_session,
                    (int) p_socket->id );
        lib_erreur( 0, 0, G_msgerr );
   }

    /* Liberation du buffer de reception */
    /* --------------------------------- */
    free( p_socket->buf );
    L_nb_free++;

    /* La session passe a l'etat libre pour un nouvel accept */
    /* ----------------------------------------------------- */
    p_socket->connectee = 0;

    sprintf( G_msgerr, "free & Close ses=%d '%s'",
             P_session, p_socket->str_inet );
    lib_erreur( 0, 0, G_msgerr );

    /* On previent la tache avec le numero de session */
    /* ---------------------------------------------- */
    if( P_call_user == 1 && L_fct_close != NULL )
    {
        (*L_fct_close)(    P_session, p_socket->str_inet );
    }

} /* end close_socket_client() */

/*XYZ***************************************************************************
/
/ NOM:         lib_sock_s_close_socket_client()
/
/ IN:          P_session  Numero de la session a fermer
/ OUT:         Neant
/ RETOUR:      Neant
/ DESCRIPTION:
/
/ *************************************************************************ZYX*/
void
lib_sock_s_close_socket_client( P_session )
int P_session;
{

    if( L_lib_sock_s_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_close_socket_client()" );
        lib_erreur( 0, 0, "Avant lib_sock_s_init() => exit en dur" );
        exit( -1 );
    }

    /* Test numero de session */
    /* ---------------------- */
    if( P_session >= SOCK_NB_MAX_CLIENTS || P_session < 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_close_socket_client() => exit" );
        sprintf( G_msgerr, "\t Err P_session = %d", P_session );
        lib_erreur( 0, 0, G_msgerr );
        (*L_fct_quit)();
    }
    
    close_socket_client( P_session, 1 );

} /* end lib_sock_s_close_socket_client() */

/*XYZ***************************************************************************
/
/ NOM:         lib_sock_s_quit()
/
/ IN:          Neant
/ OUT:         Neant
/ RETOUR:      Neant
/ DESCRIPTION:
/
/ *************************************************************************ZYX*/
void
lib_sock_s_quit()
{
    int i;
    int ret;

    lib_erreur( 0, 0, "deb lib_sock_s_quit()" );

    if( L_lib_sock_s_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_quit()" );
        lib_erreur( 0, 0, "Avant lib_sock_s_init() => exit en dur" );
        exit( -1 );
    }

    /* ------------------------------------------ */
    /* fermeture de toutes les sockets connectees */
    /* ------------------------------------------ */
    for( i=0; i<SOCK_NB_MAX_CLIENTS; i++ )
    {
        if( L_sock_client[i].connectee == 1 )
        {
            /* Fermeture de la socket */
            /* ---------------------- */
            close_socket_client( i, 1 );
        }
    }

    /* ------------------------- */
    /* Fermeture socket d'ecoute */
    /* ------------------------- */
    ret = close( L_sock_ecoute.id );
    if( ret < 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_quit()" );
         sprintf( G_msgerr,
                    "\t Err close() L_sock_ecoute.id = %d",
                    (int) L_sock_ecoute.id );
        lib_erreur( 0, 0, G_msgerr );
    }
    
    L_lib_sock_s_init_ok = 0;
    lib_erreur( 0, 0, "\t L_lib_sock_s_init_ok=0" );
    lib_erreur( 0, 0, "\t lib_sock_s n'est plus utilisable" );

    sprintf( G_msgerr, "\tL_nb_malloc = %d", L_nb_malloc );
    lib_erreur( 0, 0, G_msgerr );
    sprintf( G_msgerr, "\tL_nb_free   = %d", L_nb_free );
    lib_erreur( 0, 0, G_msgerr );

    /* liberation buffer d emission */
    /* ---------------------------- */
    free( L_trame );

    lib_erreur( 0, 0, "fin lib_sock_s_quit()" );

} /* end lib_sock_s_quit() */

/*XYZ***************************************************************************
/
/ NOM:      lib_sock_s_nb_max_clients()
/
/ IN:       Neant
/ OUT:      Neant 
/ RETOUR:   Neant
/ DESCRIPTION:
/
/ *************************************************************************ZYX*/
int
lib_sock_s_nb_max_clients()
{
    return( SOCK_NB_MAX_CLIENTS );

} /* end lib_sock_s_nb_max_clients() */

/*XYZ***************************************************************************
/
/ NOM:      lib_sock_s_check_nb_max_clients()
/
/ IN:       Neant
/ OUT:      Neant 
/ RETOUR:
/    1 si P_nb_max_clients ne depasse pas le nombre max de
/        clients geres par la librairie
/    0 sinon
/ DESCRIPTION:
/
/ *************************************************************************ZYX*/
int
lib_sock_s_check_nb_max_clients( P_nb_max_clients )
int P_nb_max_clients;
{
    if( P_nb_max_clients > 0
        &&
        P_nb_max_clients <= SOCK_NB_MAX_CLIENTS )
    {
        return( 1 );
    }
    else
    {
        return( 0 );
    }

} /* end lib_sock_s_check_nb_max_clients() */

/*XYZ***************************************************************************
/
/ NOM:      lib_sock_s_nb_clients_connectes()
/
/ IN:       Neant
/ OUT:      Neant 
/ RETOUR:   Neant
/ DESCRIPTION:
/
/ *************************************************************************ZYX*/
int
lib_sock_s_nb_clients_connectes()
{
int                 i;
int                 nb_connect;
sigset_t            old_mask, bidon;
LT_socket_client    *p_socket;


    /* On masque les signaux */
    /* --------------------- */
    old_mask = lib_mask_masque_signal( (int) SIGIO );
    bidon    = lib_mask_masque_signal( (int) SIGALRM );
    bidon    = lib_mask_masque_signal( (int) SIGUSR1 );
    bidon    = lib_mask_masque_signal( (int) SIGUSR2 );
    bidon    = lib_mask_masque_signal( (int) SIGCLD );

    if( L_lib_sock_s_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_nb_clients_connectes()" );
        lib_erreur( 0, 0, "Avant lib_sock_s_init() => exit en dur" );
        exit( -1 );
    }

    nb_connect = 0;
    for( i=0; i<SOCK_NB_MAX_CLIENTS; i++ )
    {
        p_socket = (LT_socket_client*) &( L_sock_client[i] );

        /* Test si session connectee */
        /* ------------------------- */
        if( p_socket->connectee == 0 )
            continue;
        nb_connect++;
    }

    /* On demasque les signaux */
    /* ----------------------- */
    lib_mask_demasque_signal( old_mask );
    
    return( nb_connect );

} /* end lib_sock_s_nb_clients_connectes() */

/*XYZ***************************************************************************
/
/ NOM:      lib_sock_s_is_client_connecte()
/
/ IN:       Neant
/ OUT:      Neant 
/ RETOUR:   Neant
/ DESCRIPTION:
/
/ *************************************************************************ZYX*/
int
lib_sock_s_is_client_connecte( P_session )
int P_session;
{
sigset_t         old_mask, bidon;
LT_socket_client *p_socket;


    /* On masque les signaux */
    /* --------------------- */
    old_mask = lib_mask_masque_signal( (int) SIGIO );
    bidon    = lib_mask_masque_signal( (int) SIGALRM );
    bidon    = lib_mask_masque_signal( (int) SIGUSR1 );
    bidon    = lib_mask_masque_signal( (int) SIGUSR2 );
    bidon    = lib_mask_masque_signal( (int) SIGCLD );

    if( L_lib_sock_s_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_is_client_connecte()" );
        lib_erreur( 0, 0, "Avant lib_sock_s_init() => exit en dur" );
        exit( -1 );
    }

    /* Test numero de session */
    /* ---------------------- */
    if( P_session >= SOCK_NB_MAX_CLIENTS || P_session < 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_is_client_connecte() => exit" );
        sprintf( G_msgerr,
                    "\t Err P_session = %d",
                    P_session );
        lib_erreur( 0, 0, G_msgerr );
        (*L_fct_quit)();
    }

    p_socket = (LT_socket_client*) &( L_sock_client[P_session] );

    /* On demasque les signaux */
    /* ----------------------- */
    lib_mask_demasque_signal( old_mask );
    
    return( p_socket->connectee );

} /* end lib_sock_s_is_client_connecte() */

/*XYZ***************************************************************************
/
/ NOM:      lib_sock_s_client_origine()
/
/ IN:       Neant
/ OUT:      Neant 
/ RETOUR:
/    entre 0 et 32 selon l'origine de la machine connectee sur P_session
/    -1 sinon
/ DESCRIPTION:
/
/ *************************************************************************ZYX*/
int
lib_sock_s_client_origine( P_session )
int P_session;
{
sigset_t          old_mask, bidon;
LT_socket_client  *p_socket;


    /* On masque les signaux */
    /* --------------------- */
    old_mask = lib_mask_masque_signal( (int) SIGIO );
    bidon    = lib_mask_masque_signal( (int) SIGALRM );
    bidon    = lib_mask_masque_signal( (int) SIGUSR1 );
    bidon    = lib_mask_masque_signal( (int) SIGUSR2 );
    bidon    = lib_mask_masque_signal( (int) SIGCLD );

    if( L_lib_sock_s_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_client_origine()" );
        lib_erreur( 0, 0, "Avant lib_sock_s_init() => exit en dur" );
        exit( -1 );
    }

    /* Test numero de session */
    /* ---------------------- */
    if( P_session >= SOCK_NB_MAX_CLIENTS || P_session < 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_client_origine() => exit" );
        sprintf( G_msgerr,
                    "\t Err P_session = %d",
                    P_session );
        lib_erreur( 0, 0, G_msgerr );
        (*L_fct_quit)();
    }

    p_socket = (LT_socket_client*) &( L_sock_client[P_session] );

    /* On demasque les signaux */
    /* ----------------------- */
    lib_mask_demasque_signal( old_mask );
    
    return( machine_origine( p_socket->str_inet ) );

} /* end lib_sock_s_client_origine() */

/*XYZ***************************************************************************
/
/ NOM:      lib_sock_s_find_session()
/
/ IN:       Neant
/ OUT:      Neant 
/ RETOUR:
/ DESCRIPTION:
/
/ *************************************************************************ZYX*/
int
lib_sock_s_find_session( P_str_inet )
char* P_str_inet;
{
    int                 i;
    int                 num_ses = -1;
    sigset_t            old_mask, bidon;
    LT_socket_client    *p_socket;

    /* On masque les signaux */
    /* --------------------- */
    old_mask = lib_mask_masque_signal( (int) SIGIO );
    bidon    = lib_mask_masque_signal( (int) SIGALRM );
    bidon    = lib_mask_masque_signal( (int) SIGUSR1 );
    bidon    = lib_mask_masque_signal( (int) SIGUSR2 );
    bidon    = lib_mask_masque_signal( (int) SIGCLD );

    if( L_lib_sock_s_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_find_session()" );
        lib_erreur( 0, 0, "Avant lib_sock_s_init() => exit en dur" );
        exit( -1 );
    }

    for( i=0; i<SOCK_NB_MAX_CLIENTS; i++ )
    {
        p_socket = (LT_socket_client*) &( L_sock_client[i] );

        /* Test si session connectee */
        /* ------------------------- */
        if( p_socket->connectee == 0 )
            continue;

        if( strcmp( p_socket->str_inet, P_str_inet ) != 0 )
            continue;
        
        num_ses = i;
        break;
    }

    /* On demasque les signaux */
    /* ----------------------- */
    lib_mask_demasque_signal( old_mask );
    
    return( num_ses );

} /* end lib_sock_s_find_session() */

/*XYZ***************************************************************************
/
/ NOM:      lib_sock_s_find_session_from_origine()
/
/ IN:       Neant
/ OUT:      Neant 
/ RETOUR:
/ DESCRIPTION:
/
/ *************************************************************************ZYX*/
int
lib_sock_s_find_session_from_origine( P_origine )
int P_origine;
{
int                 i;
int                 num_ses = -1;
sigset_t            old_mask, bidon;
LT_socket_client    *p_socket;

    /* On masque les signaux */
    /* --------------------- */
    old_mask = lib_mask_masque_signal( (int) SIGIO );
    bidon    = lib_mask_masque_signal( (int) SIGALRM );
    bidon    = lib_mask_masque_signal( (int) SIGUSR1 );
    bidon    = lib_mask_masque_signal( (int) SIGUSR2 );
    bidon    = lib_mask_masque_signal( (int) SIGCLD );

    if( L_lib_sock_s_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_find_session_from_origine()" );
        lib_erreur( 0, 0, "Avant lib_sock_s_init() => exit en dur" );
        exit( -1 );
    }

    if( P_origine <= 0 || P_origine > 32 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_find_session_from_origine()" );
        sprintf( G_msgerr, "\t Err P_origine=%d => exit", P_origine );
        lib_erreur( 0, 0, G_msgerr );
        (*L_fct_quit)();
    }

    for( i=0; i<SOCK_NB_MAX_CLIENTS; i++ )
    {
        p_socket = (LT_socket_client*) &( L_sock_client[i] );

        /* Test si session connectee */
        /* ------------------------- */
        if( p_socket->connectee == 0 )
            continue;

        if( machine_origine( p_socket->str_inet ) != P_origine )
            continue;
        
        num_ses = i;
        break;
    }

    /* On demasque les signaux */
    /* ----------------------- */
    lib_mask_demasque_signal( old_mask );
    
    return( num_ses );

} /* end lib_sock_s_find_session_from_origine() */

/*XYZ***************************************************************************
/
/ NOM:      lib_sock_s_find_nb_session_from_origine()
/
/ IN:       Neant
/ OUT:      Neant 
/ RETOUR:
/ DESCRIPTION:
/    Trouve le nombre de connexions existants simultanement
/    pour une origine donnee ...
/ *************************************************************************ZYX*/
int
lib_sock_s_find_nb_session_from_origine( P_origine )
int P_origine;
{
int                 i;
int                 nb_ses = 0;
sigset_t            old_mask, bidon;
LT_socket_client    *p_socket;


    /* On masque les signaux */
    /* --------------------- */
    old_mask = lib_mask_masque_signal( (int) SIGIO );
    bidon    = lib_mask_masque_signal( (int) SIGALRM );
    bidon    = lib_mask_masque_signal( (int) SIGUSR1 );
    bidon    = lib_mask_masque_signal( (int) SIGUSR2 );
    bidon    = lib_mask_masque_signal( (int) SIGCLD );

    if( L_lib_sock_s_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_find_nb_session_from_origine()" );
        lib_erreur( 0, 0, "Avant lib_sock_s_init() => exit en dur" );
        exit( -1 );
    }

    if( P_origine <= 0 || P_origine > 32 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_find_nb_session_from_origine()" );
        sprintf( G_msgerr, "\t Err P_origine=%d => exit", P_origine );
        lib_erreur( 0, 0, G_msgerr );
        (*L_fct_quit)();
    }

    for( i=0; i<SOCK_NB_MAX_CLIENTS; i++ )
    {
        p_socket = (LT_socket_client*) &( L_sock_client[i] );

        /* Test si session connectee */
        /* ------------------------- */
        if( p_socket->connectee == 0 )
            continue;

        if( machine_origine( p_socket->str_inet ) != P_origine )
            continue;
        
        nb_ses++;
    }

    /* On demasque les signaux */
    /* ----------------------- */
    lib_mask_demasque_signal( old_mask );
    
    return( nb_ses );

} /* end lib_sock_s_find_nb_session_from_origine() */


/*XYZ***************************************************************************
/ NOM         : lib_sock_s_kill_sigio() 
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/
/ *************************************************************************ZYX*/
void
lib_sock_s_kill_sigio()
{
int                i, ret, bytes_avail;
int                y_a_qqch_a_lire = 0;
LT_socket_client   *p_socket;


#ifdef DAODEBUG
    lib_erreur( 0, 0, "deb lib_sock_s_kill_sigio()" );
#endif

    /* pas de sockets serveurs , on passe */
    /* ---------------------------------- */
    if ( L_lib_sock_s_init_ok == 0 )
    {
#ifdef DAODEBUG
        lib_erreur( 0, 0, "\t init non faite , return" );
#endif
        return;
    }
    
    /* quelque chose a lire ?? */
    /* ----------------------- */
    for( i=0; i < SOCK_NB_MAX_CLIENTS; i++ )
    {
        p_socket = (LT_socket_client*) &( L_sock_client[i] );
        if( p_socket->connectee == 0 ) continue;
        
        ret = ioctl( p_socket->id, FIONREAD, &bytes_avail );
        if( ret < 0 )
        {
#ifdef DAODEBUG
            lib_erreur( 0, 0, "Err lib_sock_s_kill_sigio()" );
            lib_erreur( 0, 1, "\t Err ioctl(FIONREAD)" );
#endif
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
        lib_erreur( 0, 0, "-->on appelle sock_s_read( 0 )" );
#endif
    }
        
    /* oui, allons y pour la lecture */
    /* ----------------------------- */
    sock_s_read( 0 );
    
#ifdef DAODEBUG
    lib_erreur( 0, 0, "fin lib_sock_s_kill_sigio()" );
#endif

} /* end lib_sock_s_kill_sigio() */


/*XYZ***************************************************************************
/
/ NOM:      lib_sock_s_get_inet_from_session()
/
/ IN:       Neant
/ OUT:      Neant 
/ RETOUR:
/ DESCRIPTION:
/
/ *************************************************************************ZYX*/
char*
lib_sock_s_get_inet_from_session( P_num_session )
int P_num_session;
{
int                 i;
static char         str_inet[SOCK_LNG_INET];
sigset_t            old_mask, bidon;
LT_socket_client    *p_socket;


    /* On masque les signaux */
    /* --------------------- */
    old_mask = lib_mask_masque_signal( (int) SIGIO );
    bidon    = lib_mask_masque_signal( (int) SIGALRM );
    bidon    = lib_mask_masque_signal( (int) SIGUSR1 );
    bidon    = lib_mask_masque_signal( (int) SIGUSR2 );
    bidon    = lib_mask_masque_signal( (int) SIGCLD );

    if( L_lib_sock_s_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_get_inet_from_session()" );
        lib_erreur( 0, 0, "Avant lib_sock_s_init() => exit en dur" );
        exit( -1 );
    }

    if( P_num_session < 0 || P_num_session >= SOCK_NB_MAX_CLIENTS )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_get_inet_from_session()" );
        sprintf( G_msgerr,
                    "Err P_num_session = %d => exit",
                        P_num_session );
        lib_erreur( 0, 0, G_msgerr );
        (*L_fct_quit)();
    }

    p_socket = (LT_socket_client*) &( L_sock_client[P_num_session] );

    strcpy( str_inet, p_socket->str_inet );

    /* On demasque les signaux */
    /* ----------------------- */
    lib_mask_demasque_signal( old_mask );
    
    return( str_inet );

} /* end lib_sock_s_get_inet_from_session() */


/*XYZ***************************************************************************
/
/ NOM:      lib_sock_s_get_flag_swap()
/
/ IN:       Neant
/ OUT:      Neant 
/ RETOUR:
/ DESCRIPTION:
/
/ *************************************************************************ZYX*/
int
lib_sock_s_get_flag_swap( P_num_session )
int P_num_session;
{
char                str_inet[SOCK_LNG_INET];
static int          flag_swap;
sigset_t            old_mask, bidon;
LT_socket_client    *p_socket;


    /* On masque les signaux */
    /* --------------------- */
    old_mask = lib_mask_masque_signal( (int) SIGIO );
    bidon    = lib_mask_masque_signal( (int) SIGALRM );
    bidon    = lib_mask_masque_signal( (int) SIGUSR1 );
    bidon    = lib_mask_masque_signal( (int) SIGUSR2 );
    bidon    = lib_mask_masque_signal( (int) SIGCLD );

    if( L_lib_sock_s_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_get_flag_swap()" );
        lib_erreur( 0, 0, "Avant lib_sock_s_init() => exit en dur" );
        exit( -1 );
    }

    if( P_num_session < 0 || P_num_session >= SOCK_NB_MAX_CLIENTS )
    {
        lib_erreur( 0, 0, "Err lib_sock_s_get_flag_swap()" );
        sprintf( G_msgerr,
                    "Err P_num_session = %d => exit",
                        P_num_session );
        lib_erreur( 0, 0, G_msgerr );
        (*L_fct_quit)();
    }

    p_socket = (LT_socket_client*) &( L_sock_client[P_num_session] );

    strcpy( str_inet, p_socket->str_inet );
    flag_swap = machine_flag_swap( str_inet );

    /* On demasque les signaux */
    /* ----------------------- */
    lib_mask_demasque_signal( old_mask );
    
    return( flag_swap );

} /* end lib_sock_s_get_flag_swap() */


/*XYZ***************************************************************************
/
/ NOM:          sock_s_read()
/
/ IN:           Neant
/ OUT:          Neant
/ RETOUR:       Neant
/ DESCRIPTION:
/               Fonction de lecture d'une trame a partir de la socket
/               appelee sur reception du signal SIGIO
/               Cette routine ne fait pas partie de l'interface de
/               programmation disponible aux programmeurs
/
/               ALGO DE LECTURE SUR SESSIONS SOCKETS 
/               ------------------------------------
/
/ *************************************************************************ZYX*/
static void sock_s_read( P_signal )
int P_signal;
{
int                 i;
int                 ret;
int                 nb_oct;
int                 stop;
int                 flag_swap;
T_entete            tmp_head;
sigset_t            old_mask, bidon;
T_entete            *p_entete;
LT_socket_client    *p_socket;

    /*
    lib_erreur( 0, 0, "" );
    lib_mask_aff_masque( "deb sock_s_read()" );
    */

    /* Masque certains signaux */
    /* ----------------------- */
    old_mask = lib_mask_masque_signal( (int) SIGIO );
    bidon    = lib_mask_masque_signal( (int) SIGALRM );
    bidon    = lib_mask_masque_signal( (int) SIGUSR1 );
    bidon    = lib_mask_masque_signal( (int) SIGUSR2 );
    bidon    = lib_mask_masque_signal( (int) SIGCLD );

    /* Parcours du tableau de session */
    /* ------------------------------ */
    for( i=0; i<SOCK_NB_MAX_CLIENTS; i++ )
    {
        p_socket = (LT_socket_client*) &( L_sock_client[i] );

        if( p_socket->connectee == 0 )
        {
            continue;
        }
        flag_swap = machine_flag_swap( p_socket->str_inet );

#ifdef DAODEBUG
        sprintf( G_msgerr, "\t sock_s_read() ses=%d connectee=%d",
                 i, p_socket->connectee );
        lib_erreur( 1, 0, G_msgerr );
#endif

        stop = 0;
        do
        {
            if( p_socket->idx >= L_longueur_RCVBUF )
            {
                lib_erreur( 0, 0, "Err sock_s_read() => exit" );
                 sprintf( G_msgerr,
                            "\t Err L_sock_client[%d].idx = %d >= %d",
                            i, p_socket->idx, L_longueur_RCVBUF );
                lib_erreur( 0, 0, G_msgerr );
                (*L_fct_quit)();
            }

            nb_oct = read(  p_socket->id,
                            p_socket->buf + p_socket->idx,
                            p_socket->alire - p_socket->lus );

#ifdef DAODEBUG
            sprintf( G_msgerr, "\t\t read()=%d ses=%d", nb_oct, i );
            lib_erreur( 1, 0, G_msgerr );
#endif
    
            if( nb_oct > 0 )
            {
                if( p_socket->lus + nb_oct == p_socket->alire )
                {
                    /* SI on a termine de lire sur l'etat courant */
                    /* de la lecture ALORS                        */
                    /* ------------------------------------------ */
                    switch( p_socket->etat_read )
                    {
                        case SOCK_ETAT_READ_WAIT_STX1 :
                            /* Test 1er caractere de debut de trame */
                            if( *(p_socket->buf) == L_deb_trame[0] )
                            {
#ifdef DAODEBUG
                                lib_erreur( 1, 0, "ST1 " );
#endif
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX2;
                                p_socket->alire = 1;
                            }
                            else
                            {
#ifdef DAODEBUG
                                lib_erreur( 1, 0, "_ST1_" );
#endif
                            }
                            break;

                        case SOCK_ETAT_READ_WAIT_STX2 :
                            /* Test 2eme caractere de debut de trame */
                            if( *(p_socket->buf) == L_deb_trame[1] )
                            {
#ifdef DAODEBUG
                                lib_erreur( 1, 0, "ST2 " );
#endif
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX3;
                            }
                            else
                            {
#ifdef DAODEBUG
                                lib_erreur( 1, 0, "_ST2_" );
#endif
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                            }
                            p_socket->alire = 1;
                            break;

                        case SOCK_ETAT_READ_WAIT_STX3 :
                            /* Test 3eme char de debut de trame */
                            if (*(p_socket->buf) == L_deb_trame[2])
                            {
#ifdef DAODEBUG
                                lib_erreur( 1, 0, "ST3 " );
#endif
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX4;
                            }
                            else
                            {
#ifdef DAODEBUG
                                 lib_erreur( 1, 0, "_ST3_" );
#endif
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                            }
                            p_socket->alire = 1;
                            break;

                        case SOCK_ETAT_READ_WAIT_STX4 :
                            /* Test 4eme char de debut de trame */
                            if (*(p_socket->buf) == L_deb_trame[3])
                            {
#ifdef DAODEBUG
                                lib_erreur( 1, 0, "ST4 " );
#endif
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_HEAD;
                                p_socket->alire = sizeof(T_entete);
                                /* On stocke en debut de buffer */
                                /* les prochains octets         */
                                p_socket->idx = 0;
                            }
                            else
                            {
#ifdef DAODEBUG
                                 lib_erreur( 1, 0, "_ST4_" );
#endif
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                                p_socket->alire = 1;
                            }
                            break;

                        case SOCK_ETAT_READ_WAIT_HEAD :
#ifdef DAODEBUG
                             lib_erreur( 1, 0, "HEAD " );
#endif
                            memcpy(    (char*) &tmp_head,
                                    (char*)    p_socket->buf,
                                    sizeof(T_entete) );

                            p_entete = (T_entete*) &( p_socket->entete );

                            if( flag_swap == 1 )
                            {
                                p_entete->lng_data = LBSWAP(tmp_head.lng_data);
                                p_entete->typ_tram = LBSWAP(tmp_head.typ_tram);
                                p_entete->typ_emet = LBSWAP(tmp_head.typ_emet);
                                p_entete->pid_emet = LBSWAP(tmp_head.pid_emet);
                                p_entete->typ_dest = LBSWAP(tmp_head.typ_dest);
                            }
                            else
                            {
                                p_entete->lng_data = tmp_head.lng_data;
                                p_entete->typ_tram = tmp_head.typ_tram;
                                p_entete->typ_emet = tmp_head.typ_emet;
                                p_entete->pid_emet = tmp_head.pid_emet;
                                p_entete->typ_dest = tmp_head.typ_dest;
                            }

                            /* Test si trame sans donnees */
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
                            /* prochaine lecture.                          */
                            /* ------------------------------------------- */
                            p_socket->idx += nb_oct;
                            break;

                        case SOCK_ETAT_READ_WAIT_DATA :
#ifdef DAODEBUG
                             lib_erreur( 1, 0, "DATA " );
#endif
                            p_socket->etat_read = SOCK_ETAT_READ_WAIT_EOT1;
                            p_socket->alire = 1;
                            p_socket->idx += nb_oct;
                            break;

                        case SOCK_ETAT_READ_WAIT_EOT1 :
                            /* Test 1er caractere de fin de trame */
                            if (*(p_socket->buf+p_socket->idx) == L_fin_trame[0])
                            {
#ifdef DAODEBUG
                                 lib_erreur( 1, 0, "ET1 " );
#endif
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_EOT2;
                            }
                            else
                            {
#ifdef DAODEBUG
                                 lib_erreur( 1, 0, "_ET1_" );
#endif
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                                p_socket->idx = 0;
                            }
                            p_socket->alire = 1;
                            break;

                        case SOCK_ETAT_READ_WAIT_EOT2 :
                            /* Test 2eme char de fin de trame */
                            if (*(p_socket->buf+p_socket->idx) == L_fin_trame[1])
                            {
#ifdef DAODEBUG
                                 lib_erreur( 1, 0, "ET2 " );
#endif
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_EOT3;
                            }
                            else
                            {
#ifdef DAODEBUG
                                 lib_erreur( 1, 0, "_ET2_" );
#endif
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                                p_socket->idx = 0;
                            }
                            p_socket->alire = 1;
                            break;

                        case SOCK_ETAT_READ_WAIT_EOT3 :
                            /* Test 3eme char de fin de trame */
                            if (*(p_socket->buf+p_socket->idx) == L_fin_trame[2])
                            {
#ifdef DAODEBUG
                                 lib_erreur( 1, 0, "ET3 " );
#endif
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_EOT4;
                            }
                            else
                            {
#ifdef DAODEBUG
                                 lib_erreur( 1, 0, "_ET3_" );
#endif
                                p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                                p_socket->idx = 0;
                            }
                            p_socket->alire = 1;
                            break;

                        case SOCK_ETAT_READ_WAIT_EOT4 :
                            /* Test 4eme caractere de fin de trame */
                            if (*(p_socket->buf+p_socket->idx) == L_fin_trame[3])
                            {
#ifdef DAODEBUG
                                 lib_erreur( 1, 0, "ET4 " );
#endif
                                /* Appel a la routine de traitement de la trame */
                                /* -------------------------------------------- */
                                (*L_fct_trt_trame)
                                ( i,
                                  (char*)     p_socket->str_inet,
                                  (T_entete*) &(p_socket->entete),
                                  (char*)     (p_socket->buf + sizeof(T_entete)),
                                  (int)       p_socket->entete.lng_data );
                            }
                            else
                            {
#ifdef DAODEBUG
                                 lib_erreur( 1, 0, "_ET4_" );
#endif
                            }
                            p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                            p_socket->alire = 1;
                            p_socket->idx = 0;
                            break;

                        default :
                            /* Cas normalement impossible */
                            lib_erreur( 0, 0, "Err sock_s_read()" );
                             sprintf( G_msgerr,
                                      "\t Err L_sock_client[%d].etat_read = %d",
                                      i, p_socket->etat_read );
                            lib_erreur( 0, 0, G_msgerr );

                            p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                            p_socket->alire = 1;
                            p_socket->idx = 0;
                            break;

                    } /* end switch */

                    /* Pour l'etat courant nous avons lus tous les octets */
                    /* -> Remise a zero du compteur d'octets deja lus */
                    p_socket->lus = 0;
                }
                else
                {
                    /* Pour l'etat courant on a pas termine de lire la totalite */
                    /* des octets -> on stocke */
                    p_socket->lus += nb_oct;
                    p_socket->idx += nb_oct;

                    /* Controle du depassement de taille de buffer */
                    /* ------------------------------------------- */
                    if( p_socket->idx >= L_longueur_RCVBUF )
                    {
                        lib_erreur( 0, 0, "Err sock_s_read() => exit" );
                        sprintf( G_msgerr,
                                 "\t Err 2 L_sock_client[%d].idx = %d >= %d",
                                 i, p_socket->idx, L_longueur_RCVBUF );
                        lib_erreur( 0, 0, G_msgerr );
                        (*L_fct_quit)();
                    }
                }
            }
            else if( nb_oct == 0 )
            {
                /* Le READ a renvoye une valeur egale a zero */
                /* ----------------------------------------- */

                /* Fermeture de la socket  */
                /* ----------------------- */
                close_socket_client( i, 1 );
                
                stop = 1;
            }
            else if( nb_oct < 0 )
            {
                /* ------------------------------------- */
                /* Le READ a renvoye une valeur negative */
                /* ------------------------------------- */
                switch( errno )
                {
                    case EAGAIN :
                    /* case EWOULDBLOCK : */
                        /* No message waiting to be read on a stream and */
                        /* O_NDELAY flag set.                            */
                        /* --------------------------------------------- */
#ifdef DEBUG
                        sprintf( G_msgerr,
                                    "%s %d -> EAGAIN read = %d sur sess %d",
                                    __FILE__, __LINE__, nb_oct, i );
                        lib_erreur( 0, 1, G_msgerr );
#endif
                        break;

/***************************************************************************
                    case EWOULDBLOCK :
#ifdef DEBUG
                        sprintf( G_msgerr,
                                    "%s %d -> EWBLOCK read = %d sur sess %d",
                                    __FILE__, __LINE__, nb_oct, i );
                        lib_erreur( 0, 1, G_msgerr );
#endif
                        break;
****************************************************************************/

                    default :
                        /* Erreur non gerable => deconnexion ! */
                        /* ----------------------------------- */
                        sprintf( G_msgerr,
                                    "%s %d -> Cas default read %d byte sess %d",
                                    __FILE__, __LINE__, nb_oct, i );
                        lib_erreur( 0, 1, G_msgerr );
        
                        p_socket->lus = 0;
                        p_socket->etat_read = SOCK_ETAT_READ_WAIT_STX1;
                        p_socket->alire = 1;
                        p_socket->idx = 0;

                        /* Fermeture socket ... */
                        /* -------------------- */
                        close_socket_client( i, 1 );

                        break;
                }
                stop = 1;
            }
    
        } while( nb_oct > 0 && stop == 0 && p_socket->connectee == 1 );

#ifdef DAODEBUG
        sprintf( G_msgerr,
                    "end do() nb_oct=%d stop=%d ses=%d",
                    nb_oct, stop, i );
        lib_erreur( 1, 0, G_msgerr );
#endif

    } /* end for i */

    /* Demasque certains signaux */
    /* ------------------------- */
    lib_mask_demasque_signal( old_mask );

    /* lib_mask_aff_masque( "fin sock_s_read()" ); */

} /* end sock_s_read() */


/*XYZ***************************************************************************
/ NOM:      sock_s_accept()
/
/ IN:       Neant
/ OUT:      Neant
/ RETOUR:
/    nbre nvelles connections si des connections ont ete acceptees
/    -1 si erreur ou rien accepte
/ DESCRIPTION:
/
/ *************************************************************************ZYX*/
static void
sock_s_accept()
{
int         i, lg;
int         k;
int         ret;
int         pid;
int         sock_flag, lg_sndbuf, lg_rcvbuf;
int         length = sizeof (T_sockaddr_in);
char        strtmp[1];
int         nb_accept ;
sigset_t    old_mask, bidon;


    if( L_lib_sock_s_init_ok == 0 )
    {
        lib_erreur( 0, 0, "Err sock_s_accept()" );
        lib_erreur( 0, 0, "Avant lib_sock_s_init() => exit en dur" );
        exit( -1 );
    }

    /* On masque les signaux */
    /* --------------------- */
    old_mask = lib_mask_masque_signal( (int) SIGIO );
    bidon    = lib_mask_masque_signal( (int) SIGALRM );
    bidon    = lib_mask_masque_signal( (int) SIGUSR1 );
    bidon    = lib_mask_masque_signal( (int) SIGUSR2 );
    bidon    = lib_mask_masque_signal( (int) SIGCLD );

    /* Parcours du tableau de session */
    /* ------------------------------ */
    nb_accept = 0;
    for( i=0; i<SOCK_NB_MAX_CLIENTS; i++ )
    {
        if( L_sock_client[i].connectee == 1 )
        {
            continue;
        }

        nb_accept++;
        if (nb_accept > L_nb_max_connections)
        {
#ifdef DAODEBUG
            sprintf(G_msgerr, "NB Accept=%d >= %d",
                    nb_accept, L_nb_max_connections );
            lib_erreur( 0, 0, G_msgerr );
#endif
            goto fin_proc;
        }
        
        /* Accept sur une socket libre */
        /* --------------------------- */
        L_sock_client[i].id = accept( L_sock_ecoute.id,
                              (struct sockaddr*) &L_sock_client[i].sock,
                              &length );

        if( L_sock_client[i].id < 0 )
        {
            switch( errno )
            {

/**********************************************************************
                case EWOULDBLOCK :
                    lib_erreur( 0, 0, "Err sock_s_accept()" );
                    lib_erreur( 0, 0, "\t Err accept(EW)" );
                    break;
**********************************************************************/

                case EAGAIN :
                /****
                    lib_erreur( 0, 0, "Err sock_s_accept()" );
                    lib_erreur( 0, 0, "\t Err accept(EAGAIN)" );
                ****/
                    break;

                case EINTR :
                    lib_erreur( 0, 0, "Err sock_s_accept()" );
                    lib_erreur( 0, 0, "\t Err accept(EINTR)" );
                    break;

                default :
                    lib_erreur( 0, 0, "Err sock_s_accept()" );
                    lib_erreur( 0, 1, "\t Err accept()" );
                    goto fin_proc;
                    break;
            }
            continue;
        }

        /* recuperation Adresse Internet du client */
        /* --------------------------------------- */
        ret = getpeername(  L_sock_client[i].id,
                            (struct sockaddr*) &(L_sock_client[i].sock),
                            &length );
        if( ret < 0 )
        {
            lib_erreur( 0, 0, "Err sock_s_accept()" );
            lib_erreur( 0, 1, "\t Err getpeername()" );
            goto fin_proc;
        }

/************************************************************************/
/* Pourquoi ecrire un caractere sur la socket apres le "accept" ?       */
/* --------------------------------------------------------------       */
/* En l'absence d'erreur, un client peut ecrire sur une socket apres    */
/* un "connect" meme si celui-ci n'a pas ete "accepte" par le serveur.  */
/* Afin d'empecher cette ecriture sans que la session soit completement */
/* etablie, la routine de connexion de la librairie client attend un    */
/* caractere du serveur avant d'autoriser l'ecriture.                   */
/* -> Apres un "accept", le serveur envoit un caractere au client       */
/************************************************************************/

        /* Ecriture d'un caractere pour initialiser le dialogue */
        /* ---------------------------------------------------- */
        strtmp[0] = SOCK_START_CHAR;
        ret = write( L_sock_client[i].id, strtmp, 1 );
        if( ret != 1 )
        {
            lib_erreur( 0, 0, "Err sock_s_accept()" );
            lib_erreur( 0, 1, "\t Err write()" );
            goto fin_proc;
        }

        /* MAJ parametres de la session */
        /* ---------------------------- */
        for( k=0; k<SOCK_LNG_INET; k++ )
        {
            L_sock_client[i].str_inet[k] = '\0';
        }

        strcpy( (char*) L_sock_client[i].str_inet,
                (char*) inet_ntoa( L_sock_client[i].sock.sin_addr ) );

        /* Allocation du buffer de reception */
        /* --------------------------------- */
        L_sock_client[i].buf = (char*) malloc( (unsigned) L_longueur_RCVBUF);

        sprintf(G_msgerr, "lib_sock_s malloc(%d)= %d", i, L_longueur_RCVBUF);
        lib_erreur( 0, 0, G_msgerr );

        if( L_sock_client[i].buf == (char*) NULL )
        {
            lib_erreur( 0, 0, "Err sock_s_accept()" );
            lib_erreur( 0, 1, "\t Err malloc()" );
            goto fin_proc;
        }

        L_nb_malloc++;

        /* adaptation taille buffer d emission SO_SNDBUF */
        /* --------------------------------------------- */
        lg = sizeof(int);
        ret = getsockopt(   L_sock_client[i].id,
                            (int) SOL_SOCKET,
                            (int) SO_SNDBUF,
                            &lg_sndbuf,
                            &lg );
        if( ret < 0 ) 
        {
            lib_erreur( 0, 0, "Err sock_s_accept()" );
            lib_erreur( 0, 1, "\t Err getsockopt(SO_SNDBUF)" );
            goto fin_proc;
        }
        if ( L_longueur_SNDBUF > lg_sndbuf ) 
        {
            sprintf( G_msgerr, "lib_sock_s setsockopt SO_SNDBUF (%d -> %d)", 
                     lg_sndbuf, L_longueur_SNDBUF );
            lib_erreur( 0, 0, G_msgerr );
            
            ret = setsockopt(   L_sock_client[i].id,
                                (int) SOL_SOCKET,
                                (int) SO_SNDBUF,
                                &L_longueur_SNDBUF,
                                (int) 4 );
            if( ret < 0 ) 
            {
                lib_erreur( 0, 0, "Err sock_s_accept()" );
                lib_erreur( 0, 1, "\t Err setsockopt(SO_SNDBUF)" );
                goto fin_proc;
            }
        }
        
        /* adaptation taille buffer de reception SO_RCVBUF */
        /* ----------------------------------------------- */
        ret = getsockopt(   L_sock_client[i].id,
                            (int) SOL_SOCKET,
                            (int) SO_RCVBUF,
                            &lg_rcvbuf,
                            &lg );
        if( ret < 0 ) 
        {
            lib_erreur( 0, 0, "Err sock_s_accept()" );
            lib_erreur( 0, 1, "\t Err getsockopt(SO_RCVBUF)" );
            goto fin_proc;
        }
        if ( L_longueur_RCVBUF > lg_rcvbuf ) 
        {
            sprintf( G_msgerr, "lib_sock_s setsockopt SO_RCVBUF (%d -> %d)", 
                     lg_rcvbuf, L_longueur_RCVBUF );
            lib_erreur( 0, 0, G_msgerr );
            
            ret = setsockopt(   L_sock_client[i].id,
                                (int) SOL_SOCKET,
                                (int) SO_RCVBUF,
                                &L_longueur_RCVBUF,
                                (int) 4 );
            if( ret < 0 ) 
            {
                lib_erreur( 0, 0, "Err sock_s_accept()" );
                lib_erreur( 0, 1, "\t Err setsockopt(SO_RCVBUF)" );
                goto fin_proc;
            }
        }
        
        ret = getsockopt(   L_sock_client[i].id,
                            (int) SOL_SOCKET,
                            (int) SO_SNDBUF,
                            &lg_sndbuf,
                            &lg );
        if( ret < 0 ) 
        {
            lib_erreur( 0, 0, "Err sock_s_accept()" );
            lib_erreur( 0, 1, "\t Err getsockopt(SO_SNDBUF)" );
            goto fin_proc;
        }
        ret = getsockopt(   L_sock_client[i].id,
                            (int) SOL_SOCKET,
                            (int) SO_RCVBUF,
                            &lg_rcvbuf,
                            &lg );
        if( ret < 0 ) 
        {
            lib_erreur( 0, 0, "Err sock_s_accept()" );
            lib_erreur( 0, 1, "\t Err getsockopt(SO_RCVBUF)" );
            goto fin_proc;
        }
        sprintf( G_msgerr, "lib_sock_s_accept SO_SNDBUF(%d) SO_RCVBUF(%d)", 
                 lg_sndbuf, lg_rcvbuf );
        lib_erreur( 0, 0, G_msgerr );


        /* Definition du proprietaire de la socket SIOCSPGRP */
        /* ---------------------------------------------------- */
        pid = (int) getpid();
        ret = ioctl( L_sock_client[i].id, (int) SIOCSPGRP, (int*) &pid );
        if( ret < 0 )
        {
            lib_erreur( 0, 0, "Err sock_s_accept()" );
            lib_erreur( 0, 1, "\t Err ioctl(SIOCSPGRP)" );
            goto fin_proc;
        }

        /* Socket declaree non blocante */
        /* ---------------------------- */
        sock_flag = (int) 1;
        ret = ioctl( L_sock_client[i].id, (int) FIONBIO, (int*) &sock_flag );
        if( ret < 0 )
        {
            lib_erreur( 0, 0, "Err sock_s_accept()" );
            lib_erreur( 0, 1, "\t Err ioctl(FIONBIO)" );
            goto fin_proc;
        }

        /* Socket declaree asynchrone */
        /* -------------------------- */
        sock_flag = (int) 1;
        ret = ioctl( L_sock_client[i].id, (int) FIOASYNC, (int*) &sock_flag );
        if( ret < 0 )
        {
            lib_erreur( 0, 0, "Err sock_s_accept()" );
            lib_erreur( 0, 1, "\t Err ioctl(FIOASYNC)" );
            goto fin_proc;
        }

        L_sock_client[i].connectee = 1;

        L_sock_client[i].etat_read = SOCK_ETAT_READ_WAIT_STX1;
        L_sock_client[i].alire = 1;
        L_sock_client[i].lus = 0;
        L_sock_client[i].idx = 0;

        sprintf( G_msgerr, "Accept ses=%d id=%d '%s'",
                 i, (int) L_sock_client[i].id, L_sock_client[i].str_inet );
        lib_erreur( 0, 0, G_msgerr );

        if( machine_nb_max_client( L_sock_client[i].str_inet ) == 0 )
        {
            sprintf( G_msgerr,
                        "'%s' n'est pas autorisee ds 'sock_autor' => close",
                        L_sock_client[i].str_inet );
            lib_erreur( 0, 0, G_msgerr );
            close_socket_client( i, 0 );
            continue;
        }
        
        if( nb_socket_client( L_sock_client[i].str_inet )
            > /* != */
            machine_nb_max_client( L_sock_client[i].str_inet ) )
        {
            sprintf( G_msgerr,
                        "'%s' possede deja %d clients => close",
                        L_sock_client[i].str_inet,
                        machine_nb_max_client( L_sock_client[i].str_inet ) );
            lib_erreur( 0, 0, G_msgerr );
            aff_socket_client( L_sock_client[i].str_inet );

            close_socket_client( i, 0 );
            continue;
        }

        if( L_fct_accept != NULL )
        {
            (*L_fct_accept)( i, L_sock_client[i].str_inet );
        }
            
    } /* end for i */

    fin_proc:;

    /* On demasque les signaux */
    /* ----------------------- */
    lib_mask_demasque_signal( old_mask );
    
} /* end sock_s_accept() */

/*XYZ***************************************************************************
/ NOM         : sock_s_sigusr2()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/ *************************************************************************ZYX*/
static void sock_s_sigusr2( P_timer )
int P_timer;
{

    lib_erreur( 0, 0, "SIGUSR2 => machine_lit_fichier()" );
    
    machine_lit_fichier();

    if( sigset( (int) SIGUSR2, sock_s_sigusr2 ) == SIG_ERR )
    {
        lib_erreur( 0, 1, "Err sock_s_sigusr2()" );
        lib_erreur( 0, 0, "\t Err sigset(SIGUSR2)" );
    }

} /* end sock_s_sigusr2() */

/*XYZ***************************************************************************
/ NOM         : nb_socket_client()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      :
/    nbre de connections en cours sur la machine P_str_inet
/ DESCRIPTION :
/ *************************************************************************ZYX*/
static int nb_socket_client( P_str_inet )
char* P_str_inet;
{
int                 i;
int                 nb_sockets;
LT_socket_client    *p_socket;
   
    nb_sockets = 0;

    for( i=0; i<SOCK_NB_MAX_CLIENTS; i++ )
    {
        p_socket = (LT_socket_client*) &( L_sock_client[i] );

        if ( p_socket->connectee == 0 ) continue;

        if ( strcmp( p_socket->str_inet, P_str_inet ) != 0 ) continue;
        
        nb_sockets++;

    } /* end for i */

    return( nb_sockets );

} /* end nb_socket_client() */

/*XYZ***************************************************************************
/ NOM         : aff_socket_client()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      :
/    nbre de connections en cours sur la machine P_str_inet
/ DESCRIPTION :
/ *************************************************************************ZYX*/
static void aff_socket_client( P_str_inet )
char* P_str_inet;
{
int                 i;
LT_socket_client    *p_socket;
    
    for( i=0; i<SOCK_NB_MAX_CLIENTS; i++ )
    {
        p_socket = (LT_socket_client*) &( L_sock_client[i] );

        if ( p_socket->connectee == 0 ) continue;

        if ( strcmp( p_socket->str_inet, P_str_inet ) != 0 ) continue;

        sprintf( G_msgerr, "\t i=%d", i );
        lib_erreur( 0, 0, G_msgerr );

    } /* end for i */

} /* end aff_socket_client() */
