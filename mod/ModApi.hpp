#pragma once
#include <raft_server/mod/Mod.h>
#include <raft_server/net/Conn.h>
#include <google/protobuf/message.h>

#include <raft_server/protocol/api.pb.h>
#include <iostream>
#include <sys/time.h>
namespace
{
const int FollowerHeartbeatTimer = 0;
const int FollowerApplyLogTimer  = 1;
const int LeaderHeartbeatTimer   = 2;
}


namespace dan
{
namespace mod
{

class ModApi : public Mod
{
public:
 ModApi():    
     Mod(),
    m_stProcs_()
    {
        // TODO 1. 注册消息
        m_stProcs_.push_back(std::move(std::bind(&ModApi::HandshakeQ, this, std::placeholders::_1, std::placeholders::_2)));
        m_stProcs_.push_back(std::move(std::bind(&ModApi::HandshakeR, this, std::placeholders::_1, std::placeholders::_2)));
        m_stProcs_.push_back(std::move(std::bind(&ModApi::AppendEntriesQ, this, std::placeholders::_1, std::placeholders::_2)));
        m_stProcs_.push_back(std::move(std::bind(&ModApi::AppendEntriesR, this, std::placeholders::_1, std::placeholders::_2))); 
        m_stProcs_.push_back(std::move(std::bind(&ModApi::RequestVoteQ, this, std::placeholders::_1, std::placeholders::_2)));
    }

    void Proc(std::weak_ptr<dan::net::Conn>& pstConn, int iID, std::unique_ptr<google::protobuf::Message>& pstMessage)
    {
        if(m_stProcs_.empty() || m_stProcs_[iID] == nullptr)
            return;
        m_stProcs_[iID](pstConn, pstMessage);
    }

private:

// TODO 2. 实现消息
void HandshakeQ(std::weak_ptr<dan::net::Conn>& pstConn, std::unique_ptr<google::protobuf::Message>& pstMessage)
{
        struct timeval tv;
        uint64_t ulOldTime = 0;
        ::gettimeofday(&tv, NULL);
        ulOldTime = tv.tv_sec*1000000 + tv.tv_usec;
       

        auto p = dynamic_cast<api::handshake_q*>(pstMessage.get());
        api::handshake_r stMsg;
        stMsg.set_result(false);

        //TODO 更替连接
        if(auto pst = pstConn.lock())
        {

            int iOldConnFd = -1;
            if(pst->Server_IsLeader() == false)
            {
                //TODO 本机不是leader  把leader信息回包给他
               
                 stMsg.set_result(false);
                // stMsg.set_leaderhost();
                // stMsg.set_leaderraftport();
                 goto sendreponse;
            }

            pst->SetRaftPort(p->raftport());
            //pst->SetHttpPort(p->httpport());

            iOldConnFd = pst->ProxyConnFd(static_cast<uint32_t>(p->nodeid()));
           
            if(iOldConnFd != -1)
            {
                // TODO 之前就存在该代理 干掉那个老的conn 替换本conn
                printf("==============old ass online!");

                if(iOldConnFd != pst->Fd())
                {
                     pst->Server_CloseConn(iOldConnFd);
                }

                printf("==============tie to old ass !");
                pst->Tie(p->nodeid());
                
            }
            else
            {
                // TODO 新的节点配置
                pst->Server_AddProxy(p->nodeid());
                pst->Tie(p->nodeid());
                pst->Server_AppendCfgLog(pst->Addr(), p->raftport(), p->nodeid());             // 广播配置给节点们
                
                pst->Server_BroadCastAppendEntries();
            }

            stMsg.set_result(true);

sendreponse:
            ::gettimeofday(&tv, NULL);
            printf("mod use:%ld us\n", (tv.tv_sec*1000000 + tv.tv_usec) - ulOldTime); 

            HandlePreSendMsg(pstConn, 258, std::move(stMsg));
        }
}

void HandshakeR(std::weak_ptr<dan::net::Conn>& pstConn, std::unique_ptr<google::protobuf::Message>& pstMessage)
{
    struct timeval tv;
    uint64_t ulOldTime = 0;
    ::gettimeofday(&tv, NULL);
    ulOldTime = tv.tv_sec*1000000 + tv.tv_usec;
      
    auto p = dynamic_cast<api::handshake_r*>(pstMessage.get());
    std::cout<<"handshake response get result:"<<p->result()<<std::endl;

    ::gettimeofday(&tv, NULL);
    printf("mod use:%ld us\n", (tv.tv_sec*1000000 + tv.tv_usec) - ulOldTime);
}

void AppendEntriesQ(std::weak_ptr<dan::net::Conn>& pstConn, std::unique_ptr<google::protobuf::Message>& pstMessage)
{
    struct timeval tv;
    uint64_t ulOldTime = 0;
    ::gettimeofday(&tv, NULL);
    ulOldTime = tv.tv_sec*1000000 + tv.tv_usec;
     
    auto p = dynamic_cast<::api::appendentries_q*>(pstMessage.get());

    ::api::appendentries_r stMsg;
    stMsg.set_success(false);
    
    if(auto pst = pstConn.lock())
    {
        //1 检查任期
        if(pst->Server_IsCandidate() == true && pst->Server_CurrentTerm() == p->term())
        {
            // 1.1 如果服务器是candidate 收到appenentries消息就成为follower 放弃选举
            printf("---------------1\n");
            pst->Server_BecomeFollower();
        }
        else if(pst->Server_CurrentTerm() < p->term())
        {
            // 1.2 设置并持久化term
             printf("---------------2\n");
            pst->Server_SetTerm(p->term());
            pst->Server_BecomeFollower();
        }
        else if(pst->Server_CurrentTerm() > p->term())
        {
            // 1.3
            stMsg.set_success(false);
             printf("---------------3\n"); 
            goto sendreponse;                       // 通知leader变成follower
        }

       // pst->Server_BecomeFollower();                    // 和这个conn绑定的proxy作为leader

        //printf("leader ip:%s\n", pst->Server_LeaderHost().c_str());
        //不是第一次收到appendentries 如果是第一次收到appendentries, 即p->prelogindex() == 0 (节点的初始都为0) 
        
        
        if(0 < p->prelogindex())
        {
             printf("---------------4\n"); 
          
            int iPrevTerm = pst->Server_EntryTermByIndex(p->prelogindex());
            
            // 如果失败，就让leader 减小nextid重试
            if(iPrevTerm == -1)
            {
                printf("---------------4.1:%d  %d\n", iPrevTerm, p->prelogindex()); 
                stMsg.set_success(false);
                goto sendreponse;
            }

            if(static_cast<uint32_t>(iPrevTerm) != p->prelogterm())
            {
                 printf("---------------4.2%d , %d\n", iPrevTerm,  p->prelogterm());
                stMsg.set_success(false);
                goto sendreponse;
            }
        }


        // 2 处理心跳消息
        if(p->entries_size() == 0)
        {
            printf("get heartbeat===>log term:%d prelog index:%d prelog term:%d leader commitindex:%d\n", p->term(), p->prelogindex(), p->prelogterm(), p->leadercommit());
            //TODO 刷新过期时间

            pst->Server_FreshTime(FollowerHeartbeatTimer);
            stMsg.set_success(true);
            stMsg.set_isheartbeat(true);
            goto sendreponse;
        }


        
        // 3. 解决日志冲突
        int i;
        uint32_t dwNewIndex;
        for(i = 0 ; i < p->entries_size(); ++i)
        {
            dwNewIndex = p->prelogindex() + 1 + i;                         // 新的索引号
            int iExistingTerm = pst->Server_LogTermByIndex(dwNewIndex);    // 是否这个新索引号已经有entry
        
            if(iExistingTerm != -1 && iExistingTerm != static_cast<int32_t>((p->entries(i)).term()) && pst->Server_CommitIndex() < dwNewIndex)
            {
                // 准备添加的日志在相同索引的任期冲突,并且这个新日志尚未提交, 删除这个索引和其之后全部日志
                pst->Server_DelLogsFromIndex(iExistingTerm);
            }
            else
            {
                break;
            }
        }
        printf("---------------6\n"); 
          
        // 4. 干掉冲突之后添加日志
        for(; i < p->entries_size(); ++i)
        {
            if(p->entries(i).type() == ::api::entry::CFGADD ||p->entries(i).type() == ::api::entry::CFGREM) 
            {
                printf("get cfg===>log term:%d prelog index:%d prelog term:%d leader commitindex:%d\n", p->term(), p->prelogindex(), p->prelogterm(), p->leadercommit());
                pst->Server_AppendCfgLog(p->entries(i).host(), p->entries(i).port(), p->entries(i).nodeid());
            }
            else
            {
                // 普通日志
            }
        }

        stMsg.set_success(true);


sendreponse:

        if(pst->Server_CommitIndex() < p->leadercommit())
        {
            //TODO 应用日志项
           // printf("follower try apply log to FSM\n");

            
            uint32_t dwLastNewIndex = p->prelogindex() + p->entries_size();
            pst->Server_SetCommitIndex(std::min(p->leadercommit(), dwLastNewIndex));
            printf("set follower===>update to commit index:%d\n", pst->Server_CommitIndex());

      //      pst->Server_TryApplyLogToFSM();
        }

        stMsg.set_term(pst->Server_CurrentTerm());
        ::gettimeofday(&tv, NULL);
        printf("mod use:%ld us\n", (tv.tv_sec*1000000 + tv.tv_usec) - ulOldTime); 
       
        HandlePreSendMsg(pstConn, 260, std::move(stMsg));
    }
}


void AppendEntriesR(std::weak_ptr<dan::net::Conn>& pstConn, std::unique_ptr<google::protobuf::Message>& pstMessage)
{
    struct timeval tv;
    uint64_t ulOldTime = 0;
    ::gettimeofday(&tv, NULL);
    ulOldTime = tv.tv_sec*1000000 + tv.tv_usec;
     

    auto p = dynamic_cast<::api::appendentries_r*>(pstMessage.get());
    printf("term:%d success:%d\n", p->term(), p->success());
    
    if(auto pst = pstConn.lock())
    {
        if(pst->Server_CurrentTerm() < p->term())
        {
            // 收到的回包中的term 大于当前节点的term
            pst->Server_SetTerm(p->term());
            pst->Server_BecomeFollower();
            return;
        }




        if(p->success() != true)
        {
            // appendentries失败 减少nextindex重试
        }
        else
        {
            if(p->isheartbeat() == true)
            {
            }
            else
            {
                // 成功了就更新信息
                pst->Proxy_IncrMatchIndex();
                pst->Proxy_IncrNextIndex();
            }
        }

        // TODO 处理之前的！ 如果leader的上次的commit index > apply index 就应用这个日志到状态机然后回复客户端
        // (因为到达本次就可以保证之前的commit index 已经被大家接受了,并应用到各自的FSM)

        // TODO 本次的commit index是否要更新
        bool bResult = pst->Server_ChangeCommitIndex();         // 只是follower的copy情况
        if(bResult == true)
        {

            //1 已经复制给多数的节点(安全复制) 应用这个日志到本机的状态机(比如配置就新建一个conn, 比如逻辑协议就处理)
           // printf("leader apply log to FSM\n");

            //2 回复给客户端
           // printf("exec this log and reply to the client if the log is an request\n")
        }
        else
        {
            printf("leader can not apply log to FSM\n");
        }
    }
    ::gettimeofday(&tv, NULL);
    printf("mod use:%ld us\n", (tv.tv_sec*1000000 + tv.tv_usec) - ulOldTime);
}


void RequestVoteQ(std::weak_ptr<dan::net::Conn>& pstConn, std::unique_ptr<google::protobuf::Message>& pstMessage)
{
   // auto p = dynamic_cast<::api::requestvote_q*>(pstMessage.get());
    
    if(auto pst = pstConn.lock())
    {

    }
}


private:
    typedef std::function<void(std::weak_ptr<dan::net::Conn>& ,std::unique_ptr<google::protobuf::Message>&)> TProc;

    std::vector<TProc> m_stProcs_;

};

}
}
