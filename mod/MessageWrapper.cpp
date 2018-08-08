#include <raft_server/mod/MessageWrapper.h>
#include <google/protobuf/message.h>

#include <raft_server/protocol/api.pb.h>

namespace dan
{
namespace mod
{

MessageWrapper::MessageWrapper(int iID, const char* szName):
        m_iID_(iID),
        m_szName_(szName),
        m_pstDescriptor_(google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(szName)),
        m_pstMessage_(google::protobuf::MessageFactory::generated_factory()->GetPrototype(m_pstDescriptor_))
    {
          std::cout<<"reload "<<szName<<" is done\n";
    }

MessageWrapper::~MessageWrapper(){}

google::protobuf::Message* MessageWrapper::NewMessage()
{
    return m_pstMessage_->New();
}

}
}
