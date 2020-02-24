#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <audio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/ip.h>


#define MAX_FILENAME_SIZE 1024
///////////////////////////////////
struct packet {
    int frequenceEchantillonnage;
    int tailleEchantillonnage;
    int canal;
};
///////////////////////////////////////


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
    addrServer.sin_port           = htons (8080);
    addrServer.sin_addr.s_addr    = htonl (INADDR_ANY);

    int i = 200 ;
   while(i > 0)
   { /*sendto*/
   i--;
    int err_sendto;
    char  msg [1024] ;
    ////////////////
        printf("Entrez le ici : ");
        fgets(msg ,( 1024 / sizeof(char)), stdin);
        *(msg + strlen(msg)-1) = '\0';
    int  sizeMsg = strlen(msg)+1;
    //flags = 0 ;
    socklen_t tolen  = sizeof (struct sockaddr_in);

    err_sendto = sendto (fd, msg , sizeMsg, 0, (struct sockaddr*) &addrServer , tolen) ;

      if (err_sendto == -1 )
    {
        puts("non_sendto");
    } else puts("oui_sendto");
    //

    /*recvfrom */
    //char messageBuffer [64];
    struct packet pRecu ;
    int bufferSize = sizeof(struct packet);
    // int flags = 0 ;
    socklen_t len , flen ;
    flen = sizeof(struct sockaddr_in);
    len = recvfrom(fd , &pRecu , bufferSize, 0 , (struct sockaddr*) &addrServer , &flen);
      //
    if (len == -1 )
    {
        puts("non_recvfrom");
    } else {puts("oui_recvfrom");
    printf("Received %d bytes from host %s port %d: %d \n", len,
    inet_ntoa(addrServer.sin_addr),
     ntohs(addrServer.sin_port), 
     pRecu.canal);   
    }
    //
    puts("");
     int ecriture_audio = aud_writeinit(pRecu.frequenceEchantillonnage , pRecu.tailleEchantillonnage , pRecu.canal);
    puts("");

    
     bool arretDuSon = false;
    ssize_t  writeAudio ;
    char audio_buffer[pRecu.frequenceEchantillonnage];
    int  sizeMsg2 = strlen(audio_buffer)+1;
    

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
        writeAudio = write(ecriture_audio, audio_buffer , pRecu.frequenceEchantillonnage);

        if (writeAudio != pRecu.frequenceEchantillonnage)   arretDuSon = true ;

        // vider le buffer apres chaque passage.
		// bzero(audio_buffer, pRecu.frequenceEchantillonnage);

         err_sendto = sendto (fd, audio_buffer , sizeMsg2, 0, (struct sockaddr*) &addrServer , tolen) ;

            // if (err_sendto == -1 )
            // {
            //     puts("non_sendto");
            // } else puts("oui_sendto");

    }

        close(ecriture_audio);


    //fin tqq
    }

    /*close*/

       int fini = close( fd);
       if (fini == -1 )
    {
        puts("non_close");
    } else puts("oui_close");
    //

    return 0;
}