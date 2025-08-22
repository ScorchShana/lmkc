#ifndef Counter_H
#define Counter_H
#include "definition.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>


typedef moodycamel::ConcurrentQueue<char*> Concurrent_Queue_char;
typedef moodycamel::ConcurrentQueue<c_reads> Concurrent_Queue_c_reads;


class Counter
{
public:
	virtual void count(size_t pos) = 0;
//	void release_fna_block(Concurrent_Queue_char* address_array, Concurrent_Queue_c_reads* creads_list_addr);
	virtual void print(uint64_t min_cnt,uint64_t max_cnt) = 0;
	virtual ~Counter(){};
	virtual void reset(char *new_data, size_t new_total_file_size)=0;

protected:
	int k;
	string filename;
	atomic<uint_32> block_sum;
	int c2k_24;
	char* data;
	size_t total_file_size;
};


#endif