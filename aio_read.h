#ifndef AIO_READ_HEADER
#define AIO_READ_HEADER

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <aio.h>
#include <thread>
#include <string>
#include <cstdio>
#include <cstring>

#include <iostream>

class aio_readq{


    public:

        bool is_block1;
    
        explicit aio_readq(const std::string &in_filename, const int in_k, char *in_outblock1, char* in_outblock2,  ssize_t in_chunk_size) : 
        filename(in_filename), k(in_k), outblock1(in_outblock1), outblock2(in_outblock2), chunk_size(in_chunk_size)
        {
            block1=new char[chunk_size];
            block2=new char[chunk_size];
            front_k=new char[2*k];
            is_block1=true;
            firsted=true;
            front_k_size=0;

            fd = open(filename.c_str(), O_RDONLY);
            if(fd<0){
                printf("Cannot open file %s",filename.c_str());
                exit(-1);
            }

            struct stat finfo;
            fstat(fd, &finfo); // 获取文件信息，例如文件长度
            filesize = finfo.st_size;
            already_read=0;

            line=0;

            bzero(&cb1,sizeof(struct aiocb));
            bzero(&cb2,sizeof(struct aiocb));

            cb1.aio_fildes = fd;            // 文件描述符
            cb1.aio_buf    = block1;           // 用户缓冲区
            cb1.aio_nbytes = chunk_size;      // 想读多少
            cb1.aio_offset = 0;
            cb2.aio_fildes = fd;            // 文件描述符
            cb2.aio_buf    = block2;           // 用户缓冲区
            cb2.aio_nbytes = chunk_size;      // 想读多少
            cb2.aio_offset = 0;

            read_time=0;

            if (aio_read(&cb1) != 0) {
                printf("Cannot read the file %s",filename.c_str());
                close(fd);
                exit(-1);
            }
        }

        ssize_t read(){

            ssize_t now_read;
            ssize_t out_len;
            ssize_t now_pos=0;
            long long temp=1;
            char *line_end_ptr;
            ssize_t length=0;

            std::string s;

            if(already_read<filesize){

                if(is_block1){
                    while (aio_error(&cb1) == EINPROGRESS);
                    int ret = aio_error(&cb1);
                    if (ret != 0) {
                        printf("Cannot read the file %s",filename.c_str());
                        close(fd);
                        exit(-1);
                    }
                    now_read=aio_return(&cb1);
                    already_read+=now_read;


                    if(already_read<filesize){
                        bzero(&cb2,sizeof(struct aiocb));
                        cb2.aio_fildes = fd;            // 文件描述符
                        cb2.aio_buf    = block2;           // 用户缓冲区
                        cb2.aio_nbytes = chunk_size;      // 想读多少
                        cb2.aio_offset = already_read;
                        if (aio_read(&cb2) != 0) {
                            printf("Cannot read the file %s",filename.c_str());
                            close(fd);
                            exit(-1);
                        }
                    }

                    if(firsted){
                        firsted=false;
                    }
                    else{
                        length=front_k_size;
                        std::memcpy(outblock1,front_k,front_k_size);
                    }

                    while(now_pos+1-temp<now_read){

                        line_end_ptr=(char*)std::memchr(block1+1+now_pos-temp,'\n',now_read*sizeof(char)-now_pos-1+temp);
                        if(line_end_ptr==nullptr) line_end_ptr=block1+now_read;
                        if(line%4==1){
                            out_len=line_end_ptr-1-block1-now_pos+temp;

                            if(out_len>0){
                                std::memcpy(outblock1+length,block1+now_pos-temp+1,out_len);
                                length+=out_len;
                            }
                        }
                        
                        now_pos=line_end_ptr-block1;
                        if(now_pos>=now_read){
                            break;
                        }

                        temp=0;
                        line++;
                    }

                }
                else{
                    while (aio_error(&cb2) == EINPROGRESS);
                    int ret = aio_error(&cb2);
                    if (ret != 0) {
                        printf("Cannot read the file %s",filename.c_str());
                        close(fd);
                        exit(-1);
                    }
                    now_read=aio_return(&cb2);
                    already_read+=now_read;

                    if(already_read<filesize){
                        bzero(&cb1,sizeof(struct aiocb));
                        cb1.aio_fildes = fd;            // 文件描述符
                        cb1.aio_buf    = block1;           // 用户缓冲区
                        cb1.aio_nbytes = chunk_size;      // 想读多少
                        cb1.aio_offset = already_read;
                        if (aio_read(&cb1) != 0) {
                            printf("Cannot read the file %s",filename.c_str());
                            close(fd);
                            exit(-1);
                        }
                    }

                    length=front_k_size;
                    std::memcpy(outblock2,front_k,front_k_size);

                    while(now_pos+1-temp<now_read){

                        line_end_ptr=(char*)std::memchr(block2+1+now_pos-temp,'\n',now_read*sizeof(char)-now_pos-1+temp);
                        if(line_end_ptr==nullptr) line_end_ptr=block2+now_read;
                        if(line%4==1){
                            out_len=line_end_ptr-1-block2-now_pos+temp;

                            if(out_len>0){
                                std::memcpy(outblock2+length,block2+now_pos-temp+1,out_len);
                                length+=out_len;
                            }
                        }
                        
                        now_pos=line_end_ptr-block2;
                        if(now_pos>=now_read){
                            break;
                        }

                        temp=0;
                        line++;
                    }


                }
                
                if(length+1>=k){
                    front_k_size=k-1;
                    if(is_block1) std::memcpy(front_k,outblock1+length+1-k,k-1);
                    else std::memcpy(front_k,outblock2+length+1-k,k-1);
                }
                else{
                    if(is_block1){
                        std::memcpy(front_k,outblock1,length);
                        front_k_size=length;
                    }
                    else{
                        std::memcpy(front_k,outblock2,length);
                        front_k_size=length;
                    }
                }
                
            }

            is_block1=!is_block1;

            return length;
        }

        inline bool is_outblock1() const {
            return !is_block1;
        }

        inline bool eof() const {
            return already_read>=filesize;
        }

        ~aio_readq(){
            delete[] block1;
            delete[] block2;
            delete[] front_k;
        }

    private:
        char *outblock1, *outblock2;
        char *block1,*block2;
        ssize_t chunk_size,front_k_size;
        int k;
        long long line;
        std::string filename;
        int fd;
        struct aiocb cb1,cb2;
        ssize_t filesize,already_read,read_time;
        char *front_k;
        bool firsted;
};


#endif