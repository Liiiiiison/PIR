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
#include <system.h>
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
	
	system_init_without_cache(rom_size, ram_size, ram_start_addr);
	
	system_init_tracing();
	system_tracing_enable(SYSTEM_TRACING_REGISTER_IP);
	system_tracing_enable(SYSTEM_TRACING_POWER_RISCV_ISA);
	//system_tracing_enable(SYSTEM_TRACING_POWER_RISCV_REGISTER_FILE_BIT_0);
	
	//system_set_register(10,0x12345678);
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
