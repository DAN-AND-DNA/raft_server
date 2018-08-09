#include <raft_server/nanoraft/RaftProxy.h>
#include <raft_server/net/Conn.h>

namespace dan
{
namespace nanoraft
{

RaftProxy::RaftProxy() noexcept:
    m_stRole_(RaftProxyRole::Candidate),
    m_stState_(RaftProxyState::IsVoting),
    m_ulNextIndex_(1),
    m_ulMatchIndex_(0),
    m_iConnFd_(-1),
    m_pstConn_()
{}

RaftProxy::~RaftProxy() noexcept{}

void RaftProxy::Tie(std::shared_ptr<dan::net::Conn>& pstConn){m_pstConn_ = pstConn; m_iConnFd_ = pstConn->Fd();}


void RaftProxy::SendAppendEntries()
{
    if(auto pstConn = m_pstConn_.lock())
    {
        pstConn->SendAppendEntries();
    }
}



}
}
