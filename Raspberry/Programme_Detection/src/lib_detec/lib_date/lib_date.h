/*ABC***********************************************************************
*								
* NOM        : lib_date.h
*
* PROJET     : Detection
*
* PROCESS    :
*
* TYPE       : Include Librairie
*
* ECRIT PAR  : LOLIO			
*
* MODIFS     : 
*
* DESCRIPTION: 
*		Include de la librairie lib_date.
*
***********************************************************************CBA*/
#ifndef LIB_DATE
#define LIB_DATE

#include <date.h>

typedef struct
{
	int	jour;
	int	mois;
	int	an;

} Jour;

typedef struct
{
	int	heure;
	int	minute;
	int	seconde;

} Heure;

typedef struct
{
    int seconde;
    int microseconde;

} Seconde;

/* fonctions de lib_date.c */
/* ----------------------- */

int		lib_date_gps(Date *P_p_d);
int		lib_date_jour_heure(Date *P_p_d);
int		lib_date_heure(Heure *P_p_j);
int		lib_date_jour(Jour *P_p_j);

int		lib_date_plus_1_seconde(Date    *P_p_d);

int		lib_date_nb_jours_ds_mois();
int		lib_date_num_jour_ds_semaine();
int		lib_date_jour_semaine();
void	        lib_date_jour_suivant();
void	        lib_date_jour_suivant2();
void	        lib_date_jour_precedent();
void	        lib_date_jour_precedent2();
int		lib_date_dernier_jour_du_mois();

int		lib_date_plus_10m();
int		lib_date_moins_10m();
int		lib_date_plus_1h();
int		lib_date_moins_1h();

int		lib_date_compare();
void		lib_date_ajoute_periode();
void		lib_date_enleve_periode();
int		lib_date_nb_sec();

int		lib_date_seconde();
int		lib_date_compare_minute();
int		lib_date_compare_seconde();
int		lib_date_compare_microseconde();
int		lib_date_compare_milliseconde();

#endif
