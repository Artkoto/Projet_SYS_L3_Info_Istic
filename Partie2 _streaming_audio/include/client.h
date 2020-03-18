#ifndef CLIENT_H_
#define CLIENT_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <audio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/ip.h>

/* les constantes */
#define BUFFER_SIZE 1024
#define PORT 8586

#define SOCKET_ERROR -1
#define AUCUN_MSG ""


// Timeouts
#define TIMEOUT 60000000  // 1 min

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

static void param(int argc, char const *argv[]);
static void recup_audio_header( char *header);


static void create_packet(struct packet* pack, int type, void* content);
static void clear_packet(struct packet* pack);

static int init_connexion(const char *address, struct  sockaddr_in * addrserveur);
static void fin_connexion(int sockfd);
static void envoye_packet(int sock, struct  sockaddr_in *addrserveur, struct packet *buffer);
static socklen_t lire_packet(int sock, struct  sockaddr_in *addrserveur, struct packet *buffer);



#endif /*CLIENT_H_*/

