#include "large_mer.h"

void large_mer::get_end(){

    this->m_mer&=get_m_mer_end;
    this->suffix_mer&=get_suffix_mer_end;

}

void large_mer::get_one_end(){

    this->m_mer&=get_one_m_mer_end;
    this->suffix_mer&=get_one_suffix_mer_end;

}

void large_mer::get_next_k_mer(char* readline){
	
    this->suffix_mer<<=8;
    this->suffix_mer^=int_to_bit[char_to_int[readline[0]]][char_to_int[readline[1]]][char_to_int[readline[2]]][char_to_int[readline[3]]];
	uint64_t suffix_to_m=this->suffix_mer & this->get_suffix_to_m;
    this->m_mer<<=8;
    this->m_mer^=suffix_to_m>>(2*(suffix_block_len-4-4*(m%4>0)));

}



void large_mer::get_next_one_k_mer(char* readline){
	
	this->suffix_mer<<=2;
	this->suffix_mer^=int_to_bit[0][0][0][char_to_int[readline[0]]];
	uint64_t one_suffix_to_m=this->suffix_mer & this->get_one_suffix_to_m;
	this->m_mer<<=2;
    this->m_mer^=one_suffix_to_m>>(2*(suffix_block_len-4-4*(m%4>0)));
    
}