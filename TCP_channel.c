#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <complex.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
int port = 3333;

void send_data(unsigned int * tab, int nb_message) {
    int sock_tcp = -1;
    struct hostent * hp ;
    struct sockaddr_in addr_serveur ;
    if ((sock_tcp=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))==-1){
        printf("[étape 0] échec de la création du socket\n");
        exit(1);
    }
    printf("socket créé\n");

    //creation adresse serveur
    memset((char*)&addr_serveur, 0, sizeof(addr_serveur));
    addr_serveur.sin_family = AF_INET ;
    printf("port %d\n", port);
    addr_serveur.sin_port = htons(port);
    addr_serveur.sin_addr.s_addr = INADDR_ANY ;
    unsigned int lg_addr_serveur = sizeof(addr_serveur);
    if ((hp = gethostbyname("localhost"))==NULL){
        printf("[etape 1 : construction ] erreur gethostbyname\n");
        exit(1);
    }
    printf("adresse part 2 créé\n");
    memcpy((char*)&(addr_serveur.sin_addr.s_addr),hp->h_addr_list[0], hp->h_length);
    printf("adresse part 3 créé\n");

    // demande de connexion
    if (connect(sock_tcp,(struct sockaddr*)&addr_serveur, lg_addr_serveur)==-1){
        printf("[etape 4 : demande de connexion] erreur connect");
        exit(1);
    } 
    printf("Alice : envoi\n");
    int lg_emis = -1 ;
    for (int i=0; i <nb_message; i++){
        if ((lg_emis=write(sock_tcp, &tab[i], sizeof(tab[i])))==-1){
            perror("[etape 2 : ] erreur sendto");
            exit(1);
        }
    }
    printf("Alice : fin\n");
    close(sock_tcp);
    
}

void receive_data(unsigned int * tab, int * nb_message) {
    int sock_tcp = -1;
    int sock_tcp_bis = -1;
    struct sockaddr_in addr_serveur ;
    struct sockaddr_in addr_client ;
    unsigned int lg_addr_client = sizeof(addr_client);
    int max_file=5;
    int lg_rec;
    if ((sock_tcp=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))==-1){
        printf("[étape 0] échec de la création du socket\n");
        exit(1);
    }
    printf("socket créé\n");

    //creation adresse
    memset((char*)&addr_serveur, 0, sizeof(addr_serveur));
    addr_serveur.sin_family = AF_INET ;
    printf("port %d\n", port);
    addr_serveur.sin_port = htons(port);
    addr_serveur.sin_addr.s_addr = INADDR_ANY ;
    unsigned int lg_addr_serveur= sizeof(addr_serveur);
        //faire avec getsockname 

    //bind
    if (bind(sock_tcp, (struct sockaddr *)&addr_serveur, lg_addr_serveur)==-1){
        printf("[etape 2 : bind @socket <-> représentation interne] erreur bind\n");
        exit(1);
    }

    //dimensionnement de la file
    if (listen(sock_tcp, max_file)==-1){
        printf("[etape 3] erreur dimensionnement de la file");
        exit(1);
    }

    //acceptation d'une demande de connexion
    if ((sock_tcp_bis = accept(sock_tcp, (struct sockaddr *)&addr_client, &lg_addr_client)) == -1)
        {printf("échec du accept\n") ;
        exit(1) ;
    }

    unsigned int received ;
    printf("Bob : réception\n");
    int nb_reception = 0;
    while (1) {
        if ((lg_rec = read(sock_tcp_bis, &received, sizeof(received))) == -1){
            printf("échec du read\n") ; exit(1) ;
        }
        else if(lg_rec==0){ /* si on ne recoit rien on shutdown*/
            if (shutdown(sock_tcp_bis, 2) == -1){
                printf("Erreur shutdown\n");
            }
            close(sock_tcp_bis);
            break ;
        }
        else {
            tab[nb_reception]=received;
            nb_reception += 1 ;
        }     
    }
    *nb_message = nb_reception;
    printf("Bob : fin\n");
}