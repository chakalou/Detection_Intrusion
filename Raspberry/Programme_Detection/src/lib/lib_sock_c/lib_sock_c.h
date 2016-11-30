/*ABC***********************************************************************
*
* NOM         : lib_sock_c.h
*
* PROJET      : PSCL
*
* PROCESS     : 
*
* TYPE        : Include C
*
* ECRIT PAR   : D. DELEFORTERIE             Le 18/05/2012
*
* MODIFS      :
*
* DESCRIPTION : 
*
***********************************************************************CBA*/
#ifndef _LIB_SOCK_C_
#define _LIB_SOCK_C_

#include <lib_sock_incl.h>

/* --------------------------- */
/* Declarations des constantes */
/* --------------------------- */
#include <lib_sock_const.h>

#define NB_MAX_SERVEURS                8

/* ---------------------- */
/* Declarations des types */
/* ---------------------- */
#include <lib_sock_type.h>

typedef struct
{
    long            id ;                /* Descripteur de la socket        */
    
    T_sockaddr_in   sock;               /* Structure systeme               */
    
    int             init_ok;
    int             connect_en_cours;
    int             connectee;          /* Etat de la session              */
    int             etat_read;          /* Etat de la lecture              */
    int             alire;              /* Nb octets a lire pour 1 etat    */
    int             lus;                /* Nb octets lus pour 1 etat       */
    int             idx;                /* Index acces au buffer reception */
    int             gap1;
    
    char            *buf;               /* Buffer de lecture socket        */

    T_entete        entete;             /* entete en cours de lecture      */
                                        /* utile si lecture dure plusieurs */
                                        /* appels a sock_c_read()          */

    void            (*fct_trame)();
    void            (*fct_connect)();
    void            (*fct_close)();

    char            str_serveur[32];
    char            str_service[32];
    char            host[32];
    
    int             typ_emet;
    int             flipflop;
    int             tentative;
    int             gap2;

} LT_socket_serveur;

/* --------------------------- */
/* Declarations des fonctions  */
/* --------------------------- */

extern void	lib_sock_c_init();
extern int  lib_sock_c_is_init_ok();
extern int	lib_sock_c_init_socket_client();
extern int	lib_sock_c_write_socket_client();
extern void	lib_sock_c_close_socket_client();
extern void	lib_sock_c_quit();
extern void	lib_sock_c_kill_sigio();
extern int	lib_sock_c_is_client_connecte();

#endif
