#ifndef _SPP_PROXY_DEFAULT_H_
#define _SPP_PROXY_DEFAULT_H_
#include "serverbase.h"
#include "shmcommu.h"
#include "sockcommu.h"
#include "benchapiplus.h"
#include <vector>
#include <set>
#include <dlfcn.h>
#define IPLIMIT_DISABLE	  0x0
#define IPLIMIT_WHITE_LIST    0x1
#define IPLIMIT_BLACK_LIST    0x2

using namespace spp::base;
using namespace comm::commu;
using namespace comm::commu::shmcommu;
using namespace comm::sockcommu;
using namespace std;

namespace spp
{
namespace proxy
{
class CDefaultProxy : public CServerBase
{
public:
    CDefaultProxy();
    ~CDefaultProxy();

    //ʵ�����к���
    void realrun(int argc, char* argv[]);
    //������������
    int servertype() {return SERVER_TYPE_PROXY;}
    //��ʼ������
    int initconf(bool reload = false);

    virtual int spp_get_group_num();

    virtual int spp_get_group_id();

    //һЩ�ص�����
    static int ator_recvdata(unsigned flow, void* arg1, void* arg2);    //��Ҫ

    static int ctor_recvdata(unsigned flow, void* arg1, void* arg2);    //��Ҫ

    static int ator_overload(unsigned flow, void* arg1, void* arg2);    //�Ǳ�Ҫ
    static int ator_connected(unsigned flow, void* arg1, void* arg2);   //�Ǳ�Ҫ

    static int ator_timeout(unsigned flow, void* arg1, void* arg2);    //�Ǳ�Ҫ
    static int ator_disconn(unsigned flow, void* arg1, void* arg2);    //�Ǳ�Ҫ

    //static int ator_recvdone(unsigned flow, void* arg1, void* arg2);    //�Ǳ�Ҫ
    //static int ctor_recvdone(unsigned flow, void* arg1, void* arg2);    //�Ǳ�Ҫ



    //������
    CTCommu* ator_;
    //������
    vector<CTCommu*> ctor_;
    //ip��������
    unsigned char iplimit_;
    //ip����
    set<unsigned> iptable_;    
    spp_handle_local_process_t local_handle; 

protected:
    virtual void do_timer_callback(const struct timeval & nowtime);

    struct IProxyParams;//�����˽�����ݽṹ
    IProxyParams * IParam_;

};

}
}
#endif

