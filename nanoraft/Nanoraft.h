#pragma once


namespace dan
{

namespace nanoraft
{


// FIXME enum 容易重定义 用enum class
// 集群中的3种节点角色: 领导人 跟随者 候选者
enum class RaftProxyRole:uint8_t
{
    NewOne = 0,                 // 新节点
    Follower,                   // 作为follower 只是响应leader
    Candidate,                  // leader奔溃了！
    Leader                      // 作为leader  决定follower的日志，不需要跟follower商量
};


// 集群中的节点的状态: 
enum class RaftProxyState:uint8_t
{
    WaitingForLog = 0,          // 等待日志同步
    SufficientLog,              // 足量的日志
};




}
}
