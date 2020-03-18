#ifndef LISTCLIENT_H_
#define LISTCLIENT_H_

#include "serveur.h"

typedef struct client {
	int s_fd;//socketfd
	int frequenceEchantillonnage;
    int audio_fd; // descripteur diu fichier audio
	int delait_attente ; // 60 s avant deconnexion automatique
	int delait_renvoie_de_trame ; // 5 s => 5000 * 1000 miliscd
	bool est_disponible ; //pour verifier si l'espace est disponible pour une connection
	bool en_lecture ; //le client connecté est en cours de lecture
	char titre_son[BUFFER_SIZE]; 
	struct  sockaddr_in  address ; //addresse du client
	struct packet data  ; 
} client;

#endif /*LISTCLIENT_H_*/