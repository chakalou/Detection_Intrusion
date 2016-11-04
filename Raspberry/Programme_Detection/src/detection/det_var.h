/*ABC**********************************************************************
*
* NOM         : det_var.h
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
***********************************************************************CBA*/
#ifndef _DET_VAR_
#define _DET_VAR_
#include "det_type.h"
short G_Pin_Detection;
int G_cpt_attente;

T_Detecteur_List G_Detecteurs;
int firstinit;

/*G_nomdb*/
char G_nomdb[124];
char G_mail[128];
char G_numero[128];

#endif

