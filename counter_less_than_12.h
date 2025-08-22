#ifndef counter_less_than_12_HEADER
#define counter_less_than_12_HEADER

#include "Counter.h"
#include <string>
#include <atomic>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cmath>



class counter_less_than_12: public Counter
{

	constexpr static const int char_to_int[128]={-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                                 -1, -1, -1, -1, -1, 0, -1, 1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1,
                                                 -1, -1, -1, -1, -1, -1, 0, -1, 1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, -1, -1, -1, -1, -1,
                                                 -1, -1, -1, -1, -1,-1};

public:

	inline uint64_t bitcode(int i, char* str){
    	return int_to_bit[char_to_int[str[i]]][char_to_int[str[i+1]]][char_to_int[str[i+2]]][char_to_int[str[i+3]]];
	};
	
	counter_less_than_12(std::atomic_ullong* def_root_table, char* address_array, size_t file_size)	{

		//将4个碱基作为一组，即八个Bit作为一组。
		root_table = def_root_table;
		k = exe_arg.k;
		filename = exe_arg.filename;
		
		get_k_end=(1u<<(2*(block_len-4)))-1;;
		
		this->data = address_array;
        total_file_size = file_size;
		block_len=4*(k/4+(k%4>0)+1);
		
		//初始化将转化的数组
		for(int i=0;i<4;i++){
			for(int j=0;j<4;j++){
				for(int k=0;k<4;k++){
					for(int l=0;l<4;l++){
						int_to_bit[i][j][k][l]=(i<<6)^(j<<4)^(k<<2)^l;
					}
				}
			}
		}
		
		//每次移动时，取去除前面四位的数字
		get_k_mer[0]=((1ull<<(2*k))-1)<<(2*(block_len-k));
		for(int i=1;i<8;i++) get_k_mer[i]=get_k_mer[i-1]>>2;
		
	};
	
	//统计出现频率
	void count(size_t pos);
	//输出到文件
	void print(uint64_t min_cnt,uint64_t max_cnt);

	void reset(char *new_data, size_t new_total_file_size);

private:
	std::atomic_ullong* root_table;
	int block_len;
    int k;
	
    std::string filename;
	
	uint64_t get_k_end=0;
	uint64_t get_k_mer[8];
	uint64_t int_to_bit[4][4][4][4];
};
#endif