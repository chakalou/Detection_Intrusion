/*ABC***********************************************************************
*
* NOM         : detection.c
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
*
***********************************************************************CBA*/
/*pour connaitre librairies détection*/
#include "det_incl.h"
#include "det_var.h"
#include "det_type.h"
#include "det_proto.h"
#include "det_const.h"
#include <unistd.h>

/* pour connaitre G_msgerr.. */
/* ------------------------- */
#include <lib_erreur.h>

/* pour connaitre Date */
/* -------------------------- */
#include <lib_date.h>

/* pour connaitre lib_sleep() */
/* -------------------------- */
#include <lib_sleep.h>

/* pour connaitre les constantes timer */
/* ----------------------------------- */
#include <lib_timer.h>
#include <time.h>

/* pour connaitre lib_mask() */
/* ------------------------- */
#include <lib_mask.h>

/* fonctions locales au fichier */
/* ---------------------------- */
void           det_trt_loop();
void			  det_trt();
int det_analysedetec(char* buffer);
int det_detection(T_Detecteur* detecteur);
int test_fic(char* cheminfic);

/*XYZ*********************************************************************
/ NOM         : main()
/
/ IN          : Standard
/ OUT         : Neant
/ INOUT       : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/    - boucle principale de traitement de la tache 
/
*********************************************************************ZYX*/
int
main( P_argc, P_argv )
int      P_argc;
char   **P_argv;
{

	char  derr[512], dparam[512];
	Date  datem;

	
    /* 1ere chose a faire : on passe le nom du process a lib_erreur */
    /* ------------------------------------------------------------ */
    sprintf( derr,   "%s", getenv( "DIR_ERREUR" ) );
    sprintf( dparam, "%s", getenv( "DIR_PARAMETRAGE" ) );
    lib_erreur_init( P_argv[0], derr );
    lib_erreur( 0, 0, "Debut DETECTION" );

    /* NB: Au moment ou DETECTION est lance par GP, GP a      */
    /*  masque ces 4 signaux. DETECTION herite de ce masquage.*/
    /*  Donc, il faut les rajouter artificiellement au      */
    /*  lancement !!!                                       */
    /*------------------------------------------------------*/
    lib_mask_rend_signal( SIGALRM );
    lib_mask_rend_signal( SIGIO );
    lib_mask_rend_signal( SIGUSR1 );
    lib_mask_rend_signal( SIGUSR2 );

    /* initialisation des divers signaux */
    /*-----------------------------------*/
    lib_erreur( 0, 0, "-->> dif_signal_init()" );
    det_signal_init();
	
	/* initialisation des divers timers */
    /*----------------------------------*/
    lib_erreur( 0, 0, "-->> dif_timer_init()" );
    det_timer_init();

    /* initialisation des communications */
    /*-----------------------------------*/
    lib_erreur( 0, 0, "-->> det_init()" );
    det_init();
	
	/* Lance le timer pour remettre à zéro les détection toutes les 10s */
    /* ---------------------------------------------------------------- */
    lib_erreur( 0, 0, "-->> lib_timer_lancer()" );
    lib_timer_lancer( TIMER_TRAITEMENT, 1, 0 );
    
    lib_erreur( 0, 0, "init OK" );
    lib_erreur( 0, 0, "  " );
    
    /* Boucle de traitement de detection */
    /*---------------------------------*/
    lib_erreur( 0, 0, "Boucle de traitement : det_trt_loop()" );
    lib_sleep( 5000 );
    lib_erreur( 0, 0, "debut trt_loop");
    det_trt_loop();

} /* end main() */




/*XYZ*********************************************************************
/ NOM        : int det_trt_loop()
/
/ IN         : Neant
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/  boucle sur les arrivees de messages et lance les traitements associes
/   
*********************************************************************ZYX*/
void
det_trt_loop()
{
	sigset_t     old_mask, bid_mask;
	
	lib_erreur(0,0,"Avant det_trt !");
    while(1)
    {
		old_mask = lib_mask_masque_signal( SIGUSR1 );
		bid_mask = lib_mask_masque_signal( SIGUSR2 );
		bid_mask = lib_mask_masque_signal( SIGIO );
		bid_mask = lib_mask_masque_signal( SIGALRM );
		
        det_trt();
		
		lib_mask_demasque_signal( old_mask );	            
    } /* fin while(1) */

} /* end det_trt_loop() */

/*XYZ*********************************************************************
/ NOM        : int det_trt()
/
/ IN         : Neant
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/  boucle sur les arrivees de messages et lance les traitements associes
/   
*********************************************************************ZYX*/
void
det_trt()
{
	char buffer[512];
	strcpy(buffer,"");
	
	/*On regarde si il y a une detection*/
	if(!lib_serial_readln(buffer, 512))
		det_quit();
		
	if(buffer!="")
	{
		det_analysedetec(buffer);
	}
		
}/* end det_trt() */
int det_analysedetec(char* buffer)
{
	/*On decode le message*/
	int message,codedetecteur,data,i;
	int indextrouve=-1;
	
	if(!strncmp(buffer,"message recu",11))
	{
		lib_erreur(0,0,"sscanf");
		sscanf(buffer, "message recu: m=%d, id=%d, data=%d",&message,&codedetecteur,&data);
		sprintf(G_msgerr,"message recu: m=%d, id=%d, data=%d",message,codedetecteur,data);
		lib_erreur(0,0,G_msgerr);
	}
	else
	{
		sprintf(G_msgerr,"Arduino ecrit: %s",buffer);
		lib_erreur(0,0,G_msgerr);
		return 0;
	}
	
	/*on cherche le detecteur dont le code de detection correspond au message*/
	for(i=0;i<G_Detecteurs.Nbdetect;i++)
	{
		if(G_Detecteurs.Liste_det[i].codedetec==codedetecteur)
		{
			sprintf(G_msgerr,"index trouve=%d",i);
			lib_erreur(0,0,G_msgerr);
			indextrouve=i;
			break;
		}
	}
	/*si il existe, on declenche les actions de detection*/
	if(indextrouve<0)
	{
		sprintf(G_msgerr, "Le detecteur ayant le code %d n'a pas ete trouve",codedetecteur);
		lib_erreur(0,0,G_msgerr);
		return 0;
	
	}
	else
	{
		det_detection(&(G_Detecteurs.Liste_det[indextrouve]));
	}
	return 1;
	
}
int det_detection(T_Detecteur* detecteur)
{
	char sqlcmde[256];
	char cheminphoto[256];
	char cmd[512];	
	char msg[256];
	char nomfickarotz[256];
	char mailnotif[256];
	int i;
	lib_erreur(0,0,"DEB det_detection");	
	/*Sauvegarde de la detection en base*/
	if(G_Adm.camera || G_Adm.karotz)
		sprintf(detecteur->nomficimage,"%d_%04d.jpg",detecteur->iddetecteur, detecteur->numimage);
	else
		strcpy(detecteur->nomficimage,"");
	
	sprintf(sqlcmde, "INSERT INTO yana_detection(ID_DETECTEUR, IMAGE, DATE) VALUES(%d,'%s',datetime('now'));", detecteur->iddetecteur, detecteur->nomficimage);
	det_sqlite_executerequete(G_nomdb, sqlcmde);
	
	/*On prend une photo*/
	if(G_Adm.camera)
	{
		sprintf(cheminphoto,"%s/%s", DIR_IMAGE,detecteur->nomficimage );
		sprintf(cmd, "raspistill -o \"%s\" > /home/pi/Projet/log/cmd.log",  cheminphoto);
		lib_erreur(0,0,cmd);
		system(cmd);
	}
	if(G_Adm.karotz)
	{
		sprintf(nomfickarotz,"karotz_%d_%04d",detecteur->iddetecteur, detecteur->numimage);
		lib_kartoz_photo(DIR_IMAGE,nomfickarotz);
	}
	detecteur->numimage++;
	
	lib_sleep_sec( 7, 0 );
	
	for(i=0;i<G_Contacts.Nbcontact;i++)
	{
		if(G_Contacts.Liste_contact[i].notifymail)
		{
			
			sprintf(mailnotif,"Mail envoye sur %s",G_Contacts.Liste_contact[i].adremail);
			/*On envoi le mail*/
			lib_erreur(0,0,"Une détection a eu lieu ! ==> envoi d'un mail d'alerte à raspilolio");
			
			if(G_Adm.camera && test_fic(cheminphoto))
			{	
				sprintf(cmd, "echo \"ceci est un mail d'alerte !\" | mail -s \"alerte\" -a \"%s\" %s", cheminphoto, G_Contacts.Liste_contact[i].adremail );
				lib_erreur(0,0,cmd);
				system(cmd);
			}
			sprintf(cheminphoto, "%s/%s.jpg",DIR_IMAGE,nomfickarotz);
			
			if(G_Adm.karotz && test_fic(cheminphoto))
				sprintf(cmd, "echo \"ceci est un mail d'alerte !\" | mail -s \"alerte_karotz\" -a \"%s\" %s", cheminphoto, G_Contacts.Liste_contact[i].adremail );
			else
				sprintf(cmd, "echo \"ceci est un mail d'alerte !\" | mail -s \"alerte_karotz\" %s", G_Contacts.Liste_contact[i].adremail );
			lib_erreur(0,0,cmd);
			system(cmd);
		}
		else
			strcpy(mailnotif,"");
		if(G_Contacts.Liste_contact[i].notifysms)
		{
			/*On envoie un sms*/
			sprintf(cmd, "sudo gammu-smsd-inject TEXT %s -text 'une detection a eu lieu : detec=%d salle=%s. %s' > /home/pi/Projet/log/sms.log 2>&1 &",
			G_Contacts.Liste_contact[i].tel,detecteur->iddetecteur,
			detecteur->piece,mailnotif );
			lib_erreur(0,0,cmd);
			system(cmd);
		}
	}
	/*On notifie karotz*/
	if(G_Adm.karotz)
	{
		sprintf(msg,"Alerte detection %s",detecteur->piece);
		lib_kartoz_speak(msg);
	}

	lib_erreur(0,0,"FIN det_detection");	
}

int test_fic(char* cheminfic)
{
	if(!access(cheminfic, F_OK))
		return 0 ;
	else
		return 1;
	
	
}





