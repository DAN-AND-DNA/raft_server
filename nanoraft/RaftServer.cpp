#include <raft_server/nanoraft/RaftServer.h>
#include <raft_server/nanoraft/RaftProxy.h>
#include <raft_server/nanoraft/RaftLogEntry.h>
#include <raft_server/mod/Mod.h>
#include <raft_server/eventloop/EventLoop.h>
#include <raft_server/eventloop/Channel.h>
#include <raft_server/net/SocketWrapper.h>
#include <raft_server/net/Conn.h>
#include <raft_server/leveldb/db.h>
#include <raft_server/leveldb/comparator.h>
#include <raft_server/easytimer/Timer.h>

#include <arpa/inet.h>
#include <errno.h>
#include <sstream>

namespace dan
{
namespace nanoraft
{

RaftServer::RaftServer(dan::eventloop::EventLoop* pstEventLoop) noexcept:
    m_dwCurrentTerm_(0),
    m_dwVotedFor_(0),
    m_dwCommitIndex_(0),
    m_dwLastApplied_(0),
    m_stRole_(RaftProxyRole::Follower),
    m_dwLogBase_(0),
    m_pstLeader_(),
    m_stProxys_(),
    m_stLogs_(),
    m_stConns_(),
    m_pstEventLoop_(pstEventLoop),
    m_pstServerSocket_(new dan::net::SocketWrapper(dan::net::SocketWrapper::SSOCKET)),
    m_pstServerChannel_(new dan::eventloop::Channel(m_pstServerSocket_->Fd(), pstEventLoop, true)),
    m_pstStateDB_()
{}

RaftServer::RaftServer(dan::eventloop::EventLoop* pstEventLoop, const char* szAddress, int iPort) noexcept:
    m_dwCurrentTerm_(0),
    m_dwVotedFor_(0),
    m_dwCommitIndex_(0),
    m_dwLastApplied_(0),
    m_stRole_(RaftProxyRole::Follower),
    m_dwLogBase_(0),
    m_pstLeader_(),
    m_stProxys_(),
    m_stLogs_(),
    m_stConns_(),
    m_pstEventLoop_(pstEventLoop),
    m_pstServerSocket_(new dan::net::SocketWrapper(dan::net::SocketWrapper::SSOCKET, iPort, szAddress)),
    m_pstServerChannel_(new dan::eventloop::Channel(m_pstServerSocket_->Fd(), pstEventLoop, true)),
    m_pstStateDB_()
{
    dan::mod::Mod::LoadMsg();
    
    ::leveldb::Options stOptions;
    ::leveldb::DB* pstStateDB;
    ::leveldb::DB* pstEntriesDB;

    ::leveldb::Status stStatus;


    // 日志项的索引比较器
    class IndexComparator : public ::leveldb::Comparator
    {
    public:
        int Compare(const ::leveldb::Slice& a, const ::leveldb::Slice& b) const 
        {
            int iIndex1, iIndex2;
            std::istringstream is1(a.ToString());
            std::istringstream is2(b.ToString());

            is1 >> iIndex1;
            is2 >> iIndex2;

            if(iIndex1 < iIndex2)
                return -1;
            if(iIndex1 > iIndex2)
                return 1; 
            return 0;
        }

        const char* Name()const {return "IndexComparator";}
        void FindShortestSeparator(std::string*, const ::leveldb::Slice&) const {}
        void FindShortSuccessor(std::string*) const{}
    };

    stOptions.create_if_missing = true;
    IndexComparator cmp;
    stOptions.comparator = &cmp;
    stStatus = ::leveldb::DB::Open(stOptions, "./state_db", &pstStateDB);
    if(!stStatus.ok())
    {
         printf("RaftServer::RaftServer() error:%s\n", stStatus.ToString().c_str());
         exit(1);
    }
    else
    {
        m_pstStateDB_ = std::move(std::unique_ptr<::leveldb::DB>(pstStateDB));

    }

    stStatus = ::leveldb::DB::Open(stOptions, "./entries_db", &pstEntriesDB);
    if(!stStatus.ok())
    {
         printf("RaftServer::RaftServer() error:%s\n", stStatus.ToString().c_str());
         exit(1);
    }
    else
    {
        m_pstEntriesDB_ = std::move(std::unique_ptr<::leveldb::DB>(pstEntriesDB));
    }
}




RaftServer::~RaftServer() noexcept{}

std::weak_ptr<RaftProxy> RaftServer::GetProxyByID(uint32_t dwID)
{
    return std::weak_ptr<RaftProxy>(m_stProxys_[dwID]);
}

int RaftServer::ProxyConnFd(uint32_t dwID)
{
    auto it = m_stProxys_.find(dwID);
    if(it != m_stProxys_.end())
    {
        return it->second->ConnFd();
    }
    return -1;
}


void RaftServer::AddProxy(uint32_t dwID)
{
    auto it = m_stProxys_.find(dwID);
    if(it != m_stProxys_.end())
    {
        // 已经存在 
        // TODO 修改状态为初始的投票
        printf("already proxy\n");
        it->second->SetState(RaftProxyState::IsVoting);
    }
    else
    {
        // 不存在
        printf("new proxy\n");
        m_stProxys_[dwID] = std::shared_ptr<RaftProxy>(new RaftProxy());
        m_stProxys_[dwID]->SetID(dwID);
    }
}

void RaftServer::BecomeLeader()
{
    m_stRole_ = RaftProxyRole::Leader;
    for(auto& it : m_stProxys_)
    {
        if(it.second->State() != RaftProxyState::InActive)
            continue;

        it.second->SetNextIndex(LastLogIndex() + 1);                // 成为leader后要设置每个代理的要发送日志序列号 为本机的日志的最大值+1
        it.second->SetMatchIndex(0);
        it.second->SendAppendEntries();
    }
}

void RaftServer::BecomeFollower()
{
    printf("become follower\n");
    m_stRole_ = RaftProxyRole::Follower;
    //TODO 随机选举超时时间
}

void RaftServer::Run()
{

    std::shared_ptr<dan::nanoraft::RaftServer> pst = shared_from_this();
    std::shared_ptr<dan::timer::Timer> pstTimer(new dan::timer::Timer(10, m_pstEventLoop_, pst));
    pstTimer->Init();

    m_pstServerChannel_->SetReadCallback(std::bind(&RaftServer::TcpAcceptCallback, this));
    m_pstServerChannel_->EnableRead();

    m_pstEventLoop_->Loop();
}

void RaftServer::ConnectToPeer(const char* szAddress, int iPort)
{
    dan::net::SocketWrapper stSocket(dan::net::SocketWrapper::CSOCKET, iPort, szAddress);
    std::shared_ptr<dan::nanoraft::RaftServer> pst = shared_from_this();
    std::shared_ptr<dan::net::Conn> pstConn(new dan::net::Conn(stSocket.Fd(), m_pstEventLoop_, pst));
    pstConn->Init();
    pstConn->TryConnect(szAddress, iPort);

    m_stConns_[pstConn->Fd()] = pstConn;
    m_pstEventLoop_->Loop();
}


void RaftServer::OfflineConn(int iFd)
{
    auto it = (this->m_stConns_).find(iFd);
    (this->m_stConns_).erase(it);
}

void RaftServer::SetAndPersistTerm(uint32_t dwTerm)
{
    // 1. 固化任期
    // 2. 根据固化结果来更新任期
    ::leveldb::WriteOptions stWriteOptions;
    stWriteOptions.sync = true;

    ::leveldb::Status stStatus = m_pstStateDB_->Put(stWriteOptions, "term", std::to_string(dwTerm));
    if(stStatus.ok())
    {
        m_dwCurrentTerm_ = dwTerm;
    }
    else
    {
        printf("persist Term error:%s\n", stStatus.ToString().c_str());
        exit(1);
    }
}

uint32_t RaftServer::PreLogTerm()
{
    if(m_stLogs_.empty())
    {
        return 0;
    }
    else
    {
        return m_stLogs_.back()->Term();
    }

}

int RaftServer::LogTermByIndex(uint32_t dwIndex)
{
    if(m_stLogs_.size() < dwIndex + 1)
    {
        return -1;
    }

    return m_stLogs_[dwIndex]->Term();
}

void RaftServer::DelLogsFromIndex(uint32_t dwIndex)
{
    if(m_stLogs_.size() < dwIndex + 1)
    {
        //do nothing
    }
    else
    {
        //del logs from index 根据这个结果来更新内存
        std::unique_ptr<::leveldb::Iterator> it(m_pstEntriesDB_->NewIterator(::leveldb::ReadOptions()));
        
        for(it->Seek(std::to_string(dwIndex)); it->Valid(); it->Next())
        {
            printf("del k:v = %s:%s\n", it->key().ToString().c_str(), it->value().ToString().c_str());
            ::leveldb::WriteOptions stWriteOptions;

            stWriteOptions.sync = true;
            m_pstEntriesDB_->Delete(stWriteOptions, it->key());
            if(!m_stLogs_.empty())
                m_stLogs_.pop_back();                                       //O(1)
        }
    }
}


void RaftServer::AppendLog(uint32_t dwIndex, uint32_t dwTerm, uint32_t dwWriteIt)
{
    //0 1 2 3
    //4  4
    if(m_stLogs_.size() != dwIndex)
    {
        // do nothing
    }
    else
    {
        ::leveldb::WriteOptions stWriteOptions;
        stWriteOptions.sync = true;
        m_pstEntriesDB_->Put(stWriteOptions, std::to_string(dwIndex), std::to_string(dwWriteIt));
        m_stLogs_.push_back(std::move(std::unique_ptr<RaftLogEntry>(new RaftLogEntry(dwIndex, dwTerm, dwWriteIt))));              //O(1)
    }
}


void RaftServer::BroadCastAppendEntries()
{
    for(auto& it : m_stProxys_)
    {
        if(it.second->State() == RaftProxyState::InActive)
            continue;
      
        it.second->SendAppendEntries();
    }
}



void RaftServer::TcpAcceptCallback()
{
    printf("new peernode online!\n");

    struct sockaddr_in stClientAddr;
    socklen_t stSocklen = sizeof stClientAddr;
    void* pstTemp = static_cast<void*>(&stClientAddr);

    while(true)
    {
        int iFd = ::accept4(m_pstServerSocket_->Fd(), 
                            static_cast<struct sockaddr*>(pstTemp),
                            &stSocklen, 
                            SOCK_CLOEXEC|SOCK_NONBLOCK);
        if(iFd == -1)
        {
            if(errno == EINTR)
                continue;

            printf("accept failed! error:%s\n", ::strerror(errno));
            return;
        }
        else
        {
            printf("new peernode addr:%s:%d fd:%d\n", ::inet_ntoa(stClientAddr.sin_addr), ::ntohs(stClientAddr.sin_port), iFd);
            std::shared_ptr<dan::nanoraft::RaftServer> pst = shared_from_this();
            std::shared_ptr<dan::net::Conn> pstConn(new dan::net::Conn(iFd, m_pstEventLoop_, pst));
            pstConn->Init();
            m_stConns_[iFd] = pstConn;
        }
        break;
    }
}

void RaftServer::TcpSendAppendEntries()
{

}


}
}
