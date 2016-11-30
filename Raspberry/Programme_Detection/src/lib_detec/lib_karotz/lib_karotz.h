/*ABC***********************************************************************
*								
* NOM        : lib_erreur.h
*
* PROJET     : PSCL
*
* PROCESS    :
*
* TYPE       : Include Librairie
*
* ECRIT PAR  : D. DELEFORTERIE		Le: 22/05/2012
*
* MODIFS     :
*
* DESCRIPTION: 
*		Include de la librairie lib_erreur.
*
***********************************************************************CBA*/
#ifndef LIB_KAROTZ
#define LIB_KAROTZ

int lib_kartoz_speak(char* p_dialog);
int lib_kartoz_set_led(int p_pulse, char* p_color1, int p_speed, char * p_color2);
int lib_kartoz_photo(char* P_dir, char* P_nomfic);
int lib_kartoz_delete_cache();

#endif
