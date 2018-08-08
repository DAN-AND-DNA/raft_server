#pragma once

#include <memory>
#include <vector>
//#include <raft_server/nanoraft/RaftProxy.h>

namespace dan
{

namespace nanoraft
{
class RaftServer;
class RaftProxy;
//enum class RaftProxyRole:uint8_t;
}

namespace mod
{
class Mod;
}

namespace eventloop
{
class Channel;
class EventLoop;
}


namespace net
{

class Conn : public std::enable_shared_from_this<Conn>
{
public:

    Conn(int iFd, dan::eventloop::EventLoop* pstEventLoop, std::shared_ptr<dan::nanoraft::RaftServer>& pstServer);

    ~Conn();    // 需要 unique_ptr<T> 其中T必须完整

    void Init();

    uint8_t* InBufferPtr(int iSize){return &m_stInBuffer_[iSize];}

    uint8_t* OutBufferPtr(int iSize){return &m_stOutBuffer_[iSize];}

    int InBufferSize(){return m_iInBufferSize_;}

    int OutBufferSize(){return m_iOutBufferSize_;}

    void SetInBufferSize(int iSize){m_iInBufferSize_ = iSize;}

    void SetOutBufferSize(int iSize){m_iOutBufferSize_ = iSize;}

    void EnableWrite();

    void DisableWrite();

    int Fd(){return m_iFd_;}

    void TryConnect(const char* szAddress, int iPort);
//    void Clear(){m_stInBuffer_.clear();}

//    int Count(){return static_cast<int>(m_stInBuffer_.size());}
    void SendAppendEntries();

    void Tie(std::shared_ptr<dan::nanoraft::RaftProxy>& pstProxy){m_pstProxy_ = pstProxy;}

    void Tie(uint32_t dwID);    // 绑定到指定的节点ID

    void SetRaftPort(int iRaftPort){m_iRaftPort_ = iRaftPort;}
    void SetHttpPort(int iHttpPort){m_iHttpPort_ = iHttpPort;}
    int ProxyConnFd(uint32_t dwID);

    void Server_CloseConn(int iFd);
    void Server_AddProxy(uint32_t dwID);                    // 添加代理
    bool Server_IsCandidate();
    void Server_BecomeFollower();
    uint32_t Server_CurrentTerm();
    void Server_SetTerm(uint32_t dwTerm);
    void Server_SetLeader();
    int Server_LogTermByIndex(uint32_t dwIndex);
    uint32_t Server_CommitIndex();                          // Server 已经提交的索引号
    void Server_DelLogsFromIndex(uint32_t dwIndex);
private:
    void RecvCallback();

    void SendCallback();

    void CloseCallback();

    void AcceptedCallback();

    void SendHandShakeQ();
private:
    int m_iFd_;

    std::vector<uint8_t> m_stInBuffer_;         //FIXME 静态的,容易溢出

    std::vector<uint8_t> m_stOutBuffer_;

    std::unique_ptr<dan::eventloop::Channel> m_pstChannel_;
    
    std::unique_ptr<dan::mod::Mod> m_pstMod_;

    std::weak_ptr<dan::nanoraft::RaftServer> m_pstServer_;

    int m_iInBufferSize_;

    int m_iOutBufferSize_;

    std::weak_ptr<dan::nanoraft::RaftProxy> m_pstProxy_;

    int m_iRaftPort_;
    int m_iHttpPort_;
};

}
}
