#include <iostream>
#include <map>
#include "thread_pool.h"
#include "definition.h"
#include "counter_less_than_12.h"
#include "counter_more_than_12.h"
#include "aio_read.h"
#include <string>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;
int mm = 0;
ssize_t chunk_size=4*1024*1024;

int main(int argc, char* argv[]) {
    int opt = 0;
    int arg_count = 0;
    bool input_k_or_m=false;
    bool input_s_or_d=false;
    while ((opt = getopt(argc, argv, "k:i:t:m:o:s:d:n:c:vh")) != -1)
    {
        switch (opt)
        {
            case 'k':
                arg_count++;
                exe_arg.k = atoi(optarg);
                if(exe_arg.k < 1 || exe_arg.k > 36){
                    printf("k must satisfy this  conditions : 1 <= k <= 30\n");
                    exit(-1);
                }
                else if(input_k_or_m && ((exe_arg.k-exe_arg.m) < 1 || (exe_arg.k-exe_arg.m) > 24)){
                    printf("k and m must satisfy this  conditions : 1 <= (k-m) <= 18\n");
                    exit(-1);
                }
                break;
            case 'i':
                arg_count++;
                exe_arg.path = optarg;
                break;
            case 't':
                arg_count++;
                exe_arg.core_num = atoi(optarg);
                break;
            case 'm':
                arg_count++;
                exe_arg.m = atoi(optarg);
                mm = exe_arg.m;
                if(exe_arg.m < 0 || exe_arg.m > 12){
                    printf("m must satisfy this  conditions : 0 <= m <= 10\n");
                    exit(-1);
                }
                else if(input_k_or_m && ((exe_arg.k-exe_arg.m) < 1 || (exe_arg.k-exe_arg.m) > 24)){
                    printf("k and m must satisfy this  conditions : 1 <= (k-m) <= 20\n");
                    exit(-1);
                }
                break;
            case 'o':
                arg_count++;
                exe_arg.filename = optarg;
                break;
            case 's':
                arg_count++;
                exe_arg.min_cnt = atoll(optarg); 
                if(exe_arg.min_cnt<1){
                    printf("s must satisfy this conditions : 1 <= s <= d\n");
                    exit(-1);
                }
                else if(input_s_or_d && exe_arg.min_cnt>exe_arg.max_cnt){
                    printf("s must satisfy this conditions : 1 <= s <= d\n");
                    exit(-1);
                }
                break;
            case 'd':
                arg_count++;
                exe_arg.max_cnt = atoll(optarg); 
                if(exe_arg.max_cnt<1){
                    printf("d must satisfy this conditions : 1 <= s <= d\n");
                    exit(-1);
                }
                else if(input_s_or_d && exe_arg.min_cnt>exe_arg.max_cnt){
                    printf("d must satisfy this conditions : 1 <= s <= d\n");
                    exit(-1);
                }
                break;
            case 'n':
                arg_count++;
                exe_arg.n = atoll(optarg);
                if(exe_arg.n<=0){
                    printf("n must satisfy this conditions : 1 <= n\n");
                    exit(-1);
                }
                break;
            case 'c':
                arg_count++;
                chunk_size=atoll(optarg);
                if(chunk_size<0){
                    printf("c must satisfy this conditions : 1 <= c\n");
                    exit(-1);
                }
                break;
            case 'v':
                printf("lmkc - 1.0.0\n");
                exit(-1);
                break;
            case 'h':
                printf("Example:./lmkc -k 28 -i \"./test_files/test_data.fa\" -t 6 -m 10 -o out_file -s 1 -d 10000 -n 100 -c 4194304\n");
                printf("Parameters:\n");
                printf("  -k <value>\t\t\tk-mer length (1 <= k <= 36 and 0 <= (k-m) <= 24)\n");
                printf("  -i <string>\t\t\tinput file path\n");
                printf("  -t <value>\t\t\tnumber of threads\n");
                printf("  -m <value>\t\t\tsizes of m-mer (0 <= m <= 12)\n");
                printf("  -o <string>\t\t\toutput file path\n");
                printf("  -s <value>\t\t\tthe frequency more than s will be counted\n");
                printf("  -d <value>\t\t\tthe frequency fewer than d will be counted\n");
                printf("  -n <value>\t\t\tthe size reserved for each hash map (totally have 4^m hash maps)\n");
                printf("  -c <value>\t\t\tsizes of the block each time read from the file (1 <= c)\n");
                exit(-1);
                break;
            default:
                printf("Unknown command");
                exit(-1);
                break;
        }
    }

    if (arg_count < 9)
    {
        // 帮助信息
        printf("Too few arguments\n");
        printf("Example:./lmkc -k 28 -i \"./test_files/test_data.fa\" -t 6 -m 10 -o out_file -s 1 -d 10000 -n 100 -c 4194304\n");
        printf("Parameters:\n");
        printf("  -k <value>\t\t\tk-mer length (1 <= k <= 36 and 1 <= (k-m) <= 24)\n");
        printf("  -i <string>\t\t\tinput file path\n");
        printf("  -t <value>\t\t\tnumber of threads\n");
        printf("  -m <value>\t\t\tsizes of m-mer (0 <= m <= 12)\n");
        printf("  -o <string>\t\t\toutput file path\n");
        printf("  -s <value>\t\t\tthe frequency more than s will be counted\n");
        printf("  -d <value>\t\t\tthe frequency fewer than d will be counted\n");
        printf("  -n <value>\t\t\tthe size reserved for each hash map (totally have 4^m hash maps)\n");
        printf("  -c <value>\t\t\tsizes of the block each time read from the file (1 <= c)\n");
        exit(-1);
    }

    ////auto tp1 = std::chrono::steady_clock::now();
    //const int k = atoi(argv[1]);
    //const string path = argv[2];
    //const int core_num = atoi(argv[3]);
    //const int m = atoi(argv[4]); //定义表大小
    //const string filename = argv[5];

    const size_t k = exe_arg.k;
    const string path = exe_arg.path;
    const int core_num = exe_arg.core_num;
//    const int m = exe_arg.m;
    const string filename = exe_arg.filename;
//    const short threshold1 = 1;

//    const int mm = 8;

//    cout<<k<<endl;

    if (k > 12 && arg_count < 6)
    {
        printf("Parameters: \n");
        printf(
                "-d <value> sizes of hash table(Number of k-mer species,Non-single occurrence)  is necessary when k > 14.\n");
        exit(-1);
    }


    //cout << "k值" << k << "   path" << path << "   核心数量" << core_num << endl;
    //printf("k值:%d  path:\"%s\"  线程数:%d  块大小:%d条 输出文件名:\"%s\"\n", k, path.c_str(), core_num, m, filename.c_str());
//	ConcurrentBloomfilter* bloom_filter = nullptr;
//    MultipleConcurrentBloomFilter


//    mkc::MultipleConcurrentBloomFilter *bloom_filter = nullptr;

//    moodycamel::ConcurrentQueue<char*> address_array; //用于回收数组
    char* mapped;
    Counter* counter;
//    moodycamel::ConcurrentQueue<uint>* queue_table;
    mer_map *map_cnt = nullptr;
    atomic_ullong* root_table = nullptr;
    mutex *locks=nullptr;
    bool *reserved;


    uint32_t l = 1u<<(2*mm);


    /*打开文件 */
    char *out_block1,*out_block2;
    out_block1=new char[chunk_size+k];
    out_block2=new char[chunk_size+k];

    aio_readq reader(exe_arg.path,k,out_block1,out_block2,chunk_size);

    if (0 < k && k <= 12)
    {
        // 存放4^k个数 的数组长度
        root_table = new atomic_ullong[1ull<<(2*k)]();
        counter = new counter_less_than_12(root_table, nullptr, 0);
        mm = k;
    }
    else
    {
//        queue_table = new moodycamel::ConcurrentQueue<uint>[(int)pow(4, mm)];
        map_cnt = new mer_map[l];
        locks = new mutex[l];
        reserved = new bool[l]();

        counter = new counter_more_than_12(map_cnt, mm, nullptr, 0,locks,reserved);
//        counter = new counter_more_than_14(queue_table, mm,&address_array);
    }

    auto *executor = new thread_pool<std::function<void(size_t)>,size_t>(core_num, [&](size_t read_pos) {counter->count(read_pos);});

    ssize_t now_read=0;
    size_t actual_block_size = 4096;

    executor->apply();

    while((now_read=reader.read()) > 0){

        //cout<<"read"<<endl;
        if(now_read<k) continue;
        

        
        size_t offset = 0;


        

        if(reader.is_outblock1()){

            counter->reset(out_block1,now_read);

            executor->enqueue(offset);
            while (offset + actual_block_size <= now_read) {
                offset += actual_block_size - k + 1;
                executor->enqueue(offset);
            }
            offset += actual_block_size - k + 1;

            // 最后一块，长度小于一个块
            // 长度小过 k 就不要了
            if (offset + k <= now_read) {
                executor->enqueue(offset);
            }
        }
        else{
            counter->reset(out_block2,now_read);

            executor->enqueue(offset);
            while (offset + actual_block_size <= now_read) {
                offset += actual_block_size - k + 1;
                executor->enqueue(offset);
            }
            offset += actual_block_size - k + 1;

            // 最后一块，长度小于一个块
            // 长度小过 k 就不要了
            if (offset + k <= now_read) {
                executor->enqueue(offset);
            }
        }
        

        /************************************************************************************************************************************************************************************************************/
        while(!executor->finished()) usleep(100);

    }

    executor->stop();

    delete executor;
    executor=nullptr;
    //释放内存映射文件占用的内存

    if(k > 12) {
        counter->print(exe_arg.min_cnt,exe_arg.max_cnt);

        delete counter;
        counter=nullptr;

        delete[] map_cnt;
        map_cnt=nullptr;

        delete[] locks;
        locks=nullptr;
        
        delete[] reserved;
        reserved=nullptr;

    }
    else{
        counter->print(exe_arg.min_cnt,exe_arg.max_cnt);

        delete counter;
        counter=nullptr;

        delete[] root_table;
        root_table=nullptr;
    }

    delete[] out_block1;
    delete[] out_block2;

    return 0;
}
