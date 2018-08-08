#include <raft_server/nanoraft/RaftLogEntry.h>

namespace dan
{

namespace nanoraft
{

RaftLogEntry::RaftLogEntry() noexcept:
    m_dwTerm_(0),
    m_dwIndex_(0),
    m_stType_(RaftLogEntryType::Normal),
    m_pstDataBuf_(),
    m_pstDataLen_(0)
{}

RaftLogEntry::~RaftLogEntry() noexcept
{}



}
}
