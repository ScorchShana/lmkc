#ifndef definition_H
#define definition_H

////////////////////////
#include <bitset>
///////////////////////
#include <getopt.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<unistd.h>
#include <queue>
#include <string>
#include <stack>
#include <string.h>
#include <atomic>
#include <unistd.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <concurrentqueue.h>
#include "function_def.h"
#include "large_mer.h"
#include <map>
#include <unordered_map>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cmath>

#define get_end_n_bit(x,n) ((x) & (~(~0ull << (n)))) //取x的末n位

using namespace std;

typedef unsigned int uint_32;
typedef unsigned long long int uint_64;

struct
{
	int k;
	string path;
	int core_num;
	int m;
	int passes = 1; // 默认不分趟
	bool use_compact_hash_map = false; // 默认不用紧凑哈希表
	string filename;
	uint64_t max_cnt;
	uint64_t min_cnt;
	long long n;
}exe_arg;


/****************************************************************************************/
//struct c_reads
//{
//	vector<uint_32> cell;
//	int end_point = 0; // 记录unsinged int中的空间剩余
//};
typedef uint_32* c_reads;

/****************************************************************************************/
struct str2bin_return
{
	uint_64 bin;
	int flag;
};

/***************************************************************************************/


/******************************************************************************************/




//struct bigInt{
//	ushort nums[24];
//};

typedef moodycamel::ConcurrentQueue<uint_64> my_queue;
typedef std::unordered_map<uint64_t,uint64_t> mer_map;


/******************************************************************************************/
#endif