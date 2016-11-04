/*ABC**********************************************************************
*
* NOM         : det_proto.h
*
* PROJET      : Detection
*
* PROCESS     :
*
* TYPE        : Include C
*
* ECRIT PAR   : LOLIO                   25/06/2013
*
* MODIFS      :
*
* DESCRIPTION :
*
***********************************************************************CBA*/
#ifndef _DET_PROTO_
#define _DET_PROTO_


/* procedures de det_quit.c */
/*-------------------------*/
extern void           det_quit();
extern void           det_quit_SIGINT();
extern void           det_quit_SIGBUS();
extern void           det_quit_SIGTERM();

/* procedures de dif_init.c */
/*-------------------------*/
extern void           det_init();
extern void			  det_charge_struct_detect();

/* procedures de det_timer.c */
/* ------------------------ */
extern void           det_timer_init();

/* procedures de det_signal.c */
/* -------------------------- */
extern void           det_signal_init();
extern void           det_signal_SIGIO();
extern	void		  det_quit_SIGINT();

/*procedure det_env.c*/
/*-------------------*/
extern char* 		  det_env_DIR_BDD();
extern char* 		  det_env_DIR_IMAGE();
#define DIR_BDD 	  det_env_DIR_BDD()
#define DIR_IMAGE 	  det_env_DIR_IMAGE()

/*procedure det_sqlite.c*/
/*-------------------*/
extern int det_sqlite_createtable(char * nombase);
extern int det_sqlite_select(void* data, char* nombase, char * sqlcmde, int (*callback)(void*,int,char**,char**));
extern int det_sqlite_executerequete(char * nombase, char* sqlcmde);

/*callback pour select*/
extern int callback_detec(void *data, int argc, char **argv, char **azColName);
extern int callback_image(void *data, int argc, char **argv, char **azColName);
extern int callback(void *data, int argc, char **argv, char **azColName);



#endif

