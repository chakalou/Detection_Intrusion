/*ABC***********************************************************************
*
* NOM         : lib_sock_c_timer.c
*
* PROJET      : PSCL
*
* PROCESS     : 
*
* TYPE        : Source C
*
* ECRIT PAR   : D. DELEFORTERIE           Le 18/05/2012
*
* MODIFS      :
*
* DESCRIPTION : 
*    Fichier inclus directement dans lib_sock_c.c
*
***********************************************************************CBA*/

/*XYZ*********************************************************************
/ NOM         : timer_init_et_lancer()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/ ********************************************************************ZYX*/
static void
timer_init_et_lancer()
{
int ret;

    if ( L_num_timer_connect >= 0 )
    {
        ret = lib_timer_init(   L_num_timer_connect,
                                LIB_TIMER_MONO, LIB_TIMER_REPETITION,
                                timer_connect );
        if( ret == -1 )
        {
            sprintf( G_msgerr, "Err lib_timer_init(%d) => exit",
                     L_num_timer_connect );
            lib_erreur( 0, 0, G_msgerr );
            (*L_fct_quit)();
        }
        else
        {
            sprintf( G_msgerr, "lib_timer_init(%d) OK ds lib_sock_c",
                     L_num_timer_connect );
            lib_erreur( 0, 0, G_msgerr );
        }
    }

    if ( L_num_timer_service >= 0 )
    {
        ret = lib_timer_init(   L_num_timer_service,
                                LIB_TIMER_MONO, LIB_TIMER_REPETITION,
                                timer_service );
        if( ret == -1 )
        {
            sprintf( G_msgerr, "Err lib_timer_init(%d) => exit",
                     L_num_timer_service );
            lib_erreur( 0, 0, G_msgerr );
            (*L_fct_quit)();
        }
        else
        {
            sprintf( G_msgerr, "lib_timer_init(%d) OK ds lib_sock_c",
                     L_num_timer_service );
            lib_erreur( 0, 0, G_msgerr );
        }
    }

    if ( L_num_timer_connect >= 0 )
    {
        if( L_nb_sec_timer_connect > 0 )
        {
            lib_timer_lancer( L_num_timer_connect,
                              L_nb_sec_timer_connect, 0 );
            sprintf( G_msgerr, "lib_timer_lancer(%d,%d) OK ds lib_sock_c",
                     L_num_timer_connect, L_nb_sec_timer_connect );
            lib_erreur( 0, 0, G_msgerr );
        }
        else
        {
            sprintf( G_msgerr, "Err lib_timer_lancer(%d,%d) ds lib_sock_c",
                     L_num_timer_connect, L_nb_sec_timer_connect );
            lib_erreur( 0, 0, G_msgerr );
        }
    }

    if ( L_num_timer_service >= 0 )
    {
        if( L_nb_sec_timer_service > 0 )
        {
            lib_timer_lancer( L_num_timer_service,
                              L_nb_sec_timer_service, 0 );
            sprintf( G_msgerr, "lib_timer_lancer(%d,%d) OK ds lib_sock_c",
                     L_num_timer_service, L_nb_sec_timer_service );
            lib_erreur( 0, 0, G_msgerr );
        }
        else
        {
            sprintf( G_msgerr, "Err lib_timer_lancer(%d,%d) ds lib_sock_c",
                     L_num_timer_service, L_nb_sec_timer_service );
            lib_erreur( 0, 0, G_msgerr );
        }
    }

} /* end timer_init_et_lancer() */


/*XYZ***************************************************************************
/ NOM         : timer_connect()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/ *************************************************************************ZYX*/
static void
timer_connect( P_timer )
int P_timer;
{
int i;
    
    for( i=0; i<NB_MAX_SERVEURS; i++ )
    {          
        /* si socket non initialisee , on passe */
        if ( L_socket_client[i].init_ok == 0 )
        continue; 

        /* si socket deja connectee , on passe */
        if ( (L_socket_client[i].connect_en_cours == 1)
          || (L_socket_client[i].connectee == 1) )
          continue;

        connect_socket_client( i );
    } 
    
} /* end timer_connect() */


/*XYZ***************************************************************************
/ NOM         : timer_service()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/    Envoi une trame de Service sur toutes les sockets connectees
/ *************************************************************************ZYX*/
static void
timer_service( P_timer )
int P_timer;
{
int   i, ret;

    for ( i=0; i<NB_MAX_SERVEURS; i++ )
    {
        /* si socket non initialisee , on passe */
        if ( L_socket_client[i].init_ok == 0 ) continue;

        /* si socket non connectee , on passe */
        if ( L_socket_client[i].connectee == 0 ) continue;

        ret = lib_sock_c_write_socket_client( i, TT_SERVICE, TED_INCONNU,
                                              0, (char*) NULL );
        if ( ret == 0 ) lib_erreur( 0, 0, "Err Env TT_SERVICE" );
    } 
    
} /* end timer_service() */
