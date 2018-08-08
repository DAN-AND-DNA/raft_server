#pragma once
#include <vector>
#include <memory>

namespace dan
{
namespace nanoraft
{

class RaftLogEntry;

// 这个消息用来使得follower应用日志项到FSM
// 这个消息也用来作为心跳
class AppendEntries
{

public:
    AppendEntries(uint32_t dwTerm = 0, uint32_t dwPrevLogIndex = 0, uint32_t dwPrevLogTerm = 0, uint32_t dwLeaderCommit = 0):
        m_dwTerm_(dwTerm),
        m_dwPrevLogIndex_(dwPrevLogIndex),
        m_dwPrevLogTerm_(dwPrevLogTerm),
        m_dwLeaderCommit_(dwLeaderCommit),
        m_stEntries_()
    {}

    uint32_t Term(){return m_dwTerm_;}
    uint32_t PrevLogIndex(){return m_dwPrevLogIndex_;}
    uint32_t PrevLogTerm(){return m_dwPrevLogTerm_;}
    uint32_t LeaderCommit(){return m_dwLeaderCommit_;}
    uint64_t EntriesNum(){return m_stEntries_.size();}

    void SetTerm(uint32_t dwTerm) {m_dwTerm_ = dwTerm;}
    void SetPrevLogIndex(uint32_t dwPrevLogIndex) {m_dwPrevLogIndex_ = dwPrevLogIndex;}
    void SetPrevLogTerm(uint32_t dwPrevLogTerm) {m_dwPrevLogTerm_ = dwPrevLogTerm;}
    void SetLeaderCommit(uint32_t dwLeaderCommit) {m_dwLeaderCommit_ = dwLeaderCommit;}
private:
    uint32_t                                    m_dwTerm_;            // 当前任期
    uint32_t                                    m_dwPrevLogIndex_;    // 上个索引号 
    uint32_t                                    m_dwPrevLogTerm_;     // 上个任期
    uint32_t                                    m_dwLeaderCommit_;    // leader已经提交(多数节点已经添加了该日志)了的序列号
    std::vector<std::shared_ptr<RaftLogEntry>>  m_stEntries_;         
};

}
}
