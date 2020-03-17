#ifndef LISTCLIENT_H_
#define LISTCLIENT_H_

#include "serveur.h"

typedef struct client {
	int s_fd;//socketfd
	int frequenceEchantillonnage;
    int audio_fd;
	int delait_attente ; // 60 s
	int delait_renvoie_de_trame ; // 5 s => 5000 * 1000 miliscd
	bool est_disponible ;
	bool en_lecture ;
	char titre_son[BUFFER_SIZE];
	struct  sockaddr_in  address ;
	struct packet data  ;
} client;

#endif /**/