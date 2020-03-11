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
    char  msg [64];
};
///////////////////////////////////////


int main(int argc, char const *argv[])

{

       // int fd , err ;
    char fileName [MAX_FILENAME_SIZE];
    char caractereSaisie [MAX_FILENAME_SIZE];
    char audioPath[] = "audio/" ;
    int lecture_audio ;
    bool audio_ouvert = false ;
   // bool unClientConnecte = false ;

    
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
    addrServer.sin_port           = htons (3685);
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
        ////////////////////////
        struct packet pEnvoye ;
        flen = sizeof(struct sockaddr_in);
        len = recvfrom(fd , messageBuffer , bufferSize, 0 , (struct sockaddr*) &addrClient , &flen);
        char * ipaddrcoirant  = inet_ntoa(addrClient.sin_addr);
        int portaddrcoirant  = ntohs(addrClient.sin_port);
        puts(ipaddrcoirant); //// 
        printf("%d\n",portaddrcoirant); ///

        ////////////////////////////////

        //
        if (len == -1 )
        {
            puts("non_recvfrom");
        } else {puts("oui_recvfrom");
        printf("Received %d bytes from host %s port %d: %s \n", len,
        inet_ntoa(addrClient.sin_addr),
        ntohs(addrClient.sin_port), 
        messageBuffer);  
        strcpy (pEnvoye.msg , "connexion");
       // unClientConnecte = true ;
        
            }

           // if (unClientConnecte){
                //
            //}
          
            //
        /////////////////////////////////////////////////////////////////////
        strcpy(caractereSaisie , messageBuffer);
        strcpy(fileName,audioPath);
            strcat(fileName , caractereSaisie);

        puts("");
            if (audio_ouvert){
                int fini2 = close(lecture_audio);
                audio_ouvert = false ;
               if (fini2 == -1 )
                {
                    puts("non_close2");
                } else puts("oui_close2");
            }
             lecture_audio = aud_readinit(fileName , &pEnvoye.frequenceEchantillonnage, &pEnvoye.tailleEchantillonnage, &pEnvoye.canal);  
            puts("");

            //en cas d'erreur
            if (lecture_audio == -1 ) return -1 ;

            fprintf(stdout,
            " Fichier audio : %s \n Frequence echantillonnage : %d \n Taille echantillonnage : %d \n Canal : %d \n", 
            caractereSaisie ,
            pEnvoye.frequenceEchantillonnage,
            pEnvoye.tailleEchantillonnage,
            pEnvoye.canal);
            
        //////////////////////////////////////////////////////////////////////

        /*sendto*/
        
        int err_sendto;
        //char  msg [64] = "Hello World ! du server";
        //int  sizeMsg = strlen(msg)+1;
        //flags = 0 ;
        socklen_t tolen  = sizeof (struct sockaddr_in);

        err_sendto = sendto (fd, &pEnvoye , sizeof(struct packet), 0, (struct sockaddr*) &addrClient , tolen) ;

        if (err_sendto == -1 )
        {
            puts("non_sendto");
        } else puts("oui_sendto");
        //


        /////////////////////////////////////////////////////////////////////


         bool arretDuSon = false;
        char audio_buffer[pEnvoye.frequenceEchantillonnage];
       // int  sizeMsg = strlen(audio_buffer)+1;
       int nb_timeout = 0; 
        ssize_t  readAudio  ;

        while (!arretDuSon )
        {
            //lecture des echantillons dans audio_buffer à partir du descripteur de fichier (fd)
            readAudio = read (lecture_audio ,audio_buffer ,pEnvoye.frequenceEchantillonnage);
            // sizeMsg = strlen(audio_buffer)+1;
            
            
            err_sendto = sendto (fd, audio_buffer , pEnvoye.frequenceEchantillonnage, 0, (struct sockaddr*) &addrClient , tolen) ;

            // if (err_sendto == -1 )
            // {
            //     puts("non_sendto");
            // } else puts("oui_sendto");
            bzero(audio_buffer, pEnvoye.frequenceEchantillonnage);

/////////////////////////////////////////////////////////////////////
           
                fd_set read_set;
                struct timeval timeout;
                FD_ZERO(&read_set);
                FD_SET(fd , &read_set);
                timeout.tv_sec = 0 ;
                timeout.tv_usec = 5000000;
                
                
                int nb1 = select(fd+1 , &read_set , NULL, NULL, &timeout);
                
                if(nb1  < 0){
                    puts("erro_select");
                }

                if (nb1 == 0)
                {
                    puts("timeout");
                    ++nb_timeout ;
                    if (nb_timeout == 4)
                    {
                       arretDuSon = true ; 
                    }
                    

                }

                if (FD_ISSET (fd , &read_set))
                {

                 do
              {
                 len = recvfrom(fd , messageBuffer , bufferSize, 0 , (struct sockaddr*) &addrClient , &flen);
                 //////
                 if(strcmp(ipaddrcoirant,inet_ntoa(addrClient.sin_addr) ) != 0 || portaddrcoirant != ntohs(addrClient.sin_port) ){
                strcpy (pEnvoye.msg ,"stop");
                int err_sendt = sendto (fd, &pEnvoye , sizeof(struct packet), 0, (struct sockaddr*) &addrClient ,sizeof (struct sockaddr_in) ) ;
                 }

             }while ((strcmp(ipaddrcoirant,inet_ntoa(addrClient.sin_addr) ) != 0 || portaddrcoirant != ntohs(addrClient.sin_port)) );
                }

//////////////////////////////////////////////////////////
             
             
             

        //
            // if (len == -1 )
            // {
            //     puts("non_recvfrom");
            // } else {puts("oui_recvfrom");
              
            // }
            if(readAudio != pEnvoye.frequenceEchantillonnage) 
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
                } else {
                    puts("oui_close1");
                   audio_ouvert = false ;
                    }
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

