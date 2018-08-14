#include <raft_server/eventloop/Channel.h>
#include <raft_server/mod/Mod.h>
#include <raft_server/eventloop/EventLoop.h>
#include <raft_server/net/Conn.h>
#include <raft_server/nanoraft/RaftServer.h>
#include <raft_server/nanoraft/RaftProxy.h>
//#include <sync_server/SyncServer.h>


#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <google/protobuf/message.h>

#include <raft_server/protocol/api.pb.h>


namespace dan
{
namespace net
{

Conn::Conn(int iFd, dan::eventloop::EventLoop* pstEventLoop, std::shared_ptr<dan::nanoraft::RaftServer>& pstServer):
    m_iFd_(iFd),
    m_stInBuffer_(1440),
    m_stOutBuffer_(1440),
    m_pstChannel_(new dan::eventloop::Channel(iFd, pstEventLoop)),
    m_pstMod_(new dan::mod::Mod()),
    m_pstServer_(pstServer),
    m_iInBufferSize_(0),
    m_iOutBufferSize_(0),
    m_iRaftPort_(0),
    m_strAddr_(),
    m_iJoinPort_(0),
    m_strJoinAddr_()
{
    m_pstChannel_->SetReadCallback(std::bind(&Conn::RecvCallback, this));
    m_pstChannel_->SetWriteCallback(std::bind(&Conn::SendCallback, this));
    m_pstChannel_->SetCloseCallback(std::bind(&Conn::CloseCallback, this));

    m_pstChannel_->DisableWrite();
    m_pstChannel_->EnableRead();

    std::cout<<m_stInBuffer_.size()<<std::endl;
}

Conn::~Conn()
{
    if(m_iFd_ > 0)
        ::close(m_iFd_);
}

void Conn::Init()
{
    if(m_pstChannel_->IsInit())
    {
        std::shared_ptr<Conn> pst = shared_from_this(); //+1

        m_pstChannel_->Tie(pst);

        m_pstMod_->Tie(pst);

        // -1
    }
}

void Conn::EnableWrite()
{
    m_pstChannel_->EnableWrite();
}

void Conn::DisableWrite()
{
    m_pstChannel_->DisableWrite();
}

void Conn::TryConnect(const char* szAddress, int iPort, int iNodeID, int iRaftPort)
{
    struct sockaddr_in stClientAddr; 
    ::bzero(&stClientAddr, sizeof(stClientAddr));

    stClientAddr.sin_family = AF_INET;
    stClientAddr.sin_port = ::htons(static_cast<uint16_t>(iPort));
    ::inet_pton(AF_INET, szAddress, &stClientAddr.sin_addr);

    ::socklen_t stSocklen = sizeof stClientAddr;
    
    void* pstTemp = static_cast<void*>(&stClientAddr);

    int iOldSet = ::fcntl(m_iFd_, F_GETFL);
    printf("fd:%d\n", m_iFd_);
    ::fcntl(m_iFd_, F_SETFL, iOldSet|O_NONBLOCK);

    int iR = ::connect(m_iFd_, static_cast<struct sockaddr*>(pstTemp), stSocklen);

    if (iR != 0 && errno == EINPROGRESS)
    {
        // 交给epoll
        printf("交给epoll进行connect\n");
        m_iJoinPort_ = iPort;
        m_strJoinAddr_ = szAddress;
        m_iNodeID_ = iNodeID;
        m_iRaftPort_ = iRaftPort;

        m_pstChannel_->SetWriteCallback(std::bind(&Conn::AcceptedCallback, this));
        m_pstChannel_->EnableWrite();
    }
    else if(iR == 0)
    {
        //TODO 直接连
        printf("直接connect\n");
    }
    else if(iR != 0)
    {
        printf("进行connect发生错误:%s\n", strerror(errno));
    }
}

void Conn::RecvCallback()
{
    printf("eeee\n");
    if(m_pstServer_.lock())
    {
        size_t dwLength = 1440;
        int iAllInSize = InBufferSize();
        ssize_t iSize = ::recv(m_iFd_, InBufferPtr(iAllInSize), dwLength, 0);

        if(iSize == 0)
        {
            CloseCallback();
            std::cout<<"close client\n";
            // TODO 客户端关闭
        }
        else if(iSize > 0)
        {
            struct timeval tv;
            ::gettimeofday(&tv, NULL);
            printf("in recvcall:%ld\n", tv.tv_sec*1000000 + tv.tv_usec);

            SetInBufferSize(iAllInSize + static_cast<int>(iSize));

            std::cout<<"get size:"<<InBufferSize()<<std::endl;
        
            m_pstMod_->HandleArrivedMsg();
        }
        else if(errno != EAGAIN)
        {
            // TODO 发生错误
            printf("err1\n");
        }

        printf("123\n");   
    }
}

void Conn::SendCallback()
{
    printf("ffff\n");
    if(m_pstServer_.lock())
    {
        //size_t dw
        std::cout<<"call sendcall:"<<OutBufferSize()<<std::endl;
        if(OutBufferSize() <= 0)
            return;

        int iSended = static_cast<int>(::send(Fd(), OutBufferPtr(0), OutBufferSize(), 0));

        if(iSended > 0)
        {
            std::cout<<"poll发送:"<<iSended<<std::endl;

            int iLeftSize = OutBufferSize() - iSended;
            SetOutBufferSize(iLeftSize);
            if(iLeftSize == 0)
            {
                struct timeval tv;
                ::gettimeofday(&tv, NULL);
                printf("发送完毕:%ld\n", tv.tv_sec*1000000 + tv.tv_usec);

                SetOutBufferSize(0);
                DisableWrite();
            }
            else if(iLeftSize > 0)
            {
                ::memcpy(OutBufferPtr(0), OutBufferPtr(iSended), iLeftSize);
                SetOutBufferSize(iLeftSize);
            }
        }
        else if(iSended == 0)
        {
            // nothing
        }
        else if(errno != EAGAIN)
        {
        // TODO error
        }
    }

}

void Conn::CloseCallback()
{
    if(auto pstS = m_pstServer_.lock())
    {
        m_pstChannel_->DisableAll();
        std::cout<<"清理 conn\n";

        ::close(m_iFd_);
        pstS->OfflineConn(m_iFd_);
    }
}

void Conn::AcceptedCallback()
{
    if(auto pstS = m_pstServer_.lock())
    {
        m_pstChannel_->DisableWrite();
        //TODO 设置为fd不阻塞
        struct sockaddr_in stClientAddr; 
        ::bzero(&stClientAddr, sizeof(stClientAddr));

        stClientAddr.sin_family = AF_INET;
        stClientAddr.sin_port = ::htons(static_cast<uint16_t>(m_iJoinPort_));
        ::inet_pton(AF_INET, m_strJoinAddr_.c_str(), &stClientAddr.sin_addr);

        ::socklen_t stSocklen = sizeof stClientAddr;
        void* pstTemp = static_cast<void*>(&stClientAddr);

        int iR = ::connect(m_iFd_, static_cast<struct sockaddr*>(pstTemp), stSocklen);

        if (iR == 0)
        {
            printf("连接成功\n");
            m_pstChannel_->SetReadCallback(std::bind(&Conn::RecvCallback, this));
            m_pstChannel_->SetWriteCallback(std::bind(&Conn::SendCallback, this));
            m_pstChannel_->SetCloseCallback(std::bind(&Conn::CloseCallback, this));

            m_pstChannel_->EnableRead();
            SendHandShakeQ();
        }
        else
        {
            printf("连接失败%s\n", strerror(errno));
        }
    }
}


void Conn::SendHandShakeQ()
{
    api::handshake_q stMessage;
    stMessage.set_raftport(m_iRaftPort_);
    stMessage.set_nodeid(m_iNodeID_);

    uint16_t dwMsgID = 257;
    uint32_t dwLen = stMessage.ByteSize();

    ::memcpy(OutBufferPtr(OutBufferSize()), reinterpret_cast<uint8_t*>(&dwMsgID), 2);
    ::memcpy(OutBufferPtr(OutBufferSize() + 2), reinterpret_cast<uint8_t*>(&dwLen), 4);
    stMessage.SerializeToArray(OutBufferPtr(OutBufferSize() + 6) , dwLen);

    SetOutBufferSize(OutBufferSize() + 6 + dwLen);
    // FIXME 尝试直接发送 -10us左右的延迟
    int iSended = static_cast<int>(::send(Fd(), OutBufferPtr(0), OutBufferSize(), 0));

    if(iSended > 0)
    {
        std::cout<<"直接发送给:"<<Fd()<<" 字节:"<<iSended<<std::endl;
        int iLeftSize = OutBufferSize() - iSended;
        SetOutBufferSize(iLeftSize);

        if(iLeftSize > 0)
            ::memcpy(OutBufferPtr(0), OutBufferPtr(iSended), iLeftSize); 
        printf("发送完毕\n");
    }
    else if(iSended == 0)
    {
            // nothing
    }
    else if(errno != EAGAIN) 
    {
            // TODO error
    }
}

void Conn::SendAppendEntries(bool bIsHeart, bool bIsCfg)
{
    if(auto pstServer = m_pstServer_.lock())
    {
        if(auto pstProxy = m_pstProxy_.lock())
        {
            // 发送的appendentries消息
            api::appendentries_q stMessage;
            stMessage.set_term(pstServer->CurrentTerm());
            stMessage.set_prelogindex(static_cast<uint32_t>(pstProxy->NextIndex() - 1)); // 收到回包再增加nextindex
              
            int iResult = pstServer->EntryTermByIndex(static_cast<uint32_t>(pstProxy->NextIndex() - 1));
            uint32_t dwTerm = 0;
            if(iResult != -1)
            {
               dwTerm = iResult; 
            }

            stMessage.set_prelogterm(dwTerm);
            stMessage.set_leadercommit(pstServer->CommitIndex());
           
            if(bIsHeart == false)
            {}

            if(bIsCfg)
            {}

            if(pstServer->LastEntryIndex() >= pstProxy->NextIndex())
            {
                // 最新的日志序号 >= 本次要发送的序号

                if(pstProxy->NextIndex() - 1 == 0)
                { 
                    auto pstEntry1 = stMessage.add_entries();
                    pstServer->EntryByIndex(0, pstEntry1);
                }

                auto pstEntry = stMessage.add_entries();
                pstServer->EntryByIndex(static_cast<uint32_t>(pstProxy->NextIndex()), pstEntry);
            }
            else
            {
                printf("send heart:%d , %u!!!\n", dwTerm, static_cast<uint32_t>(pstProxy->NextIndex())); 
            }
            //auto pstEntry = stMessage.add_entries();
            
            //pstEntry->set_write_it(7);
           

            uint16_t dwMsgID = 259;
            uint32_t dwLen = stMessage.ByteSize();
        
            ::memcpy(OutBufferPtr(OutBufferSize()), reinterpret_cast<uint8_t*>(&dwMsgID), 2);
            ::memcpy(OutBufferPtr(OutBufferSize() + 2), reinterpret_cast<uint8_t*>(&dwLen), 4);
            stMessage.SerializeToArray(OutBufferPtr(OutBufferSize() + 6) , dwLen);

            SetOutBufferSize(OutBufferSize() + 6 + dwLen);
            // FIXME 尝试直接发送 -10us左右的延迟
            int iSended = static_cast<int>(::send(Fd(), OutBufferPtr(0), OutBufferSize(), 0));

            if(iSended > 0)
            {
                std::cout<<"直接发送给:"<<Fd()<<" 字节:"<<iSended<<std::endl;
                int iLeftSize = OutBufferSize() - iSended;
                SetOutBufferSize(iLeftSize);

                if(iLeftSize > 0)
                    ::memcpy(OutBufferPtr(0), OutBufferPtr(iSended), iLeftSize); 
                printf("发送完毕\n");
            }
            else if(iSended == 0)
            {
                // nothing
            }
            else if(errno != EAGAIN) 
            {
                // TODO error
            }

        }
    }
}

void Conn::Tie(uint32_t dwID)
{ 
    if(auto pstServer = m_pstServer_.lock())
    {
        m_pstProxy_ = pstServer->GetProxyByID(dwID);
        if(auto pstProxy = m_pstProxy_.lock())
        {
            std::shared_ptr<dan::net::Conn> pst = shared_from_this();
            pstProxy->Tie(pst);
        }
    }
}

int Conn::ProxyConnFd(uint32_t dwID)
{
    if(auto pstServer = m_pstServer_.lock())
    {
        return pstServer->ProxyConnFd(dwID);
    }
    return -1;
}

void Conn::Server_CloseConn(int iFd)
{
    if(auto pstS = m_pstServer_.lock())
    {
        m_pstChannel_->DisableAll();
        std::cout<<"清理 conn\n";

        ::close(iFd);
        pstS->OfflineConn(iFd);
    }

}

void Conn::Server_AddProxy(uint32_t dwID)
{
    if(auto pstS = m_pstServer_.lock())
    {
        pstS->AddProxy(dwID);
    }

}

bool Conn::Server_IsCandidate()
{
    if(auto pstS = m_pstServer_.lock())
    {
        return pstS->Role() == dan::nanoraft::RaftProxyRole::Candidate;
    }
    return false;
}


bool Conn::Server_IsLeader()
{
    if(auto pstS = m_pstServer_.lock())
    {
        return pstS->Role() == dan::nanoraft::RaftProxyRole::Leader;
    }
    return false;
}

void Conn::Server_BecomeFollower()
{
    if(auto pstS = m_pstServer_.lock())
    {
        return pstS->BecomeFollower();
    }
}

uint32_t Conn::Server_CurrentTerm()
{
    if(auto pstS = m_pstServer_.lock())
    {
        return pstS->CurrentTerm();
    }
    return 0;
}

void Conn::Server_SetTerm(uint32_t dwTerm)
{
    if(auto pstS = m_pstServer_.lock())
    {
        return pstS->SetAndPersistTerm(dwTerm);
    }
}

void Conn::Server_SetLeader()
{
    if(auto pstS = m_pstServer_.lock())
    {
        if(auto pstP = m_pstProxy_.lock())
            pstS->SetLeader(pstP);
    }

}

int Conn::Server_LogTermByIndex(uint32_t dwIndex)
{
    if(auto pstS = m_pstServer_.lock())
    {
        return pstS->LogTermByIndex(dwIndex);
    }
    return -1;
  
}

int Conn::Server_EntryTermByIndex(uint32_t dwIndex)
{
    if(auto pstS = m_pstServer_.lock())
    {
        return pstS->EntryTermByIndex(dwIndex);
    }
    return -1;
  
}



uint32_t Conn::Server_CommitIndex()
{
    if(auto pstS = m_pstServer_.lock())
    {
        return pstS->CommitIndex();
    }

    return 0;
}

void Conn::Server_DelLogsFromIndex(uint32_t dwIndex)
{
    if(auto pstS = m_pstServer_.lock())
    {
         pstS->DelLogsFromIndex(dwIndex);
    }
}

void Conn::Server_AppendLog(uint32_t dwIndex, uint32_t dwTerm, uint32_t dwWriteIt)
{
    if(auto pstS = m_pstServer_.lock())
    {
        pstS->AppendLog(dwIndex, dwTerm, dwWriteIt);
    }
}

void Conn::Server_SetCommitIndex(uint32_t dwIndex)
{
    if(auto pstS = m_pstServer_.lock())
    {
        pstS->SetCommitIndex(dwIndex);
    }
}

void Conn::Server_AppendCfgLog(std::string strHost, int iRaftPort, int iNodeId)
{
    if(auto pstS = m_pstServer_.lock())
    {
        pstS->AppendCfgLog(strHost, iRaftPort, iNodeId);
    }
}

std::string Conn::Server_LeaderHost()
{
    if(auto pstS = m_pstServer_.lock())
    {
        return pstS->LeaderHost();
    }
    return "";
}

void Conn::Server_BroadCastAppendEntries()
{
    if(auto pstS = m_pstServer_.lock())
    {
        return pstS->BroadCastAppendEntries(false);
    }
}

void Conn::Server_FreshTime(std::string&& strRole)
{
    if(auto psTServer = m_pstServer_.lock())
    {
        printf("fresh time:%s\n", strRole.c_str());
        return psTServer->FreshTime(strRole);
    }
}



void Conn::Proxy_SetMatchIndex(uint32_t dwIndex)
{
    if(auto pstServer = m_pstServer_.lock())
    {
        if(auto pstProxy = m_pstProxy_.lock())
        {
            pstProxy->SetMatchIndex(dwIndex);
        }
    }
}

void Conn::Proxy_SetNextIndex(uint32_t dwIndex)
{
    if(auto pstServer = m_pstServer_.lock())
    {
        if(auto pstProxy = m_pstProxy_.lock())
        {
            pstProxy->SetNextIndex(dwIndex);
        }
    }
}

void Conn::Proxy_IncrMatchIndex()
{
    if(auto pstServer = m_pstServer_.lock())
    {
        if(auto pstProxy = m_pstProxy_.lock())
        {
            pstProxy->SetMatchIndex(pstProxy->MatchIndex() + 1);
        }
    }
}

void Conn::Proxy_IncrNextIndex()
{
    if(auto pstServer = m_pstServer_.lock())
    {
        if(auto pstProxy = m_pstProxy_.lock())
        {
            pstProxy->SetNextIndex(pstProxy->NextIndex() + 1);
        }
    }
}




}
}
