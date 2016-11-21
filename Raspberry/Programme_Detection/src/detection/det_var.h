/*ABC**********************************************************************
*
* NOM         : det_var.h
*
* PROJET      : PSCL
*
* PROCESS     :
*
* TYPE        : Include C
*
* ECRIT PAR   : D. DELEFORTERIE                   25/06/2013
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
T_Contact_List   G_Contacts;
int firstinit;

char G_nomdb[124];

#endif

