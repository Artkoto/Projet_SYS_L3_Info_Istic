#include <client.h>

                        /* ##################################################
                           #            Les variables globales              #
                           ##################################################
                        */
////sockfd
int sockfd  = 0;


//id du client dans le serveur
int id = 0 ; 

////audio dir
// char msgBuffer[BUFFER_SIZE]; 
char filename [BUFFER_SIZE];
char address[BUFFER_SIZE];

 bool en_ligne = false; // pour verifier si l'utilisateur est connecter au serveur

//////// pour gerer les receptions de trames 
socklen_t recvfd;

//descripteur de fichier audio 
int ecriture_audio  = 0;

struct  sockaddr_in addrserveur ;

//timeout
fd_set read_set;
struct timeval timeout;

//les packets envoyes et reçu
struct packet packEnvoye ;
struct packet packRecu ;

//les info du son 

struct headerSon audio_header;

/// MSG ERRUR
int err_connexion;

//filtre
int filtre;
int param_filtre;
char param_temp[BUFFER_SIZE];
char Parametre [BUFFER_SIZE];
char audioBuffer [BUFFER_SIZE];
int volume = 1;



                    /* ##################################################
                        #           Les methodes de l'application        #
                        ##################################################
                    */
///////////////////// parametres //////////////////////////////////
static void param (int argc, char const *argv[]){
    // verifier les parametres 
	if ((argv[1] == NULL) || (argv[2] == NULL) || (argc < 3) ) {
		perror("Respecter la syntaxe : audioclient server_host_name file_name filter_name [filter_parameter]");
		exit(1);
	}
    strcpy(address , argv[1]);
    strcpy(filename , argv[2]);
    

    //recupreation des parametres
    if (argc > 3)
    {
        for (int i = 3; i < argc;  i = i+2)
        {
            if (strcmp(argv[i], MONO_NAME) == 0 )
            {
                filtre = MONO;
                param_filtre = 0;
                i--;
            }
           else if (strcmp(argv[i], VOLUME_NAME) == 0 )
            {
                filtre = VOLUME;
                param_filtre = atoi(argv[i+1]);

                if (param_filtre < VOLUME_PARAMETER_MIN || param_filtre > VOLUME_PARAMETER_MAX)
                {
                   perror("parametre invalide");
                   exit(0);
                }
                
            }
            else if (strcmp(argv[i], VITESSE_NAME) == 0 )
            {
                filtre = VITESSE;
                param_filtre = atoi(argv[i+1]);
                if (param_filtre < VITESSE_PARAMETER_MIN || param_filtre > VITESSE_PARAMETER_MAX)
                {
                   perror("parametre invalide");
                   exit(0);
                }

            }
            else
            {
                perror("erreur de syntaxe");
                exit(0);
            }
            
            snprintf(param_temp, sizeof(param_temp),
                          "%d %d ",
                          filtre , param_filtre);

            strcat(Parametre , param_temp);    
        }
        
    }
    
}
////////////////////// recuperatiion des l'entete de l' audios ///////////////////

static void recup_audio_header( char *header){
    char *token = strtok(header, " ");
						int i = 0;
						while ((token != NULL) && (i < 3)) {
							switch (i) {

								// frequance d'chantillonange
								case 0:
									audio_header.frequenceEchantillonnage = atoi(token);
									break;

								// taille d'echantillonnange
								case 1:
									audio_header.tailleEchantillonnage = atoi(token);
									break;

								// le canal
								case 2:

									audio_header.canal = atoi(token);
									break;
							}

							token = strtok(NULL, " ");
							i++;
						}
                        
    char *token2 = strtok(Parametre , " ");
						
						while ((token2 != NULL)) {
							switch (atoi(token2)) {

								//
								case MONO:
                                audio_header.canal = 1 ;
                                break;

								// 
								case VOLUME:
                                token2 = strtok(NULL, " ");
                                volume = atoi(token2);
                                break;

                                // 
								case VITESSE:
                                token2 = strtok(NULL, " ");
                                if (atoi(token2) >= 0 )
                                {
                                   audio_header.frequenceEchantillonnage *= (atoi(token2) == 0)? 1 : atoi(token2);
                                }else
                                {
                                    audio_header.frequenceEchantillonnage /= -atoi(token2);
                                }
                                
                                break;
							}
							token2 = strtok(NULL, " ");
						}
}

///////////////////////// creation et suppression de packets////////////////
static void create_packet(struct packet* pack, int type, void* content){
    pack->type = type ;
    pack->id_client = id ;
    memcpy(pack->message, content , BUFFER_SIZE);
}

static void clear_packet(struct packet* pack){
    pack->type = VIDE ;
    bzero(pack->message, BUFFER_SIZE);
}


/////////// initialiser une sconnexion ////////////////
static int init_connexion(const char *address, struct  sockaddr_in * addrserveur){
    sockfd = socket(AF_INET , SOCK_DGRAM , 0); 
    // 
    if (sockfd == SOCKET_ERROR )
    {
        perror("socket()");
        return -1;
    }

    addrserveur->sin_family         = AF_INET ;
    addrserveur->sin_port           = htons (PORT);
    addrserveur->sin_addr.s_addr    = inet_addr(address);

    return sockfd ;

}

///////////fermer  une sconnexion ////////////////
static void fin_connexion(int sockfd){
    close(sockfd) ;
}

/////////// envoyer un packet ///////////////////
static void envoye_packet(int sock, struct  sockaddr_in *addrclient, struct packet *buffer){
    int bufferSize = sizeof(struct packet);
    socklen_t flen ;
    flen = sizeof(struct sockaddr_in);
   if(sendto(sock, buffer, bufferSize, 0, (struct sockaddr*) addrclient, flen) < 0)
   {
      perror("sendto()");
      exit(errno);
   }
}


////////////    lire un packet      //////////////////////////
static socklen_t lire_packet(int sock, struct  sockaddr_in *addrserveur, struct packet *buffer){
 
   int bufferSize = sizeof(struct packet);
   socklen_t len , flen ;
    flen = sizeof(struct sockaddr_in);

   if((len = recvfrom(sock, buffer, bufferSize , 0, (struct sockaddr*) addrserveur , &flen)) < 0)
   {
      perror("recvfrom()");
   }
   return len;
}

///     debut du programme  ///////////////
static void init(){
    
    clear_packet(&packEnvoye);
    clear_packet(&packRecu);

    if (init_connexion(address, &addrserveur) < 0)
    {
       exit(errno);
    }
    en_ligne = true ;
    create_packet(&packEnvoye , FILENAME ,filename);
    envoye_packet(sockfd , &addrserveur , &packEnvoye); // envoie du premier packet (filename)
    clear_packet(&packEnvoye);
 
}

///  corp du programme    //////////////
static void app(){

    while (en_ligne)  // tant que l'utilisateur est connecte au serveur
    {
        FD_ZERO(&read_set);
        FD_SET(sockfd , &read_set);
        timeout.tv_sec = 0 ;
        timeout.tv_usec = TIMEOUT; // 5 secondes

        //Surveillance du socket avec select
        int activity = select(sockfd+1 , &read_set , NULL, NULL, &timeout);
        if (activity < 0)
        {
            perror("select()");
        }

        if (activity == 0)
        {
            //timeout
            puts("delai d'attente depassé"); 
            if (ecriture_audio > 0)
            {
                //fermer le peripherique audio si le delai d'attente est depasse
                close(ecriture_audio); 
            }
            exit(0);
        }

        if (FD_ISSET(sockfd,&read_set))
        {
                //***** recvfrome
           recvfd = lire_packet(sockfd , &addrserveur , &packRecu);
           if (recvfd <= 0)
           {
               perror("recvfrom()");
           }

            //******* verification du type de packet 

            switch (packRecu.type)
            {
            case FILE_HEADER: //premier packet du serveur au client
            //recuperation des informations contenus dans le packet recu
            recup_audio_header(packRecu.message);
            ecriture_audio = aud_writeinit(audio_header.frequenceEchantillonnage ,
            audio_header.tailleEchantillonnage ,
            audio_header.canal);
            if (ecriture_audio < 0)
            {
                exit(errno);
                puts("Impossible d'ouvrir le peripherique audio");
            }
            id = packRecu.id_client ;
            //le client signale au serveur qu'il a recu l' entete du fchier
            create_packet(&packEnvoye , HEADER_MUSIC_RECU , AUCUN_MSG ); 
            envoye_packet(sockfd , &addrserveur , &packEnvoye);
            clear_packet(&packEnvoye);
            clear_packet(&packRecu);
            break;

            case ECHANTILLON: // reception d'un echantillon de son
            memset(audioBuffer, 0, BUFFER_SIZE);
            for (int i = 0; i < BUFFER_SIZE/(audio_header.tailleEchantillonnage/8); i++)
            {
                *((int*)(audioBuffer + i*sizeof(int))) = 
                *((int*)(packRecu.message + i*sizeof(int))) * volume ;
            }
            
            if (write(ecriture_audio, audioBuffer , BUFFER_SIZE) < 0 )
            {
                 puts("impossible de jouer le son");
                 exit(errno);
            }
            
            create_packet(&packEnvoye , ACQUITTEMENT , AUCUN_MSG ); //le client envoie un acquittement au serveur
            envoye_packet(sockfd , &addrserveur , &packEnvoye);
            clear_packet(&packEnvoye);
            clear_packet(&packRecu);
            break; 

            //les cas d'erreur de connection au serveur 
            case MSG_ERREUR_SERVEUR_COMPLET:  
            puts ("le serveur est complet");
            en_ligne = false ; 
            break;

            case MSG_ERREUR_SON_INDISPONIBLE:
            puts ("le son souhaite n'est pas disponible");
            en_ligne = false ;
            break;

            case MSG_ERREUR_CO_NON_AUTORISE:
            puts ("vous n'avez pas le droit de vous connnecter");
            en_ligne = false ;
            break;

            case MSG_ERREUR_LECTURE_AUDIO:
            puts ("erreur lors de la lecture ");
            en_ligne = false ;
            break;

            case MSG_ERREUR_SOCKET:
            puts ("probleme de connection : init socket");
            en_ligne = false ;
            break;

            case MSG_ERREUR_PORT:
            puts ("probleme de connection : port");
            en_ligne = false ;
            break;

            case FIN_DU_SON: // fin de la lecture 
            puts ("Fin de lecture");
            en_ligne = false ;
            close(ecriture_audio);
            break;
            
            default:
            create_packet(&packEnvoye , MSG_ERREUR_CO_NON_AUTORISE, AUCUN_MSG);
            envoye_packet(sockfd , &addrserveur , &packEnvoye);
            clear_packet(&packEnvoye);
            clear_packet(&packRecu);
                break;
            }
           
        }
    }

}


// fin du programme /////
static void fin(){

    fin_connexion(sockfd);
    puts("Fermeture de l'application");

}


//// main ()
int main(int argc, char const *argv[])
{
    param ( argc, argv);
    init();
    app();
    fin();
    return 0;
}

