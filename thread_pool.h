#ifndef THREAD_POOL_HEADER
#define THREAD_POOL_HEADER

#include <vector>
#include <functional>
#include <thread>
#include <queue>
#include "concurrentqueue.h"
#include <atomic>

template<typename F,typename arg_type>
class thread_pool{

    public:
        
        explicit thread_pool(int in_thread_num, const F &in_function) : thread_num(in_thread_num), running(false), function(in_function) {
            pool.reserve(in_thread_num);
            cnt=0;
        };                                                                                                                         

        virtual ~thread_pool(){
			if(running){
				running=false;
				for(int i=0;i<thread_num;i++){
					pool[i].join();//function join() can only be called once
				}
			}
            arg_type arg;
            while (arg_queue.try_dequeue(arg));
            pool.clear();
        }

        inline void enqueue(arg_type in_arg){
            arg_queue.enqueue(in_arg);
            cnt.fetch_add(1);
        }

        void apply(){

            running=true;

            for(int i=0;i<thread_num;i++){
                pool.emplace_back(std::thread([&](){
                    arg_type arg;
                    while (running) {
                        if(arg_queue.try_dequeue(arg)){
                            function(arg);
                            cnt.fetch_sub(1);
                        }
                    }
                }));
            }

        }

        void stop(){
			if(running){
				running=false;
				for(int i=0;i<thread_num;i++){
					pool[i].join();//function join() can only be called once
				}
			}
        }
		
		void join(){
			for(int i=0;i<thread_num;i++){
				pool[i].join();//function join() can only be called once
			}
			running=false;
		}

        bool finished() const {
            return cnt.load()==0ull;
        }

    private:

        std::vector<std::thread> pool;
        moodycamel::ConcurrentQueue<arg_type>arg_queue;

        std::atomic<unsigned long long>cnt;
        int thread_num;
        bool running;
        F function;

};

#endif