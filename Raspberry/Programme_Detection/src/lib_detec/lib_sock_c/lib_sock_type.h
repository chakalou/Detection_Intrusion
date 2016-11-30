/*ABC***********************************************************************
*
* NOM         : lib_sock_type.h
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
#ifndef _LIB_SOCK_TYPE_
#define _LIB_SOCK_TYPE_

typedef struct
{
    int         lng_data;
    int         typ_tram;    /* les valeurs sont des csttes TT_xxx */
    int         typ_emet;    /* les valeurs sont des csttes TED_xxx */
    int         pid_emet;
    int         typ_dest;    /* les valeurs sont des csttes TED_xxx */
    int         gap;

} T_entete;

/* Structure trame */
/* --------------- */
typedef struct
{
    char        deb[4];       /* Mot-cle debut de trame */
    char        fin[4];       /* Mot-cle fin de trame   */
    T_entete    entete;       /* Entete                 */
    char        *data;        /* Pointeur sur donnees   */

} T_trame;

typedef struct sockaddr_in T_sockaddr_in ;

#endif
