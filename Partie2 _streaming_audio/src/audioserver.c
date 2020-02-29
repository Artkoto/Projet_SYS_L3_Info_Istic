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
    char fileName [MAX_FILENAME_SIZE];
    char caractereSaisie [MAX_FILENAME_SIZE];
    char audioPath[] = "audio/" ;
    int frequenceEchantillonnage ;
    int tailleEchantillonnage;
    int canal ;

    
    //on range le chemin du fichier dans fileName

    
    

    /////////
   // int fd , err ;

    struct  sockaddr_in addrServer , addrClient;

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

/*bind*/
   int  err_bind = bind(fd, (struct sockaddr *)&addrServer , sizeof(addrServer) );
    //
    if (err_bind == -1 )
    {
        puts("non_bind");
    } else puts("oui_bind");
    //

    /*recvfrom */
    char messageBuffer [1024];
    int bufferSize = sizeof(messageBuffer);
    // int flags = 0 ;
    
    socklen_t len , flen ;

    while (true)
    {
        flen = sizeof(struct sockaddr_in);
        len = recvfrom(fd , messageBuffer , bufferSize, 0 , (struct sockaddr*) &addrClient , &flen);
        //
        if (len == -1 )
        {
            puts("non_recvfrom");
        } else {puts("oui_recvfrom");
        printf("Received %d bytes from host %s port %d: %s \n", len,
        inet_ntoa(addrClient.sin_addr),
        ntohs(addrClient.sin_port), 
        messageBuffer);   
        }
        //
/////////////////////////////////////////////////////////////////////
 strcpy(caractereSaisie , messageBuffer);
strcpy(fileName,audioPath);
    strcat(fileName , caractereSaisie);

puts("");
    int lecture_audio = aud_readinit(fileName , &frequenceEchantillonnage, &tailleEchantillonnage, &canal);  
    puts("");

    //en cas d'erreur
    if (lecture_audio == -1 ) return -1 ;

    fprintf(stdout,
    " Fichier audio : %s \n Frequence echantillonnage : %d \n Taille echantillonnage : %d \n Canal : %d \n", 
    caractereSaisie ,
    frequenceEchantillonnage,
    tailleEchantillonnage,
    canal);
//////////////////////////////////////////////////////////////////////

        /*sendto*/
        struct packet pEnvoye ;
        int err_sendto;
        //char  msg [64] = "Hello World ! du server";
        //int  sizeMsg = strlen(msg)+1;
        //flags = 0 ;
        pEnvoye.canal =canal ;
        pEnvoye.frequenceEchantillonnage = frequenceEchantillonnage;
        pEnvoye.tailleEchantillonnage = tailleEchantillonnage ;
        socklen_t tolen  = sizeof (struct sockaddr_in);

        err_sendto = sendto (fd, &pEnvoye , sizeof(struct packet), 0, (struct sockaddr*) &addrClient , tolen) ;

        if (err_sendto == -1 )
        {
            puts("non_sendto");
        } else puts("oui_sendto");
        //


        ///////////////////////////////////////////
         bool arretDuSon = false;
        char audio_buffer[frequenceEchantillonnage];
        int  sizeMsg = strlen(audio_buffer)+1;
        ssize_t  readAudio  ;

        while (!arretDuSon )
        {
            //lecture des echantillons dans audio_buffer à partir du descripteur de fichier (fd)
            readAudio = read (lecture_audio ,audio_buffer ,frequenceEchantillonnage);
             sizeMsg = strlen(audio_buffer)+1;
            
            
            err_sendto = sendto (fd, audio_buffer , sizeMsg, 0, (struct sockaddr*) &addrClient , tolen) ;

            // if (err_sendto == -1 )
            // {
            //     puts("non_sendto");
            // } else puts("oui_sendto");
            bzero(audio_buffer, frequenceEchantillonnage);

             len = recvfrom(fd , messageBuffer , bufferSize, 0 , (struct sockaddr*) &addrClient , &flen);
        //
            // if (len == -1 )
            // {
            //     puts("non_recvfrom");
            // } else {puts("oui_recvfrom");
              
            // }
            if(readAudio != frequenceEchantillonnage) 
            {
                arretDuSon = true ;
               // strcpy(audio_buffer , "fin")
                err_sendto = sendto (fd, "fin" , 4, 0, (struct sockaddr*) &addrClient , tolen) ;

            }


            // vider le buffer apres chaque passage.
            //bzero(audio_buffer, frequenceEchantillonnage);

        }

           int fini2 = close(lecture_audio);
               if (fini2 == -1 )
                {
                    puts("non_close1");
                } else puts("oui_close1");
                //

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

