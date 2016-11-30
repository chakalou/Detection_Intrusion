/*ABC***********************************************************************
*
* NOM         : lib_sock_const.h
*
* PROJET      : PSCL
*
* PROCESS     : 
*
* TYPE        : Include C
*
* ECRIT PAR   : D. DELEFORTERIE             Le 22/05/2012
*
* MODIFS      : 
*
* DESCRIPTION : 
*
***********************************************************************CBA*/
#ifndef _LIB_SOCK_CONST_
#define _LIB_SOCK_CONST_

/* ---------------------------------------------------- */
/* Valeurs des constantes generales a la gestion socket */
/* ---------------------------------------------------- */

/* Taille par defaut des buffer de socket SO_SNDBUF et SO_RECVBUF (32 Ko) */
#define LNG_BUFFER              32768     
/* et pour tenir compte de l entete */
#define LNG_UTILE_BUFFER        (32768-28)  

/* taille maximale autorisee par l appli en ecriture socket (4 Mo) */
#define MAX_SND_BUFFER          4194304     
 

#define SOCK_LNG_INET           24        /* Longueur adresse INTERNET */
#define SOCK_DEB_TRAME          "YODA"    /* Chaine de debut de trame  */
#define SOCK_FIN_TRAME          "JEDI"    /* Chaine de fin de trame    */
#define SOCK_LG_DEB_TRAME       4         /* Longueur debut de trame   */
#define SOCK_LG_FIN_TRAME       4         /* Longueur fin de trame     */
#define SOCK_START_CHAR         '$'       /* char envoye sur accept    */

#define SOCK_ETAT_READ_WAIT_STX1    0     /* Attente 1er char debut */
#define SOCK_ETAT_READ_WAIT_STX2    1     /* Attente 2eme char debut */
#define SOCK_ETAT_READ_WAIT_STX3    2     /* Attente 3eme char debut */
#define SOCK_ETAT_READ_WAIT_STX4    3     /* Attente 4eme char debut */
#define SOCK_ETAT_READ_WAIT_HEAD    4     /* Attente entete */
#define SOCK_ETAT_READ_WAIT_DATA    5     /* Attente data */
#define SOCK_ETAT_READ_WAIT_EOT1    6     /* Attente 1er char fin */
#define SOCK_ETAT_READ_WAIT_EOT2    7     /* Attente 2eme char fin */
#define SOCK_ETAT_READ_WAIT_EOT3    8     /* Attente 3eme char fin */
#define SOCK_ETAT_READ_WAIT_EOT4    9     /* Attente 4eme char fin */
#define SOCK_ETAT_READ_WAIT_INIT    10    /* Attente 1er char du serveur */


/* ------------------------------------------------------------------ */
/*          Valeurs du champ "typ_tram" de la struct T_entete         */
/* ------------------------------------------------------------------ */

#define TT_SERVICE                         1   
#define TT_TRAITEMENT                      2
#define TT_YOU_MUST_DIE                    3   
#define TT_ACQUIT_CONNEXION                4
#define TT_MESSAGE                         5
#define TT_SYNCHRO                         6



/* ------------------------------------------------------------------ */
/*     Valeurs du champ "typ_emet" de la struct T_entete              */
/*     Valeurs du champ "typ_dest" de la struct T_entete              */
/*     ED = 'Emetteur/Destinataire'                                   */
/* ------------------------------------------------------------------ */

#define TED_INCONNU             0
#define TED_SKELET              1
#define TED_TRORNN              2

#define TED_GP                  3


#endif
