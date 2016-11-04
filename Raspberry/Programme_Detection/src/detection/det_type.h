/*ABC*********************************************************************
*
* NOM         : det_type.h
*
* PROJET      : Detection
*
* PROCESS     :
*
* TYPE        : Include C
*
* ECRIT PAR   : LOLIO                   25/06/2013
*
* MODIFS      :
*
* DESCRIPTION :
*
**********************************************************************CBA*/
#ifndef _DET_TYPE_
#define _DET_TYPE_
#include "det_const.h"

typedef struct
{
	short iddetecteur; // Id du detecteur (unique)
	short gap;
	char piece[24];     // Piece associe au detecteur
	int  codedetec;    // Code detection radio associe au detecteur
	int  numimage;
	char nomficimage[16];
}T_Detecteur;

typedef struct
{
	T_Detecteur Liste_det[MAX_DETECTEUR];
	int Nbdetect;
	int NumImage;
}T_Detecteur_List;

#endif

