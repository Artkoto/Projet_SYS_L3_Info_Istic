#include <listclient.h>

                    /* ##################################################
                    #            Les variables globales              #
                    ##################################################
                    */
////sockfd
int sockfd , sd , fdMax = 0;

//tableau de clients pour la connexiont simultanée de plusieur clients
struct  client listClient [CLIENT_MAX];

////audio dir
char audioPath[BUFFER_SIZE];

//msgBuffer
char msgBuffer[BUFFER_SIZE];
ssize_t  readAudio ;

//////// pour gerer les recvfrom
socklen_t recvfd;

//descripteur de fichier audio 
int lecture_audio ;

//les addresses des clients et serveur
struct  sockaddr_in addrserveur , addrClient  ;

//timeout
fd_set read_set;
struct timeval timeout;

//les packets envoyes et reçu
struct packet packEnvoye ;
struct packet packRecu ;

/// MSG ERRERU
int err_connexion;

                        /* ##################################################
                        #           Les methodes de l'application        #
                        ##################################################
                        */

///////////////////////// creation , suppression et copie de packets////////////////
static void create_packet(struct packet* pack, int type, void* content){
    pack->type = type ;
    memcpy(pack->message, content , BUFFER_SIZE);
}

static void clear_packet(struct packet* pack){
    pack->type = VIDE ;
    pack->id_client = CLIENT_MAX ;
    bzero(pack->message, BUFFER_SIZE);
}

static void copy_packet(struct packet* pack2 , struct packet* pack1){
    clear_packet(pack2);
    pack2->id_client = pack1->id_client;
    pack2->type = pack1->type;
    memcpy(pack2->message , pack1->message  , BUFFER_SIZE );
}


///////////initialiser une sconnexion  avec un client ////////////////
static int init_connexion(int id_client ){
    struct  sockaddr_in socket2 ;
   int  newsocket = socket(AF_INET , SOCK_DGRAM , 0);
    if (newsocket == SOCKET_ERROR )
    {
        perror("socket()");
        err_connexion = MSG_ERREUR_SOCKET ; 
        return -1;
    } 
    socket2.sin_family         = AF_INET ;
    socket2.sin_addr.s_addr    = htonl (INADDR_ANY);

    //attribuer un port à la scoket 
    int  err_bind = bind(newsocket, (struct sockaddr *)&socket2 , sizeof(socket2) );
            if (err_bind < 0 )
            {
                err_connexion = MSG_ERREUR_PORT; 
                return -1;
            }

    // ajouter le client dans le tableau de client
    listClient[id_client].s_fd = newsocket ;
    //mise a jour du descripteur de socket max
    fdMax = (newsocket > fdMax) ? newsocket : fdMax ; 
    clear_packet(&listClient[id_client].data);

return newsocket;

}

///////////fermer  une sconnexion ////////////////
static void fin_connexion(int rmfd){
    close(rmfd) ; // fermeture de la socket du client 
}

/////////// envoyer un packet ///////////////////
static void envoye_packet(int sock, struct  sockaddr_in *addrclient, struct packet *buffer){
    int bufferSize = sizeof(struct packet);
    socklen_t flen ;
    flen = sizeof(struct sockaddr_in);
   if(sendto(sock, buffer, bufferSize, 0, (struct sockaddr*) addrclient, flen) < 0)
   {
      perror("sendto()");
   }
}

////////////    lire un packet  //////////////////////////
static socklen_t lire_packet(int sock, struct  sockaddr_in *addrclient, struct packet *buffer){
   int bufferSize = sizeof(struct packet);
   socklen_t len , flen ;
   len = 0 ;
    flen = sizeof(struct sockaddr_in);
   if((len = recvfrom(sock, buffer, bufferSize , 0, (struct sockaddr*) addrclient , &flen)) < 0)
   {
      perror("recvfrom()");
   }

   return len;
}

/////////////   ajouter client ///////////////////////
static bool add_client( struct packet* pack ){

    for (int i = 0; i < CLIENT_MAX; i++)
    {
        if (listClient[i].est_disponible) // s'il reste encore de l'espace dans le serveur 
        {
            pack->id_client = i ; //attribuer un id au client 

            if (init_connexion( i) < 0)
            {  
                return false ;
            }
            listClient[i].est_disponible =false ;
            listClient[i].address = addrClient;
            listClient[i].data.id_client = i ;
             
            return true ;
        }
    }
    err_connexion = MSG_ERREUR_SERVEUR_COMPLET ;
    return false;
} 


/////////////   supprimer client ///////////////
    //toute information concernant le client est reinitialisée
    //et l'espace est de nouveau libre
static void remove_client(int id_client ){
    int fdtemp = listClient[id_client].s_fd;
    fin_connexion(listClient[id_client].s_fd);
    listClient[id_client].s_fd = 0;
    close(listClient[id_client].audio_fd);
    listClient[id_client].audio_fd =  0 ;
    listClient[id_client].delait_attente = 0;
    listClient[id_client].delait_renvoie_de_trame = 0 ;
    listClient[id_client].est_disponible = true ;
    listClient[id_client].frequenceEchantillonnage  =  0 ;
    listClient[id_client].en_lecture = false ;
    bzero(listClient[id_client].titre_son , BUFFER_SIZE);
     //mise a jour du desccripteur de socket Max
    fdMax = (fdtemp == fdMax) ? calcul_fdMax (sockfd , fdMax) : fdMax ;
}


////// construction au path du fichier audio
static void audio_path(const char *filename ,  char *audioPath){
        strcpy( audioPath,AUDIO_DIR);
            strcat(audioPath , filename);
}


///calcule du desccripteur de socket Max
static int calcul_fdMax(int sfd , int fdmax){
    fdmax = sfd;
    for (int i = 0; i < CLIENT_MAX; i++)
    {
        if(listClient[i].s_fd > fdmax){
            fdmax = listClient[i].s_fd ;
        }
    } 
    return fdmax ;
}

//FD_SET///
static void fdSet(){
    FD_SET(sockfd , &read_set);
    for (int i = 0; i < CLIENT_MAX; i++)
    {   
        int sd = listClient[i].s_fd ;
        if ( sd > 0)
        {
             FD_SET(sd , &read_set);
        }
        
    }
    

}

///debut du programme ///////////////

static void init(){

    sockfd = socket(AF_INET , SOCK_DGRAM , 0);
    //
    if (sockfd == SOCKET_ERROR )
    {
        exit(errno);
    }
    addrserveur.sin_family         = AF_INET ;
    addrserveur.sin_port           = htons (PORT);
    addrserveur.sin_addr.s_addr    = htonl (INADDR_ANY);

    int  err_bind = bind(sockfd, (struct sockaddr *)&addrserveur , sizeof(addrserveur) );
            if (err_bind == -1 )
            {
                puts("non_bind");
            }
    fdMax = sockfd ;

    //initialisation de tous les espaces client
    for (int i = 0; i < CLIENT_MAX; i++)
    {
        listClient[i].s_fd = 0 ;
        listClient[i].audio_fd = 0 ;
        listClient[i].delait_attente = 0;
        listClient[i].delait_renvoie_de_trame = 0 ;
        listClient[i].est_disponible = true ;
        listClient[i].en_lecture = false ;
        bzero(listClient[i].titre_son , BUFFER_SIZE);

    }

    clear_packet(&packEnvoye);
    clear_packet(&packRecu);
    
}

/// corp du prog////////////////
static void app(){
    while (true)
    {
        FD_ZERO(&read_set);
        fdSet() ;
        timeout.tv_sec = 0 ;
        timeout.tv_usec = TIMEOUT;


        usleep(3000);// pour gerer les pertes individuellement
        int activity = select(fdMax+1 , &read_set , NULL, NULL, &timeout);
        if (activity < 0)
        {
            perror("select()"); 
        }

        if (activity == 0) //timeout 
        {
                for (int i = 0; i < CLIENT_MAX; i++) // le serveur renvoie les packets aux clients en cours de lecture
            {
                sd = listClient[i].s_fd ;
                if( sd > 0 )
                {
                    listClient[i].delait_attente ++ ;
                    listClient[i].delait_renvoie_de_trame = 0 ;
                    envoye_packet(sd , &listClient[i].address, &listClient[i].data);
                }
            }
        }

                     /* #########################################################
                        #  Premiere connexion du client (socket principale)    #
                        #########################################################
                    */
        if (FD_ISSET(sockfd,&read_set)) 
        {
            sd = sockfd ;
           recvfd = lire_packet(sd , &addrClient , &packRecu);
           if (recvfd <= 0)
           {
               perror("recvfrom()");
           }

            // verification du type de packet (doit etre de type : nom de fichier)
            if (packRecu.type != FILENAME)
            {
                create_packet(&packEnvoye , MSG_ERREUR_CO_NON_AUTORISE, AUCUN_MSG);
            }
            else ///le bon cas 
            {
                struct headerSon headerMusic ; 
                audio_path(packRecu.message, audioPath );
                lecture_audio = aud_readinit(audioPath ,
                 &headerMusic.frequenceEchantillonnage,
                 &headerMusic.tailleEchantillonnage,
                 &headerMusic.canal);
                 // tous les son du serveur etants compatible, la seule erreur est pour :"fichier introuvable"
                if (lecture_audio < 0) 
                {
                    /* son indisponible */
                    create_packet(&packEnvoye , MSG_ERREUR_SON_INDISPONIBLE, AUCUN_MSG );
                }else
                {
                    if (add_client(&packEnvoye))
                    {
                        // fprintf(stdout,
                        //             " Fichier audio : %s \n Frequence echantillonnage : %d \n Taille echantillonnage : %d \n Canal : %d \n", 
                        //             packRecu.message ,
                        //             headerMusic.frequenceEchantillonnage,
                        //             headerMusic.tailleEchantillonnage,
                        //             headerMusic.canal);

                        sd = listClient[packEnvoye.id_client].s_fd;
                        listClient[packEnvoye.id_client].audio_fd = lecture_audio ; //sauvegarde du descripteur audio
                        //ecriture de l'entete du fichier audio dans le buffeur
                        snprintf(msgBuffer, sizeof(msgBuffer),
                          "%d %d %d",
                          headerMusic.frequenceEchantillonnage,
                          headerMusic.tailleEchantillonnage, 
                          headerMusic.canal);

                        create_packet(&packEnvoye , FILE_HEADER, msgBuffer);
                        //sauvegarde du titre du son (juste pour afficher la trace d'activite du serveur)
                        strncpy(listClient[packEnvoye.id_client].titre_son, packRecu.message, strlen(packRecu.message) -4 );

                    }else
                    {
                        //le serveur n'a pas pu ajouter le client 
                        create_packet(&packEnvoye , err_connexion, AUCUN_MSG);
                        close(lecture_audio);
                    }
                }
            }
            envoye_packet(sd , &addrClient, &packEnvoye);
            clear_packet(&packEnvoye);
            clear_packet(&packRecu);
            bzero(msgBuffer , sizeof(msgBuffer));

        }

                     /* #########################################################
                        #    le cas ou le client est deja connecté au serveur   #
                        #########################################################
                    */

        for (int i = 0; i < CLIENT_MAX; i++)  // Un client envoie un acquittement
        {
            //recuperation des donnees permettant de communiquer avec le client
            sd = listClient[i].s_fd;
            addrClient = listClient[i].address;
            lecture_audio = listClient[i].audio_fd ;

            if (listClient[i].en_lecture)
            {   
                //si le client est en lecture, on recupere le packet sauvegarde
                // pour le renvoyer en cas d'inactivite du client
                copy_packet(&packEnvoye , &listClient[i].data);
            }
            

            if(sd > 0 )
            {
                if (FD_ISSET(sd , &read_set))
                {   
                    //reinitialisation des delais d'attente (deconnexion automatique) et de renvoie de trame 
                    listClient[i].delait_renvoie_de_trame = 0 ; 
                    listClient[i].delait_attente= 0 ;

                    clear_packet(&listClient[i].data);
                    recvfd = lire_packet(sd , &listClient[i].address , &packRecu);
                    if (recvfd <= 0)
                    {
                        perror("recvfrom()"); 
                    }
                     
                    //Verification du type de packet recu 
                    switch (packRecu.type)
                    {
                        case HEADER_MUSIC_RECU: // le client signale la reception de l'entete du son 
                            bzero(msgBuffer, BUFFER_SIZE);
                        if(read (lecture_audio ,msgBuffer ,BUFFER_SIZE) < 0){
                                create_packet(&packEnvoye , MSG_ERREUR_LECTURE_AUDIO, AUCUN_MSG);
                        }
                            else {
                                create_packet(&packEnvoye , ECHANTILLON, msgBuffer);
                                copy_packet(&listClient[i].data , &packEnvoye);
                                listClient[i].en_lecture = true;
                                puts("-------------------------------------"  );
                                printf("+ Le client (%d) joue le son : %s.\n" , i+1,
                                listClient[i].titre_son );
                                puts("-------------------------------------"  );
                            }
                            break;

                        case ACQUITTEMENT: // Acquittement du client
                            bzero(msgBuffer, sizeof(msgBuffer));
                            readAudio = read (lecture_audio ,msgBuffer ,BUFFER_SIZE);
                        if(readAudio < 0){
                                create_packet(&packEnvoye , MSG_ERREUR_LECTURE_AUDIO, AUCUN_MSG);
                        }
                        else
                        {
                            if(readAudio != BUFFER_SIZE){
                                create_packet(&packEnvoye , FIN_DU_SON, AUCUN_MSG); //fin du son
                                listClient[i].en_lecture = false;
                            }
                            else
                            { 
                                create_packet(&packEnvoye , ECHANTILLON, msgBuffer);
                                copy_packet(&listClient[i].data , &packEnvoye);
                            }
                        }
                        
                            break;
                        
                        default:  //autres cas
                            create_packet(&packEnvoye , MSG_ERREUR_CO_NON_AUTORISE, AUCUN_MSG);
                            break;
                    }
                    envoye_packet(sd , &addrClient, &packEnvoye);
                    if (packEnvoye.type == FIN_DU_SON )
                    {
                        remove_client(i);
                        printf("- Le client (%d)  a fini sa lecture.\n" , i+1 );
                        puts("-------------------------------------"  );
                    }else if (packEnvoye.type == MSG_ERREUR_LECTURE_AUDIO)
                    {
                        remove_client(i);
                        printf("~ Le client (%d) : Interruption inattendue du son.\n" , i+1 );
                        puts("-------------------------------------"  );
                    }
                    
                    
                    bzero(msgBuffer , BUFFER_SIZE);
                    clear_packet(&packEnvoye);
                    clear_packet(&packRecu);
                    
                } else
                {
                    listClient[i].delait_renvoie_de_trame ++ ;
                    if ( listClient[i].delait_renvoie_de_trame >= 1000)
                    {
                        envoye_packet(sd , &addrClient, &packEnvoye);
                        listClient[i].delait_attente ++;
                        clear_packet(&packEnvoye);
                        clear_packet(&packRecu);
                    }
                }
                if (listClient[i].delait_attente >= 12)
                    {
                        remove_client(i);
                        printf("# Le client (%d) s'est déconnecte !\n" , i+1 );
                        puts("-------------------------------------"  );
                    }
            }
            
        }
        
    }

}

// fin du programme /////
static void fin(){

    close(sockfd) ;

}

int main(int argc, char const *argv[])
{
    puts("AKOTO&KONEStream");
    init();
    app();
    fin();
    return 0;
}

