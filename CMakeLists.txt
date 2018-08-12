cmake_minimum_required(VERSION 3.2)

project(raft_server)

if(NOT MUDUO_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "release")
endif()

set(RAFT_SERVER_NET_PATH "/home/$ENV{USER}/Server")

set(CXX_FLAGS
 -std=c++0x                     #TODO c+11
 -g                             #TODO 为了gdb
 -Wall                          #TODO 显示全部的警告
 -Wextra                        #TODO 打印一些额外的警告
 -Werror                        #TODO 警告当作错误
 -Wconversion                   #TODO 当隐式类型转换可能改变值时给出警告
 -Wno-unused-parameter          #TODO 未使用参数会被警告
 -Wold-style-cast               #TODO 程序使用 C风格的类型转换时给出警告
 -Woverloaded-virtual           #TODO 重载虚函数名时给出警告
 -Wpointer-arith                #TODO 当在算术表达式中使用函数指针时给出警告
 # -Wshadow                       #TODO 当一个局部变量掩盖了另一个局部变量时给出警告
 -Wwrite-strings                #TODO 在 C++中，非零值表示将字面字符串转换为‘char*’时给出警告。在 C中，给出相似的警告，但这种类型转换是符合ISO C 标准的
 -march=native                  #TODO gcc会自动检测你的CPU支持的指令集

 -rdynamic                      #TODO 给backtrace_symbols使用 指示连接器把所有符号（而不仅仅只是程序已使用到的外部符号）都添加到动态符号表（即.dynsym表）里，
                                #     以便那些通过 dlopen() 或 backtrace() （这一系列函数使用.dynsym表内符号）这样的函数使用
)

string (REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-o0")                                        #TODO 不做优化
set(CMAKE_CXX_FLAGS_RELEASE "-o2 -finline-limit=1000 -DNDEBUG")         #TODO 做优化 最大内联函数的长度被设置 关闭断言
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)                   #TODO 可执行的位置
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)                      #TODO 库输出的位置

include_directories(${RAFT_SERVER_NET_PATH})
include_directories(${RAFT_SERVER_NET_PATH}/raft_server)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/protocol/deps/protobuf/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/redis)

add_executable(raft_server 
    ./protocol/api.pb.cc 
    ./mod/MessageWrapper.cpp 
    ./mod/Mod.cpp 
    ./mod/ModApi.hpp 
    ./net/Conn.cpp 
    ./log/Logger.cpp 
    ./log/Exception.cpp 
    ./net/SocketWrapper.cpp 
    ./eventloop/EventLoop.cpp 
    ./eventloop/IPoller.cpp 
    ./eventloop/Channel.cpp 
    ./eventloop/RedisChannel.cpp 
    ./eventloop/EPoller.cpp 
    ./nanoraft/RaftLogEntry.cpp 
    ./nanoraft/RaftMsgs.cpp 
    ./nanoraft/RaftProxy.cpp 
    ./nanoraft/RaftServer.cpp 
    ./easytimer/Timer.cpp 
    ./main.cpp 
    )

target_link_libraries(raft_server pthread dl m)
TARGET_LINK_LIBRARIES(raft_server ${CMAKE_CURRENT_SOURCE_DIR}/lib/libleveldb.a)
TARGET_LINK_LIBRARIES(raft_server ${CMAKE_CURRENT_SOURCE_DIR}/lib/libhiredis.a)
TARGET_LINK_LIBRARIES(raft_server ${CMAKE_CURRENT_SOURCE_DIR}/protocol/deps/protobuf/lib/libprotobuf.a)
