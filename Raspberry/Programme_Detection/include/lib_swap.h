/*ABC************************************************************
/
/ NOM       : lib_swap.h
/
/ PROJET    : PSCL
/
/ PROCESS   : 
/
/ TYPE      : Fichier include C
/
/ ECRIT PAR : D DELEFORTERIE                  Le 18/05/2012
/
/ MODIFS    :
/
/ DESCRIPTION:
/  Definition des casts entre BIGENDIAN et LITTLEENDIAN
/                       entre flottant MODCOMP et IEEE
/
*************************************************************CBA*/
#ifndef _LIB_SWAP_
#define _LIB_SWAP_

extern short   WBSWAP(  );
extern int     LBSWAP(  );
extern float   FBSWAP(  );
extern double  DBSWAP(  );

#endif
