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
#include<netinet/ip.h>



int main(int argc, char const *argv[])

{
   // int fd , err ;

    struct  sockaddr_in addr;
    
       
    int id_sock = socket(AF_INET , SOCK_DGRAM , 0);
    //
    if (id_sock == -1 ) 
    {
        puts("non");
    } else puts("oui");
    //

    addr.sin_family         = AF_INET ;
    addr.sin_port           = htons (1235);
    addr.sin_addr.s_addr    = htonl (INADDR_ANY);

   int  id_bind = bind(id_sock, (struct sockaddr *)&addr , sizeof(struct sockaddr_in) );
    //
    if (id_bind == -1 ) 
    {
        puts("non2");
    } else puts("oui2");
    //
    
    return 0;
}

