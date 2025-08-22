#include "counter_more_than_12.h"


void counter_more_than_12::count(size_t pos) {


    int length = pos+4096>total_file_size?total_file_size-pos:4096;
    char* reads_address = data + pos;


    large_mer k_mer(m,k);
    int mer_len=0;

    //std::cout<<"start\n";
    // int base_m_move_bit=m_block_len-m;
    // int base_suffix_move_bit=suffix_block_len-m%4-((k-m));
    uint32_t now_m_mer=0;
    
    for(int i=0;i+3<length;i+=4){
        k_mer.get_end();
        k_mer.get_next_k_mer(reads_address+i);
        mer_len+=4*(mer_len<block_len);
        if(mer_len>=block_len){
            for(int j=0;j<4;j++){
                // std::cout<<"i : "<<i<<std::endl;
                // std::cout<<"m-mer : "<<((k_mer.m_mer&get_m_mer[j])>>(2*(m_block_len-m-j)))<<std::endl;
                // std::cout<<"large_mer m-mer : "<<k_mer.m_mer<<std::endl;
                // std::cout<<"large_mer suffix-mer : "<<k_mer.suffix_mer<<std::endl;
                now_m_mer=(k_mer.m_mer&get_m_mer[j])>>(2*(m_block_len-m-j));
                locks[now_m_mer].lock();
                if(!reserved[now_m_mer]){
                    reserved[now_m_mer]=true;
                    root_table[now_m_mer].reserve(exe_arg.n);
                }
                ++root_table[now_m_mer][(k_mer.suffix_mer&get_suffix_mer[j])>>(2*(suffix_block_len-m%4-((k-m))-j))];
                locks[now_m_mer].unlock();
            }
        }
    }
    
    for(int i=0;i<length%4;i++){
        k_mer.get_one_end();
        k_mer.get_next_one_k_mer(reads_address+(length/4)*4+i);
        now_m_mer=(k_mer.m_mer&get_m_mer[3])>>(2*(m_block_len-m-3));
        locks[now_m_mer].lock();
        ++root_table[now_m_mer][(k_mer.suffix_mer&get_suffix_mer[3])>>(2*(suffix_block_len-m%4-((k-m))-3))];
        locks[now_m_mer].unlock();
    }

    char zero='A';
	for(int j=0;j<(4-k%4)*(k%4>0)+1;j++){
		k_mer.get_one_end();
        k_mer.get_next_one_k_mer(&zero);
        now_m_mer=(k_mer.m_mer&get_m_mer[3])>>(2*(m_block_len-m-3));
        locks[now_m_mer].lock();
        ++root_table[now_m_mer][(k_mer.suffix_mer&get_suffix_mer[3])>>(2*(suffix_block_len-m%4-((k-m))-3))];
        locks[now_m_mer].unlock();
	}

}

void counter_more_than_12::print(uint64_t min_cnt,uint64_t max_cnt){

    uint64_t *histo_table = new uint64_t[max_cnt + 1ull - min_cnt]();

    for(uint64_t i=0;i<(1ull<<(2*m));i++){
        for(const auto& mer_pair:root_table[i]){
            if(min_cnt<=mer_pair.second && mer_pair.second<=max_cnt) histo_table[mer_pair.second-min_cnt]++;
        }
    }

    //输出到文件
    FILE *out_file=fopen((filename+".histo").c_str(),"w+");
    if(out_file==NULL){
        std::cout<<"Cannot create file "<<filename<<".histo"<<std::endl;
        exit(-1);
    }
    for(uint64_t i=min_cnt;i<=max_cnt;i++){
        fprintf(out_file,"%lu\t%lu\n",i,histo_table[i-min_cnt]);
    }
    fclose(out_file);
    delete[] histo_table;
}

void counter_more_than_12::reset(char *new_data, size_t new_total_file_size){
    data=new_data;
    total_file_size=new_total_file_size;
}