#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "riscv_firmware_riscv_sim_cmd.h"




// ALICE SIDE
// BOB'S is just a subset of it without key generation
int main() {
	unsigned long alice_bases = 0; // = 0b1001'0110'1001'0000'0000'0011'0110'0000;// 0100 0000 0010 0000 0011 0000 0100 0000;
	unsigned long bob_bases = 0; // = 0b1011'0010'1001'0000'0000'0011'0010'0000;//0100 0000 0010 0000 0001 0001 0100 0000; 
	unsigned long decoded_bits = 0;
	//unsigned long mask = 0;

	// load les clés données au setup du simulateur grâce à system_set_register
	// petit trick pour bouger la valeur en dehors de x10 on fait un add immediat de 0
	asm("addi %0,x5,0" : "=r" (alice_bases));
	asm("addi %0,x6,0" : "=r" (bob_bases));
	asm("addi %0,x7,0" : "=r" (decoded_bits)); // masked value

	// mask
	//asm("addi %0,x8,0" : "=r" (mask));
	//decoded_bits = decoded_bits ^ mask;


	/*
	38368
	10090
	val: 10090 (0x0000276a)
	val: 10090 (0x0000276a)
	POURQUOI LA DERNIERE VALEUR EST SUR LES 2 VARIABLES ???

	wtf, le truc n'est appelé qu'une fois ???
	*/



	/*asm("csrrs %0, 0x015, x0" : "=r" (k1));
	riscv_sim_print_int(k1);
	
	asm("csrrs %0, 0x015, x0" : "=r" (k2));
	riscv_sim_print_int(k2);*/




	// remplacer par x5 et x6 car ceux la sont pour ole passage d'arguments ( t0 et t1 vs a0 et a1)
	riscv_sim_tracing_on

	char bit;
	//char mask_bit;
    unsigned long final_k = 0;
    unsigned long extracting_mask = ~(alice_bases ^ bob_bases);
    //riscv_sim_print_int(extracting_mask)
    unsigned int nb_zeros;
	unsigned int absolute_bit_pos = 0;
	unsigned int packing_pos = 0;
	//unsigned int final_mask = 0;

	// asm ( "assembly code" : output operands : input operands : clobbered registers );

    while(extracting_mask != 0) { // loops until the mask (k3) has no 1 bit

		asm("ctz %0,%1" : "=r" (nb_zeros) : "r" (extracting_mask));
		
		
		// do not increment relative position by 1 more yet
		// as index starts at 0 in registers
		// nb_zeros is the right index minus one
		absolute_bit_pos += nb_zeros; 

		// LEAK
		// l'instruction construit un masque par décalage 1<<n
		// puis extrait avec le masque
    	asm volatile ("bext %0,%1,%2" : "=r" (bit): "r" (decoded_bits) , "r" (absolute_bit_pos));
    	//asm volatile ("bext %0,%1,%2" : "=r" (mask_bit): "r" (mask) , "r" (absolute_bit_pos));

		absolute_bit_pos ++; // add 1 so we actually have the real bit index


		// LEAK 
        final_k |= bit << packing_pos; // places the bit value at the next position in final_k (packing)
		//asm volatile("addi t0,t0,0");
		//final_mask |= mask_bit << packing_pos;

		packing_pos++; 

		// mask with upper 16 bits random value
		// asm("lui %0,%1" : "=r" (final_k) :"r" ())

        extracting_mask = extracting_mask >> (nb_zeros + 1); // shifts extracting_mask so the last lowest weight bit 1 is gone
    }


	//final_k = final_k ^ final_mask;

	riscv_sim_tracing_off

	riscv_sim_print_str("[+] generated key :")
	riscv_sim_print_int(final_k)
	return 0;
}


/*

- bext %0, %1, %2	bext a4, a6, a5	Bit-Extraction Leakage: bext (Bit Extract) often shows significant side-channel signatures because it involves internal hardware muxing that correlates with the value of the bit being extracted.
- final_k |= bit << absolute_pos	sll, or	Value-Correlation: Shifting a secret bit (bit) by a variable amount (absolute_pos) creates a very distinct power signature that can reveal both the value of the bit and its index.
*/






/*:

- comment palier au bits de sécurité perdus par analyse de la single trace (nb de bits de clé en plus à échanger pour y palier)
- peut-on faire fuiter toute la clé par single trace ? Car clé symétrique de session donc usage unique
- peut-on récupérer les bits sur le module de commande avant même le packing ? 
- Analyser un module de commande moyen ou de l'industrie si on peut
- regarder les signaux pour piloter les sondes
- regarder concrêtement comment le circuit fait l'interface entre la sonde et le microcontroleur
- est-ce attaquable en une seule trace
- Simuler un module de commande en vhdl et les bits reçus pour récupérer une trace de consommation VCD
- montrer que le masquage devrait être symétrique et échangé car on ne veut pas démasquer en fin d'algo pour passer le secret masqué à AES
- et donc n'est pas compatible avec BB84 car cela reviendrait à échanger une clé 
- BIEN MODELISER LES DIFFERENTES PARTIES
- trouver un chemin d'attaque en donnant à chaque niveau
*/