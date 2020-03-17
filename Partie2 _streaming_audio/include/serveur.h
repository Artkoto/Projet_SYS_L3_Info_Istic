#ifndef SERVEUR_H_
#define SERVEUR_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <audio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/ip.h>


/* les constantes */
#define BUFFER_SIZE 1024
#define PORT 8586
#define CLIENT_MAX  10

#define SOCKET_ERROR -1

/* Nom de fichier */

#define AUDIO_DIR "audio/"
#define AUCUN_MSG ""



// Timeouts
#define TIMEOUT 5000000  // 5 seconds

// Message types
#define VIDE 0 
#define FILENAME 1 // nom de fichier
#define FILE_HEADER  2 // information du son 
#define ECHANTILLON  3 // echantillons
#define ACQUITTEMENT  4 // acquitement
#define MSG_ERREUR_SERVEUR_COMPLET   5 //message d'erreur
#define MSG_ERREUR_SON_INDISPONIBLE  6
#define MSG_ERREUR_CO_NON_AUTORISE  7
#define MSG_ERREUR_LECTURE_AUDIO 8
#define MSG_ERREUR_SOCKET  9
#define MSG_ERREUR_PORT  10
#define FIN_DU_SON 11
#define HEADER_MUSIC_RECU 12





// Structures

typedef struct headerSon {
	int frequenceEchantillonnage;
    int tailleEchantillonnage;
    int canal;
} headerSon;


typedef struct packet {
	int type;
	int id_client;
	char message[BUFFER_SIZE];
} packet;





// signatures des Methodes
static void init();
static void app();
static void fin();


static void create_packet(struct packet* pack, int type, void* content);
static void clear_packet(struct packet* pack);
static void copy_packet(struct packet* pack2 , struct packet* pack1);

static int init_connexion(int id_client);
static void fin_connexion(int rmfd);

static void envoye_packet(int sock, struct  sockaddr_in *addrclient, struct packet *buffer);
static socklen_t lire_packet(int sock, struct  sockaddr_in *addrclient, struct packet *buffer);

static bool add_client( struct packet* pack ); //on attribut un id au client 
static void remove_client(int id_client ); //

static int calcul_fdMax(int sfd , int fdmax);

static void fdSet();

static void audio_path(const char *filename ,  char *audioPath);



#endif /**/
