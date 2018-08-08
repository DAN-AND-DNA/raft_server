#include <raft_server/eventloop/IPoller.h>
#include <raft_server/eventloop/EPoller.h>
#include <raft_server/eventloop/EventLoop.h>


namespace dan
{
namespace eventloop
{

IPoller::~IPoller(){}

IPoller* IPoller::NewPoller(EventLoop* pstEventLoop){return new EPoller(pstEventLoop);}

}
}
