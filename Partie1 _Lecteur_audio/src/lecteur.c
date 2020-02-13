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

                 /* #########################################################
                   #                          BINOME                       #
                   #           AKOTO YAO ARNAUD  && KONE KAFONGO           #
                   ######################################################### 
                */

#define MAX_FILENAME_SIZE 1024


int main(int argc, char const *argv[])
{
    //si l'utilisateur met un argument dans l'appel du programme.
   
    char fileName [MAX_FILENAME_SIZE];
    char caractereSaisie [MAX_FILENAME_SIZE];
    char audioPath[] = "audio/" ;
    int frequenceEchantillonnage ;
    int tailleEchantillonnage;
    int canal ;

                //recuperation du nom de l'audio

    //si l'utilisateur oublie de mettre le nom du fichier en argument lors de l'execution.
    if (argc <= 1 ) { 
        perror("vous avez oublié de mettre le nom du fichier en parametre.\n");
        printf("Entrez le ici : ");
        fgets(caractereSaisie ,( MAX_FILENAME_SIZE / sizeof(char)), stdin);
        *(caractereSaisie + strlen(caractereSaisie)-1) = '\0';
         }
         
    //si l'utilisateur met le nom du fichier en argument lors de l'execution.
    else
    {
        strcpy(caractereSaisie , argv[1]);
    }

    //on range le chemin du fichier dans fileName

    strcpy(fileName,audioPath);
    strcat(fileName , caractereSaisie);
 

    /* ##################################################
       #  Recuperation la frequence d echantillonnage,  #
       #  la taille des echantillons et                 #
       #  le nombre de canaux  de du fichier audio      #
       ### ##############################################
    */
    puts("");
    int lecture_audio = aud_readinit(fileName , &frequenceEchantillonnage, &tailleEchantillonnage, &canal);  
    puts("");

    //en cas d'erreur
    if (lecture_audio == -1 ) return -1 ;

    /* #### Afficher les informations relatives au fichier audio ########  */
    
    fprintf(stdout,
    " Fichier audio : %s \n Frequence echantillonnage : %d \n Taille echantillonnage : %d \n Canal : %d \n", 
    caractereSaisie ,
    frequenceEchantillonnage,
    tailleEchantillonnage,
    canal);

    /* ############ Modification des parametres audio #############  */
    // frequenceEchantillonnage =
    // tailleEchantillonnage =
    // canal = 

    /* ##### recuperation du descripteur du fichier audio #########  */
    puts("");
     int ecriture_audio = aud_writeinit(frequenceEchantillonnage , tailleEchantillonnage , canal);
    puts("");

    //en cas d'erreur
    if (ecriture_audio == -1) return -1 ;
        
    bool arretDuSon = false;
    char audio_buffer[frequenceEchantillonnage];
    ssize_t  readAudio ,writeAudio ;

    while (!arretDuSon )
    {
        //lecture des echantillons dans audio_buffer à partir du descripteur de fichier (fd)
        readAudio = read (lecture_audio ,audio_buffer ,frequenceEchantillonnage);

        if(readAudio != frequenceEchantillonnage) arretDuSon = true ;

        //ecriture des echantillons à partir de audio_buffer
        writeAudio = write(ecriture_audio, audio_buffer , frequenceEchantillonnage);

        if (writeAudio != frequenceEchantillonnage)   arretDuSon = true ;

        // vider le buffer apres chaque passage.
		 bzero(audio_buffer, frequenceEchantillonnage);

    }

        close(lecture_audio);
        close(ecriture_audio);

    
    return 0;
}

