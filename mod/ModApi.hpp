#pragma once
#include <raft_server/mod/Mod.h>
#include <raft_server/net/Conn.h>
#include <google/protobuf/message.h>

#include <raft_server/protocol/api.pb.h>
#include <iostream>
#include <sys/time.h>

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
        ::gettimeofday(&tv, NULL);
        printf("in Api:%ld\n", tv.tv_sec*1000000 + tv.tv_usec);


        auto p = dynamic_cast<api::handshake_q*>(pstMessage.get());
        std::cout<<"handshake get peer nodeid:"<<p->nodeid()<<" port:"<<p->raftport()<<std::endl;


        //TODO 更替连接
        if(auto pst = pstConn.lock())
        {
            pst->SetRaftPort(p->raftport());
            pst->SetHttpPort(p->httpport());

            int iOldConnFd = pst->ProxyConnFd(static_cast<uint32_t>(p->nodeid()));
            if(iOldConnFd != -1)
            {
                // TODO 之前就存在该代理 干掉那个老的conn 替换本conn
                if(iOldConnFd != pst->Fd())
                {
                    pst->Server_CloseConn(iOldConnFd);
                }
            }
            else
            {
                // TODO 新的节点配置
                pst->Server_AddProxy(p->nodeid());
                pst->Tie(p->nodeid());
            }

            pst->SetRaftPort(p->raftport());
            pst->SetHttpPort(p->httpport());
        }

        api::handshake_r stMsg;
        stMsg.set_result(true);

        HandlePreSendMsg(pstConn, 258, std::move(stMsg));
        
        ::gettimeofday(&tv, NULL);
        printf("in Api done:%ld\n", tv.tv_sec*1000000 + tv.tv_usec);
}

void HandshakeR(std::weak_ptr<dan::net::Conn>& pstConn, std::unique_ptr<google::protobuf::Message>& pstMessage)
{
               struct timeval tv;
        ::gettimeofday(&tv, NULL);
        printf("in Api:%ld\n", tv.tv_sec*1000000 + tv.tv_usec);


        auto p = dynamic_cast<api::handshake_r*>(pstMessage.get());
        std::cout<<"handshake response get result:"<<p->result()<<std::endl;
        
        ::gettimeofday(&tv, NULL);
        printf("in Api done:%ld\n", tv.tv_sec*1000000 + tv.tv_usec);
}

void AppendEntriesQ(std::weak_ptr<dan::net::Conn>& pstConn, std::unique_ptr<google::protobuf::Message>& pstMessage)
{
    auto p = dynamic_cast<::api::appendentries_q*>(pstMessage.get());

    ::api::appendentries_r stMsg;
    stMsg.set_success(false);
    
    if(auto pst = pstConn.lock())
    {

        // 处理心跳消息
        if(p->entries_size() == 0)
        {
            //TODO 刷新过期时间
            stMsg.set_success(true);
            goto sendreponse;
        }

        // 处理日志消息
        if(pst->Server_IsCandidate() == true && pst->Server_CurrentTerm() == p->term())
        {
            // 1. 如果服务器是candidate 收到appenentries消息就成为follower 放弃选举
            pst->Server_BecomeFollower();
        }
        else if(pst->Server_CurrentTerm() < p->term())
        {
            // 2. 设置并持久化term
            pst->Server_SetTerm(p->term());
            pst->Server_BecomeFollower();
            pst->Server_SetLeader();                // 和这个conn绑定的proxy作为leader
        }
        else if(pst->Server_CurrentTerm() > p->term())
        {
            stMsg.set_success(false);
            goto sendreponse;                       // 通知leader变成follower
        }

        // 如果是第一次收到appendentries, 即p->prelogindex() == 0 (节点的初始都为0) 
        if(0 < p->prelogindex())
        {
            int iPrevTerm = pst->Server_LogTermByIndex(p->prelogindex());
            
            // 如果失败，就让leader 减小nextid重试
            if(iPrevTerm == -1)
            {
                stMsg.set_success(false);
                goto sendreponse;
            }

            if(static_cast<uint32_t>(iPrevTerm) != p->prelogterm())
            {
                stMsg.set_success(false);
                goto sendreponse;
            }
        }

        
        // 3. 解决日志冲突
        int i;
        uint32_t dwNewIndex;
        for(i = 0 ; i < p->entries_size(); ++i)
        {
            dwNewIndex = p->prelogindex() + 1 + i;                // 新的索引号
            int iExistingTerm = pst->Server_LogTermByIndex(dwNewIndex);    // 是否这个新索引号已经有entry
        
            if(iExistingTerm != -1 && iExistingTerm != static_cast<int32_t>((p->entries(i)).term()) && pst->Server_CommitIndex() < dwNewIndex)
            {
                // 准备添加的日志在相同索引的任期冲突,并且这个新日志尚未提交, 删除这个索引和其之后全部日志
                pst->Server_DelLogsFromIndex(iExistingTerm);
            }
        }

        // 4. 干掉冲突之后添加日志
        for(; i < p->entries_size(); ++i)
        {
            dwNewIndex = p->prelogindex() + 1 + i;  
            pst->Server_AppendLog(p->prelogindex() + dwNewIndex, (p->entries(i)).term(), p->entries(i).write_it());
        }


        // 5. 更新commit 索引
        if(pst->Server_CommitIndex() < p->leadercommit())
        {
            uint32_t dwLastNewIndex = p->prelogindex() + p->entries_size();
            pst->Server_SetCommitIndex(std::min(p->leadercommit(), dwLastNewIndex));
        }

        stMsg.set_success(true);


sendreponse:
        stMsg.set_term(pst->Server_CurrentTerm());
        HandlePreSendMsg(pstConn, 260, std::move(stMsg));
    }
}


void AppendEntriesR(std::weak_ptr<dan::net::Conn>& pstConn, std::unique_ptr<google::protobuf::Message>& pstMessage)
{
    auto p = dynamic_cast<::api::appendentries_q*>(pstMessage.get());

    ::api::appendentries_r stMsg;
    stMsg.set_success(false);
}

private:
    typedef std::function<void(std::weak_ptr<dan::net::Conn>& ,std::unique_ptr<google::protobuf::Message>&)> TProc;

    std::vector<TProc> m_stProcs_;

};

}
}
