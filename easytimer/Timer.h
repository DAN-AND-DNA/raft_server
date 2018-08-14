#pragma once

#include <memory>
#include <queue>


struct stTimerspec;

namespace dan
{
namespace nanoraft
{
class RaftServer;
}

namespace eventloop
{
class Channel;
class RedisChannel;
class EventLoop;
}

namespace timer
{

class Timer : public std::enable_shared_from_this<Timer>
{
public:
    Timer(uint64_t ulExpireTime, dan::eventloop::EventLoop* pstEventLoop, std::shared_ptr<dan::nanoraft::RaftServer>& pstServer, bool bIsLeader = true);

    ~Timer();

    void Init();

    bool IsInit(){return m_bIsInit_;}

    bool IsRun(){return m_bIsRun_;}
    void FreshTime(uint64_t ulExpireTime);
private:    
    // 定时器触发回调
    void SendAppendEntriesCallback();
    void HeartBeatTimeoutCallback();
private:
    bool                                            m_bIsRun_;
    bool                                            m_bIsInit_;
    int                                             m_iFd_;
    std::unique_ptr<dan::eventloop::Channel>        m_pstChannel_;                  // 和epoll交互的通道代理 
    std::weak_ptr<dan::nanoraft::RaftServer>        m_pstServer_;                   // 所属的服务器
    struct itimerspec*                              m_pstTimerSpec_;
};


}
}
