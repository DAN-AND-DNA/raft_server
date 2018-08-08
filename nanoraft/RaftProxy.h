#pragma once

#include <stdint.h>
#include <memory>

namespace dan
{
namespace net
{
class Conn;
}

namespace nanoraft 
{

class RaftServer;

// FIXME enum 容易重定义 用enum class
// 集群中的3种节点角色: 领导人 跟随者 候选者
enum class RaftProxyRole:uint8_t
{
    None,
    Candidate,                  // 初始就是候选人进行选举
    Follower,                   // 作为follower 只是响应leader
    Leader                      // 作为leader  决定follower的日志，不需要跟follower商量
};


// 集群中的节点的状态: 
enum class RaftProxyState:uint8_t
{
    IsVoting = 0,               // 初始就作为候选人进行选举
    SufficientLog,              // 足量的日志
    InActive                    // 断线
};


// 集群中节点(在本机的代理) 
class RaftProxy
{
public:
    RaftProxy() noexcept;
    ~RaftProxy() noexcept;

    RaftProxyRole Role() {return m_stRole_;}
    void SetRole(RaftProxyRole stRole){m_stRole_ = stRole;}
    RaftProxyState State() {return m_stState_;} 
    void SetState(RaftProxyState stState){m_stState_ = stState;}
    uint32_t ID() {return m_dwID_;}
    void SetID(uint32_t dwID) {m_dwID_ = dwID;}
    uint64_t NextIndex(){return m_ulNextIndex_;}                            // 返回要发送给这个代理的日志序列号
    void SetNextIndex(uint64_t ulNextIndex){m_ulNextIndex_ = ulNextIndex;}  // 设置要发送给这个代理的日志序列号
    uint64_t MatchIndex(){return m_ulMatchIndex_;}
    void SetMatchIndex(uint64_t ulMatchIndex){m_ulMatchIndex_ = ulMatchIndex;}

    void Tie(std::shared_ptr<dan::net::Conn>& pstConn);

    void SendAppendEntries();

    int ConnFd(){return m_iConnFd_;}
private:
    RaftProxyRole m_stRole_;                            // 代理的角色: 0领导人 1跟随者 2候选者
    RaftProxyState m_stState_;                          // 代理的状态: 0进行投票 1足够的日志满足投票的要求

    uint32_t m_dwID_;                                   // 节点的唯一ID
    uint64_t m_ulNextIndex_;                            // 如果本机作为leader 需要发给这个节点的日志索引号
    uint64_t m_ulMatchIndex_;                           // 如果本机作为leader 已经发给这个节点的日志索引号最大值

    int m_iConnFd_;
    //std::weak_ptr<RaftServer> m_pstOwnerRaftServer_;
    std::weak_ptr<dan::net::Conn> m_pstConn_;           // 代理和节点的通信连接

};


}
}

