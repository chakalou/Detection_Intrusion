/*ABC***********************************************************************
*
* NOM         : det_env.c
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
*    - Fonctions relatives au process diffuse pour acces BD via OCI test
*
***********************************************************************CBA*/
#include "det_incl.h"
#include "det_const.h"
#include "det_type.h"
#include "det_var.h"
#include "det_proto.h"

/* pour connaitre lib_erreur() et G_msgerr */
/* --------------------------------------- */
#include <lib_erreur.h>

/* Variables locales a ce fichier */
/* ------------------------------ */

char     L_directory[256];



/*************************************************************************
 ***************************   LES DIRECTORIES    ************************
 *************************************************************************/



/*XYZ*********************************************************************
/ NOM        : int det_env_DIR_BDD()
/
*********************************************************************ZYX*/
char *det_env_DIR_BDD()
{
char *penv;

     if ((penv = getenv("DIR_BDD")) == NULL) 
         strcpy(L_directory, "/var/www/yana-server/db");
     else 
         strcpy(L_directory, penv);
         
     return(L_directory);
}
/*XYZ*********************************************************************
/ NOM        : int det_env_DIR_IMAGE()
/
*********************************************************************ZYX*/
char *det_env_DIR_IMAGE()
{
char *penv;

     if ((penv = getenv("DIR_IMAGE")) == NULL) 
         strcpy(L_directory, "/var/www/yana-server/plugins/Detection/img");
     else 
         strcpy(L_directory, penv);
     return(L_directory);
}

