/*ABC*********************************************************************
*
* NOM         : det_type.h
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
	short idcontact; // Id contact (unique)
	char tel[16]; //Telephone
	char adremail[128]; //adressmail
	short notifysms; //bool notif sms
	short notifymail; //bool notif mail
}T_Contact;

typedef struct
{
	T_Contact Liste_contact[MAX_CONTACT];
	int Nbcontact;
}T_Contact_List;

typedef struct
{
	T_Detecteur Liste_det[MAX_DETECTEUR];
	int Nbdetect;
	int NumImage;
}T_Detecteur_List;

#endif

