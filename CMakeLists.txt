cmake_minimum_required(VERSION 3.21)
set (CMAKE_C_COMPILER "/home/luyifeng/gcc/gcc-build-15.2.0/bin/gcc")
set (CMAKE_CXX_COMPILER "/home/luyifeng/gcc/gcc-build-15.2.0/bin/g++")

project(lmkc)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS "-lrt -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")



file(GLOB SOURCE ./*.h ./*.c ./*.cpp)
#file(GLOB_RECURSE SOURCE_INCLUDE ./include/*.h ./include/*.hh ./include/*.cc ./include/*.cpp)
include_directories(./include)


add_executable(lmkc ${SOURCE})

# pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(lmkc Threads::Threads)
# __sync_bool_compare_and_swap_16
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mcx16")
