#pragma once


#include <stdint.h>
#include <memory>

namespace dan
{
namespace nanoraft
{
// 日志项的类型
enum class RaftLogEntryType:uint8_t
{
    Normal = 0,             // 普通日志
    AddNode,                // 配置变更 新增节点
    RemoveNode              // 配置变更 移除节点
};


class RaftLogEntry
{
public:
    RaftLogEntry(uint32_t dwIndex, uint32_t dwTerm, uint32_t dwWriteIt) noexcept;
    ~RaftLogEntry() noexcept;

    uint32_t Term() {return m_dwTerm_;}
private:
    uint32_t                    m_dwTerm_;              // 日志项对应的任期
    uint32_t                    m_dwIndex_;             // 日志项对应的序列号
    RaftLogEntryType            m_stType_;              // 日志项的类型
    
    uint32_t                    m_dwWriteIt_;           //
   
    std::unique_ptr<uint8_t>    m_pstDataBuf_;          // pb二进制数据
    uint32_t                    m_pstDataLen_;          // pb长
};



}
}
