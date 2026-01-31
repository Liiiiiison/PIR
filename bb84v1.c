/* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>
#include <time.h> 
#include "TCP_channel.c"

int nb_message =-1 ;

void generate_rand_bit(unsigned short * tab){
    for (int i = 0;i<nb_message;i++){
        tab[i] = (unsigned short) rand()%2; 
    } 
} 

void generate_rand_bases(unsigned char * tab){
    int v;
    for (int i = 0;i<nb_message;i++){
        v = rand()%2; 
        if (v){
            tab[i]='+'; 
        } else {
            tab[i]='x'; 
        } 
    }  
} 

void generate_states(unsigned short * tab_bit, unsigned char * tab_bases, unsigned int * tab_state){ 
    for (int i = 0; i < nb_message;i++){
        if (tab_bases[i] == '+'){
            if (tab_bit[i]){
                tab_state[i] = 90;
            } else {
                tab_state[i]= 0; 
            } 
        } else {
            if (tab_bit[i]){
                tab_state[i] = 135;
            } else {
                tab_state[i]= 45; 
            } 
        } 
    } 
} 

void decode_data(unsigned int * tab_state, unsigned char * tab_bases, unsigned short * tab_bit){ 
    for (int i = 0; i < nb_message;i++){
        if (tab_bases[i] == '+'){
            if (tab_state[i]==0){
                tab_bit[i] = 0;
            } else if (tab_state[i]==90){
                tab_bit[i]= 1; 
            } else {
                tab_bit[i]=1; //arbitraire
            }
        } else {
            if (tab_state[i]==135){
                tab_bit[i] = 1;
            } else if (tab_state[i]==45){
                tab_bit[i]= 0; 
            } else {
                tab_bit[i]=1; //arbitraire
            }
        } 
    } 
} 

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
int main (int argc, char **argv)
{
    srand( time( NULL ) );
	int c;
	extern char *optarg;
	extern int optind;
    int alice = -1 ;
    int mode = 0 ;
	while ((c = getopt(argc, argv, "abn:")) != -1) {
		switch (c) {
		case 'a':
            if (mode) {
                fprintf(stderr, "Erreur: -a et -b sont exclusifs\n");
                exit(EXIT_FAILURE);
            }
            mode = 'a';
			alice = 1;
			break;
		case 'b':
            if (mode) {
                fprintf(stderr, "Erreur: -a et -b sont exclusifs\n");
                exit(EXIT_FAILURE);
            }
            mode = 'b';
			alice = 0;
			break;
		case 'n':
			nb_message = atoi(optarg);
			break;
		default:
            printf("default\n");
			printf("usage: cmd [-a|-b][-n ##]\n");
			break ;
		}
	}

    if (alice == -1 || (alice==1 && nb_message < 0 )) {
        printf("usage: cmd [-a|-b][-n ##]\n");
        exit(1) ;
    }

    if (alice) {
        unsigned short tab_bit[nb_message];
        unsigned char tab_bases[nb_message];
        unsigned int tab_state[nb_message];
        generate_rand_bit(tab_bit);
        generate_rand_bases(tab_bases);
        generate_states(tab_bit, tab_bases, tab_state);
        for (int i=0; i < nb_message; i++){
            printf("%d bit :%d, base : %c, state :%d\n", i, tab_bit[i], tab_bases[i], tab_state[i]);
        }
        send_data(tab_state, nb_message);
    } else {
        unsigned int tab_state[1000];
        receive_data(tab_state, &nb_message);
        unsigned short tab_bit[nb_message];
        unsigned char tab_bases[nb_message];
        generate_rand_bases(tab_bases);
        decode_data(tab_state,tab_bases,tab_bit);
        for (int i=0; i < nb_message; i++){
            printf("%d state :%d, base : %c, bit :%d\n", i, tab_state[i], tab_bases[i], tab_bit[i]);
        }
    } 
}
