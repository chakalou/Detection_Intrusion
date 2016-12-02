/*ABC************************************************************
/
/ NOM       : lib_swap.c
/
/ PROJET    : PSCL
/
/ PROCESS   : 
/
/ TYPE      : Fichier include C
/
/ ECRIT PAR : D DELEFORTERIE    Le 18/08/2012
/
/ MODIFS    : 
/
/ DESCRIPTION:
/  Fonction de casts entre BIGENDIAN et LITTLEENDIAN
/                    entre flottant MODCOMP et IEEE
//
*************************************************************CBA*/

#include <stdio.h>
#include <stdlib.h>

typedef union
{
    char   c[8];
    short  s[4];
    int    i[2];
    float  f[2];
    double d;
    
} T_equiv;

/*XYZ*********************************************************************
/
/ NOM        : WBSWAP()
/
/ IN         : P_valeur, valeur a convertir
/
/ OUT        : Neant
/
/ RETOUR     : valeur convertie
/
/ DESCRIPTION:
/   Conversion entre BIGENDIAN et LITTLEENDIAN
/   Conversion entre flottant MODCOMP et IEEE
/   pour le cas d un short
/
*********************************************************************ZYX*/

short WBSWAP( P_valeur )
short P_valeur;
{
T_equiv  tmp;
char     tf;

    tmp.s[0] = P_valeur;
    
    tf = tmp.c[0];
    tmp.c[0] = tmp.c[1];
    tmp.c[1] = tf;
    
    return( tmp.s[0] );

} /* end WBSWAP() */


/*XYZ*********************************************************************
/
/ NOM        : LBSWAP()
/
/ IN         : P_valeur, valeur a convertir
/
/ OUT        : Neant
/
/ RETOUR     : valeur convertie
/
/ DESCRIPTION:
/   Conversion entre BIGENDIAN et LITTLEENDIAN
/   Conversion entre flottant MODCOMP et IEEE
/   pour le cas d un int
/
*********************************************************************ZYX*/

int LBSWAP( P_valeur )
int P_valeur;
{
T_equiv  tmp;
char     tf;

    tmp.i[0] = P_valeur;
    
    tf = tmp.c[0];
    tmp.c[0] = tmp.c[3];
    tmp.c[3] = tf;
    tf = tmp.c[1];
    tmp.c[1] = tmp.c[2];
    tmp.c[2] = tf;
    
    return( tmp.i[0] );

} /* end LBSWAP() */


/*XYZ*********************************************************************
/
/ NOM        : FBSWAP()
/
/ IN         : P_valeur, valeur a convertir
/
/ OUT        : Neant
/
/ RETOUR     : valeur convertie
/
/ DESCRIPTION:
/   Conversion entre BIGENDIAN et LITTLEENDIAN
/   Conversion entre flottant MODCOMP et IEEE
/   pour le cas d un float
/
*********************************************************************ZYX*/
float FBSWAP( P_valeur )
float P_valeur;
{
T_equiv  tmp;
char     tf;

    tmp.f[0] = P_valeur;
    
    tf = tmp.c[0];
    tmp.c[0] = tmp.c[3];
    tmp.c[3] = tf;
    tf = tmp.c[1];
    tmp.c[1] = tmp.c[2];
    tmp.c[2] = tf;
    
    return( tmp.f[0] );

} /* end FBSWAP() */


/*XYZ*********************************************************************
/
/ NOM        : DBSWAP()
/
/ IN         : P_valeur, valeur a convertir
/
/ OUT        : Neant
/
/ RETOUR     : valeur convertie
/
/ DESCRIPTION:
/   Conversion entre BIGENDIAN et LITTLEENDIAN
/   Conversion entre flottant MODCOMP et IEEE
/   pour le cas d un double
/
*********************************************************************ZYX*/

double DBSWAP( P_valeur )
double P_valeur;
{
T_equiv  tmp;
char     tf;

    tmp.d = P_valeur;
    
    tf = tmp.c[0];
    tmp.c[0] = tmp.c[7];
    tmp.c[7] = tf;
    tf = tmp.c[1];
    tmp.c[1] = tmp.c[6];
    tmp.c[6] = tf;
    tf = tmp.c[2];
    tmp.c[2] = tmp.c[5];
    tmp.c[5] = tf;
    tf = tmp.c[3];
    tmp.c[3] = tmp.c[4];
    tmp.c[4] = tf;
    
    return( tmp.d );

} /* end DBSWAP() */

