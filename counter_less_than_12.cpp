#include "counter_less_than_12.h"
#include <iostream>

void counter_less_than_12::count(size_t pos){
	
	int length = pos+4096>total_file_size?total_file_size-pos:4096;
    char* reads_address = data + pos;
	int mer_len=0;

    uint32_t k_mer=0;
	
	for(int i=0;i+3<length;i+=4){
		
        k_mer=(k_mer<<8)^bitcode(i,reads_address);
        
        mer_len=mer_len+4*(mer_len<block_len);
        // for(int j=0;j<4;j++){
        //     if((k_mer&get_k_mer[0])>>(2*(block_len-k)) > 1u<<(2*k)){
        //         std::cout<<"shit"<<std::endl;
        //     }
        // }
        root_table[(k_mer&get_k_mer[0])>>(2*(block_len-k))].fetch_add(mer_len>=block_len);
        root_table[(k_mer&get_k_mer[1])>>(2*(block_len-k-1))].fetch_add(mer_len>=block_len);
        root_table[(k_mer&get_k_mer[2])>>(2*(block_len-k-2))].fetch_add(mer_len>=block_len);
        root_table[(k_mer&get_k_mer[3])>>(2*(block_len-k-3))].fetch_add(mer_len>=block_len);


        k_mer=k_mer&get_k_end;
    }
	

    for(int i=0;i<length%4;i++){
        k_mer=(k_mer<<2)^int_to_bit[0][0][0][char_to_int[reads_address[(length/4)*4+i]]];
        root_table[(k_mer&get_k_mer[3])>>(2*(block_len-k-3))]++;
    }

	for(int j=0;j<(4-k%4)*(k%4>0)+1;j++){
		k_mer=(k_mer<<2)^int_to_bit[0][0][0][0];
        root_table[(k_mer&get_k_mer[3])>>(2*(block_len-k-3))]++;
	}

}


void counter_less_than_12::print(uint64_t min_cnt,uint64_t max_cnt) {

    uint64_t *histo_table = new uint64_t[max_cnt + 1ull - min_cnt]();
	
    for(uint64_t i=0;i<1ull<<(2*k);i++){
        if(min_cnt<=root_table[i] && root_table[i]<=max_cnt) histo_table[root_table[i]-min_cnt]++;
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

void counter_less_than_12::reset(char *new_data, size_t new_total_file_size){
    data=new_data;
    total_file_size=new_total_file_size;
}