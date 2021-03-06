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

#include <raft_server/protocol/api.pb.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>

#include <arpa/inet.h>
#include <errno.h>
#include <sstream>
#include <random>

namespace
{   
// 日志项的索引比较器
class IndexComparator : public ::leveldb::Comparator
{
public:
    int Compare(const ::leveldb::Slice& a, const ::leveldb::Slice& b) const 
    {
        int iIndex1, iIndex2;
        std::istringstream is1(a.ToString());
        std::istringstream is2(b.ToString());

 //       printf("-----------------\n");
        is1 >> iIndex1;
        is2 >> iIndex2;


  //      printf("i1:%d  i2:%d\n", iIndex1, iIndex2);
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

IndexComparator  pstCmp;
}


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
    m_dwEntryBase_(0),
    m_pstLeader_(),
    m_stProxys_(),
    m_stLogs_(),
    m_stConns_(),
    m_pstEventLoop_(pstEventLoop),
    m_pstServerSocket_(new dan::net::SocketWrapper(dan::net::SocketWrapper::SSOCKET)),
    m_pstServerChannel_(new dan::eventloop::Channel(m_pstServerSocket_->Fd(), pstEventLoop, true)),
    m_pstStateDB_(),
    m_pstCfgLogIndex_(0)
{}

RaftServer::RaftServer(dan::eventloop::EventLoop* pstEventLoop, const char* szAddress, int iPort) noexcept:
    m_dwCurrentTerm_(0),
    m_dwVotedFor_(0),
    m_dwCommitIndex_(0),
    m_dwLastApplied_(0),
    m_stRole_(RaftProxyRole::Follower),
    m_dwLogBase_(0),
    m_dwEntryBase_(0),
    m_pstLeader_(),
    m_stProxys_(),
    m_stLogs_(),
    m_stConns_(),
    m_pstEventLoop_(pstEventLoop),
    m_pstServerSocket_(new dan::net::SocketWrapper(dan::net::SocketWrapper::SSOCKET, iPort, szAddress)),
    m_pstServerChannel_(new dan::eventloop::Channel(m_pstServerSocket_->Fd(), pstEventLoop, true)),
    m_pstStateDB_(),
    m_pstCfgLogIndex_(0)
{
    dan::mod::Mod::LoadMsg();
    
    ::leveldb::Options stOptions;
    ::leveldb::DB* pstStateDB;
    ::leveldb::DB* pstEntriesDB;

    ::leveldb::Status stStatus;

    stOptions.create_if_missing = true;
    
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

    stOptions.comparator = &pstCmp;

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

uint64_t RaftServer::LastEntryIndex()
{
    if(m_stEntries_.empty())
    {
        return 0;
    }
    else
    {
        return m_stEntries_.size() - 1;
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

void RaftServer::BecomeCandidate()
{
    printf("become candidate\n");

    //1. 任期+1
    m_dwCurrentTerm_++;

    //2. 随机选举超时时间
    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(200, 500);       // 
    int iRandomTime = uniform_dist(e1);
    printf("become candidate new term:%d random vote time:%d ms\n", m_dwCurrentTerm_, iRandomTime);
}

void RaftServer::Run()
{

    std::shared_ptr<dan::nanoraft::RaftServer> pst = shared_from_this();
    
    // leader定时器
    m_stTimers_[dan::timer::Timer::LeaderHeartbeatTimer] = std::move(std::shared_ptr<dan::timer::Timer>(new dan::timer::Timer(10, m_pstEventLoop_, pst, dan::timer::Timer::LeaderHeartbeatTimer)));
    m_stTimers_[dan::timer::Timer::LeaderHeartbeatTimer]->Init();

    m_pstServerChannel_->SetReadCallback(std::bind(&RaftServer::TcpAcceptCallback, this));
    m_pstServerChannel_->EnableRead();

    m_pstEventLoop_->Loop();
}

void RaftServer::ConnectToPeer(const char* szAddress, int iPort, int iNodeID, int iRaftPort)
{
    dan::net::SocketWrapper stSocket(dan::net::SocketWrapper::CSOCKET, iPort, szAddress);
    std::shared_ptr<dan::nanoraft::RaftServer> pst = shared_from_this();
    std::shared_ptr<dan::net::Conn> pstConn(new dan::net::Conn(stSocket.Fd(), m_pstEventLoop_, pst));
    pstConn->Init();
    pstConn->TryConnect(szAddress, iPort, iNodeID, iRaftPort);
    pstConn->SetAddr(szAddress);


    //1 follower 心跳定时器
    m_stTimers_[dan::timer::Timer::FollowerHeartbeatTimer] = std::move(std::shared_ptr<dan::timer::Timer>(new dan::timer::Timer(21, m_pstEventLoop_, pst, dan::timer::Timer::FollowerHeartbeatTimer)));
    m_stTimers_[dan::timer::Timer::FollowerHeartbeatTimer]->Init();
/*
    //2 follower 日志定时器
    m_stTimers_[dan::timer::Timer::FollowerApplyLogTimer] = std::move(std::shared_ptr<dan::timer::Timer>(new dan::timer::Timer(3, m_pstEventLoop_, pst, dan::timer::Timer::FollowerApplyLogTimer)));
    m_stTimers_[dan::timer::Timer::FollowerApplyLogTimer]->Init();

*/

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
   // printf("size:%d\n", m_stLogs_.size());
    if(m_stLogs_.size() < dwIndex + 1)
    {
        return -1;
    }

    return m_stLogs_[dwIndex]->Term();
}

int RaftServer::EntryTermByIndex(uint32_t dwIndex)
{
    if(m_stEntries_.size() < dwIndex + 1)
    {
        return -1;
    }

    return m_stEntries_[dwIndex]->term();
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
        //m_pstEntriesDB_->Put(stWriteOptions, std::to_string(dwIndex), std::to_string(dwWriteIt));
        m_stLogs_.push_back(std::move(std::unique_ptr<RaftLogEntry>(new RaftLogEntry(dwIndex, dwTerm, dwWriteIt))));              //O(1)
    }
}


void RaftServer::BroadCastAppendEntries(bool bIsHeart)
{
    for(auto& it : m_stProxys_)
    {
        if(it.second->State() == RaftProxyState::InActive)
            continue;
      
        it.second->SendAppendEntries(bIsHeart);
    }
}

void RaftServer::BroadCastRequestVote()
{
   // for(auto& it : m_stProxys_)
    {
        //if(it.second->)
        printf("broadcast vote msg\n");
    }
}


void RaftServer::AppendCfgLog(std::string strHost, int iRaftPort, int iNodeID)
{
    uint32_t dwIndex = static_cast<uint32_t>(m_stEntries_.size());
   
    ::leveldb::WriteOptions stWriteOptions;
    stWriteOptions.sync = true;

    std::unique_ptr<::api::entry> pstEntry(new ::api::entry());

    pstEntry->set_type(::api::entry::CFGADD);
    pstEntry->set_term(m_dwCurrentTerm_);
    pstEntry->set_nodeid(iNodeID);
    pstEntry->set_port(iRaftPort);
    pstEntry->set_host(strHost);
    uint32_t dwLen = pstEntry->ByteSize();
   
    char* pstBuffer = new char[dwLen]();
    pstEntry->SerializeToArray(pstBuffer, dwLen);


    ::leveldb::Slice stValues(pstBuffer);
    ::leveldb::Status stStatus = m_pstEntriesDB_->Put(stWriteOptions, std::to_string(dwIndex), stValues);
    if(stStatus.ok())
    {
        m_stEntries_.push_back(std::move(pstEntry));
        printf("RaftServer::AppendCfgLog log index:%d term:%d nodeid:%d port:%d host:%s\n", 
                            dwIndex, m_dwCurrentTerm_, iNodeID, iRaftPort, strHost.c_str());
    }
    else
    {
        printf("RaftServer::AppendCfgLog error:%s\n", stStatus.ToString().c_str());
    }

    if(pstBuffer != nullptr)
    { 
        delete [] pstBuffer;
    }
}

std::string RaftServer::LeaderHost()
{
    if(auto pst = m_pstLeader_.lock())
    {
        return pst->ConnHost();
    }
    return "";
}

void RaftServer::EntryByIndex(uint32_t dwIndex, api::entry* pstEntry)
{
    pstEntry->CopyFrom(*(m_stEntries_[dwIndex]));
}


void RaftServer::FreshTime(const int iTimer)
{
    if(m_stTimers_[iTimer])
    {
        if(iTimer == dan::timer::Timer::FollowerHeartbeatTimer)
        {
            m_stTimers_[iTimer]->FreshTime(10);
        }
        else if(iTimer == dan::timer::Timer::LeaderHeartbeatTimer)
        {
            m_stTimers_[iTimer]->FreshTime(20);
        }
    }
}

bool RaftServer::ChangeCommitIndex()
{

    int iMostNum =static_cast<int>((ProxysNum()/2) +1);
    int iCurrNum = 0;

    for(auto& it : m_stProxys_)
    {
        if(it.second->MatchIndex() >= (m_dwCommitIndex_ + 1))
        {
            iCurrNum ++;
        }
    }

    if(iCurrNum >= iMostNum && m_stEntries_[m_dwCommitIndex_ + 1]->term() == m_dwCurrentTerm_)
    {
        // 大多数都已经拿到了这个索引的日志 并且日志的任期等于现在的任期
        m_dwCommitIndex_++;
        return true;
    }
    return false;
}

bool RaftServer::TryApplyLogToFSM()
{
    if(m_dwLastApplied_ > m_dwCommitIndex_)
    {
        // 应用日志到FSM
        if(m_stEntries_.empty() || m_stEntries_[m_dwLastApplied_] == nullptr)
        {
            return false;
        }

        if(m_stEntries_[m_dwLastApplied_]->type() == ::api::entry::CFGADD)
        {
            //配置变化更新连接信息

        }
        m_dwLastApplied_++;
        return true;
    }

    return false;
}

void RaftServer::LoadEntries()
{
    if(!m_stEntries_.empty())
    {
        m_stEntries_.clear();
    }



    leveldb::Iterator* it = m_pstEntriesDB_->NewIterator(leveldb::ReadOptions());
    printf("start load...\n");               
    for(it->SeekToFirst(); it->Valid(); it->Next())
    {
        std::unique_ptr<::api::entry> pstEntry(new ::api::entry());
        pstEntry->ParseFromArray(it->value().data(), 128);
        std::string s;

        google::protobuf::TextFormat::PrintToString(*pstEntry, &s);
        printf("%s\n", s.c_str());

        if(pstEntry->type() == ::api::entry::CFGADD)
        {
            AddProxy(pstEntry->nodeid());
        }
        m_stEntries_.push_back(std::move(pstEntry));

    }
    
    if(!it->status().ok())
    {
        printf("RaftServer::LoadEntries error:%s\n", it->status().ToString().c_str()); 
        delete it;
        exit(1);
    }
    delete it;
    printf("load done\n");
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
            pstConn->SetAddr(::inet_ntoa(stClientAddr.sin_addr));

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
