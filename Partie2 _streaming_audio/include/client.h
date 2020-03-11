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


// Timeouts
#define TIMEOUT 5000000  // 5 seconds

// Message types
#define VIDE 0 
#define FILENAME 1 // nom de fichier
#define INFOSON  2 // information du son 
#define ECHANTILLON  3 // echantillons
#define ACQUITTEMENT  4 // acquitement
#define MSG_ERREUR_SERVEUR_COMPLET   5 //message d'erreur
#define MSG_ERREUR_CO_NON_AUTORISE  7
#define FIN_DU_SON 8

// Structures

typedef struct infoSon {
	int frequenceEchantillonnage;
    int tailleEchantillonnage;
    int canal;
} infoSon;

// typedef struct echantillon {
//     const int type = ECHANTILLON;
// 	char race[BUFFER_SIZE;]
// } echantillon;

// typedef struct filename {
//     const int  type = FILENAME;
// 	char name[BUFFER_SIZE;]
// } filename;

// typedef struct acquittement {
//     const int type = ACQUITTEMENT;
// 	char acquit[BUFFER_SIZE;]
// } acquittement;

// typedef struct meassageErr {
//     const int type = MSG_ERREUR;
// 	char err[BUFFER_SIZE;]
// } meassageErr;

typedef struct packet {
	int type;
	int id_client;
	void *message;
} packet;





// signatures des Methodes
static void init();
static void app(/*const char *address , const char *name*/);
static void fin(int socket);

static void param(int argc, char const *argv[]);

static void create_packet(struct packet* pack, int type, void* content);
static void clear_packet(struct packet* pack);

static int init_connexion(const char *address, struct  sockaddr_in * addrserveur);
static void fin_connexion(int sockfd);
static void envoye_packet(int sock, struct  sockaddr_in *addrserveur, struct packet *buffer);
static socklen_t lire_packet(int sock, struct  sockaddr_in *addrserveur, struct packet *buffer);









