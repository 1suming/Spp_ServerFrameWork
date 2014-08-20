#ifndef _SPP_WORKER_DEFAULT_H_
#define _SPP_WORKER_DEFAULT_H_
#include <dlfcn.h>
#include <algorithm>

#include "serverbase.h"
#include "shmcommu.h"
#include "sockcommu.h"
#include "benchapiplus.h"
#include "commumng.h"



using namespace spp::base;
using namespace comm::commu;
using namespace comm::commu::shmcommu;
using namespace comm::sockcommu;

namespace spp
{
namespace worker 
{
class CDefaultWorker : public CServerBase
{
public:
    CDefaultWorker();
    ~CDefaultWorker();

    //ʵ�����к���
    void realrun(int argc, char* argv[]);
    //������������
    int servertype() {return SERVER_TYPE_WORKER;}
    //��ʼ������
    int initconf(bool reload = false);

	virtual int spp_sendto(unsigned int& to_flow, unsigned int serv_type, unsigned int route_key,void * arg1, void * arg2);

//    virtual int spp_sendto(unsigned int serv_type, unsigned int route_key,void * arg1, void * arg2);

    virtual int spp_sendbyflow(unsigned int flow, void * arg1, void * arg2);


    virtual int spp_get_group_num();

    virtual int spp_get_group_id();

    //һЩ�ص�����
    static int ator_recvdata(unsigned flow, void* arg1, void* arg2);	//��Ҫ
    static int ator_senddata(unsigned flow, void* arg1, void* arg2);	//�Ǳ�Ҫ
    static int ator_overload(unsigned flow, void* arg1, void* arg2);	//�Ǳ�Ҫ

    static int ator_connected(unsigned flow, void* arg1, void* arg2);   //�Ǳ�Ҫ
    static int ator_timeout(unsigned flow, void* arg1, void* arg2);	//�Ǳ�Ҫ
    static int ator_disconn(unsigned flow, void* arg1, void* arg2);	//�Ǳ�Ҫ


    //connector��һЩ�ص�����
    static int ctor_recvdata(unsigned flow,void * arg1,void * arg2);
    static int ctor_senddata(unsigned flow,void * arg1,void * arg2);

    static int ctor_overload(unsigned flow, void* arg1, void* arg2);	//�Ǳ�Ҫ
    static int ctor_connected(unsigned flow, void* arg1, void* arg2);   //�Ǳ�Ҫ

    static int ctor_timeout(unsigned flow, void* arg1, void* arg2);	//�Ǳ�Ҫ
    static int ctor_disconn(unsigned flow, void* arg1, void* arg2);	//�Ǳ�Ҫ

    static int ctor_connect(unsigned int flow,void * arg1,void * arg2);//��Ҫ

protected:

    //��д����ĳ�ʱ�ص�
    virtual void do_timer_callback(const struct timeval & nowtime );


    /*************************************
    //��ʼ��·�ɱ�
    //TRouteTable:·�ɱ���Ϣ	
    //route_cfg:·�ɱ��ļ�
    //����ֵ:�ɹ�0��-1ʧ��
    **************************************/
    int InitRouteTable(TRouteTable & route_tbl,const string & route_cfg);

    int InitRouteConfig(TRouteConfig * config,const string & route_file,bool reload);


    //������
    CTCommu* ator_;

    //������ӹ���
    CCommuMng commu_mng_;

    //������
    CTCommu * ctor_;


    TRouteConfig * route_config_;//·��������Ϣ

};

}
}
#endif

