#include <raft_server/nanoraft/RaftLogEntry.h>

namespace dan
{

namespace nanoraft
{

RaftLogEntry::RaftLogEntry(uint32_t dwIndex, uint32_t dwTerm, uint32_t dwWriteIt) noexcept:
    m_dwTerm_(dwTerm),
    m_dwIndex_(dwIndex),
    m_stType_(RaftLogEntryType::Normal),
    m_dwWriteIt_(dwWriteIt)
    // m_pstDataBuf_(),
  //  m_pstDataLen_(0)
{}

RaftLogEntry::~RaftLogEntry() noexcept
{}



}
}
