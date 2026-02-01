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

int nb_message = 100 ;

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

void find_key(unsigned char * tab_bases_1, int len1, unsigned char * tab_bases_2, int len2,unsigned short * tab_bit, unsigned short * key, int * key_length) {
    int k = 0;
    if (len1==len2){
        for (int i = 0; i<len1 ;i++){
            if (tab_bases_1[i]==tab_bases_2[i]){
                key[k]=tab_bit[i];
                k++;
            }
        }
    }
    *key_length = k;
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
        unsigned short tab_bit_alice[nb_message];
        unsigned int tab_state_alice[nb_message];
        unsigned char tab_base_alice[nb_message];       
        generate_rand_bit(tab_bit_alice);
        generate_rand_bases(tab_base_alice);
        generate_states(tab_bit_alice, tab_base_alice, tab_state_alice);
        printf("ALICE |\n");
        printf("états : ");
        for (int i=0; i < nb_message; i++){
            printf("%d-", tab_state_alice[i]);
        }
        printf("\n");
        printf("bases : ");
        for (int i=0; i < nb_message; i++){
            printf("%c", tab_base_alice[i]);
        }
        printf("\n");
        printf("bits : ");
        for (int i=0; i < nb_message; i++){
            printf("%d", tab_bit_alice[i]);
        }
        printf("\n");
        printf("-------------------------------------------\n");

        connect_alice();
        send_data(tab_state_alice, nb_message);
        unsigned char tab_base_bob[nb_message];
        receive_base(tab_base_bob, &nb_message);
        send_base(tab_base_alice,nb_message);
        unsigned short key[nb_message];
        int key_length;
        find_key(tab_base_alice,sizeof(tab_base_alice),tab_base_bob,sizeof(tab_base_bob),tab_bit_alice,key,&key_length);
        printf("KEY : ");
        for (int i=0;i<key_length;i++){
            printf("%d",key[i]);
        }
        printf("\n");
        printf("-------------------------------------------\n");
        wait_for_bob();
    } else {
        //recoit les états
        listen_bob();
        unsigned int tab_state_bob[nb_message];
        receive_data(tab_state_bob, &nb_message);
        //génère ses bases
        unsigned char tab_base_bob[nb_message];
        generate_rand_bases(tab_base_bob);
        //decode le message
        unsigned short tab_bit_bob[nb_message];
        decode_data(tab_state_bob,tab_base_bob,tab_bit_bob);
        printf("BOB |\n");
        printf("états : ");
        for (int i=0; i < nb_message; i++){
            printf("%d-", tab_state_bob[i]);
        }
        printf("\n");
        printf("bases : ");
        for (int i=0; i < nb_message; i++){
            printf("%c", tab_base_bob[i]);
        }
        printf("\n");
        printf("bits : ");
        for (int i=0; i < nb_message; i++){
            printf("%d", tab_bit_bob[i]);
        }
        printf("\n");
        //renvoie ses bases
        send_base(tab_base_bob, nb_message);
        unsigned char tab_base_alice[nb_message];
        receive_base(tab_base_alice, &nb_message);
        unsigned short key[nb_message];
        int key_length;
        find_key(tab_base_alice,sizeof(tab_base_alice),tab_base_bob,sizeof(tab_base_bob),tab_bit_bob,key,&key_length);
        printf("KEY : ");
        for (int i=0;i<key_length;i++){
            printf("%d",key[i]);
        }
        printf("\n");
        printf("-------------------------------------------\n");
        disconnect_bob();
    } 
}
