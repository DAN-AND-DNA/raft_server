#include <sys/timerfd.h>


#ifdef __cplusplus
extern "C" {
#endif

//#include <raft_server/redis/hiredis.h>
//#include <raft_server/redis/async.h>

#ifdef __cplusplus 
}
#endif



#include <sys/time.h>
#include <raft_server/easytimer/Timer.h>
#include <raft_server/eventloop/EventLoop.h>
#include <raft_server/eventloop/Channel.h>
//#include <raft_server/eventloop/RedisChannel.h>
#include <raft_server/nanoraft/RaftServer.h>
#include <raft_server/log/Logger.h> 


#include <string.h>
#include <unistd.h>

namespace dan
{

namespace timer
{

Timer::Timer(uint64_t ulExpireTime, dan::eventloop::EventLoop* pstEventLoop, std::shared_ptr<dan::nanoraft::RaftServer>& pstServer, bool bIsLeader):
    m_bIsRun_(false),
    m_bIsInit_(true),
    m_iFd_(::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)),
    m_pstChannel_(new dan::eventloop::Channel(m_iFd_, pstEventLoop)),
    m_pstServer_(pstServer),
    m_pstTimerSpec_(new struct ::itimerspec())
    {
        if(m_iFd_ == -1)
        {
            m_bIsInit_ = false;
            return;
        }
        
        if(bIsLeader == true)
        {
            // leader  发送心跳包
            ::bzero(m_pstTimerSpec_, sizeof (struct ::itimerspec));
            m_pstTimerSpec_->it_value.tv_sec = ulExpireTime;
            m_pstTimerSpec_->it_interval.tv_sec = ulExpireTime;

            if(::timerfd_settime(m_iFd_, 0, m_pstTimerSpec_, nullptr) == -1)
            {
                printf("set error:%s\n", strerror(errno));
            }
            else
            {
                // 处理服务器 过期 和 更新 
                m_pstChannel_->SetReadCallback(std::bind(&Timer::SendAppendEntriesCallback, this));
                m_pstChannel_->DisableWrite();          //FIXME  只读不写
                m_pstChannel_->EnableRead(); 
            }
        }
        else if(bIsLeader == false)
        {
             // follower  判读是否应该继续选举
            ::bzero(m_pstTimerSpec_, sizeof (struct ::itimerspec));
            m_pstTimerSpec_->it_value.tv_sec = ulExpireTime;
            m_pstTimerSpec_->it_interval.tv_sec = 0;             // 触发一次

            if(::timerfd_settime(m_iFd_, 0, m_pstTimerSpec_, nullptr) == -1)
            {
                printf("set error:%s\n", strerror(errno));
            }
            else
            {
                // 处理服务器 过期 和 更新 
                m_pstChannel_->SetReadCallback(std::bind(&Timer::HeartBeatTimeoutCallback, this));
                m_pstChannel_->DisableWrite();          //FIXME  只读不写
                m_pstChannel_->EnableRead(); 
            }
        }


    }

Timer::~Timer()
{
    printf("close timer\n");
    m_pstChannel_->DisableAll();
    if(m_iFd_ > 0)
    {
        ::close(m_iFd_);
    }
    delete m_pstTimerSpec_;
}


void Timer::Init()
{
    if(m_pstChannel_->IsInit())
    {
        std::shared_ptr<void> pst = shared_from_this();
        m_pstChannel_->Tie(pst);
    }
}

void Timer::FreshTime(uint64_t ulExpireTime)
{
    // 相对调用的时间

    if(::timerfd_settime(m_iFd_, 0, m_pstTimerSpec_, nullptr) == -1)
    {
        printf("settime error:%s\n", ::strerror(errno));
    }
}

void Timer::SendAppendEntriesCallback()
{
    if(auto pstServer = m_pstServer_.lock())
    {
        m_bIsRun_ = true;
        // 处理服务器自己的业务
        uint64_t i = 0;
        ::read(m_iFd_, &i , sizeof(uint64_t));
        
        printf("leader send appendentries\n");
        pstServer->BroadCastAppendEntries();
    }
}

void Timer::HeartBeatTimeoutCallback()
{
    if(auto pstServer = m_pstServer_.lock())
    {
        uint64_t i = 0;
        ::read(m_iFd_, &i, sizeof(uint64_t));

        printf("follower heartbeat timeout\n");
        pstServer->BecomeCandidate();
        // TODO 广播投票消息
        pstServer->BroadCastRequestVote();
    }
}



}
}

