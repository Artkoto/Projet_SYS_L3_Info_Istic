//#include <client.h>
#include "../include/client.h"

//////////////// Variables Globales/////////////
int sockfd;
struct  sockaddr_in addrserveur  ;

//les packets envoyes et reçu
struct packet packEnvoye ;
struct packet packRecu ;

//les info du son 

struct infoSon info_du_fichier_audio;

///////////////////// param//////////////////////////////////
static void param (int argc, char const *argv[]){
    // verifier les parametres 
	if ((argv[1] == NULL) || (argv[2] == NULL) || (argc != 3) ) {
		perror("Respecter la syntaxe : audioclient server_host_name file_name  ");
		exit(1);
	}    
}
////////////////////////////////////////////////

///////////////////////// creation et suppression de packets////////////////
static void create_packet(struct packet* pack, int type, void* content){
    pack->type = type ;
    memcpy(pack->message, content , BUFFER_SIZE);
}

static void clear_packet(struct packet* pack){
    pack->type = VIDE ;
    bzero(pack->message , BUFFER_SIZE);
}


///////////initialiser une sconnexion ////////////////
static int init_connexion(const char *address, struct  sockaddr_in * addrserveur){
    int sockfd = socket(AF_INET , SOCK_DGRAM , 0); 
    //
    if (sockfd == SOCKET_ERROR )
    {
        perror("socket()");
        exit(errno);
    } else puts("oui_socket");
    //

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
static void envoye_packet(int sock, struct  sockaddr_in *addrserveur, struct packet *buffer){
    int bufferSize = sizeof(struct packet);
    socklen_t flen ;
    flen = sizeof(struct sockaddr_in);
   if(sendto(sock, buffer, bufferSize, 0, (struct sockaddr*) addrserveur, flen) < 0)
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

   if((len = recvfrom(sock, buffer, bufferSize - 1, 0, (struct sockaddr*) addrserveur , &flen)) < 0)
   {
      perror("recvfrom()");
     // exit(errno);
   }

//    buffer[n] = 0;

   return len;
}

///debut du programme ///////////////
static void init(){

 
}

// fin du programme /////
static void fin(int socket){

    close(socket) ;

}


/// corp du prog////////////////
static void app(){

    struct  sockaddr_in addrserveur , addrClient  ;
    addrserveur.sin_family         = AF_INET ;
    addrserveur.sin_port           = htons (PORT);
    addrserveur.sin_addr.s_addr    = htonl (INADDR_ANY);

    

}


// int main(int argc, char const *argv[])
// {
//     init();
//     app();
//     fin();
//     return 0;
// }




//###########################################################################################


// /*



/////////////////////////////////// metre la stucture dans un autre fichier h
struct packet2 {
    int frequenceEchantillonnage;
    int tailleEchantillonnage;
    int canal;
    char  msg [64];
};
///////////////////////////////////////

// declarer fonction pour les parametres 


int main(int argc, char const *argv[])

{
   // int fd , err ;
  
    //on range le chemin du fichier dans fileName

    ////////////////////////////////////////////////////

    struct  sockaddr_in addrServer;

       /*Socket*/
    int fd = socket(AF_INET , SOCK_DGRAM , 0);
    //
    if (fd == -1 )
    {
        puts("non_socket");
    } else puts("oui_socket");
    //

    addrServer.sin_family         = AF_INET ;
    addrServer.sin_port           = htons (PORT);
    addrServer.sin_addr.s_addr    = inet_addr(argv[1]);
    


    ///////////////////////////////////////////////////////

  /*sendto*/
   
    int err_sendto;
    char  msg [1024] ;
    ////////////////
    if (argc <= 2)
     {   printf("aucun parametre detecté ");
        exit(1);
    }
    else
    {strcpy(msg , argv[2]);
    puts(msg);}
    int  sizeMsg = strlen(msg)+1; 
    //flags = 0 ;
    socklen_t tolen  = sizeof (struct sockaddr_in);

    err_sendto = sendto (fd, msg , sizeMsg, 0, (struct sockaddr*) &addrServer , tolen) ;

      if (err_sendto == -1 )
    {
        puts("non_sendto");
    } else puts("oui_sendto");

    ////////////////////////////////////////////////////////////////////////////////////

    /*recvfrom */
    //char messageBuffer [64];
    struct packet2 pRecu ;
    int bufferSize = sizeof(struct packet2);
    // int flags = 0 ;
    socklen_t len , flen ;
    flen = sizeof(struct sockaddr_in);
    len = recvfrom(fd , &pRecu , bufferSize, 0 , (struct sockaddr*) &addrServer , &flen);
      //
    if (len == -1 )
    {
        puts("non_recvfrom");
    } else {puts("oui_recvfrom");
    printf("Received %d bytes from host %s port %d: %d , %d , %d  %s \n", len,
    inet_ntoa(addrServer.sin_addr),
     ntohs(addrServer.sin_port), 
     pRecu.frequenceEchantillonnage, pRecu.tailleEchantillonnage ,pRecu.canal , pRecu.msg);   
    }

    if (strcmp(pRecu.msg , "stop") == 0)
    {
        puts("stoppppppppppp");
        exit(1);
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////
    
    puts("");
     int ecriture_audio = aud_writeinit(pRecu.frequenceEchantillonnage , pRecu.tailleEchantillonnage , pRecu.canal);
    puts("");

    
     bool arretDuSon = false;
    ssize_t  writeAudio ;
    char audio_buffer[pRecu.frequenceEchantillonnage];
    int  sizeMsg2 = strlen(audio_buffer)+1;

    /////////////////////////////////////////////////////////////////////////////////////////
    

    while (!arretDuSon )
    {
      
     
       
       len = recvfrom(fd , audio_buffer , sizeof(audio_buffer), 0 , (struct sockaddr*) &addrServer , &flen);
        sizeMsg2 = strlen(audio_buffer)+1;
        //
        // if (len == -1 )
        // {
        //     puts("non_recvfrom");
        // } else {puts("oui_recvfrom");
        //         }
        // //

        //ecriture des echantillons à partir de audio_buffer

        if (strcmp(audio_buffer,"fin") == 0 )  
         {
             arretDuSon = true ;
             puts("fin");
             }
        else
        {
                 

        // vider le buffer apres chaque passage.
        writeAudio = write(ecriture_audio, audio_buffer , pRecu.frequenceEchantillonnage);

		

         err_sendto = sendto (fd, audio_buffer , sizeMsg2, 0, (struct sockaddr*) &addrServer , tolen) ;

            // if (err_sendto == -1 )
            // {
            //     puts("non_sendto");
            // } else puts("oui_sendto");
        }
         bzero(audio_buffer, pRecu.frequenceEchantillonnage);

    }

        close(ecriture_audio);

    /*close*/

       int fini = close( fd);
       if (fini == -1 )
    {
        puts("non_close");
    } else puts("oui_close");
    //

    return 0;
}


//  */ 