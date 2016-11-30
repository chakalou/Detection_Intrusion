/*ABC***********************************************************************
*                                
* NOM        : lib_erreur.c
*
* PROJET     : Detection
*
* PROCESS    :
*
* TYPE       : Code Librairie
*
* ECRIT PAR  : LOLIO    
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
#include <unistd.h>
#include <sys/stat.h>


#include <lib_date.h>

#include "lib_erreur.h"

/* variables locales a ce fichier */
/* ------------------------------ */
int     L_lib_erreur_init_ok = 0;
char    L_nom_process[81];
char    L_dir_erreur[512];


/*XYZ*********************************************************************
/ NOM        : lib_erreur_init()
/ 
/ IN         : P_nom_process de la forme "seqs.x"
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/ 
*********************************************************************ZYX*/
void
lib_erreur_init( char *P_nom_process, char *P_dir_erreur )
{

    if( strlen( P_nom_process ) >= 80 )
    {
        strncpy( L_nom_process, P_nom_process, 80 );
        L_nom_process[79] = '\0';
    }
    else strcpy( L_nom_process, P_nom_process );

    if( strlen( P_dir_erreur ) >= 512 )
    {
        strncpy( L_dir_erreur, P_dir_erreur, 512 );
        L_dir_erreur[511] = '\0';
    }
    else strcpy( L_dir_erreur, P_dir_erreur );

    /* le nom des taches est de la forme "nnnnnn.x" */
    /* Au 1er appel, on supprime l'extension ".x"   */
    /* pour diminuer la taille du nom du fichier.   */
    /*----------------------------------------------*/
    L_nom_process[strlen(L_nom_process)-2] = '\0';

    L_lib_erreur_init_ok = 1;

    sprintf( G_msgerr, "Fin lib_erreur_init( %s )", L_nom_process );
    lib_erreur( 0, 0, G_msgerr );

} /* end lib_erreur_init() */


/*XYZ*********************************************************************
/ NOM        : void lib_erreur()
/ 
/ IN         : char *P_message = message d'erreur a afficher
/ 
/              int P_ecran = 0 => message dans fichier
/                  P_ecran = 1 => message dans fichier et a l'ecran
/ 
/              int P_perror >= 0 => message d'erreur systeme (perror) ne
/                     sera pas mis dans le fichier des erreurs
/                     ni a l'ecran.
/                  P_perror >= 1 => message d'erreur systeme est rajoute.
/ 
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/              Sortie par exit(-1)
/
/ DESCRIPTION:
/     Ecrit tous les messages d'erreur dans un fichier ".erreur"
/     Un fichier sera cree tous les jours pour eviter des fichiers
/     trop importants. Le "cron" se chargera de les detruire
/     au fur et a mesure
/ 
*********************************************************************ZYX*/
void
lib_erreur( int P_ecran, int P_perror, char *P_message )
{
Date         date;
char         nom_repertoire[257];
int          mode, ret, errno_a_appel;
FILE         *f;
static int   first_time = 1;


    if( L_lib_erreur_init_ok == 0 )
    {
    	lib_erreur( 0, 0, "Err lib_erreur()" );
    	lib_erreur( 0, 0, "Avant lib_erreur_init() => exit en dur" );
    	exit( -1 );
    }
    
    /* on stocke la valeur de errno pour eviter son */
    /* ecrasement par tout probleme sur un appel    */
    /* systeme dans la procedure                    */
    /*----------------------------------------------*/
    errno_a_appel = errno;

    /* message affiche ds la fenetre du process */
    /*------------------------------------------*/
    if( P_ecran == 1 )
    {
        if( P_perror == 0 )
        {
            printf( "%s(%d): '%s'\n", L_nom_process, getpid(), P_message );
        }
        else if( P_perror == 1 )
        {
            printf( "%s(%d): '%s': '%s'\n",
                    L_nom_process, getpid(), P_message,
                    strerror(errno_a_appel) );
        }
    }

    /* recuperation de la date du systeme */
    /*------------------------------------*/
    lib_date_jour_heure( &date );

    /* constitution du nom du fichier */
    /*--------------------------------*/
    sprintf( nom_repertoire, "%s/%s_%i_%i%i%i_er",
             L_dir_erreur, L_nom_process, getpid(),
             date.jour, date.mois, date.an - 2000 );

    /* ecriture de l'erreur */
    /*----------------------*/
    if( (f = fopen(nom_repertoire,"r+")) == NULL )
    {
          if( (f = fopen(nom_repertoire,"w")) == NULL )
          {
            fclose( f );
            printf( "lib_erreur() fopen(%s) => exit\n", nom_repertoire );
            printf( "lib_erreur() Err a verifier ?!?\n" );
            printf( "lib_erreur() Err '%s'\n", strerror(errno) );
    	    exit( -1 );
        }
    }

    /* on se place en fin de fichier */
    /*-------------------------------*/
    if( fseek( f, 0, 2 ) == -1 )
    {
        fclose( f );
        printf( "lib_erreur() fseek() => exit\n" );
    	exit( -1 );
    }
    
    if( fprintf( f, "%2i:%2i:%2i: %s\n",
                 date.heure, date.minute, date.seconde, P_message ) == EOF )
    {
        fclose( f );
        printf( "lib_erreur() fprintf() => exit\n" );
    	exit( -1 );
    }
    
    /* recuperation du message systeme */
    /*---------------------------------*/
    if( P_perror == 1 )
    {
        fprintf( f, "\t%i : %s\n", errno_a_appel, strerror(errno_a_appel) );
    }

    fclose(f);
    
    mode = 0666;
    ret = chmod( nom_repertoire, mode );

} /* end lib_erreur() */


/*XYZ*********************************************************************
/ NOM        : void lib_erreur_pid_in_fichier()
/ 
/ IN         : Neant
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/              Sortie par exit(-1)
/
/ DESCRIPTION:
/     Ecrit le pid du process qui tourne dans un fichier dont le nom
/         est "nom_programme.pid". Ce fichier sera dans
/         DIR_FICHIER.
/ 
*********************************************************************ZYX*/
void
lib_erreur_pid_in_fichier()
{
char    nom_repertoire[257];
FILE     *f;

    /* constitution du nom du fichier */
    /*--------------------------------*/
    sprintf(nom_repertoire, "%s/%s.pid", getenv("DIR_FICHIER"), L_nom_process);
    
    /* ecriture du pid */
    /*-----------------*/
    if( (f = fopen( nom_repertoire, "w" )) == NULL )
    {
        if( (f = fopen( nom_repertoire, "w" )) == NULL )
        {
            fclose( f );
            lib_erreur( 1, 1, "lib_erreur_pid_in_fichier() fopen() => exit" );
            exit( -1 );
        }
    }

    if( fprintf( f, "%i", getpid() ) == EOF )
    {
        fclose( f );
        lib_erreur( 1, 1, "lib_erreur_pid_in_fichier() fprintf() => exit" );
        exit( -1 );
    }

    fclose( f );

} /* end lib_erreur_pid_in_fichier() */


/*XYZ*********************************************************************
/ NOM        : int lib_erreur_pid_out_fichier()
/ 
/ IN         : char *P_nom = nom du process dont on veut le pid.
/ OUT        : Neant
/ INOUT      : Neant
/ 
/ RETOUR     : pid du process.
/                 exit(-1) en cas de probleme disque.
/ 
/ DESCRIPTION:
/     Recupere le pid d'un process dans un fichier dont le nom
/         est "nom_programme.pid". Ce fichier sera dans
/         DIR_FICHIER.
/ 
*********************************************************************ZYX*/
int
lib_erreur_pid_out_fichier(char    *P_nom)
{
    char    nom_repertoire[257];
    FILE     *f;
    int        pid;

    /* constitution du nom du fichier */
    /* ------------------------------ */
    sprintf(nom_repertoire, "%s/%s.pid", getenv("DIR_FICHIER"), P_nom );

    /* lecture du pid */
    /* -------------- */
    if( (f = fopen( nom_repertoire, "r" )) == NULL )
    {
        if( (f = fopen( nom_repertoire, "w" )) == NULL )
        {
            fclose( f );
            lib_erreur( 1, 1, "lib_erreur_pid_out_fichier() fopen() => exit" );
            exit( -1 );
        }
    }

    if( fscanf( f, "%i", &pid ) != 1 )
    {
        /* impossible de recuperer le pid */
        fclose( f );
        lib_erreur( 1, 1, "lib_erreur_pid_out_fichier() fscanf() => exit" );
        exit( -1 );
    }

    fclose( f );
        
    return( pid );

} /* end lib_erreur_pid_out_fichier() */


/*XYZ*********************************************************************
/ NOM        : lib_erreur_nom_process()
/ 
/ IN         : Neant
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/ 
*********************************************************************ZYX*/
char
*lib_erreur_nom_process()
{
    return( L_nom_process );
    
} /* end lib_erreur_nom_process() */
