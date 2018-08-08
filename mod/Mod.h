#pragma once

#include <vector>
#include <memory>
namespace google
{
namespace protobuf
{
class Message;

}
}


namespace dan
{

namespace net
{
class Conn;
}

namespace mod
{
class MessageWrapper;

class Mod
{
public:
    Mod();

    static void LoadMsg();

    virtual ~Mod();

    void HandleArrivedMsg();                                                             //  到达的消息

    static void HandlePreSendMsg(std::weak_ptr<dan::net::Conn>& pstConn, 
                                 uint16_t dwID, google::protobuf::Message&& stMessage);         //  准备发送的消息

    virtual void Proc(std::weak_ptr<dan::net::Conn>& pstConn, int iID, std::unique_ptr<google::protobuf::Message>& pstMessage) {};

    void Tie(std::shared_ptr<dan::net::Conn>& pstConn);

    //Message* InBuffer2Message();                  //

    //static SendToClient();

    //int Name2ID(const char* szName);

    //const char* ID2Name(int iID);

    std::weak_ptr<dan::net::Conn>* Conn(){return &s_pstConn;}
private:
    typedef std::vector<std::unique_ptr<MessageWrapper>> TSubMessages;

    static std::vector<std::unique_ptr<TSubMessages>> s_stMessages;   // 1*256+1

    static std::vector<std::unique_ptr<Mod>> s_stMods;
    
    std::weak_ptr<dan::net::Conn> s_pstConn;
};

}
}
