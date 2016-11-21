/*ABC***********************************************************************
*
* NOM         : det_sqlite.c
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

/* Pour connaitre lib_erreur() */
/* --------------------------- */
#include <lib_erreur.h>

/*Fonction locale a ce fichier*/
/*----------------------------*/



int det_sqlite_select(void* data, char* nombase, char * sqlcmde, int (*callback)(void*,int,char**,char**))
{

 sqlite3 *db;
 char *zErrMsg = 0;
 int rc;
 char chemindb[256];

 /*Connection a la base*/
	sprintf(chemindb, "%s/%s", DIR_BDD, nombase);
	rc = sqlite3_open(chemindb, &db);

	if( rc )
	{
      sprintf(G_msgerr, "Echec de l'ouverture de la base de donnee: %s\n", 
	  sqlite3_errmsg(db));
      lib_erreur(0, 0, G_msgerr);
	  return 0;
	}
	  lib_erreur(0, 0, "Connecte a la base de donnee\n");


	/* Execute SQL statement */
	rc = sqlite3_exec(db, sqlcmde, callback, (void*) data, &zErrMsg);
	
	if( rc != SQLITE_OK )
	{
		sprintf(G_msgerr, "Erreur sql: %s\n", zErrMsg);
		lib_erreur(0, 0, G_msgerr);
		sqlite3_free(zErrMsg);
		/*deconnection de la base*/
		sqlite3_close(db);
		
		return 0;
	}
	else
	{
		/*deconnection de la base*/
		sqlite3_close(db);
		return 1;
	}	


}

/*XYZ*********************************************************************
/ NOM        : int det_sqlite_executerequete()
/
/ IN         : char * nombase, char* sqlcmde
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/  permet d'executer une requete insert, update, delete
/   
*********************************************************************ZYX*/
int det_sqlite_executerequete(char * nombase, char* sqlcmde)
{
 sqlite3 *db;
 char *zErrMsg = 0;
 int rc;
 char chemindb[256];

 /*Connection a la base*/
	sprintf(chemindb, "%s/%s", DIR_BDD, nombase);
	rc = sqlite3_open(chemindb, &db);

	if( rc )
	{
      sprintf(G_msgerr, "Echec de l'ouverture de la base de donnee: %s\n", 
	  sqlite3_errmsg(db));
      lib_erreur(0, 0, G_msgerr);
	  return 0;
	}
	  lib_erreur(0, 0, "Connecte a la base de donnee\n");


	/* Execute SQL statement */
	rc = sqlite3_exec(db, sqlcmde, callback, 0, &zErrMsg);
	
	if( rc != SQLITE_OK )
	{
		sprintf(G_msgerr, "Erreur sql: %s\n", zErrMsg);
		lib_erreur(0, 0, G_msgerr);
		sqlite3_free(zErrMsg);
		/*deconnection de la base*/
		sqlite3_close(db);
		
		return 0;
	}
	else
	{
		/*deconnection de la base*/
		sqlite3_close(db);
		return 1;
	}



}/*det_sqlite_executerequete()*/


/*XYZ*********************************************************************
/ NOM        : int det_sqlite_createtable()
/
/ IN         : char * nombase
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/  permet de creer la table qui contiendra les detections
/   
*********************************************************************ZYX*/
int det_sqlite_createtable(char * nombase)
{
	char sql[256];
	
 /* Creation de la table detection */
	 strcpy(sql, "DROP TABLE IF EXISTS yana_detection; " 
		 "CREATE TABLE IF NOT EXISTS yana_detection ( "
		 "ID  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
		 "ID_DETECTEUR INTEGER NOT NULL, "
		 "IMAGE TEXT, "
		 "DATE DATE NOT NULL);");
	lib_erreur(0, 0, sql);
	
	if(!det_sqlite_executerequete(nombase, sql))
	{
		lib_erreur(0, 0, "La table detection n'a pas ete cree correctement\n");
		return 0;
	}
	
	return 1;
}

int callback_detec(void *ListDetecteur, int argc, char **argv, char **azColName)
{
   int i;
   int detecencours;
   
   if(firstinit!=0)
   {
		(( T_Detecteur_List*)ListDetecteur)->Nbdetect=0;
		firstinit=1;
   
   }
   
   detecencours=(( T_Detecteur_List*)ListDetecteur)->Nbdetect;
   for(i=0; i<argc; i++)
   {
	  if(!strcmp(azColName[i],"id"))
		(( T_Detecteur_List*)ListDetecteur)->Liste_det[detecencours].iddetecteur=argv[i] ? atoi(argv[i]) : 0;
	  else if(!strcmp(azColName[i],"piece"))
		sprintf((( T_Detecteur_List*)ListDetecteur)->Liste_det[detecencours].piece,"%s",argv[i] ? argv[i] : "NULL");
	  else if(!strcmp(azColName[i],"codedetec"))
		(( T_Detecteur_List*)ListDetecteur)->Liste_det[detecencours].codedetec=argv[i] ? atoi(argv[i]) : 0;
	  /*else if(!strcmp(azColName[i],"id_image"))
	  {
		(( T_Detecteur_List*)ListDetecteur)->Liste_det[detecencours]->numimage=( argv[i] || atoi(argv[i])!=0) ? atoi(argv[i]) : 1;
		sprintf(G_msgerr, "numimage=%d", (( T_Detecteur_List*)ListDetecteur)->Liste_det[detecencours]->numimage);
		lib_erreur(0, 0, G_msgerr);
	  }*/
   }
   (( T_Detecteur_List*)ListDetecteur)->Nbdetect++;
   return 0;
}
int callback_contact(void *GListContact, int argc, char **argv, char **azColName)
{
   int i;
   int contactencours;
   
   if(firstinit!=0)
   {
		(( T_Contact_List*)GListContact)->Nbcontact=0;
		firstinit=1;
   
   }
   
   contactencours=(( T_Contact_List*)GListContact)->Nbcontact;
   for(i=0; i<argc; i++)
   {
	  if(!strcmp(azColName[i],"id"))
		(( T_Contact_List*)GListContact)->Liste_contact[contactencours].idcontact=argv[i] ? atoi(argv[i]) : 0;
	  else if(!strcmp(azColName[i],"mail"))
		sprintf((( T_Contact_List*)GListContact)->Liste_contact[contactencours].adremail,"%s",argv[i] ? argv[i] : "NULL");
	  else if(!strcmp(azColName[i],"tel"))
		sprintf((( T_Contact_List*)GListContact)->Liste_contact[contactencours].tel,"%s",argv[i] ? argv[i] : "NULL");
	  else if(!strcmp(azColName[i],"notifysms"))
		(( T_Contact_List*)GListContact)->Liste_contact[contactencours].notifysms=argv[i] ? atoi(argv[i]) : 0;
	  else if(!strcmp(azColName[i],"notifysms"))
		(( T_Contact_List*)GListContact)->Liste_contact[contactencours].notifymail=argv[i] ? atoi(argv[i]) : 0;
   }
   (( T_Contact_List*)GListContact)->Nbcontact++;
   return 0;
}

int callback(void *data, int argc, char **argv, char **azColName)
{
	return 0;
}



