#include <stdarg.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <dlfcn.h>
#include "protocol_pack.h"
#include "benchadapter.h"
#include "benchapi.h"
#include "benchapiplus.h"
#include "commu.h"
#include "hexdump.h"

using namespace comm::commu;
using namespace comm::util;

/////////////////////////////////////////////////////////////////////////////////
//so library
////////////////////////////////////////////////////////////////////////////////
spp_dll_func_t sppdll = {NULL};

int log_init (const char* dir, int lvl, u_int size, const char* pre_name);

/*****************************************************************************
//���ݽ��գ�����ʵ�֣�
//flow:	�������־
//arg1:	���ݿ����
//arg2:	��������������
//����������ʾ�����Ѿ����������Ҹ�ֵ��ʾ���ݰ��ĳ��ȣ�
//0ֵ��ʾ���ݰ���δ����������������ʾ����
*******************************************************************************/
extern "C"  int default_handle_input(unsigned flow, void* arg1, void* arg2)
{
    const unsigned int MSG_HEADER_LEN = 29;
    blob_type* blob = (blob_type*)arg1;
    if( (unsigned int) blob->len <= MSG_HEADER_LEN)
    {
        //����û������
        return 0;
    }


    unsigned int *p = (unsigned int *)(blob->data + 3);
    unsigned int packlen = ntohl(*p);
    #ifdef OPEN_PRINT
    printf("%s:pack_len=%d,blob_len=%d\nBegin print Packet:\n",__FUNCTION__,packlen,blob->len);
    HexDumpImp(blob->data,blob->len);
    printf("end print of packet\n");
    #endif

    if( (unsigned int)blob->len < packlen)
    {
        return 0;
    }

    return packlen;
}

/***********************************************************
//�����ڴ�ܵ�������ѡʵ�֣�
//flow:	�������־
//group_id:�ڴ�ܵ��ķ���ID
//arg1:	���ݿ����
//arg2:	��������������(CServerBase *)
//����ֵ:
// 0:��ʾ���������ڿ����
// <0:��ʾ����������������ر�����
// >0:��ʾ����������������ر�����
//˵��:�ýӿڽ�proxy��Ҫ���ã����޸ýӿڣ����Ĭ�Ͻ����ݱ���
//����:��proxy�����ڻ���socket���ݵ��ڴ������󣬻ᶪ����socket���л�������ݣ����ر�����
************************************************************/
extern "C" int default_handle_queue_full(unsigned int flow,unsigned int group_id, void* arg1, void* arg2)
{
    return 0;
}

/*******************************************
//��ȡ�������ӿ� (��ѡʵ�֣�
********************************************/
//serv_type:��������
//arg1:	���ݿ����
//arg2:	��������������(CServerBase *)
//����ֵ0��ʾ�ɹ�����0ʧ��
//˵�����������Ҫ����������Ϣʱ������øýӿڻ�ȡ�������ݰ���ҵ��Ӧ�������ݿ������������Ȼ�󷵻�
//˵����Ŀǰ���������Ĭ�ϵ�ʵ�֣������������ҵ�����ݰ�������Ҫʵ��
extern "C"  int default_handle_heartbeat(unsigned int serv_type,void * arg1,void * arg2)
{
    blob_type* blob = (blob_type*)arg1;
    static unsigned int seq = 0;

    static protocol::MsgHeader header;
    static protocol::Packer pack;
    const unsigned short IM_KEEP_ALIVE = 0x0005;

    pack.Reset();
    header.main_cmd = IM_KEEP_ALIVE;
    header.seq = ++seq;

    string strMsg = pack.GetPack(header);

    memcpy(blob->data,strMsg.data(),strMsg.size());
    blob->len = strMsg.size();

    return 0;
    
}

/*******************************************
//�¼�·�ɽӿ� (��ѡʵ�֣�
********************************************/
//flow:���ݰ�Ψһ��־
//event:�¼�����
//arg1:	���ݿ����
//arg2:	��������������(CServerBase *)
//����ֵ:
//0��ʾ�������¼�·�ɵ����е���
//>0��ʾ�������¼�·�ɵ�ָ������
//<0��ʾ�����ĸ������¼�
extern "C" int default_handle_event_route(unsigned int flow,unsigned int event,void * arg1,void * arg2)
{
    return -1;
}


int load_bench_adapter(const char* file)
{

    if(unlikely(sppdll.handle != NULL))
    {
        dlclose(sppdll.handle);
    }

    memset(&sppdll, 0x0, sizeof(spp_dll_func_t));

    void* handle = dlopen(file, RTLD_NOW);
    //��������
    int nRetryCnt = 3;
    while(nRetryCnt-- && handle == NULL )
    {
        printf("dlerro info=%s,retry...\n",dlerror());
        handle = dlopen(file, RTLD_NOW);
    }

    assert(handle != NULL);

    sppdll.spp_handle_init = (spp_handle_init_t)dlsym(handle, "spp_handle_init");

    sppdll.spp_handle_input = (spp_handle_input_t)dlsym(handle,"spp_handle_input");
    if(sppdll.spp_handle_input == NULL)
    {
        sppdll.spp_handle_input = (spp_handle_input_t)default_handle_input;
    }

    sppdll.spp_handle_route = (spp_handle_route_t)dlsym(handle, "spp_handle_route");

    sppdll.spp_handle_queue_full = ( spp_handle_queue_full_t) dlsym(handle,"spp_handle_queue_full");
    if(sppdll.spp_handle_queue_full == NULL )
    {
        sppdll.spp_handle_queue_full = (spp_handle_queue_full_t ) default_handle_queue_full;
    }

    sppdll.spp_handle_reloadconfig = (spp_handle_reloadconfig_t) dlsym(handle,"spp_handle_reloadconfig");

    sppdll.spp_handle_event_route = (spp_handle_event_route_t) dlsym(handle,"spp_handle_event_route");
    if(sppdll.spp_handle_event_route == NULL )
    {
        sppdll.spp_handle_event_route = (spp_handle_event_route_t) default_handle_event_route;
    }

    sppdll.spp_handle_process = (spp_handle_process_t)dlsym(handle, "spp_handle_process");

    sppdll.spp_handle_fini = (spp_handle_fini_t)dlsym(handle, "spp_handle_fini");

    assert(sppdll.spp_handle_input != NULL && sppdll.spp_handle_process != NULL );
    sppdll.handle = handle;
    return 0;
}


