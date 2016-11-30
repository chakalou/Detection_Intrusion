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

void	lib_erreur_init(char *P_nom_process, char *P_dir_erreur);
void	lib_erreur(int P_ecran, int P_perror, char *P_message);
void	lib_erreur_pid_in_fichier();
int	lib_erreur_pid_out_fichier(char  *P_nom);
char	*lib_erreur_nom_process();

#endif
