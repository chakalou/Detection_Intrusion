/*ABC***********************************************************************
*
* NOM         : lib_sock_s_timer.c
*
* PROJET      : PSCL
*
* PROCESS     : 
*
* TYPE        : Source C
*
* ECRIT PAR   : D. DELEFORTERIE                Le 22/05/2012
*
* MODIFS      :
*
* DESCRIPTION : 
*    Fichier inclus directement dans lib_sock_s.c
*
***********************************************************************CBA*/

/*XYZ*************************************************************************
/ NOM         : timer_init_et_lancer()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/ ***********************************************************************ZYX*/
static void timer_init_et_lancer()
{
int ret;

    if( L_num_timer_accept >= 0 )
    {
        ret = lib_timer_init( (int) L_num_timer_accept,
                              (int) LIB_TIMER_MONO,
                              (int) LIB_TIMER_REPETITION,
                              timer_accept );
        if( ret == -1 )
        {
            sprintf( G_msgerr, "Err lib_timer_init(%d) => exit",
                     L_num_timer_accept );
            lib_erreur( 0, 0, G_msgerr );
            (*L_fct_quit)();
        }
        else
        {
            sprintf( G_msgerr, "lib_timer_init(%d) OK ds lib_sock_s",
                     L_num_timer_accept );
            lib_erreur( 0, 0, G_msgerr );
        }
    }

    if( L_num_timer_service >= 0 )
    {
        ret = lib_timer_init(   (int) L_num_timer_service,
                                (int) LIB_TIMER_MONO,
                                (int) LIB_TIMER_REPETITION,
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
            sprintf( G_msgerr, "lib_timer_init(%d) OK ds lib_sock_s",
                     L_num_timer_service );
            lib_erreur( 0, 0, G_msgerr );
        }
    }

    if( L_num_timer_accept >= 0 )
    {
        if( L_nb_sec_timer_accept > 0 )
        {
            sprintf( G_msgerr, "deb lib_timer_lancer(%d,%d) ds lib_sock_s",
                     L_num_timer_accept, L_nb_sec_timer_accept );
            lib_erreur( 0, 0, G_msgerr );
            
            lib_timer_lancer(   (int) L_num_timer_accept,
                                (int) L_nb_sec_timer_accept,
                                (int) 0 );
            
            sprintf( G_msgerr, "fin lib_timer_lancer(%d,%d) ds lib_sock_s",
                     L_num_timer_accept, L_nb_sec_timer_accept );
            lib_erreur( 0, 0, G_msgerr );
        }
        else
        {
            sprintf( G_msgerr, "Err lib_timer_lancer(%d,%d) ds lib_sock_s",
                     L_num_timer_accept, L_nb_sec_timer_accept );
            lib_erreur( 0, 0, G_msgerr );
        }
    }

    if ( L_num_timer_service >= 0 && L_nb_sec_timer_service > 0 )
    {
        if ( L_nb_sec_timer_service > 0 )
        {
            sprintf( G_msgerr, "deb lib_timer_lancer(%d,%d) ds lib_sock_s",
                     L_num_timer_service, L_nb_sec_timer_service );
            lib_erreur( 0, 0, G_msgerr );

            lib_timer_lancer( (int) L_num_timer_service,
                              (int) L_nb_sec_timer_service,
                              (int) 0 );
                              
            sprintf( G_msgerr, "fin lib_timer_lancer(%d,%d) ds lib_sock_s",
                     L_num_timer_service, L_nb_sec_timer_service );
            lib_erreur( 0, 0, G_msgerr );
        }
        else
        {
            sprintf( G_msgerr, "Err lib_timer_lancer(%d,%d) ds lib_sock_s",
                     L_num_timer_service, L_nb_sec_timer_service );
            lib_erreur( 0, 0, G_msgerr );
        }
    }

} /* end timer_init_et_lancer() */


/*XYZ***************************************************************************
/ NOM         : timer_accept()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/ *************************************************************************ZYX*/
static void
timer_accept( P_timer )
int P_timer;
{
    sock_s_accept();
} 


/*XYZ***************************************************************************
/ NOM         : timer_service()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/ *************************************************************************ZYX*/
static void
timer_service( P_timer )
int P_timer;
{
    /* si personne n'est connecte => cool */
    /* ---------------------------------- */
    if( lib_sock_s_nb_clients_connectes() == 0 ) return;
    
    lib_sock_s_write_socket_all_clients( (int) TT_SERVICE,
                                         (int) TED_INCONNU,
                                         (int) 0,
                                         (char*) NULL );
                                         
} /* end timer_service() */
