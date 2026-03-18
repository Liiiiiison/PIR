#include "t_test.h"
#include <random>
#include <algorithm>
#include <iostream>
#include <sstream>


extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <system.h>
#include <vc_vector.h>
#include <system_tracing.h>

int main()
{        


	size_t rom_size                = 0x10000000;
	size_t ram_size                = 0x20000000;
	uint32_t ram_start_addr        = 0x10000000;
	size_t   cache_line_size       = 64;
	uint32_t cache_ways            = 2;
	uint32_t cache_bank_partitions = 8;
	uint32_t ram_bus_size          = 64;
	char* filename = "../firmware/firmware.bin";


    int n_measure = 10000;
	Ttest *ttest_ptr = nullptr;
	std::random_device random_dev;
	std::mt19937 rnd_gen_uint32(random_dev());
	std::vector<unsigned int> trace1;
	std::vector<unsigned int> trace2;

	const uint32_t key1_fixed = 0b1001'0110'1001'0000'0000'0011'0110'0000;
	const uint32_t key2_fixed = 0b1011'0010'1001'0000'0000'0011'0010'0000;
	system_init_without_cache(rom_size, ram_size, ram_start_addr);

	system_init_tracing();
	system_tracing_enable(SYSTEM_TRACING_POWER_RISCV_ISA);
	system_load_firmware(filename);


	// Capturing fixed and random input traces //
	for(unsigned long int measure_idx = 0; measure_idx < n_measure; ++measure_idx)
	{
		// uint32_t a[2];
		// uint32_t b[2];
		// uint32_t y[2];
		// uint32_t r;


		/// Fixed + MASQUED input (a) ///
		// a[1] = rnd_gen_uint32();
		// a[0] = a[1] ^ a_fixed;
		// b[1] = rnd_gen_uint32();
		// b[0] = b[1] ^ rnd_gen_uint32();
		// r = rnd_gen_uint32();

		// x10 and x11
		system_set_register(5,key1_fixed);
		system_set_register(6,key2_fixed);
		// tout faire sur la même instance de simulateur
		system_run();

		// simulator output : C-style trace vector //

		vc_vector* riscv_leakage_trace = system_tracing_get_vector(SYSTEM_TRACING_POWER_RISCV_ISA);
		

		printf("size of power leakage vector : %d \n",vc_vector_count(riscv_leakage_trace));


		// convert c vector simulator output to c++ std::vector trace //
		void*  i = vc_vector_begin(riscv_leakage_trace);

		//void*  k = vc_vector_begin(riscv_bit_0_leakage_trace);
		for (;
			(i != vc_vector_end(riscv_leakage_trace)); 
			i = vc_vector_next(riscv_leakage_trace,i)
		){
			trace1.push_back(*(uint16_t*)i);
		}
		

		/** capturing samples and storing them into a vector called 'trace' **/
		if (measure_idx == 0)
		{
			ttest_ptr = new Ttest(trace1.size());
		}
		ttest_ptr->update1(trace1);
		trace1.clear();

		system_reset();
		system_tracing_free_vectors();



		

		/// random input keys ///

		uint32_t key1_random = rnd_gen_uint32();
		uint32_t key2_random = rnd_gen_uint32();
		
		// here, we are supposed to use a and b as input to get a trace of our algorithm

		system_set_register(5,key1_random);
		system_set_register(6,key2_random);

		system_load_firmware(filename);
		system_run();

		// simulator output : C-style trace vector //

		riscv_leakage_trace = system_tracing_get_vector(SYSTEM_TRACING_POWER_RISCV_ISA);
		

		printf("size of power leakage vector : %d \n",vc_vector_count(riscv_leakage_trace));
		// convert c vector simulator output to c++ std::vector trace //
		void*  vc_index = vc_vector_begin(riscv_leakage_trace);

		//void*  k = vc_vector_begin(riscv_bit_0_leakage_trace);
		for (;
			(vc_index != vc_vector_end(riscv_leakage_trace)); 
			vc_index = vc_vector_next(riscv_leakage_trace,vc_index)
		){
			trace2.push_back(*(uint16_t*)vc_index);
		}
		
		/** capturing samples and storing them into a vector called 'trace' **/
		ttest_ptr->update2(trace2);
		trace2.clear();


		system_reset();
		system_tracing_free_vectors();




	}
	std::vector<double> t = ttest_ptr->t_test();
	std::vector<uint32_t> t_fail;




	// t.size() = sample size as we just apply welch's formula on the sample set for each index of sample
	// so if we ad NOP
	std::stringstream ss;
	for(size_t i = 0; i < t.size(); ++i)
	{
		if((t[i] > 4.5) || (t[i] < -4.5)){
			t_fail.push_back(i);
			ss <<i;// only for printing purposes
			ss<<';';
		} 
	}
	t_fail.erase( std::unique( t_fail.begin(), t_fail.end() ), t_fail.end() );
	if(t_fail.size() >= 1){
		/** leak at some samples **/

		std::cout<<"[LEAK] at index(s) "<<ss.str()<<std::endl;
	}else{
		/** no leak **/
	}
	if (ttest_ptr != nullptr)
	{
		delete ttest_ptr;
	}
	return 0;
}

}
 