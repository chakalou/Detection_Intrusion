/*ABC***********************************************************************
*
* NOM         : lib_sock_s_machine.c
*
* PROJET      : PSCL
*
* PROCESS     : 
*
* TYPE        : Source C
*
* ECRIT PAR   : D. DELEFORTERIE              Le 22/05/2012
*
* MODIFS      :
*
* DESCRIPTION : 
*    Fichier inclus directement dans lib_sock_s.c
*
***********************************************************************CBA*/



/*XYZ*********************************************************************
/ NOM         : lib_sock_s_max_machine()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      :
/    nbre de  machines possible
/ DESCRIPTION :
/ ********************************************************************ZYX*/
int lib_sock_s_max_machine()
{
    return(NB_MAX_MACHINES);
}

/*XYZ*********************************************************************
/ NOM         : lib_sock_s_is_machine_libre()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      :
/    nbre de  machines possible
/ DESCRIPTION :
/ ********************************************************************ZYX*/
int lib_sock_s_is_machine_libre( P_ind )
int P_ind;
{
LT_machine_autor    *p_machine;

    p_machine = (LT_machine_autor*) &( L_machine[P_ind] );
    return( p_machine->place_libre );
}

/*XYZ*********************************************************************
/ NOM         : lib_sock_s_get_inet_machine()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      :
/    nbre de  machines possible
/ DESCRIPTION :
/ ********************************************************************ZYX*/
char *lib_sock_s_get_inet_machine( P_ind )
int P_ind;
{
LT_machine_autor    *p_machine;
static char         str_inet[32];

    p_machine = (LT_machine_autor*) &( L_machine[P_ind] );
    strcpy( str_inet, p_machine->str_inet );
    return( str_inet );
}



/*XYZ***************************************************************************
/ NOM         : machine_nb_max_client()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      :
/    nbre de clients autorises sur la machine P_str_inet
/    0 si la machine n'est pas connue
/ DESCRIPTION :
/ *************************************************************************ZYX*/
static int machine_nb_max_client( P_str_inet )
char* P_str_inet;
{
int                 i;
LT_machine_autor    *p_machine;

    for( i=0; i<NB_MAX_MACHINES; i++ )
    {
        p_machine = (LT_machine_autor*) &( L_machine[i] );

        if( p_machine->place_libre == 1 )
        {
            continue;
        }

        if( strcmp( p_machine->str_inet, P_str_inet ) == 0 )
        {
            return( p_machine->nb_max_client );
        }

    } /* end for i */
    
    return( 0 );
    
} /* end machine_nb_max_client() */

/*XYZ***************************************************************************
/ NOM         : machine_flag_swap()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      :
/    1 si la machine a besoin de swap
/    0 sinon
/ DESCRIPTION :
/ *************************************************************************ZYX*/
static int machine_flag_swap( P_str_inet )
char* P_str_inet;
{
int                  i;
LT_machine_autor    *p_machine;

    for( i=0; i<NB_MAX_MACHINES; i++ )
    {
        p_machine = (LT_machine_autor*) &( L_machine[i] );

        if( p_machine->place_libre == 1 )
        {
            continue;
        }

        if( strcmp( p_machine->str_inet, P_str_inet ) == 0 )
        {
            return( p_machine->flag_swap );
        }

    } /* end for i */
    
    return( 0 );
    
} /* end machine_flag_swap() */

/*XYZ***************************************************************************
/ NOM         : machine_origine()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      :
/    entre 0 et 32 selon l'origine de la machine
/    -1 sinon
/ DESCRIPTION :
/ *************************************************************************ZYX*/
static int
machine_origine( P_str_inet )
char* P_str_inet;
{
int                    i;
LT_machine_autor    *p_machine;

    for( i=0; i<NB_MAX_MACHINES; i++ )
    {
        p_machine = (LT_machine_autor*) &( L_machine[i] );

        if( p_machine->place_libre == 1 )
        {
            continue;
        }

        if( strcmp( p_machine->str_inet, P_str_inet ) == 0 )
        {
            return( p_machine->origine );
        }

    } /* end for i */
    
    return( -1 );
    
} /* end machine_origine() */

/*XYZ***************************************************************************
/ NOM         : machine_lit_fichier()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/ *************************************************************************ZYX*/
static void machine_lit_fichier()
{
int                 i;
int                 j;
int                 ok;
int                 ret;
int                 nb_max_client;
FILE                *fp;
char                ligne[256];
char                nom_fichier[256];
int                 nb_machines;
LT_machine_autor    machine[NB_MAX_MACHINES];
LT_machine_autor    *p_machine;


    nb_machines = 0;
    for( i=0; i<NB_MAX_MACHINES; i++ )
    {
        machine_init( (LT_machine_autor*) &( machine[i] ) );
    }

    /* Ouverture en Lecture du fichier  */
    /* -------------------------------- */
    sprintf( nom_fichier,
                "%s/%s/sock_autor",
                    (char*) getenv( "DIR_PARAMETRAGE" ),
                    (char*) lib_erreur_nom_process() );

    fp = fopen( nom_fichier, "r" );
    if( fp == NULL )
    {
        sprintf( G_msgerr, "Err fopen(%s) => return", nom_fichier );
        lib_erreur( 0, 0, G_msgerr );
        return;
    }

    nb_machines = 0;
    nb_max_client = 0;

    /* lecture des lignes du fichier               */
    /* ------------------------------------------- */
    while( 1 )
    {
        if( fgets( ligne, 256, fp ) == NULL )
        {
            /* Plus rien a lire */
            /* ---------------- */
            break;
        }

        if( ligne[0] == '/' )
        {
            /* ligne de commentaires, on passe */
            /* ------------------------------- */
            continue;
        }

        /* suppression de \n */
        ligne[strlen(ligne)-1] = '\0';

        if( strcmp( ligne, "" ) == 0 )
        {
            /* ligne vide, on passe */
            /* -------------------- */
            continue;
        }
        
        if( nb_machines == NB_MAX_MACHINES )
        {
            sprintf( G_msgerr, "Err NB_MAX_MACHINES (%s) => break", ligne );
            lib_erreur( 0, 0, G_msgerr );
            break;
        }

        p_machine = (LT_machine_autor*) &( machine[nb_machines] );

        ret = sscanf( ligne,
                        "%s %d %d %d",
                            p_machine->str_inet,
                            &(p_machine->nb_max_client),
                            &(p_machine->flag_swap),
                            &(p_machine->origine) );
        if( ret != 4 )
        {
            sprintf( G_msgerr, "Err sscanf(%s) = %d", ligne, ret );
            lib_erreur( 0, 0, G_msgerr );
            continue;
        }

        if( p_machine->nb_max_client <= 0 )
        {
            sprintf( G_msgerr, "Err nb_max_client (%s) => continue", ligne );
            lib_erreur( 0, 0, G_msgerr );
            continue;
        }

        if( p_machine->flag_swap != 0
            &&
            p_machine->flag_swap != 1 )
        {
            sprintf( G_msgerr, "Err flag_swap (%s) => continue", ligne );
            lib_erreur( 0, 0, G_msgerr );
            continue;
        }

        if( p_machine->origine < 0
            ||
            p_machine->origine > 32 )
        {
            sprintf( G_msgerr, "Err origine (%s) => continue", ligne );
            lib_erreur( 0, 0, G_msgerr );
            continue;
        }

        nb_max_client += p_machine->nb_max_client;

        if( nb_max_client > SOCK_NB_MAX_CLIENTS )
        {
            sprintf( G_msgerr, "Err SOCK_NB_MAX_CLIENTS (%s) => break", ligne );
            lib_erreur( 0, 0, G_msgerr );
            break;
        }
        
        /* on verifie qu'il n'y a pas de doublons */
        /* -------------------------------------- */
        ok = 1;
        for( i=0; i<nb_machines; i++ )
        {
            if( strcmp( machine[i].str_inet, p_machine->str_inet ) == 0 )
            {
                /* machine existe deja */
                ok = 0;
                break;
            }
        }

        if( ok == 0 )
        {
            sprintf( G_msgerr, "Err doublon (%s) => continue", ligne );
            lib_erreur( 0, 0, G_msgerr );
            continue;
        }

        nb_machines++;

    } /* end while( 1 ) */

    fclose( fp );

    sprintf( G_msgerr, "'%s'", nom_fichier );
    lib_erreur( 0, 0, G_msgerr );
    
    lib_erreur( 0, 0, " fichier 'sock_autor' contient :" );

    for( i=0; i<nb_machines; i++ )
    {
        sprintf( G_msgerr,
                    "\t '%s' %d clients max (swap=%d) (ori=%d)",
                    machine[i].str_inet,
                    machine[i].nb_max_client,
                    machine[i].flag_swap,
                    machine[i].origine );
        lib_erreur( 0, 0, G_msgerr );
    }

    lib_erreur( 0, 0, "avt trt L_machine[] contient :" );
    for( i=0; i<NB_MAX_MACHINES; i++ )
    {
        sprintf( G_msgerr,
                    "\t %d (%d) '%s' %d clients max (swap=%d) (ori=%d)",
                    i,
                    L_machine[i].place_libre,
                    L_machine[i].str_inet,
                    L_machine[i].nb_max_client,
                    L_machine[i].flag_swap,
                    L_machine[i].origine );
        lib_erreur( 0, 0, G_msgerr );
    }

    /* ------------------------------------------------ */
    /* on supprime dans L_machine[] toutes les machines */
    /* - qui n'existent plus dans le fichier.           */
    /* - qui ont change de parametres nb_max_client.    */
    /* - qui ont change de parametres flag_swap.        */
    /* ------------------------------------------------ */
    {
        LT_machine_autor *p_m1, *p_m2;

        for( i=0; i<NB_MAX_MACHINES; i++ )
        {
            p_m1 = (LT_machine_autor*) &( L_machine[i] );

            if( p_m1->place_libre == 1 )
            {
                continue;
            }

            ok = 0;
            for( j=0; j<nb_machines; j++ )
            {
                p_m2 = (LT_machine_autor*) &( machine[j] );

                if( (strcmp( p_m1->str_inet, p_m2->str_inet ) == 0) &&
                    (p_m1->nb_max_client == p_m2->nb_max_client) &&
                    (p_m1->flag_swap == p_m2->flag_swap) &&
                    (p_m1->origine == p_m2->origine) )
                {
                    /* L_machine[i] existe toujours ds le fichier */
                    /* ------------------------------------------ */
                    ok = 1;
                    
                    /* on utilise p_m2->place_libre pour marquer  */
                    /* les machines du fichier qui sont deja dans */
                    /* le tableau L_machine[]                     */
                    /* ------------------------------------------ */
                    p_m2->place_libre = 0;
                    break;
                }
            }
            if( ok == 0 )
            {
                p_m1->place_libre = 1;
                
                sprintf( G_msgerr,
                            "Close sur tous les Clients de '%s'",
                                p_m1->str_inet );
                lib_erreur( 0, 0, G_msgerr );

                for( j=0; j<SOCK_NB_MAX_CLIENTS; j++ )
                {
                    if( L_sock_client[j].connectee == 0 )
                    {
                        continue;
                    }
                    if( strcmp( L_sock_client[j].str_inet,
                                        p_m1->str_inet ) != 0 )
                    {
                        continue;
                    }
                    lib_sock_s_close_socket_client( j );
                }
                
                machine_init( p_m1 );   /* pour que Didier comprenne */
                                        /* les traces ... */
            }

        } /* end for i */
    }

    /* ------------------------------------------------ */
    /* on ajoute dans L_machine[] toutes les machines   */
    /* - qui existent dans le fichier.                  */
    /* ------------------------------------------------ */
    {
        LT_machine_autor *p_m1, *p_m2;

        for( i=0; i<nb_machines; i++ )
        {
            p_m2 = (LT_machine_autor*) &( machine[i] );

            if( p_m2->place_libre == 0 )
            {
                /* machine[i] existe deja ds L_machine[] */
                /* => rien a faire                       */
                /* ------------------------------------- */
                continue;
            }
            
            /* ajout de machine[i] dans L_machine[] */
            /* ------------------------------------ */
            for( j=0; j<NB_MAX_MACHINES; j++ )
            {
                p_m1 = (LT_machine_autor*) &( L_machine[j] );

                if( p_m1->place_libre == 0 )
                {
                    continue;
                }
                
                memcpy(    (char*) p_m1,
                        (char*) p_m2,
                        sizeof(LT_machine_autor) );
                
                p_m1->place_libre = 0;
                break;
            }

        } /* end for i */
    }

    lib_erreur( 0, 0, "apr trt L_machine[] contient :" );
    for( i=0; i<NB_MAX_MACHINES; i++ )
    {
        sprintf( G_msgerr,
                    "\t %d (%d) '%s' %d clients max (swap=%d) (ori=%d)",
                    i,
                    L_machine[i].place_libre,
                    L_machine[i].str_inet,
                    L_machine[i].nb_max_client,
                    L_machine[i].flag_swap,
                    L_machine[i].origine );
        lib_erreur( 0, 0, G_msgerr );
    }

} /* end machine_lit_fichier() */

/*XYZ***************************************************************************
/ NOM         : machine_init()
/
/ IN          : Neant
/ OUT         : Neant
/ RETOUR      : Neant
/ DESCRIPTION :
/ *************************************************************************ZYX*/
static void machine_init( P_p_machine )
LT_machine_autor* P_p_machine;
{
    P_p_machine->place_libre = 1;
    strcpy( P_p_machine->str_inet, "" );
    P_p_machine->nb_max_client = 0;
    P_p_machine->flag_swap = 0;
    P_p_machine->origine = 0;
    
} /* end machine_init() */
