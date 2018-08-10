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

Timer::Timer(uint64_t ulExpireTime, dan::eventloop::EventLoop* pstEventLoop, std::shared_ptr<dan::nanoraft::RaftServer>& pstServer):
    m_bIsRun_(false),
    m_bIsInit_(true),
    m_iFd_(::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)),
    m_pstChannel_(new dan::eventloop::Channel(m_iFd_, pstEventLoop)),
    m_pstServer_(pstServer)
    {
        if(m_iFd_ == -1)
        {
            m_bIsInit_ = false;
        }
        else
        {
            struct ::itimerspec stTimerspec;
            ::bzero(&stTimerspec, sizeof stTimerspec);
            stTimerspec.it_value.tv_sec = ulExpireTime;
            stTimerspec.it_interval.tv_sec = ulExpireTime;

            if(::timerfd_settime(m_iFd_, 0, &stTimerspec, nullptr) == -1)
            {
                printf("set error:%s\n", strerror(errno));
            }
            else
            {
                // 处理服务器 过期 和 更新 
                m_pstChannel_->SetReadCallback(std::bind(&Timer::ServerTimeoutCallback, this));
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
}


void Timer::Init()
{
    if(m_pstChannel_->IsInit())
    {
        std::shared_ptr<void> pst = shared_from_this();
        m_pstChannel_->Tie(pst);
    }
}

void Timer::ServerTimeoutCallback()
{
    
    if(auto pstServer = m_pstServer_.lock())
    {
        m_bIsRun_ = true;
        // 处理服务器自己的业务
        uint64_t i = 0;
        ::read(m_iFd_, &i , sizeof(uint64_t));
        
        printf("leader send appendentries\n");
        //pstServer->BroadCastAppendEntries();
    }
}


}
}

