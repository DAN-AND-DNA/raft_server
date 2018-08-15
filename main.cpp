#include <signal.h>
#include <string.h>

#include <raft_server/nanoraft/RaftServer.h>
#include <raft_server/eventloop/EventLoop.h>
#include <raft_server/log/Exception.h>

namespace
{
dan::eventloop::EventLoop stEventLoop;

void ErrorHandle(int)
{
    ::fprintf(stdout, "%s\n", dan::Exception("error:").StackTrace());
    exit(1);
}

void StopServerHandle(int)
{
    printf("保存...\n");
    stEventLoop.StopLoop();
}

int iRaftPort = -1;
//int iHttpPort = -1;
int iJoinPort = -1;
int iNodeID = 0; 
const char* szAddress = NULL;  
}




int main(int argc, char** argv)
{
    ::signal(SIGPIPE, SIG_IGN);
    ::signal(SIGINT, StopServerHandle);
    ::signal(SIGSEGV, ErrorHandle);

    if(::strcmp(argv[1], "start") == 0 || ::strcmp(argv[1], "join") == 0)
    {
        iNodeID = ::atoi(argv[2]);

        if(::strcmp(argv[1], "start") == 0)
        {
            // start
            iRaftPort = ::atoi(argv[3]);
            if (iRaftPort != -1)
            {
                printf("start server listen at 192.168.1.46 raft port:%d\n",  iRaftPort);
                std::shared_ptr<dan::nanoraft::RaftServer> pstRaftServer(new dan::nanoraft::RaftServer(&stEventLoop, "192.168.1.46", iRaftPort));
                pstRaftServer->BecomeLeader(); 
                pstRaftServer->AppendCfgLog("192.168.1.46", iRaftPort, iNodeID);
              //  pstRaftServer->SetCommitIndex(pstRaftServer->CommitIndex() + 1);      // 就一个节点，直接提交序列号0
                pstRaftServer->Run();
            }

        }
        else
        {
            // join
            iJoinPort = ::atoi(argv[4]);
            szAddress = argv[3];
            iRaftPort = ::atoi(argv[5]);
            if(iJoinPort != -1 && szAddress && iRaftPort != -1)
            {
                printf("join server listen at %s::%d raft port:%d\n", szAddress, iJoinPort, iRaftPort);
                std::shared_ptr<dan::nanoraft::RaftServer> pstRaftServer(new dan::nanoraft::RaftServer(&stEventLoop, szAddress, iRaftPort));    
                pstRaftServer->ConnectToPeer(szAddress, iJoinPort, iNodeID, iRaftPort); 
            }
        }
    }
    else if(::strcmp(argv[1], "restart") == 0)
    {
        // 重启
        printf("restart server at 192.168.1.46 raft port:7777!\n");
        // load 日志 恢复到内存
        std::shared_ptr<dan::nanoraft::RaftServer> pstRaftServer(new dan::nanoraft::RaftServer(&stEventLoop, "192.168.1.46", 7777));
        pstRaftServer->LoadEntries();
            
    }
}
