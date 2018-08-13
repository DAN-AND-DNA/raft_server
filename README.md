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
