#include "t_test.h"
#include <random>
#include <algorithm>

extern "C" {

uint32_t maps_t_test_example()
{        
        int n_measure = 10000;
	Ttest *ttest_ptr = nullptr;
	std::random_device random_dev;
	std::mt19937 rnd_gen_uint32(random_dev());
	std::vector<unsigned int> trace;
	const uint32_t a_fixed = 0xb1a6f5e5;
	const uint32_t b_fixed = 0x127cbff6;
	// Capturing fixed and random input traces //
	for(unsigned long int measure_idx = 0; measure_idx < n_measure; ++measure_idx)
	{
		uint32_t a[2];
		uint32_t b[2];
		uint32_t y[2];
		uint32_t r;
		/// Fixed ///
		a[1] = rnd_gen_uint32();
		a[0] = a[1] ^ a_fixed;
		b[1] = rnd_gen_uint32();
		b[0] = b[1] ^ rnd_gen_uint32();
		r = rnd_gen_uint32();
		/** capturing samples and storing them into a vector called 'trace' **/
		if (measure_idx == 0)
		{
			ttest_ptr = new Ttest(trace.size());
		}
		ttest_ptr->update1(trace);
		/// random ///
		a[1] = rnd_gen_uint32();
		a[0] = a[1] ^ rnd_gen_uint32();
		b[1] = rnd_gen_uint32();
		b[0] = b[1] ^ rnd_gen_uint32();
		r = rnd_gen_uint32();
		cpu.reset();
		/ ** capturing samples and storing them into a vector called 'trace' **/
		ttest_ptr->update2(trace);
	}
	std::vector<double> t = ttest_ptr->t_test();
	std::vector<uint32_t> t_fail;
	for(size_t i = 0; i < t.size(); ++i)
	{
		if((t[i] > 4.5) || (t[i] < -4.5)) t_fail.push_back(i);
	}
	t_fail.erase( std::unique( t_fail.begin(), t_fail.end() ), t_fail.end() );
	if(t_fail.size() >= 1){
		/** leak at some samples **/
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
