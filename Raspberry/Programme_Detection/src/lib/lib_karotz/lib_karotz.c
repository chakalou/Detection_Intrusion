/*ABC***********************************************************************
*                                
* NOM        : lib_karotz.c
*
* PROJET     : DETECTION
*
* PROCESS    :
*
* TYPE       : Code Librairie
*
* ECRIT PAR  : LOL    Le: 23/10/2015
*
* MODIFS     : 
*
* DESCRIPTION:
*    Procedure de stockage et affichage des messages d'erreurs.
*
***********************************************************************CBA*/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <lib_date.h>

#include "lib_erreur.h"


/*XYZ*********************************************************************
/ NOM        : lib_kartoz_speak()
/ 
/ IN         : p_dialog= texte a dire
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/ 
*********************************************************************ZYX*/
int lib_kartoz_speak(char* p_dialog)
{
  CURL *curl;
  CURLcode res;
  char data[1024];
  FILE * fp;
  int i;
  char copy[1024];
  
  copy[0]='\0';
  
  int j=0;
  for(i=0;i<strlen(p_dialog);i++)
  {
	  if(p_dialog[i]==' ')
	  {
		  copy[j]='%';
		  j++;
		  copy[j]='2';
		  j++;
		  copy[j]='0';
	  }
	  else
		  copy[j]=p_dialog[i];
	  j++;
	  
  }
  copy[j]='\0';
  
  /* In windows, this will init the winsock stuff */ 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* get a curl handle */ 
  curl = curl_easy_init();
  if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */ 
	
	sprintf(data,"http://karotz/cgi-bin/tts?voice=alice&text=\"%s\"&nocache=0",copy);
    curl_easy_setopt(curl, CURLOPT_URL,data);
	fp = fopen("/home/pi/Projet/erreur/phrase", "w"); //Création de notre fichier
	curl_easy_setopt(curl,  CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curl,  CURLOPT_WRITEFUNCTION, fwrite);
    /* Now specify the POST data */ 
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDS,data);
 
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
	{
		  sprintf(G_msgerr, "curl_easy_perform() failed: %s\n",
				  curl_easy_strerror(res));
			lib_erreur(0,0,G_msgerr);
			curl_easy_cleanup(curl);
			curl_global_cleanup();
		   fclose(fp);
		  return 1;
	}
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);
	fclose(fp);
  }
  
  
  curl_global_cleanup();
  return 0;
}
int lib_kartoz_set_led(int p_pulse,char* p_color1,int p_speed, char * p_color2)
{
  FILE * fp;	
  CURL *curl;
  CURLcode res;
  char data[1024];
 
  /* In windows, this will init the winsock stuff */ 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* get a curl handle */ 
  curl = curl_easy_init();
  if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */ 
   sprintf(data,"http://karotz/cgi-bin/leds?pulse=%d&color=%s&speed=%d&color2=%s",p_pulse,p_color1,p_speed,p_color2);
    curl_easy_setopt(curl, CURLOPT_URL, data);
	fp = fopen("/home/pi/Projet/erreur/led", "w"); //Création de notre fichier
	curl_easy_setopt(curl,  CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curl,  CURLOPT_WRITEFUNCTION, fwrite);
    /* Now specify the POST data */ 
	
	/*curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) strlen(data));
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,data);*/
 
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
	{
		  sprintf(G_msgerr, "curl_easy_perform() failed: %s\n",
				  curl_easy_strerror(res));
		  lib_erreur(0,0,G_msgerr);
		  curl_easy_cleanup(curl);
			curl_global_cleanup();
			 fclose(fp);
		  return 1;
	}
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);
	 fclose(fp);
  }
  curl_global_cleanup();
  return 0;
}

int lib_kartoz_photo(char* P_dir, char* P_nomfic)
{
  FILE * fp;	
  CURL *curl;
  CURLcode res;
  char data[2048];
 
  /* In windows, this will init the winsock stuff */ 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* get a curl handle */ 
  curl = curl_easy_init();
  if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */ 
   sprintf(data,"http://karotz/cgi-bin/snapshot_ftp?server=%s&user=%s&password=%s&remote_dir=%s&silent=1&nomfic=%s","192.168.1.100","pi","j3d1-j3d1",P_dir,P_nomfic);
    curl_easy_setopt(curl, CURLOPT_URL, data);
	fp = fopen("/home/pi/Projet/erreur/fic", "w"); //Création de notre fichier
	curl_easy_setopt(curl,  CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curl,  CURLOPT_WRITEFUNCTION, fwrite);
    /* Now specify the POST data */ 
	
	/*curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) strlen(data));
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,data);*/
 
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK)
	{
		  sprintf(G_msgerr, "curl_easy_perform() failed: %s\n",
				  curl_easy_strerror(res));
		  lib_erreur(0,0,G_msgerr);
		  curl_easy_cleanup(curl);
			curl_global_cleanup();
			 fclose(fp);
		  return 1;
	}
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);
	 fclose(fp);
  }
  curl_global_cleanup();
  return 0;
}




