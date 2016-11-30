/*ABC***********************************************************************
*								
* NOM        : lib_sleep.h
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
*	Librairie soporifique...
* 
***********************************************************************CBA*/
#ifndef LIB_SLEEP
#define LIB_SLEEP

/* le temps passe en parametre est donne en micro-secondes. */
/* le parametre est un integer.                             */
/*----------------------------------------------------------*/
extern void		lib_sleep( );

extern void		lib_sleep_sec( );    
extern void		lib_sleep_cent_sec( );
extern void		lib_sleep_milli_sec( );

#endif
