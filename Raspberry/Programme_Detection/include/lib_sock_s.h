/*ABC***********************************************************************
*
* NOM         : lib_sock_s.h
*
* PROJET      : PSCL
*
* PROCESS     : 
*
* TYPE        : Include C
*
* ECRIT PAR   : D. DELEFORTERIE              Le 13/05/2012
*
* MODIFS      :
*
* DESCRIPTION : 
*
***********************************************************************CBA*/
#ifndef _LIB_SOCK_S_
#define _LIB_SOCK_S_

#include <lib_sock_incl.h>

/* --------------------------- */
/* Declarations des constantes */
/* --------------------------- */
#include <lib_sock_const.h>

/* ------------------------------------------------------ */
/*  ATTENTION ...  ATTENTION ... ATTENTION ... ATTENTION  */
/* ------------------------------------------------------ */
/* le parametre system MAXFILES doit pouvoir contenir les */
/* streams associes aux sockets (2 streams par connexion) */
/* plus le reste des fichiers ouverts !!!                 */
/* il vaut actuellement (au 16/05/2007) : 1024            */
/* ------------------------------------------------------ */
#define SOCK_NB_MAX_CLIENTS         150
#define NB_MAX_MACHINES             30  


/* ---------------------- */
/* Declarations des types */
/* ---------------------- */
#include <lib_sock_type.h>

typedef struct
{
    int        place_libre;
    char       str_inet[SOCK_LNG_INET];
    int        nb_max_client;
    int        flag_swap;
    int        origine;

} LT_machine_autor;


typedef struct
{
    long             id;
    T_sockaddr_in    sock;

} LT_socket_ecoute;

typedef struct
{
    long            id;                   /* Descripteur de la socket        */
    
    T_sockaddr_in   sock;                 /* Structure systeme               */
    
    char            str_inet[SOCK_LNG_INET];      /* Adresse Internet ASCII  */
    
    int             connectee;            /* Etat de la session              */
    int             etat_read;            /* Etat de la lecture              */
    int             alire;                /* Nb octets a lire pour 1 etat    */
    int             lus;                  /* Nb octets lus pour 1 etat       */
    int             idx;                  /* Index acces au buffer reception */
    int             gap1;

    char            *buf;                 /* Buffer de lecture socket        */

    T_entete        entete;               /* entete en cours de lecture      */
                                          /* utile si lecture dure plusieurs */
                                          /* appels a sock_s_read()          */
    int             typ_emet;
    int gap2;

} LT_socket_client;


/* -------------------------- */
/* Declarations des fonctions */
/* -------------------------- */

extern int    lib_sock_s_init();
extern int    lib_sock_s_set_SNDBUF();
extern int    lib_sock_s_set_RCVBUF();
extern int    lib_sock_s_is_init_ok();
               
extern int    lib_sock_s_write_socket_client();
extern char*  lib_sock_s_write_socket_all_clients();
extern void   lib_sock_s_write_socket_orig_clients();

extern void   lib_sock_s_close_socket_client();
extern void   lib_sock_s_quit();
extern int    lib_sock_s_nb_max_clients();
extern int    lib_sock_s_check_nb_max_clients();
extern int    lib_sock_s_nb_clients_connectes();
extern int    lib_sock_s_is_client_connecte();
extern int    lib_sock_s_client_origine();
extern int    lib_sock_s_find_session();
extern int    lib_sock_s_find_session_from_origine();
extern int    lib_sock_s_find_nb_session_from_origine();
extern void   lib_sock_s_kill_sigio();
extern char*  lib_sock_s_get_inet_from_session();
extern int    lib_sock_s_get_flag_swap();

extern int    lib_sock_s_max_machine();
extern char*  lib_sock_s_get_inet_machine();
extern int    lib_sock_s_is_machine_libre();

#endif
