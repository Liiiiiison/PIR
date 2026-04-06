/*
    IMPRISE - RISC-V simulator built for side-channel analysis
    Copyright (C) 2025  Vincent Migliore

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vc_vector.h>
#include <system_tracing.h>


int main(int argc, char* argv[]) {

	size_t rom_size                = 0x10000000;
	size_t ram_size                = 0x20000000;
	uint32_t ram_start_addr        = 0x10000000;
	size_t   cache_line_size       = 64;
	uint32_t cache_ways            = 2;
	uint32_t cache_bank_partitions = 8;
	uint32_t ram_bus_size          = 64;
	char* filename = "firmware/firmware.bin";


	unsigned long key1_fixed = 0b1001'0110'1001'0000'0000'0011'0110'0000;// 0100 0000 0010 0000 0011 0000 0100 0000;
	unsigned long key2_fixed = 0b1011'0010'1001'0000'0000'0011'0010'0000;//0100 0000 0010 0000 0001 0001 0100 0000; 

	system_init_without_cache(rom_size, ram_size, ram_start_addr);
	
	system_init_tracing();
	system_tracing_enable(SYSTEM_TRACING_REGISTER_IP);
	system_tracing_enable(SYSTEM_TRACING_POWER_RISCV_ISA);
	//system_tracing_enable(SYSTEM_TRACING_POWER_RISCV_REGISTER_FILE_BIT_0);
	

	// put args in x10 and x11 registers before loading firmware
	srand(time(NULL));
	uint32_t base_bob_rand = rand();
	printf("Bob:%x\n",base_bob_rand);
	uint32_t base_alice_rand = rand();
	printf("Alice:%x\n",base_alice_rand);
	uint32_t alice_key_bits_rand = rand();
	printf("Alice's decoded key bits:%x\n",alice_key_bits_rand);

	uint32_t mask = rand();
	printf("Alice's mask:%x\n",mask);




	uint32_t answer = 0;
	uint32_t match_count = 0;
	uint32_t bit = 0;
	for(int i = 0;i<32;i++){

		if((base_bob_rand & (1<<i)) == ((base_alice_rand & (1<<i)))){
			// on fait genre la base de bob c'est aussi la clé lue par bob
			bit = (alice_key_bits_rand & (1<<i)) >> i;
			answer |= (bit<<match_count);
			match_count++; 
		}
	}

	printf("answer:%d\n",answer);

	system_set_register(5,base_bob_rand);
	system_set_register(6,base_alice_rand);
	system_set_register(7,alice_key_bits_rand);
	system_set_register(8,mask);


	system_load_firmware(filename);
	system_run();

	// Trace vector //

	vc_vector* riscv_instruc_trace       = system_tracing_get_vector(SYSTEM_TRACING_REGISTER_IP    );
	vc_vector* riscv_leakage_trace       = system_tracing_get_vector(SYSTEM_TRACING_POWER_RISCV_ISA);
	//vc_vector* riscv_bit_0_leakage_trace = system_tracing_get_vector(SYSTEM_TRACING_POWER_RISCV_REGISTER_FILE_BIT_0);
	


	printf("size of power leakage vector : %d \n",vc_vector_count(riscv_leakage_trace));
	// Print vector //

	printf("Leakage vector: [\n");
	void*  i = vc_vector_begin(riscv_leakage_trace);
	void*  j = vc_vector_begin(riscv_instruc_trace);
	//void*  k = vc_vector_begin(riscv_bit_0_leakage_trace);
	for (;
		((i != vc_vector_end(riscv_leakage_trace      )) &&
		 (j != vc_vector_end(riscv_instruc_trace      )) /*&&
		 (k != vc_vector_end(riscv_bit_0_leakage_trace))*/); 
		i = vc_vector_next(riscv_leakage_trace      ,i),
		j = vc_vector_next(riscv_instruc_trace      ,j) /*,
		k = vc_vector_next(riscv_bit_0_leakage_trace,k)*/){
		printf("0x%08lx: %u \n", *(uint32_t*)j, *(uint16_t*)i /*, *(uint8_t*)k*/);
	}
	
	printf("]\n");
	// Profit //
	return 0;
}
