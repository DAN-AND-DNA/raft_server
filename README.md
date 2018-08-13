# raft_server

raft是个好算法，有golang版本的，也有c版本，所以利用c++实现一下，为了之后给公司项目中使用。

还在施工中....
目前使用leveldb作为日志固化，使用protobuf作为协议和日志(压缩)

deps:
    protobuf 3.x
    
依赖:
    protobuf 3.x

使用:
    
    1. cd raft_server/protocol/;mkdir deps
	
    2. 获取protobuf 3.x c++ 并解压
	
    3. mv protobuf 3.x raft_server/protocol/deps/protobuf
    打开protobuf
    ./autogen.sh
    ./configure CXXFLAGS=-fPIC --prefix=[当前目录的绝对地址]
    make && make install
    
    4. cd raft_server;mkdir build;cmake ..;make
    
    5. ./raft_server start 1 7777 3338
    
    6. ./raft_server join 2 192.168.0.103 7777 3339
    
    
目前进度：根据算法实现了日志的存储和广播

reuslt:
================================
get size:8
in factory ID:259 len:2
mainID:0 SubID:2
api.appendentries_q
get heartbeat===>log term:0 prelog index:2 prelog term:0
mod use:7 us
locked:260 4
直接发送给:13 字节:10
send use:20 us
==========================================================
get size:8
in factory ID:259 len:2
mainID:0 SubID:2
api.appendentries_q
get heartbeat===>log term:0 prelog index:1 prelog term:0
mod use:10 us
locked:260 4
直接发送给:13 字节:10
send use:40 us
===========================================================

my compay:
PING 192.168.1.46 (192.168.1.46) 56(84) bytes of data.
64 bytes from 192.168.1.46: icmp_seq=1 ttl=64 time=0.036 ms
64 bytes from 192.168.1.46: icmp_seq=43 ttl=64 time=0.043 ms
64 bytes from 192.168.1.46: icmp_seq=44 ttl=64 time=0.060 ms
64 bytes from 192.168.1.46: icmp_seq=45 ttl=64 time=0.073 ms
64 bytes from 192.168.1.46: icmp_seq=46 ttl=64 time=0.095 ms
64 bytes from 192.168.1.46: icmp_seq=47 ttl=64 time=0.091 ms
64 bytes from 192.168.1.46: icmp_seq=48 ttl=64 time=0.044 ms
64 bytes from 192.168.1.46: icmp_seq=49 ttl=64 time=0.089 ms
64 bytes from 192.168.1.46: icmp_seq=50 ttl=64 time=0.107 ms
64 bytes from 192.168.1.46: icmp_seq=51 ttl=64 time=0.042 ms
64 bytes from 192.168.1.46: icmp_seq=52 ttl=64 time=0.062 ms
64 bytes from 192.168.1.46: icmp_seq=53 ttl=64 time=0.067 ms
64 bytes from 192.168.1.46: icmp_seq=54 ttl=64 time=0.087 ms
64 bytes from 192.168.1.46: icmp_seq=55 ttl=64 time=0.059 ms
64 bytes from 192.168.1.46: icmp_seq=56 ttl=64 time=0.038 ms
64 bytes from 192.168.1.46: icmp_seq=57 ttl=64 time=0.081 ms
64 bytes from 192.168.1.46: icmp_seq=58 ttl=64 time=0.094 ms
64 bytes from 192.168.1.46: icmp_seq=59 ttl=64 time=0.059 ms
64 bytes from 192.168.1.46: icmp_seq=60 ttl=64 time=0.080 ms
64 bytes from 192.168.1.46: icmp_seq=61 ttl=64 time=0.067 ms
64 bytes from 192.168.1.46: icmp_seq=62 ttl=64 time=0.076 ms
