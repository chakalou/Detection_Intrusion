/*ABC***********************************************************************
*
* NOM         : det_init.c
*
* PROJET      : PSCL
* PROCESS     :
* TYPE        : Include C
*
* ECRIT PAR   : D. DELEFORTERIE                   25/06/2013
*
* MODIFS      :
*
* DESCRIPTION :
*    - Inits diverses, en particulier systeme, de la tache 
*
***********************************************************************CBA*/
#include "det_incl.h"
#include "det_const.h"
#include "det_type.h"
#include "det_var.h"
#include "det_proto.h"
/*Pour connaitre lib_karotz*/
#include "lib_karotz.h"

/* Pour connaitre lib_erreur() */
/* --------------------------- */
#include <lib_erreur.h>

/* Pour connaitre lib_serial() */
/* --------------------------- */
#include <lib_serial.h>


		/*
		wiringPi
		Pin 	BCM
		GPIO 	Name 	Header 	Name 	BCM
		GPIO 	wiringPi
		Pin
		– 	– 	3.3v 	1 | 2 	5v 	– 	–
		8 	0 	SDA0 	3 | 4 	DNC 	– 	–
		9 	1 	SCL0 	5 | 6 	0v 	– 	–
		7 	4 	GPIO 7 	7 | 8 	TxD 	14 	15
		– 	– 	DNC 	9 | 10 	RxD 	15 	16
		0 	17 	GPIO 0 	11 | 12 	GPIO 1 	18 	1
		2 	21 	GPIO 2 	13 | 14 	DNC 	– 	–
		3 	22 	GPIO 3 	15 | 16 	GPIO 4 	23 	4
		– 	– 	DNC 	17 | 18 	GPIO 5 	24 	5
		12 	10 	MOSI 	19 | 20 	DNC 	– 	–
		13 	9 	MISO 	21 | 22 	GPIO 6 	25 	6
		14 	11 	SCLK 	23 | 24 	CE0 	8 	10
		– 	– 	DNC 	25 | 26 	CE1 	7 	11
		*/

/*XYZ*********************************************************************
/ NOM        : int det_init()
/
/ IN         : Neant
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/  initialise les sockets de dialogue UDP et TCP(messagerie)
/   
*********************************************************************ZYX*/
void
det_init()
{
	char cmd[256];
	lib_erreur(0,0,"lib_kartoz_speak");
	lib_kartoz_speak("debut detection intrusion");
	lib_erreur(0,0,"lib_kartoz_set_led");
	lib_kartoz_set_led(0,"000000",0,"000000");
	/*initialisation de la connection serie*/	
	lib_erreur(0,0,"det_init: port serie");

	lib_erreur(0,0,"det_init: base");
	/*init nombase*/
	strcpy(G_nomdb,"detection.db");
	/*creation de la table detection si elle n'existe pas*/
	if(!det_sqlite_createtable(G_nomdb))
		det_quit();
		
	lib_erreur(0,0,"det_init: det_charge_struct_detect");
	det_charge_struct_detect();
	lib_erreur(0,0,"det_init: det_charge_struct_contact");
	det_charge_struct_contact();

	if(!lib_serial_init(PORT_NAME))
		det_quit();
	lib_serial_config();
	
} /* end det_init() */
void
det_charge_struct_detect()
{
	char sqlcmde[256];
	/*variable first init au cas où on veut traiter le rechargement de la config via siguser1*/
	firstinit=0;
	
	/*Initialisation de la structure contenant les détecteurs*/
	int i;
	lib_erreur(0,0,"DEB det_charge_struct_detect ");
	G_Detecteurs.Nbdetect=0;
	for(i=0;i<MAX_DETECTEUR;i++)
	{
	  G_Detecteurs.Liste_det[i].iddetecteur=-1;
	  strcpy(G_Detecteurs.Liste_det[i].piece,"");
	  G_Detecteurs.Liste_det[i].codedetec=-1;
	  G_Detecteurs.Liste_det[i].numimage=0;
	  strcpy(G_Detecteurs.Liste_det[i].nomficimage,"");
	}
	
	strcpy(sqlcmde, "select * from yana_detecteur; ");
	if(!det_sqlite_select((void*) &G_Detecteurs,G_nomdb, sqlcmde, callback_detec))
		det_quit();
	firstinit=1;
	
	/*On affiche la structure*/
	for(i=0;i<MAX_DETECTEUR;i++)
	{
		sprintf(G_msgerr, "iddet=%d,piece=%s,code=%d,numim=%d,ficim=%s",
		  G_Detecteurs.Liste_det[i].iddetecteur,
		  G_Detecteurs.Liste_det[i].piece,
		  G_Detecteurs.Liste_det[i].codedetec,
		  G_Detecteurs.Liste_det[i].numimage,
		  G_Detecteurs.Liste_det[i].nomficimage);
		  lib_erreur(0,0,G_msgerr);
	}
	lib_erreur(0,0,"FIN det_charge_struct_detect ");

}
void
det_charge_struct_contact()
{
	char sqlcmde[256];
	/*variable first init au cas où on veut traiter le rechargement de la config via siguser1*/
	firstinit=0;
	
	/*Initialisation de la structure contenant les contacts*/
	int i;
	lib_erreur(0,0,"DEB det_charge_struct_contact ");
	G_Contacts.Nbcontact=0;
	for(i=0;i<MAX_CONTACT;i++)
	{
	  G_Contacts.Liste_contact[i].idcontact=-1;
	  strcpy(G_Contacts.Liste_contact[i].tel,"");
	  strcpy(G_Contacts.Liste_contact[i].adremail,"");
	  G_Contacts.Liste_contact[i].notifysms=0;
	  G_Contacts.Liste_contact[i].notifymail=0;
	}
	
	strcpy(sqlcmde, "select * from yana_contact; ");
	if(!det_sqlite_select((void*) &G_Contacts,G_nomdb, sqlcmde, callback_contact))
		det_quit();
	firstinit=1;
	
	/*On affiche la structure*/
	for(i=0;i<MAX_CONTACT;i++)
	{
		sprintf(G_msgerr, "id=%d,tel=%s,mail=%s,notifysms=%d,notifymail=%d",
		  G_Contacts.Liste_contact[i].idcontact,
		  G_Contacts.Liste_contact[i].tel,
		  G_Contacts.Liste_contact[i].adremail,
		  G_Contacts.Liste_contact[i].notifysms,
		  G_Contacts.Liste_contact[i].notifymail);
		  lib_erreur(0,0,G_msgerr);
	}
	lib_erreur(0,0,"FIN det_charge_struct_detect ");

}


