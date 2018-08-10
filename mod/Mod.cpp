#include <raft_server/mod/Mod.h>
#include <raft_server/mod/MessageWrapper.h>
#include <raft_server/mod/ModApi.hpp>
#include <raft_server/net/Conn.h>
#include <google/protobuf/message.h>
#include <sys/socket.h>

#include <sys/time.h>
#include <string.h>

namespace
{

std::unique_ptr<dan::mod::Mod> p(new dan::mod::ModApi());

}

namespace dan
{
namespace mod
{
std::vector<std::unique_ptr<Mod::TSubMessages>> Mod::s_stMessages;

std::vector<std::unique_ptr<Mod>> Mod::s_stMods;

Mod::Mod():
    s_pstConn()
{}

void Mod::LoadMsg()
{
    //std::unique_ptr<dan::mod::Mod> p(new dan::mod::ModApi());  //FIXME code dump
    
    s_stMods.push_back(std::move(p));
    
    std::unique_ptr<TSubMessages> v(new TSubMessages());

    std::cout<<"reload msg\n";
  
    std::unique_ptr<MessageWrapper> m1(new MessageWrapper(257, "api.handshake_q"));
    std::unique_ptr<MessageWrapper> m2(new MessageWrapper(258, "api.handshake_r"));
    std::unique_ptr<MessageWrapper> m3(new MessageWrapper(259, "api.appendentries_q"));
    std::unique_ptr<MessageWrapper> m4(new MessageWrapper(260, "api.appendentries_r"));
    std::unique_ptr<MessageWrapper> m5(new MessageWrapper(261, "api.requestvote_q"));
    std::unique_ptr<MessageWrapper> m6(new MessageWrapper(262, "api.requestvote_r"));

    v->push_back(std::move(m1));
    v->push_back(std::move(m2));
    v->push_back(std::move(m3));
    v->push_back(std::move(m4));
    v->push_back(std::move(m5));
    v->push_back(std::move(m6));

    s_stMessages.push_back(std::move(v));
}

Mod::~Mod(){}
/*
bool Mod::Lock()
{
    if(s_pstConn.lock())
        return true;
    return false;
}
*/

void Mod::Tie(std::shared_ptr<dan::net::Conn>& pstConn)
{

    s_pstConn = pstConn;
//    for(auto& p : s_stMods)
  //      (*p->Conn()) = pstConn;
}

void Mod::HandleArrivedMsg()
{
    if(auto s_pstsConn = s_pstConn.lock())
    {
        while(true)
        {
            //TODO 解析缓存
            if(s_pstsConn->InBufferSize() < 6)
                return;


            uint16_t dwID = *(reinterpret_cast<uint16_t*>(s_pstsConn->InBufferPtr(0)));
            uint32_t dwLen = *(reinterpret_cast<uint32_t*>(s_pstsConn->InBufferPtr(2)));
       

            // dwID = 257;
            int iMainID = (dwID / 256) - 1;

            int dwSubID = (dwID % 256) - 1;

            if(s_pstsConn->InBufferSize() < (static_cast<int>(dwLen) + 6))
                return;
            
            struct timeval tv;
       
            //TODO 消耗缓存
            std::unique_ptr<google::protobuf::Message> pstMessage(((*(s_stMessages[iMainID]))[dwSubID])->NewMessage());
 
            ::gettimeofday(&tv, NULL);
            printf("pars:%ld\n", tv.tv_sec*1000000 + tv.tv_usec);


       

            pstMessage ->ParseFromArray(s_pstsConn->InBufferPtr(6), dwLen);

            ::gettimeofday(&tv, NULL);
            printf("after pars:%ld\n", tv.tv_sec*1000000 + tv.tv_usec);



            int iLeftSize = s_pstsConn->InBufferSize() - (dwLen + 6);
        
            ::memcpy(s_pstsConn->InBufferPtr(0), s_pstsConn->InBufferPtr(dwLen + 6), iLeftSize);
            s_pstsConn->SetInBufferSize(iLeftSize);

            std::cout<<"in factory ID:"<<dwID<<" len:"<<dwLen<<std::endl;
            std::cout<<"mainID:"<<iMainID<<" SubID:"<<dwSubID<<std::endl;
        
            std::cout<<((*(s_stMessages[iMainID]))[dwSubID])->Name()<<std::endl;
 
            ::gettimeofday(&tv, NULL);
            printf("befor P:%ld\n", tv.tv_sec*1000000 + tv.tv_usec);

       
            s_stMods[iMainID]->Proc(s_pstConn, dwSubID, pstMessage);

            if(s_pstsConn->OutBufferSize() > 0)
            {
                s_pstsConn->EnableWrite();
            }
            else if(s_pstsConn->OutBufferSize() == 0)
            {
                std::cout<<"disable\n";
                s_pstsConn->DisableWrite(); 
            }
        
            ::gettimeofday(&tv, NULL);
            printf("mod done:%ld\n", tv.tv_sec*1000000 + tv.tv_usec);
        }
    }
}

void Mod::HandlePreSendMsg(std::weak_ptr<dan::net::Conn>& pstConn, uint16_t dwID, google::protobuf::Message&& stMessage)
{
                struct timeval tv;
        ::gettimeofday(&tv, NULL);
        printf("in HandleP:%ld\n", tv.tv_sec*1000000 + tv.tv_usec);


    std::cout<<"in handlePresend\n";
    uint16_t dwMsgID = dwID;

    uint32_t dwLen = stMessage.ByteSize();

    if (auto pstsConn = pstConn.lock())
    {

        std::cout<<"locked:"<<dwMsgID<<" "<<dwLen<<std::endl;
        ::memcpy(pstsConn->OutBufferPtr(pstsConn->OutBufferSize()), reinterpret_cast<uint8_t*>(&dwMsgID), 2);
        ::memcpy(pstsConn->OutBufferPtr(pstsConn->OutBufferSize() + 2), reinterpret_cast<uint8_t*>(&dwLen), 4);
        stMessage.SerializeToArray(pstsConn->OutBufferPtr(pstsConn->OutBufferSize() + 6) , dwLen);

        pstsConn->SetOutBufferSize(pstsConn->OutBufferSize() + 6 + dwLen);
        // FIXME 尝试直接发送 -10us左右的延迟
        int iSended = static_cast<int>(::send(pstsConn-> Fd(), pstsConn->OutBufferPtr(0), pstsConn->OutBufferSize(), 0));

        if(iSended > 0)
        {
            std::cout<<"直接发送给:"<<pstsConn-> Fd()<<" 字节:"<<iSended<<std::endl;
            int iLeftSize = pstsConn->OutBufferSize() - iSended;
            pstsConn->SetOutBufferSize(iLeftSize);

            if(iLeftSize > 0)
                ::memcpy(pstsConn->OutBufferPtr(0), pstsConn->OutBufferPtr(iSended), iLeftSize); 
        ::gettimeofday(&tv, NULL);
        printf("发送完毕:%ld\n", tv.tv_sec*1000000 + tv.tv_usec);


        
        }
        else if(iSended == 0)
        {
            // nothing
        }
        else if(errno != EAGAIN) 
        {
            // TODO error
        }
    }
}


}
}
