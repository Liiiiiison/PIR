#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "riscv_firmware_riscv_sim_cmd.h"




int main() {
	unsigned long k1 = 0; // = 0b1001'0110'1001'0000'0000'0011'0110'0000;// 0100 0000 0010 0000 0011 0000 0100 0000;
	unsigned long k2 = 0; // = 0b1011'0010'1001'0000'0000'0011'0010'0000;//0100 0000 0010 0000 0001 0001 0100 0000; 


	// load les clés données au setup du simulateur grâce à system_set_register
	// petit trick pour bouger la valeur en dehors de x10 on fait un add immediat de 0
	asm("addi %0,x5,0" : "=r" (k1));
	asm("addi %0,x6,0" : "=r" (k2));
		 // remplacer par x5 et x6 car ceux la sont pour ole passage d'arguments ( t0 et t1 vs a0 et a1)
	riscv_sim_tracing_on

	char bit;
    unsigned long final_k = 0;
    unsigned long k3 = ~(k1 ^ k2);
    riscv_sim_print_int(k3)
    int pos = 0;
    unsigned int nb_zeros;

	unsigned int absolute_pos = 0;

	// asm ( "assembly code" : output operands : input operands : clobbered registers );

    while(k3 != 0) { // loops until the mask (k3) has no 1 bit

		asm("ctz %0,%1" : "=r" (nb_zeros) : "r" (k3));
		/*riscv_sim_print_str("bit match : ")
		riscv_sim_print_int(nb_zeros)*/
		
		
		// do not increment relative position by 1 more yet
		// as index starts at 0 in registers
		// nb_zeros is the right index minus one
		absolute_pos += nb_zeros; 


    	asm("bext %0,%1,%2" : "=r" (bit): "r" (k1) , "r" (absolute_pos));
		absolute_pos += 1; // add 1 so we actually have the real bit index

        final_k |= bit << pos; // places the bit value at the next position in final_k (packing)
        k3 = k3 >> (nb_zeros + 1); // shifts k3 so the last lowest weight bit 1 is gone
        pos += 1;


		/*riscv_sim_print_str("generation key bit number :")
		riscv_sim_print_int(pos)*/
		//riscv_sim_print_str("value = ")


		//riscv_sim_print_int(bit)

    }

	//riscv_sim_print_str("[+] generated key :")
	//riscv_sim_print_int(final_k)

	//riscv_sim_print_str("[+] bit packing terminé\n")
	//riscv_sim_print_str(test)

	riscv_sim_tracing_off

	return 0;
}
