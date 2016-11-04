/*ABC***********************************************************************
*								
* NOM        : lib_mask.h
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
*	Declarations des fonctions contenues dans 'lib_mask.c'
*
***********************************************************************CBA*/
#ifndef LIB_MASK
#define LIB_MASK

#include <stdio.h>
#include <signal.h>

extern sigset_t	lib_mask_masque_signal(  );
extern sigset_t lib_mask_get_masque(  );
extern void	lib_mask_aff_masque(  );
extern void	lib_mask_demasque_signal(  );
extern int	lib_mask_rend_signal(  );

#endif
