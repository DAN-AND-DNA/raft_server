#pragma once


#include <memory>
#include <map>
#include <deque>
#include <functional>


namespace api
{
class entry;
}

namespace leveldb
{
class DB;
//class IndexComparatorComparator;
}



namespace dan
{

namespace eventloop
{
class EventLoop;
class Channel;
}

namespace net
{
class SocketWrapper;
class Conn;
}


namespace nanoraft
{
class IndexComparator;
enum class RaftProxyRole:uint8_t;
enum class RaftProxyState:uint8_t;
class RaftProxy;
class RaftLogEntry;
typedef std::function<void()> TRaftCallback;



// raft 服务器实体 每个线程一个
class RaftServer: public std::enable_shared_from_this<RaftServer>
{
public:
    RaftServer(dan::eventloop::EventLoop* pstEventLoop) noexcept;
    RaftServer(dan::eventloop::EventLoop* pstEventLoop, const char* szAddress, int iPort) noexcept;
    ~RaftServer() noexcept;

    std::weak_ptr<RaftProxy> GetProxyByID(uint32_t dwID);
    int ProxyConnFd(uint32_t dwID);
    void AddProxy(uint32_t dwID);
    uint64_t ProxysNum()const {return m_stProxys_.size();}                      // 当前代理的数量
    uint64_t LastLogIndex(){return m_stLogs_.size() + m_dwLogBase_;}            // 当前日志的序号
    void BecomeLeader();                                                        // 直接成为leader
    void BecomeFollower();

    void Run();
    void ConnectToPeer(const char* szAddress, int iPort, int iNodeID, int iRaftPort);
    void OfflineConn(int iFd);

    void SetAndPersistTerm(uint32_t dwTerm);                                    // 修改当前任期和固化
    uint32_t CurrentTerm(){return m_dwCurrentTerm_;}
    uint32_t CommitIndex(){return m_dwCommitIndex_;}
    uint32_t PreLogTerm();
    dan::nanoraft::RaftProxyRole Role(){return m_stRole_;}
    void SetLeader(std::shared_ptr<RaftProxy>& pstProxy){m_pstLeader_ = pstProxy;}
    int LogTermByIndex(uint32_t dwIndex);                                       // 获得指定日志的任期 
    int EntryTermByIndex(uint32_t dwIndex);
    void DelLogsFromIndex(uint32_t dwIndex);                                    // 删除从索引开始之后全部日志
    void AppendLog(uint32_t dwIndex, uint32_t dwTerm, uint32_t dwWriteIt);      // FIXME 测试用,只是写一个数字 TODO 通用
    
    void SetCommitIndex(uint32_t dwIndex){m_dwCommitIndex_ = dwIndex;}
    void BroadCastAppendEntries(bool bIsHeart = true);
    
    void AppendCfgLog(std::string strHost, int iRaftPort, int iNodeID);         // 添加cfg日志
    std::string LeaderHost();
    void EntryByIndex(uint32_t dwIndex, api::entry* pstEntry); 
private:
    void TcpAcceptCallback();                                                   // TCP accpet
    void TcpSendAppendEntries();

private:
    uint32_t                                        m_dwCurrentTerm_;           // 任期从0开始
    uint32_t                                        m_dwVotedFor_;              // 当前任期, 投票给候选人的ID
    uint32_t                                        m_dwCommitIndex_;           // leader已经提交的最大日志序号 (提交即绝大多数的节点已经获得该日志序列号)
    uint32_t                                        m_dwLastApplied_;           // 应用到状态机的最大日志序列号 (本机将日志项应用到状态机)
    RaftProxyRole                                   m_stRole_;                  // 本机的当前任期的集群身份
    uint64_t                                        m_dwLogBase_;               // 压缩日志的基数

    std::weak_ptr<RaftProxy>                        m_pstLeader_;               // leader所对应的代理
    std::map<uint32_t, std::shared_ptr<RaftProxy>>  m_stProxys_;                // 集群中其他节点的代理
    std::deque<std::unique_ptr<RaftLogEntry>>       m_stLogs_;                  // FIXME 废弃 日志
    
    std::deque<std::unique_ptr<api::entry>>         m_stEntries_;               // 
    std::map<int, std::shared_ptr<dan::net::Conn>>  m_stConns_;                 // 注册的连接

    dan::eventloop::EventLoop*                      m_pstEventLoop_;            // 事件循环
    std::unique_ptr <dan::net::SocketWrapper>       m_pstServerSocket_;         // 服务器的socket
    std::unique_ptr <dan::eventloop::Channel>       m_pstServerChannel_;        // 服务器到epoll的通道

    std::unique_ptr<::leveldb::DB>                  m_pstStateDB_;              // 状态db
    std::unique_ptr<::leveldb::DB>                  m_pstEntriesDB_;            // 日志项db
    uint32_t                                        m_pstCfgLogIndex_;          // 集群配置改变的索引
};


}
}
