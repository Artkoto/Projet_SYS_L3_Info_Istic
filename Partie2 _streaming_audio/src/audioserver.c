//#include <serveur.h>

#include "../include/serveur.h"

//////////////// Variables Globales/////////////
int tab_clients[CLIENT_MAX][2];
int sockfd;
struct  sockaddr_in addrserveur , addrClient  ;

//les packets envoyes et reçu
struct packet packEnvoye ;
struct packet packRecu ;

//les info du son 

struct infoSon info_du_fichier_audio;




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
static int init_connexion( struct  sockaddr_in * addrserveur){
    
return 1;

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
static socklen_t lire_packet(int sock, struct  sockaddr_in *addrclient, struct packet *buffer){
 
   int bufferSize = sizeof(struct packet);
   socklen_t len , flen ;
    flen = sizeof(struct sockaddr_in);

   if((len = recvfrom(sock, buffer, bufferSize - 1, 0, (struct sockaddr*) addrclient , &flen)) < 0)
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
        if (tab_clients[i][0] == 0)
        {
            pack->id_client = i ;

            return true ;
        }
        
    }

    return false;
    
} 


//////////////////////supprimer client ///////////////
static void remove_client(int id_client ){
    tab_clients[id_client][0] =  0 ;
    //TODO
}


//////AUDIO PATH
static void audio_path(const char *filename ,  char *audioPath){
        strcpy( audioPath,AUDIO_DIR);
            strcat(audioPath , filename);
}


///debut du programme ///////////////

static void init(){



    sockfd = socket(AF_INET , SOCK_DGRAM , 0);
    //
    if (sockfd == SOCKET_ERROR )
    {
        perror("socket()");
        exit(errno);
    } else puts("oui_socket");
    //
    addrserveur.sin_family         = AF_INET ;
    addrserveur.sin_port           = htons (PORT);
    addrserveur.sin_addr.s_addr    = htonl (INADDR_ANY);


    int  err_bind = bind(sockfd, (struct sockaddr *)&addrserveur , sizeof(addrserveur) );
            //
            if (err_bind == -1 )
            {
                puts("non_bind");
            } else puts("oui_bind");


    for (size_t i = 0; i < CLIENT_MAX; i++)
    {
        tab_clients[i][0] = 0 ;
        tab_clients[i][1] = 0 ;
    }
    
}

// fin du programme /////
static void fin(){

    close(sockfd) ;

}


/// corp du prog////////////////
static void app(){
    while (true)
    {
        //***** recvfrome
        lire_packet(sockfd , &addrClient , &packRecu);

        //******* verification du type de packet

        //*** si filename 



            


    }
    

 


}


// int main(int argc, char const *argv[])
// {
//     init();
//     app();
//     fin();
//     return 0;
// }




/*###########################################################################################
1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
22222222222222222222222222222222222222222222222222222
999999999999999999999999999999999999999999999999999999999999999999999999999999999
#####################################################################################"""*/


//   /*

#define MAX_FILENAME_SIZE 1024
///////////////////////////////////
struct packet2 {
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

    // struct  sockaddr_in addrServer , addrClient ,sock2;

    //    /*Socket*/
    // int fd = socket(AF_INET , SOCK_DGRAM , 0);
    // //
    // if (fd == -1 )
    // {
    //     puts("non_socket");
    // } else {puts("oui_socket2"); printf("%d \n", fd);}
    // //

    // addrServer.sin_family         = AF_INET ;
    // addrServer.sin_port           = htons (3685);
    // addrServer.sin_addr.s_addr    = htonl (INADDR_ANY);

    // int fd2 = socket(AF_INET , SOCK_DGRAM , 0);
    // if (fd2 == -1 )
    // {
    //     puts("non_socket2");
    // } else {puts("oui_socket2"); printf("%d \n", fd2);}
    // //

    // sock2.sin_family         = AF_INET ;
    // sock2.sin_addr.s_addr    = htonl (INADDR_ANY);

    

    //     /*bind*/
    //     int  err_bind = bind(fd, (struct sockaddr *)&addrServer , sizeof(addrServer) );
    //         //
    //         if (err_bind == -1 )
    //         {
    //             puts("non_bind");
    //         } else puts("oui_bind");
    //         //

    //         /*bind2*/
    //     int  err_bind2 = bind(fd2, (struct sockaddr *)&sock2 , sizeof(sock2) );
    //         //
    //         if (err_bind2 == -1 )
    //         {
    //             puts("non_bind2");
    //         } else puts("oui_bind2");
            //
//
    init();
            /*recvfrom */
            char messageBuffer [1024];
            int bufferSize = sizeof(messageBuffer);
            // int flags = 0 ;
            
            socklen_t len , flen ;

    while (true)
    { 
       // usleep(7000000);
        ////////////////////////
        struct packet2 pEnvoye ;
        flen = sizeof(struct sockaddr_in);
        len = recvfrom(sockfd , messageBuffer , bufferSize, 0 , (struct sockaddr*) &addrClient , &flen);
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
        // strcpy(caractereSaisie , messageBuffer);
        // strcpy(fileName,audioPath);
        //     strcat(fileName , caractereSaisie);

            audio_path(messageBuffer ,fileName);

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
            messageBuffer ,
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

        err_sendto = sendto (sockfd, &pEnvoye , sizeof(struct packet2), 0, (struct sockaddr*) &addrClient , tolen) ;

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
            
            
            err_sendto = sendto (sockfd, audio_buffer , pEnvoye.frequenceEchantillonnage, 0, (struct sockaddr*) &addrClient , tolen) ;

            // if (err_sendto == -1 )
            // {
            //     puts("non_sendto");
            // } else puts("oui_sendto");
            bzero(audio_buffer, pEnvoye.frequenceEchantillonnage);

/////////////////////////////////////////////////////////////////////
           
                fd_set read_set;
                struct timeval timeout;
                FD_ZERO(&read_set);
                FD_SET(sockfd , &read_set);
                timeout.tv_sec = 0 ;
                timeout.tv_usec = 5000000;
                
                
                int nb1 = select(sockfd+1 , &read_set , NULL, NULL, &timeout);
                
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

                if (FD_ISSET (sockfd , &read_set))
                {

                 do
              {
                 len = recvfrom(sockfd , messageBuffer , bufferSize, 0 , (struct sockaddr*) &addrClient , &flen);
                 //////
                 if(strcmp(ipaddrcoirant,inet_ntoa(addrClient.sin_addr) ) != 0 || portaddrcoirant != ntohs(addrClient.sin_port) ){
                strcpy (pEnvoye.msg ,"stop");
                int err_sendt = sendto (sockfd, &pEnvoye , sizeof(struct packet2), 0, (struct sockaddr*) &addrClient ,sizeof (struct sockaddr_in) ) ;
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
                err_sendto = sendto (sockfd, "fin" , 4, 0, (struct sockaddr*) &addrClient , tolen) ;

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
    int fini = close( sockfd);
       if (fini == -1 )
    {
        puts("non_close");
    } else puts("oui_close");
    //


    return 0;
}




// */
