#pragma once

#include <functional>
#include <vector>
#include <memory>
//#include <google/protobuf/message.h>

namespace google
{
namespace protobuf
{
class Descriptor;

class Message;
}
}

namespace dan
{
namespace mod
{

class MessageWrapper
{
public:
    MessageWrapper(int iID, const char* szName);

    ~MessageWrapper();

    void Deserialize();                 // 反序列化

    google::protobuf::Message* NewMessage();

    int ID(){return m_iID_;}

    const char* Name(){return m_szName_;}
private:
    int m_iID_;
    
    const char* m_szName_;

    const google::protobuf::Descriptor* m_pstDescriptor_;
    
    const google::protobuf::Message* m_pstMessage_;

};

}
}
