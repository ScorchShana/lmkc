#ifndef LARGE_MER_HEADER
#define LARGE_MER_HEADER

#include <stdint.h>


class large_mer{

    constexpr static const int char_to_int[128]={-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                                 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                                 -1, -1, -1, -1, -1, 0, -1, 1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1,
                                                 -1, -1, -1, -1, -1, -1, 0, -1, 1, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3, -1, -1, -1, -1, -1,
                                                 -1, -1, -1, -1, -1,-1};

    public:
        uint32_t m_mer;
        uint64_t suffix_mer;

        large_mer(int in_m,int in_k) : m_mer(0),suffix_mer(0),m(in_m), k(in_k){
            
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

            m_block_len=4*(m/4+(m%4>0)+1);//前缀的碱基数
			suffix_block_len=4*((m%4+k-m)/4+((k%4>0)+1));

            get_suffix_mer_end=(1ull<<(2*(suffix_block_len-4)))-1;
            get_m_mer_end=(1u<<(2*(m_block_len-4)))-1;
            get_one_suffix_mer_end=(1ull<<(2*(suffix_block_len-1)))-1;
            get_one_m_mer_end=(1u<<(2*(m_block_len-1)))-1;

            get_suffix_to_m=uint64_t(0b11111111)<<(2*(suffix_block_len-4-4*(m%4>0)));//get suffix_to_m first, then suffix move , and put the front 4 bases into the last of the m-mer
            get_one_suffix_to_m=uint64_t(0b11)<<(2*(suffix_block_len-4-4*(m%4>0)));
        }

        void get_end();//将k_mer左移（超过位数则丢弃），为拼接做准备
        void get_one_end();//将k_mer左移一个碱基
        void get_next_k_mer(char* readline);//将后面四位碱基凭拼接入k-mer
        void get_next_one_k_mer(char* readline);//将后面一位碱基凭拼接入k-mer

    private:
        uint64_t get_suffix_mer_end,get_one_suffix_mer_end;//获取除去前面四个（或1个）碱基的suffix-mer，为拼接为下一个k-mer做准备
        uint32_t get_m_mer_end,get_one_m_mer_end;//获取除去前面四个（或1个）碱基的m-mer，为拼接为下一个k-mer做准备
        uint64_t get_suffix_to_m,get_one_suffix_to_m;//拼接为下一个k-mer时，有一部分suffix的块要移动到m-mer
        int m_block_len,suffix_block_len;//m-mer和suffix-mer的块的碱基数
        int m,k;
        uint64_t int_to_bit[4][4][4][4];
        
};

#endif 


