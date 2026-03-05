#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "riscv_firmware_riscv_sim_cmd.h"


unsigned long k1 = 0b1001'0110'1001'0000'0000'0011'0110'0000;// 0100 0000 0010 0000 0011 0000 0100 0000;
unsigned long k2 = 0b1011'0010'1001'0000'0000'0011'0010'0000;//0100 0000 0010 0000 0001 0001 0100 0000; 
// masque = 0xdbffffbf

/*unsigned long long gen_reconciliation_key(void) {
    unsigned long bit;
    unsigned long long final_k = 0;
    unsigned long long k3 = ~(k1 ^ k2);
    
    int pos = 0;
    int nb_zeros;

	// asm ( "assembly code" : output operands : input operands : clobbered registers );

    while((k3 != 0) && (pos < 64)) { // loops until the mask (k3) has no 1 bit

		asm("ctz %0,%1" : "=r" (nb_zeros) : "r" (k3));
    	asm("bext %0,%1,%2" : "=r" (bit): "r" (k3) , "r" (nb_zeros));
        final_k |= bit << pos; // places the bit value at the next position in final_k (packing)
        k3 = k3 >> (nb_zeros + 1); // shifts k3 so the last lowest weight bit 1 is gone
        pos += 1;
		riscv_sim_print_str("generation key bit number :")
		riscv_sim_print_int(pos)
		riscv_sim_print_str("k3 =")
		riscv_sim_print_int(k3)

    }

	riscv_sim_print_str("returning from key reconciliation")

    return final_k;
}*/




int main() {

	//unsigned long long key = gen_reconciliation_key();


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
	return 0;
}
