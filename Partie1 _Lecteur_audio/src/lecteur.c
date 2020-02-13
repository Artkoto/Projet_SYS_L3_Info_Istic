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

    //si l'itulisateur oublie de mettre le nom du fichier en argument lors de l'execution.
    if (argc <= 1 ) { 
        perror("vous avez oublié de mettre le nom du fichier en parametre.\n");
        printf("Entrez le ici : ");
        fgets(caractereSaisie ,( MAX_FILENAME_SIZE / sizeof(char)), stdin);
        *(caractereSaisie + strlen(caractereSaisie)-1) = '\0';
         }
         
    //si l'itulisateur met le nom du fichier en argument lors de l'execution.
    else
    {
        strcpy(caractereSaisie , argv[1]);
    }

    //on range le chemin du fichier dans fileName

    strcpy(fileName,audioPath);
    strcat(fileName , caractereSaisie);
 
    /* ######### RECUPERATION DE LA DESCRIPTION DU FICHIER ############# */

    puts("");
    int lecture_audio = aud_readinit(fileName , &frequenceEchantillonnage, &tailleEchantillonnage, &canal);  
    puts("");

    /* #### AFFICHAGE DES INFORMATIONS RELATIVES AU FICHIER AUDIO ########  */
    
    fprintf(stdout,
    " Fichier audio : %s \n Frequence echantillonnage : %d \n Taille echantillonnage : %d \n Canal : %d \n", 
    caractereSaisie ,
    frequenceEchantillonnage,
    tailleEchantillonnage,
    canal);

    /* ############ MODIFFICATION DES PARAMETRE AUDIO #############  */
    // frequenceEchantillonnage =
    // tailleEchantillonnage =
    // canal = 

    /* ##### recuperer la description du fichier audio #########  */
    puts("");
     int ecriture_audio = aud_writeinit(frequenceEchantillonnage , tailleEchantillonnage , canal);
    puts("");
 
    bool arretDuSon = false;
    char audio_buffer[frequenceEchantillonnage];
    int  readAudio ,writeAudio ;
    while (!arretDuSon)
    {
        //
        readAudio = read (lecture_audio ,audio_buffer ,frequenceEchantillonnage);

        //
        writeAudio = write(ecriture_audio, audio_buffer , frequenceEchantillonnage);

        // Clear the buffer after each pass
		// bzero(audio_buffer, frequenceEchantillonnage);

         if (readAudio != frequenceEchantillonnage || writeAudio != frequenceEchantillonnage) {
             
         arretDuSon = true ;

            }
    }

        close(lecture_audio);
        close(ecriture_audio);

    
    return 0;
}

