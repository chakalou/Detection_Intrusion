/*ABC***********************************************************************
*                                
* NOM        : lib_date.c
*
* PROJET     : Detection
*
* PROCESS    :
*
* TYPE       : Code Librairie
*
* ECRIT PAR  : LOLIO      Le: 22/08/2012
*
* MODIFS     : 
*
* DESCRIPTION:
*    Contient des procedures donnant l'heure et la date systeme
*
***********************************************************************CBA*/
/* includes systeme */
/* ---------------- */
#include <sys/types.h>
#include <time.h>
#include <stdio.h>

/* pour connaitre les types Date Jour et Heure */
/* ------------------------------------------- */
#include "lib_date.h"

/* pour connaitre lib_erreur() */
/* --------------------------- */
#include <lib_erreur.h>

/* variables locales au fichier */
/* ---------------------------- */
static int L_mtbl[] = { 0, 31, 59, 90, 120, 151,
                        181, 212, 243, 273, 304, 334, 365 };

/* fonctions locales au fichier */
/* ---------------------------- */
static int  sextil();


/*XYZ*********************************************************************
/ NOM        : int lib_date_gps()
/ 
/ IN         : Neant
/ OUT        : Date *P_p_d = pointeur sur une structure Date 
/ INOUT      : Neant
/
/ RETOUR     :
/     0 si OK, -1 en cas d'erreur.
/
/ DESCRIPTION:
/     Renvoie le jour, mois, annee, heure ,min, sec cale sur GPS
*********************************************************************ZYX*/
int
lib_date_gps(Date *P_p_d )
{
          /*********************************************/
          /*  en attendant de posseder l equipement    */
          /*  on continue de retourner l heure machine */
          /*********************************************/

time_t        timeofday;
struct tm    *Stm;

    if( (timeofday = (time_t) time(0)) != -1 )
    {
        if( ( Stm = localtime( &timeofday )) != NULL )
        {
            P_p_d->jour  = Stm->tm_mday;
            P_p_d->mois = Stm->tm_mon+1;
            P_p_d->an = Stm->tm_year+1900;
            P_p_d->heure = Stm->tm_hour;
            P_p_d->minute = Stm->tm_min;
            P_p_d->seconde = Stm->tm_sec;
            return(0);
        }
    } 

    P_p_d->jour = 0;
    P_p_d->mois = 0;
    P_p_d->an = 0;
    P_p_d->heure = 0;
    P_p_d->minute = 0;
    P_p_d->seconde = 0;
    lib_erreur( 0, 0, "Err lib_date_gps()" );
    return(-1);

} /* end lib_date_gps() */


/*XYZ*********************************************************************
/ NOM        : int lib_date_jour_heure()
/ 
/ IN         : Neant
/ OUT        : Date *P_p_d = pointeur sur une structure Date 
/ INOUT      : Neant
/
/ RETOUR     :
/     0 si OK, -1 en cas d'erreur.
/
/ DESCRIPTION:
/     Renvoie le jour, mois, annee, heure ,min, sec de la machine
*********************************************************************ZYX*/
int
lib_date_jour_heure(Date *P_p_d)
{
    time_t        timeofday;
    struct tm    *Stm;

    if( (timeofday = (time_t) time(0)) != -1 )
    {
            if( ( Stm = localtime( &timeofday )) != NULL )
            {
                    P_p_d->jour  = Stm->tm_mday;
                    P_p_d->mois = Stm->tm_mon+1;
                    P_p_d->an = Stm->tm_year+1900;
                    /* Stm->tm_year = nbre d'annees depuis 1900 */
                    /*------------------------------------------*/
                    P_p_d->heure = Stm->tm_hour;
                    P_p_d->minute = Stm->tm_min;
                    P_p_d->seconde = Stm->tm_sec;
                    return(0);
            }
    } /* fin du if */

    P_p_d->jour = 0;
    P_p_d->mois = 0;
    P_p_d->an = 0;
    P_p_d->heure = 0;
    P_p_d->minute = 0;
    P_p_d->seconde = 0;

    lib_erreur( 0, 0, "Err lib_date_jour_heure()" );
    return(-1);

} /* end lib_date_jour_heure() */

/*XYZ*********************************************************************
/ NOM        : int lib_date_heure()
/ 
/ IN         : Neant
/ OUT        : Heure *P_p_h = pointeur sur une structure Date 
/ INOUT      : Neant
/
/ RETOUR     :
/     0 si OK, -1 en cas d'erreur.
/
/ DESCRIPTION:
/     Renvoie heure, minute et seconde de la machine
*********************************************************************ZYX*/
int
lib_date_heure(Heure *P_p_h)
{
    long timeofday;
    struct tm *Stm;
    
    if ( (timeofday = (long) time(0)) != -1 )
    {
             if ( (Stm = localtime(&timeofday)) != NULL)
             {
                    P_p_h->heure = Stm->tm_hour;
                    P_p_h->minute = Stm->tm_min;
                    P_p_h->seconde = Stm->tm_sec;
                    return(0);
            }
    } /* fin du if */

    P_p_h->heure = 0;
    P_p_h->minute = 0;
    P_p_h->seconde = 0;

    lib_erreur( 0, 0, "Err lib_date_heure()" );
    return(-1);

} /* end lib_date_heure() */


/*XYZ*********************************************************************
/ NOM        : int lib_date_jour()
/ 
/ IN         : Neant
/ OUT        : Jour *P_p_j = pointeur sur une structure Date 
/ INOUT      : Neant
/
/ RETOUR     :
/     0 si OK, -1 en cas d'erreur.
/
/ DESCRIPTION:
/     renvoie jour, mois, annee de la machine
*********************************************************************ZYX*/
int
lib_date_jour(Jour *P_p_j)
{
    long        timeofday;
    struct tm    *Stm;

    if ( (timeofday = (long) time(0)) != -1)
    {
            if ( (Stm = localtime( &timeofday )) != NULL)
            {
                    P_p_j->jour = Stm->tm_mday;
                    P_p_j->mois = Stm->tm_mon+1;
                    P_p_j->an = Stm->tm_year+1900;
                    return(0);
            }
    } /* fin du if */
 
    P_p_j->jour = 0;
    P_p_j->mois = 0;
    P_p_j->an = 0;

    lib_erreur( 0, 0, "Err lib_date_jour()" );
    return(-1);

} /* end lib_date_jour() */

/*XYZ*********************************************************************
/ NOM        : int lib_date_plus_1_seconde()
/
/ IN         : Neant
/ OUT        : Neant
/ INOUT      : Date *P_p_d = pointeur sur une structure Date 
/
/ RETOUR     :
/       0 si OK, -1 en cas d'erreur.
/
/ DESCRIPTION:
/      rajoute 1 seconde a '*P_p_d' . 
*********************************************************************ZYX*/
int
lib_date_plus_1_seconde(Date    *P_p_d)
{
#ifdef INUTILE
    long        timeofday;
    struct tm   *p_Stm;
    struct tm   Stm;

    /* calcul du nbre de secondes depuis 1970 */
    /* de la date passee en argument.         */
    /*----------------------------------------*/
    Stm.tm_mday     = P_p_d->jour;
    Stm.tm_mon      = P_p_d->mois - 1;
    Stm.tm_year     = P_p_d->an - 1900;
    Stm.tm_hour     = P_p_d->heure;
    Stm.tm_min      = P_p_d->minute;
    Stm.tm_sec      = P_p_d->seconde;

    timeofday = timelocal(&Stm);

    /* On rajoute 1 seconde ....             */
    /*---------------------------------------*/
    timeofday++;

    if ( (p_Stm = localtime( &timeofday )) != NULL) 
    {

        P_p_d->jour     = p_Stm->tm_mday;
        P_p_d->mois     = p_Stm->tm_mon+1;
        P_p_d->an       = p_Stm->tm_year+1900;
        P_p_d->heure    = p_Stm->tm_hour;
        P_p_d->minute   = p_Stm->tm_min;
        P_p_d->seconde  = p_Stm->tm_sec;
        return(0);
    }

    lib_erreur( 0, 0, "Err lib_date_plus_1_seconde()" );
#endif
    return(-1);

} /* end lib_date_plus_1_seconde() */

/*XYZ*********************************************************************
/ NOM        : int lib_date_nb_jours_ds_mois()
/
/ IN         : int P_an, P_mois
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     :
/    Calcule le nbre de jours ds le mois.
/ DESCRIPTION:
*********************************************************************ZYX*/
int
lib_date_nb_jours_ds_mois( P_an, P_mois )
int    P_an;
int    P_mois;
{
    int        nb_jours;
    
    nb_jours = L_mtbl[P_mois] - L_mtbl[P_mois-1];
    
    if( P_mois == 2 && P_an % 4 == 0 && 
        (P_an % 100 != 0 || P_an % 400 == 0 ) )
        nb_jours++;
    
    return( nb_jours );

} /* end lib_date_nb_jours_ds_mois() */

/*XYZ*********************************************************************
/ NOM        : int lib_date_num_jour_ds_semaine()
/
/ IN         : int P_an, P_mois, P_jour
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     :
/    Calcule le nbre de jours ds le mois.
/ DESCRIPTION:
*********************************************************************ZYX*/
int
lib_date_num_jour_ds_semaine( P_an, P_mois, P_jour )
int    P_an;
int    P_mois;
int    P_jour;
{
    long    days_ctr;
    
    P_an -= 1900;
    days_ctr = ((long)P_an * 365L) + ((P_an+3)/4 );
    days_ctr += L_mtbl[P_mois-1] + P_jour + 6;
    
    if( P_mois > 2 && (P_an % 4 == 0 ))
        days_ctr++;
        
    return( (int)(days_ctr % 7L) );

} /* end lib_date_num_jour_ds_semaine() */

/*XYZ*********************************************************************
/ NOM        : void lib_date_jour_suivant()
/
/ IN         : int P_an, P_mois, P_jour
/ OUT        : int *P_an_suiv, *P_mois_suiv, *P_jour_suiv
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/    Calcule le jour suivant.
*********************************************************************ZYX*/
void
lib_date_jour_suivant(    P_an, P_mois, P_jour,
                        P_an_suiv, P_mois_suiv, P_jour_suiv )
int    P_an;
int    P_mois;
int    P_jour;
int    *P_an_suiv;
int    *P_mois_suiv;
int    *P_jour_suiv;
{
    if( P_jour == lib_date_dernier_jour_du_mois( P_mois, P_an ) )
    {
        *P_jour_suiv = 1;
        *P_mois_suiv = (P_mois == 12) ? 1 : P_mois + 1;
        *P_an_suiv = (*P_mois_suiv == 1) ? P_an + 1 : P_an;
    }
    else
    {
        *P_jour_suiv = P_jour + 1;
        *P_mois_suiv = P_mois;
        *P_an_suiv = P_an;
    }

} /* end lib_date_jour_suivant() */


/*XYZ*********************************************************************
/ NOM        : void lib_date_jour_semaine()
/
/ IN         : int P_jour, P_mois, P_an
/ OUT        : 
/ INOUT      : Neant
/ RETOUR     : jour de la semaine 0 dimanche, 1 lundi ...
/              -1 si erreur
/ DESCRIPTION:
/    Calcule le jour de la semaine
*********************************************************************ZYX*/
int
lib_date_jour_semaine( P_jour, P_mois, P_an )
int    P_jour;
int    P_mois;
int    P_an;
{
char        chaine[32];
struct tm   tm;
time_t      t;

    sprintf( chaine, "%d %d %d",  P_jour, P_mois, P_an );
    if (strptime( chaine, "%d %m %Y", &tm ) != NULL)
    {
        t = mktime(&tm);
        return( localtime(&t)->tm_wday );
    }
    else return( -1 );

} /* end lib_date_jour_semaine() */


/*XYZ*********************************************************************
/ NOM        : void lib_date_jour_suivant2()
/
/ OUT        : int *P_an, *P_mois, *P_jour
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/    Calcule le jour suivant.
*********************************************************************ZYX*/
void
lib_date_jour_suivant2(    P_an, P_mois, P_jour )
int    *P_an;
int    *P_mois;
int    *P_jour;
{
    if( *P_jour == lib_date_dernier_jour_du_mois( *P_mois, *P_an ) )
    {
        *P_jour = 1;
        *P_mois = (*P_mois == 12) ? 1 : *P_mois + 1;
        *P_an = (*P_mois == 1) ? *P_an + 1 : *P_an;
    }
    else
    {
        *P_jour = *P_jour + 1;
        *P_mois = *P_mois;
        *P_an = *P_an;
    }

} /* end lib_date_jour_suivant2() */

/*XYZ*********************************************************************
/ NOM        : void lib_date_jour_precedent()
/
/ IN         : int P_an, P_mois, P_jour
/ OUT        : int *P_an_suiv, *P_mois_suiv, *P_jour_suiv
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/    Calcule le jour precedent.
*********************************************************************ZYX*/
void
lib_date_jour_precedent(    P_an, P_mois, P_jour,
                            P_an_prec, P_mois_prec, P_jour_prec )
int    P_an;
int    P_mois;
int    P_jour;
int    *P_an_prec;
int    *P_mois_prec;
int    *P_jour_prec;
{
    if( P_jour > 1 )
    {
        *P_jour_prec = P_jour - 1;
        *P_mois_prec = P_mois;
        *P_an_prec = P_an;
    }
    else if( P_mois > 1 )
    {
        *P_mois_prec = P_mois - 1;
        *P_an_prec = P_an;
        *P_jour_prec = 
            lib_date_dernier_jour_du_mois( *P_mois_prec, *P_an_prec );
    }
    else
    {
        *P_jour_prec = 31;
        *P_mois_prec = 12;
        *P_an_prec = P_an - 1;
    }

} /* end lib_date_jour_precedent() */

/*XYZ*********************************************************************
/ NOM        : void lib_date_jour_precedent2()
/
/ OUT        : int *P_an, *P_mois, *P_jour
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/    Calcule le jour precedent.
*********************************************************************ZYX*/
void
lib_date_jour_precedent2( P_an, P_mois, P_jour )
int    *P_an;
int    *P_mois;
int    *P_jour;
{
    if( *P_jour > 1 )
    {
        *P_jour -= 1;
    }
    else if( *P_mois > 1 )
    {
        *P_mois -= 1;
        *P_jour = lib_date_dernier_jour_du_mois( *P_mois, *P_an );
    }
    else
    {
        *P_jour = 31;
        *P_mois = 12;
        *P_an -= 1;
    }

} /* end lib_date_jour_precedent2() */

/*XYZ*********************************************************************
/NOM         : int lib_date_dernier_jour_du_mois()
/
/ IN         : int P_mois, P_an
/ OUT        : Neant
/ INOUT      : Neant
/ RETOUR     :
/    Calcule le dernier jour du mois.
/ DESCRIPTION:
*********************************************************************ZYX*/
int
lib_date_dernier_jour_du_mois( P_mois, P_an )
int    P_mois;
int    P_an;
{
    int        lastjour;

    switch( P_mois )
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            lastjour = 31;
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            lastjour = 30;
            break;
        case 2:
            lastjour = (P_an % 4) ? 28 : 29;
            break;
    }
    return (lastjour);

} /* end lib_date_dernier_jour_du_mois() */

/*XYZ*********************************************************************
NOM        : int lib_date_plus_10m()

IN         : Neant
OUT        : Neant
INOUT      : int *P_heure
INOUT      : int *P_minute
RETOUR     :
    0    Si on ne passe pas au jour suivant.
    -1    Si on passe au jour suivant.
DESCRIPTION:
    Ajoute 10 minutes a l'heure passee en parametre.

*********************************************************************ZYX*/
int
lib_date_plus_10m( P_heure, P_minute )
int *P_heure;
int *P_minute;
{
    if (*P_minute < 50)
        *P_minute += 10;
    else
    {
        *P_minute -= 50;
        if (*P_heure < 23)
            *P_heure += 1;
        else
        {
            /* Heure >= 23 h 50 */
            /* ---------------- */
            *P_heure = 0;
            return( -1 );
        }
    }
    
    return (0);
    
} /* end lib_date_plus_10m() */


/*XYZ*********************************************************************
NOM        : int lib_date_moins_10m()

IN         : Neant
OUT        : Neant
INOUT      : int *P_heure
INOUT      : int *P_minute

RETOUR     :
    0    Si on ne passe pas au jour precedent.
    -1    Si on passe au jour precedent.
DESCRIPTION:
    Enleve 10 minutes a l'heure passee en parametre.

*********************************************************************ZYX*/
int
lib_date_moins_10m( P_heure, P_minute )
int *P_heure;
int *P_minute;
{
    if (*P_minute >= 10)
    {
        *P_minute -= 10;
    }
    else
    {
        *P_minute += 50;
        if (*P_heure > 0)
        {
            *P_heure -= 1;
        }
        else
        {
            *P_heure = 23;
            return( -1 );
        }
    }

    return ( 0 );
    
} /* end lib_date_moins_10m() */



/*XYZ*********************************************************************
NOM        : int lib_date_plus_1h()

IN         : Neant
OUT        : Neant
INOUT      : int *P_heure
RETOUR     :
    0    Si on ne passe pas au jour suivant.
    -1    Si on passe au jour suivant.
DESCRIPTION:
    Ajoute 1 heure a l'heure passee en parametre.
*********************************************************************ZYX*/
int
lib_date_plus_1h( P_heure )
int *P_heure;
{
    if( *P_heure < 23 )
        *P_heure += 1;
    else
    {
        *P_heure = 0;
        return( -1 );
    }
    
    return ( 0 );

} /* end lib_date_plus_1h() */


/*XYZ*********************************************************************
NOM        : int lib_date_moins_1h()

IN         : Neant
OUT        : Neant
INOUT      : int *P_heure
RETOUR     :
    0    Si on ne passe pas au jour precedent.
    -1    Si on passe au jour precedent.
DESCRIPTION:
    Enleve 1 heure a l'heure passee en parametre.

*********************************************************************ZYX*/
int
lib_date_moins_1h( P_heure )
int *P_heure;
{
    if (*P_heure > 0)
    {
        *P_heure -= 1;
    }
    else
    {
        *P_heure = 23;
        return( -1 );
    }

    return ( 0 );
    
} /* end lib_date_moins_1h() */


/*XYZ*********************************************************************
/ NOM        : int lib_date_compare()
/ 
/ IN         : Date P_date1, Pdate2 , les dates a comparer
/ OUT        : Neant
/ INOUT      : Neant
/
/ RETOUR     :
/     la difference P_date1 - Pdate2 en secondes 
/
/ DESCRIPTION:
/     convertit les dates en nombre de secondes depuis le 01/01/90
/       a 0 heure et calcule leur difference
*********************************************************************ZYX*/
int
lib_date_compare( P_date1, P_date2 )
Date P_date1;
Date P_date2;
{
    int    ret;

    ret = lib_date_nb_sec( P_date1 ) - lib_date_nb_sec( P_date2 );
    return( ret );

} /* end lib_date_compare() */


/*XYZ*********************************************************************
/ NOM        : static int sextil()
/ 
/ IN         : int P_annee = annee a tester
/ OUT        : Neant
/ INOUT      : Neant
/
/ RETOUR     :
/      1 si annee est bissextile 0 sinon
/
/ DESCRIPTION:
/     teste si une annee est bissextile
*********************************************************************ZYX*/
static int
sextil( P_annee )
int  P_annee;
{
  if ( (((P_annee % 4) == 0) && ((P_annee % 100) != 0)) ||
       ((P_annee % 400) == 0) )
    return(1);
  else
    return(0);
}

/*XYZ*********************************************************************
/ NOM        : int lib_date_nb_sec()
/ 
/ IN         : Date P_date = la date a convertir
/ OUT        : Neant
/ INOUT      : Neant
/
/ RETOUR     :
/     nombre de secondes ecoule 
/
/ DESCRIPTION:
/     calcule le nombre de secondes entre le 01/01/90 a 0 heure
/       pour la date passee en parametre
*********************************************************************ZYX*/
int
lib_date_nb_sec( P_date )
Date  P_date;
{
    static int    nbj[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int            i, nbrsec;

    nbrsec = P_date.seconde;
    nbrsec += P_date.minute * 60;
    nbrsec += P_date.heure * 3600;
    nbrsec += (P_date.jour - 1) * 86400;
    if( sextil(P_date.an) == 1 )
        nbj[1] = 29;
    else
        nbj[1] = 28;
    
    for( i=1; i<P_date.mois; i++ )
        nbrsec += nbj[i-1] * 86400;

    for( i=1990; i<P_date.an; i++ )
    {
        if( sextil(i) == 1 )
            nbrsec += 366 * 86400;
        else
            nbrsec += 365 * 86400;
    }

    return( nbrsec );

} /* end lib_date_nb_sec() */


/*XYZ*********************************************************************
/ NOM            : void lib_date_ajoute_periode()
/
/ IN              : Heure   P_periode = intervalle de temps a ajouter
/
/ OUT        : Neant
/
/ INOUT       : Date    P_date = date a manipuler
/ RETOUR      : Neant
/ 
/ DESCRIPTION    :
/    ajoute un intervalle de temps a une date
/ 
*********************************************************************ZYX*/
void
lib_date_ajoute_periode( P_date, P_periode ) 
Date     *P_date;
Heure    P_periode;
{
int tot_periode, tot_date;
int somme;

    tot_periode = 3600*P_periode.heure + 
                  60*P_periode.minute + P_periode.seconde;
    tot_date    = 3600*P_date->heure + 
                  60*P_date->minute + P_date->seconde;
    somme = tot_date + tot_periode;
    while(somme >= 86400)
    {
        somme = somme - 86400;
        lib_date_jour_suivant2( &(P_date->an), 
                                &(P_date->mois), &(P_date->jour) );
    }
    P_date->heure = somme / 3600;
    somme = somme % 3600;
    P_date->minute = somme / 60;
    P_date->seconde = somme % 60;
                 
} /* fin de lib_date_ajoute_periode */


/*XYZ*********************************************************************
/ NOM            : void lib_date_enleve_periode()
/
/ IN              : Heure   P_periode = intervalle de temps a ajouter
/
/ OUT        : Neant
/
/ INOUT       : Date    P_date = date a manipuler
/ RETOUR      : Neant
/ 
/ DESCRIPTION    :
/    enleve un intervalle de temps a une date
/ 
*********************************************************************ZYX*/
void
lib_date_enleve_periode( P_date, P_periode ) 
Date     *P_date;
Heure    P_periode;
{
int tot_periode, tot_date;
int somme;

    tot_periode = 3600*P_periode.heure + 
                  60*P_periode.minute + P_periode.seconde;
    tot_date    = 3600*P_date->heure + 
                  60*P_date->minute + P_date->seconde;
    somme = tot_date - tot_periode;
    while(somme < 0)
    {
        somme = somme + 86400;
        lib_date_jour_precedent2( &(P_date->an), 
                                &(P_date->mois), &(P_date->jour) );
    }
    P_date->heure = somme / 3600;
    somme = somme % 3600;
    P_date->minute = somme / 60;
    P_date->seconde = somme % 60;
                 
} /* fin de lib_date_enleve_periode */


/*XYZ*********************************************************************
/ NOM        : lib_date_seconde()
/
/ OUT        : P_seconde
/ INOUT      : Neant
/ RETOUR     : Neant
/ DESCRIPTION:
/   Donne le temps en seconde et microseconde depuis 1970
*********************************************************************ZYX*/
int
lib_date_seconde( P_seconde )
Seconde *P_seconde;
{
struct timeval tp;

    if( gettimeofday( &tp, NULL ) != 0 )
    {
        lib_erreur( 0, 1, "Err gettimeofday()" );
        return( -1 );
    }
    P_seconde->seconde = tp.tv_sec;
    P_seconde->microseconde = tp.tv_usec;
    return( 0 );

} /* end lib_date_seconde() */


/*XYZ*********************************************************************
/ NOM        : lib_date_compare_minute()
/
/ RETOUR     :
/   Retourne la difference en minutes des deux dates
/ DESCRIPTION:
*********************************************************************ZYX*/
int
lib_date_compare_minute( P_seconde1, P_seconde2 )
Seconde *P_seconde1 ;
Seconde *P_seconde2 ;
{
    int microseconde;

    microseconde
        = (P_seconde1->seconde * 1000000 + P_seconde1->microseconde)
          -
          (P_seconde2->seconde * 1000000 + P_seconde2->microseconde);

    return( microseconde / ( 1000000 * 60 ) );

} /* end lib_date_compare_minute() */


/*XYZ*********************************************************************
/ NOM        : lib_date_compare_seconde()
/
/ RETOUR     :
/   Retourne la difference en seconde des deux dates
/ DESCRIPTION:
*********************************************************************ZYX*/
int
lib_date_compare_seconde( P_seconde1, P_seconde2 )
Seconde *P_seconde1;
Seconde *P_seconde2;
{
    int microseconde;

    microseconde
        = (P_seconde1->seconde * 1000000 + P_seconde1->microseconde)
          -
          (P_seconde2->seconde * 1000000 + P_seconde2->microseconde);

    return( microseconde / 1000000 );

} /* end lib_date_compare_seconde() */

/*XYZ*********************************************************************
/ NOM        : lib_date_compare_microseconde()
/
/ RETOUR     :
/   Retourne la difference en micro-seconde (10-6) des deux dates
/ DESCRIPTION:
*********************************************************************ZYX*/
int
lib_date_compare_microseconde( P_seconde1, P_seconde2 )
Seconde *P_seconde1;
Seconde *P_seconde2;
{
    int microseconde;

    microseconde
        = (P_seconde1->seconde * 1000000 + P_seconde1->microseconde)
          -
          (P_seconde2->seconde * 1000000 + P_seconde2->microseconde);
    
    return( microseconde );

} /* end lib_date_compare_microseconde() */

/*XYZ*********************************************************************
/ NOM        : lib_date_compare_milliseconde()
/
/ RETOUR     :
/   Retourne la difference en milli-seconde (10-3) des deux dates
/ DESCRIPTION:
*********************************************************************ZYX*/
int
lib_date_compare_milliseconde( P_seconde1, P_seconde2 )
Seconde *P_seconde1;
Seconde *P_seconde2;
{
    int    microseconde;

    microseconde
        =    (P_seconde1->seconde * 1000000 + P_seconde1->microseconde)
            -
            (P_seconde2->seconde * 1000000 + P_seconde2->microseconde);
    
    return( microseconde / 1000 );

} /* end lib_date_compare_milliseconde() */
