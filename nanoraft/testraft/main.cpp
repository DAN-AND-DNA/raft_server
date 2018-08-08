#include <nanoraft/RaftServer.h>
class test_server
{
public:
    test_server():
        m_stRaftServer_()
    {
    //    m_stRaftServer_.SetSendAppendCallback(std::move(std::bind(&test_server::en, this)));
       // m_stRaftServer_.doit();
    }

private:
    void en(){printf("en!!!!\n");}

private:
    dan::nanoraft::RaftServer m_stRaftServer_;
};



int main()
{   

    test_server ts;
    /*
    dan::nanoraft::RaftServer* pstRaftServer = new dan::nanoraft::RaftServer(); 

    std::weak_ptr<dan::nanoraft::RaftProxy> pstWeak(std::move(pstRaftServer->GetProxyByID(0)));
    
    if(auto p = pstWeak.lock())
    {
        printf("is not nullptr\n");
    }
    printf("is nullptr\n");

    
    */
    return 0;
}
