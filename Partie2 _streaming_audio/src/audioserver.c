//#include <serveur.h>

#include "../include/serveur.h"
#include "../include/listclient.h"

////////////////////// Variables Globales/////////////////////////////////////////////

int sockfd , fdMax = 0;

struct  client listClient [CLIENT_MAX];

////audio dir
char audioPath[BUFFER_SIZE];

//msgBuffer
char msgBuffer[BUFFER_SIZE];
ssize_t  readAudio ;

///utilisation temp des sockfd pour les fils
int sd ; 

//////// pour gerer les recvfrom
socklen_t recvfd;

//descripteur de fichier audio 

int lecture_audio ;

struct  sockaddr_in addrserveur , addrClient  ;

//timeout
fd_set read_set;
struct timeval timeout;

//les packets envoyes et reçu
struct packet packEnvoye ;
struct packet packRecu ;


/// MSG ERRERU
int err_connexion;

/////////////////////////////////////////////////////////////////////////////////////////


///////////////////////// creation et suppression de packets////////////////
static void create_packet(struct packet* pack, int type, void* content){
    pack->type = type ;
    memcpy(pack->message, content , BUFFER_SIZE);
}

static void clear_packet(struct packet* pack){
    pack->type = VIDE ;
    pack->id_client = CLIENT_MAX ;
    bzero(pack, sizeof (struct packet) );
}

static void copy_packet(struct packet* pack2 , struct packet* pack1){
    clear_packet(pack2);
    pack2->id_client = pack1->id_client;
    pack2->type = pack1->type;
    memcpy(pack2->message , pack1->message  , BUFFER_SIZE );
}


///////////initialiser une sconnexion ////////////////
static int init_connexion(int id_client ){
    struct  sockaddr_in socket2 ;
   int  newsocket = socket(AF_INET , SOCK_DGRAM , 0);
    //
    if (newsocket == SOCKET_ERROR )
    {
        perror("socket()");
        err_connexion = MSG_ERREUR_SOCKET ; 
        return -1;
    } 
    socket2.sin_family         = AF_INET ;
    socket2.sin_addr.s_addr    = htonl (INADDR_ANY);


    int  err_bind = bind(newsocket, (struct sockaddr *)&socket2 , sizeof(socket2) );
            //
            if (err_bind < 0 )
            {
                err_connexion = MSG_ERREUR_PORT; 
                return -1;
            }

    // ajouter le client dans le tableau
    listClient[id_client].s_fd = newsocket ;
    fdMax = (newsocket > fdMax) ? newsocket : fdMax ;
    // FD_SET(newsocket , &read_set);
    clear_packet(&listClient[id_client].data);

return newsocket;

}

///////////fermer  une sconnexion ////////////////
static void fin_connexion(int rmfd){
   // FD_CLR(rmfd, &read_set);
    close(rmfd) ;

}

/////////// envoyer un packet ///////////////////
static void envoye_packet(int sock, struct  sockaddr_in *addrclient, struct packet *buffer){
    int bufferSize = sizeof(struct packet);
    socklen_t flen ;
    flen = sizeof(struct sockaddr_in);
   if(sendto(sock, buffer, bufferSize, 0, (struct sockaddr*) addrclient, flen) < 0)
   {
      perror("sendto()");
      //exit(errno);
   }
}


////////////lire un packet//////////////////////////
static socklen_t lire_packet(int sock, struct  sockaddr_in *addrclient, struct packet *buffer){
 
   int bufferSize = sizeof(struct packet);
   socklen_t len , flen ;
   len = 0 ;
    flen = sizeof(struct sockaddr_in);

   if((len = recvfrom(sock, buffer, bufferSize , 0, (struct sockaddr*) addrclient , &flen)) < 0)
   {
      perror("recvfrom()");
     // exit(errno);
   }

   return len;
}

/////////////ajouter client ////////////////////////////
static bool add_client( struct packet* pack ){

    for (int i = 0; i < CLIENT_MAX; i++)
    {
        if (listClient[i].est_disponible)
        {
            pack->id_client = i ;

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


//////////////////////supprimer client ///////////////
static void remove_client(int id_client ){
    int fdtemp = listClient[id_client].s_fd;
    fin_connexion(listClient[id_client].s_fd);
    listClient[id_client].s_fd = 0;
    listClient[id_client].audio_fd =  0 ;
    listClient[id_client].delait_attente = 0;
    listClient[id_client].delait_renvoie_de_trame = 0 ;
    listClient[id_client].est_disponible = true ;
    listClient[id_client].frequenceEchantillonnage  =  0 ;
    listClient[id_client].en_lecture = false ;
    bzero(listClient[id_client].titre_son , BUFFER_SIZE);
    fdMax = (fdtemp == fdMax) ? calcul_fdMax (sockfd , fdMax) : fdMax ;
}


//////AUDIO PATH
static void audio_path(const char *filename ,  char *audioPath){
        strcpy( audioPath,AUDIO_DIR);
            strcat(audioPath , filename);
}


///calcule de fdMax
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
            //
            if (err_bind == -1 )
            {
                puts("non_bind");
            }
    fdMax = sockfd ;

//initialisation de tous les client disponibles
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


        usleep(5000);// pour gerer les pertes individuellement
        int activity = select(fdMax+1 , &read_set , NULL, NULL, &timeout);
        if (activity < 0)
        {
            perror("select()");
            //exit(errno);
        }

        if (activity == 0)
        {
            //time out 
            puts("timeout"); //TODO: a retirer
                for (int i = 0; i < CLIENT_MAX; i++) // un client deja co envoie un acquittement
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

        if (FD_ISSET(sockfd,&read_set)) //premiere connexion du client
        {
            sd = sockfd ;
            /* code */
                //***** recvfrome
           recvfd = lire_packet(sd , &addrClient , &packRecu);
           if (recvfd <= 0)
           {
               perror("recvfrom()");
           }

            //******* verification du type de packet nom de fichier
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
                if (lecture_audio < 0) // tous les son du serveur etants compatible, la seule erreur est pour fichier introuve
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
                        listClient[packEnvoye.id_client].audio_fd = lecture_audio ;
                        snprintf(msgBuffer, sizeof(msgBuffer),
                          "%d %d %d",
                          headerMusic.frequenceEchantillonnage,
                          headerMusic.tailleEchantillonnage, 
                          headerMusic.canal);

                        create_packet(&packEnvoye , FILE_HEADER, msgBuffer);
                        strncpy(listClient[packEnvoye.id_client].titre_son, packRecu.message, strlen(packRecu.message) -4 );

                    }else
                    {
                        create_packet(&packEnvoye , err_connexion, AUCUN_MSG);
                    }
                }
            }
            envoye_packet(sd , &addrClient, &packEnvoye);
            clear_packet(&packEnvoye);
            clear_packet(&packRecu);
            bzero(msgBuffer , sizeof(msgBuffer));

        }

        ////le cas ou le client est deja connecté au serveur

        for (int i = 0; i < CLIENT_MAX; i++) // un client deja co envoie un acquittement
        {
            sd = listClient[i].s_fd;
            addrClient = listClient[i].address;
            lecture_audio = listClient[i].audio_fd ;

            if (listClient[i].en_lecture)
            {
                copy_packet(&packEnvoye , &listClient[i].data);
            }
            

            if(sd > 0 )
            {
                if (FD_ISSET(sd , &read_set))
                {   
                    listClient[i].delait_renvoie_de_trame = 0 ;
                    listClient[i].delait_attente= 0 ;

                    clear_packet(&listClient[i].data);
                    //***** recvfrome
                    recvfd = lire_packet(sd , &listClient[i].address , &packRecu);
                    if (recvfd <= 0)
                    {
                        perror("recvfrom()"); 
                    }
                     
                    //les actions en fonction du type de message 
                    switch (packRecu.type)
                    {
                        case HEADER_MUSIC_RECU:
                            bzero(msgBuffer, BUFFER_SIZE);
                        if(read (lecture_audio ,msgBuffer ,BUFFER_SIZE) < 0){
                                create_packet(&packEnvoye , MSG_ERREUR_LECTURE_AUDIO, AUCUN_MSG);
                        }
                            else {
                                create_packet(&packEnvoye , ECHANTILLON, msgBuffer);
                                copy_packet(&listClient[i].data , &packEnvoye);
                                listClient[i].en_lecture = true;
                                printf("+ Le client : %d  joue le son : %s.\n" , i+1,
                                listClient[i].titre_son );
                                puts("-------------------------------------"  );

                                
                            }

                            break;

                        case ACQUITTEMENT:
                            bzero(msgBuffer, sizeof(msgBuffer));
                            readAudio = read (lecture_audio ,msgBuffer ,BUFFER_SIZE);
                        if(readAudio < 0){
                                create_packet(&packEnvoye , MSG_ERREUR_LECTURE_AUDIO, AUCUN_MSG);
                        }
                        else
                        {
                            if(readAudio != BUFFER_SIZE){
                                create_packet(&packEnvoye , FIN_DU_SON, AUCUN_MSG);
                                listClient[i].en_lecture = false;
                            }
                            else
                            { 
                                create_packet(&packEnvoye , ECHANTILLON, msgBuffer);
                                copy_packet(&listClient[i].data , &packEnvoye);
                            }
                        }
                        
                            break;
                        
                        default:
                            create_packet(&packEnvoye , MSG_ERREUR_CO_NON_AUTORISE, AUCUN_MSG);
                            break;
                    }
                    envoye_packet(sd , &addrClient, &packEnvoye);
                    if (packEnvoye.type == FIN_DU_SON )
                    {
                        remove_client(i);
                        printf("- Le client : %d  a fini sa lecture.\n" , i+1 );
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
                        printf("# Le client : %d  s'est déconnecte !\n" , i+1 );
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
    init();
    app();
    fin();
    return 0;
}

