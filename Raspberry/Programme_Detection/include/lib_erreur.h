/*ABC***********************************************************************
*								
* NOM        : lib_erreur.h
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
*		Include de la librairie lib_erreur.
*
***********************************************************************CBA*/
#ifndef LIB_ERREUR
#define LIB_ERREUR

#define	LIB_ERR_LNG_MAX		8192

char	G_msgerr[LIB_ERR_LNG_MAX];

void	lib_erreur_init();
void	lib_erreur();
void	lib_erreur_pid_in_fichier();
int	lib_erreur_pid_out_fichier();
char	*lib_erreur_nom_process();

#endif
