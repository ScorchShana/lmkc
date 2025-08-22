#ifndef LKMC_COUNTER_MORE_THAN_12_HEADER
#define LKMC_COUNTER_MORE_THAN_12_HEADER

#include "Counter.h"
#include <unordered_map>
#include "large_mer.h"
#include <mutex>


typedef std::unordered_map<uint64_t,uint64_t> mer_map;

class counter_more_than_12 : public Counter
{

    constexpr static const int char_to_int[128]={-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                                 -1, -1, -1, -1, -1, 0, -1, 1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1,
                                                 -1, -1, -1, -1, -1, -1, 0, -1, 1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, -1, -1, -1, -1, -1,
                                                 -1, -1, -1, -1, -1,-1};

public:
    counter_more_than_12(mer_map *def_root_table, int in_m, char* address_array, size_t file_size, std::mutex *in_locks, bool *in_reversed)
    {

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

        root_table = def_root_table;
        k = exe_arg.k;
        m = in_m;
        locks=in_locks;
        reserved=in_reversed;


	    m_block_len=4*(m/4+(m%4>0)+1);//前缀的碱基数
        suffix_block_len=4*((m%4+k-m)/4+((k%4>0)+1));//后缀的碱基数

        get_m_mer[0]=((1u<<(2*m))-1)<<(2*(m_block_len-m));
        for(int i=1;i<4;i++) get_m_mer[i]=get_m_mer[i-1]>>2;
        get_suffix_mer[0]=((1ull<<(2*(k-m)))-1)<<(2*(suffix_block_len-m%4-(k-m)));
        for(int i=1;i<4;i++) get_suffix_mer[i]=get_suffix_mer[i-1]>>2;

        block_len=4*(k/4+(k%4>0)+1);
        filename = exe_arg.filename;
        this->data = address_array;

        total_file_size = file_size;
    }

    
    void count(size_t pos);
    void print(uint64_t min_cnt,uint64_t max_cnt);
    void reset(char *new_data, size_t new_total_file_size);

private:
//    moodycamel::ConcurrentQueue<uint>* root_table;
    bool *reserved;
    std::mutex *locks;
    mer_map* root_table;
    int m,k;
    int block_len,m_block_len,suffix_block_len;
    string filename;
    uint64_t int_to_bit[4][4][4][4];
    uint32_t get_m_mer[4];
    uint64_t get_suffix_mer[4];
//    uint_64 get_first_m1;
};
#endif //KMC4_COUNTER_MORE_THAN_14_H
