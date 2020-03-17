//#include <client.h>
#include "../include/client.h"

////////////////////// Variables Globales/////////////////////////////////////////////
////sockfd
int sockfd  = 0;


//ifon du client 
int id = 0 ; 

////audio dir
// char msgBuffer[BUFFER_SIZE];
char filename [BUFFER_SIZE];
char address[BUFFER_SIZE];

///utilisation temp des sockfd pour les fils
 bool en_ligne = false; 

//////// por gerer les recvfrom
socklen_t recvfd;

//descripteur de fichier audio 

int ecriture_audio ;

struct  sockaddr_in addrserveur ;

//timeout
fd_set read_set;
struct timeval timeout;

//les packets envoyes et reçu
struct packet packEnvoye ;
struct packet packRecu ;

//les info du son 

struct headerSon audio_header;

/// MSG ERRERU
int err_connexion;

/////////////////////////////////////////////////////////////////////////////////////////

///////////////////// param//////////////////////////////////
static void param (int argc, char const *argv[]){
    // verifier les parametres 
	if ((argv[1] == NULL) || (argv[2] == NULL) || (argc != 3) ) {
		perror("Respecter la syntaxe : audioclient server_host_name file_name  ");
		exit(1);
	}    

    strcpy(address , argv[1]);
    strcpy(filename , argv[2]);
}
//////////////////////recuperatiion des parametres audios///////////////////

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
}

///////////////////////// creation et suppression de packets////////////////
static void create_packet(struct packet* pack, int type, void* content){
    pack->type = type ;
    pack->id_client = id ;
    memcpy(pack->message, content , BUFFER_SIZE);
}

static void clear_packet(struct packet* pack){
    pack->type = VIDE ;
    bzero(pack, sizeof (struct packet) );
}


///////////initialiser une sconnexion ////////////////
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


////////////lire un packet//////////////////////////
static socklen_t lire_packet(int sock, struct  sockaddr_in *addrserveur, struct packet *buffer){
 
   int bufferSize = sizeof(struct packet);
   socklen_t len , flen ;
    flen = sizeof(struct sockaddr_in);

   if((len = recvfrom(sock, buffer, bufferSize , 0, (struct sockaddr*) addrserveur , &flen)) < 0)
   {
      perror("recvfrom()");
     // exit(errno);
   }

//    buffer[n] = 0;

   return len;
}

///debut du programme ///////////////
static void init(){
    
    clear_packet(&packEnvoye);
    clear_packet(&packRecu);

    if (init_connexion(address, &addrserveur) < 0)
    {
       exit(errno);
    }
    en_ligne = true ;
    create_packet(&packEnvoye , FILENAME ,filename);
    envoye_packet(sockfd , &addrserveur , &packEnvoye);
    clear_packet(&packEnvoye);
 
}



/// corp du prog////////////////
static void app(){

    while (en_ligne)
    {
        FD_ZERO(&read_set);
         FD_SET(sockfd , &read_set);
        timeout.tv_sec = 0 ;
        timeout.tv_usec = TIMEOUT;


        
        int activity = select(sockfd+1 , &read_set , NULL, NULL, &timeout);
        if (activity < 0)
        {
            perror("select()");
            //exit(errno);
        }

        if (activity == 0)
        {
            //time out
            puts("delai d'attente depassé"); //TODO: a retirer
            exit(0);
        }

        if (FD_ISSET(sockfd,&read_set)) //premiere connexion du client
        {
                //***** recvfrome
           recvfd = lire_packet(sockfd , &addrserveur , &packRecu);
           if (recvfd <= 0)
           {
               perror("recvfrom()");
           }

            //******* verification du type de packet nom de fichier

            switch (packRecu.type)
            {
            case FILE_HEADER:
            recup_audio_header(packRecu.message);

            ecriture_audio = aud_writeinit(audio_header.frequenceEchantillonnage ,
            audio_header.tailleEchantillonnage ,
            audio_header.canal);
            if (ecriture_audio < 0)
            {
                /* code */
                exit(errno);
                puts("impossible d'ouvrire le p audio");
            }
            id = packRecu.id_client ;
            create_packet(&packEnvoye , HEADER_MUSIC_RECU , AUCUN_MSG );
            envoye_packet(sockfd , &addrserveur , &packEnvoye);
            clear_packet(&packEnvoye);
            clear_packet(&packRecu);
            break;

            case ECHANTILLON:
            /* code */
            if (write(ecriture_audio, packRecu.message , BUFFER_SIZE) < 0 )
            {
                /* code */
                 puts("impossible de jouer le son");
                 exit(errno);
            }
            
            create_packet(&packEnvoye , ACQUITTEMENT , AUCUN_MSG );
            envoye_packet(sockfd , &addrserveur , &packEnvoye);
            clear_packet(&packEnvoye);
            clear_packet(&packRecu);
            break;

            case MSG_ERREUR_SERVEUR_COMPLET:
            /* code */
            puts ("le serveur est complet");
            en_ligne = false ; 
            break;

            case MSG_ERREUR_SON_INDISPONIBLE:
            /* code */
            puts ("le son souhaite n'est pas disponible");
            en_ligne = false ;
            break;

            case MSG_ERREUR_CO_NON_AUTORISE:
            /* code */
            puts ("vous n'avez pas le droi de vous connnecter à ce serveur");
            en_ligne = false ;
            break;

            case MSG_ERREUR_LECTURE_AUDIO:
            /* code */
            puts ("erreur lors de la lecture ");
            en_ligne = false ;
            break;

            case MSG_ERREUR_SOCKET:
            /* code */
            puts ("probleme de connection : init socket");
            en_ligne = false ;
            break;

            case MSG_ERREUR_PORT:
            /* code */
            puts ("probleme de connection : port");
            en_ligne = false ;
            break;

            case FIN_DU_SON:
            /* code */
            puts ("lecture fini");
            en_ligne = false ;
            break;
            
            default:
                break;
            }
           
        }
    }
    
    

}


// fin du programme /////
static void fin(){

    fin_connexion(sockfd);
    puts("fin de la lecture");

}



int main(int argc, char const *argv[])
{
    param ( argc, argv);
    init();
    app();
    fin();
    return 0;
}

