
/***************************************
*�ļ���:serverbase.h
*����:�������
*������:�Ӻ���
*����ʱ��:2009.06.10
*�޸ļ�¼
****************************************/
#ifndef _SPP_COMM_SERVERBASE_H_
#define _SPP_COMM_SERVERBASE_H_
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>
#include <list>
#include "log.h"			//��־
#include "stat.h"			//ͳ��
#include "procmon.h"		//���
#include "likelydef.h"
#include <string>
#include <vector>
#include <map>
#include <assert.h>

using namespace std;

using namespace comm::log;
using namespace spp::stat;
using namespace spp::procmon;

namespace spp 
{
namespace base 
{

#ifndef SIGUSR3
#define SIGUSR3 (SIGRTMIN + 1)
#endif

#define RUN_FLAG_QUIT		0x01
#define RUN_FLAG_RELOAD		0x02
#define RUN_FLAG_RELOAD_MODULE 0x04

#define SERVER_TYPE_UNKNOWN 0x00
#define SERVER_TYPE_PROXY   0x01
#define SERVER_TYPE_WORKER  0x02
#define SERVER_TYPE_CTRL	0x04

const unsigned short PKG_SOURCE_CLIENT = 1;//�ӿͻ��˹����İ�
const unsigned short PKG_SOURCE_SERVER = 2;//�Ӻ�˷����������İ�


//ǰ��ͨѶ�¼��ص���������
typedef enum 
{
    ET_CONNECTED = 0,	//���ӽ���
    ET_DISCONNECT,		//���ӶϿ�			
    ET_OVERLOAD,		//����
    ET_TIMEOUT,		        //���ӳ�ʱ(��ָ��ʱ����û���յ�����)
}event_type;

typedef enum
{
    EVENT_CONNECTED = 0,//���ӽ���
    EVENT_DISCONNECT,//���ӶϿ�
    EVENT_OVERLOAD,//����
    EVENT_TIMEOUT,//���ӳ�ʱ
}back_event_type;


//��ʱ�ص���������
//arg1:ͨ�ò���ָ��1��һ��ָ������blob
//arg2:ͨ�ò���ָ��2,ָ�������������
//arg3:ͨ�ò���ָ��3,�û�ע��ص�ʱ����Ĳ���ָ��
typedef void * (*cb_timer_func)(void * arg1,void * arg2,void * arg3);

//�¼��ص���������
//flow: ���ݰ�Ψһ��ʾ
//arg1: ͨ�ò���ָ��1��һ��ָ������blob
//arg2:ͨ�ò���ָ��2,ָ�������������
//arg3: ͨ�ò���ָ��3���û�ע��ص�����������Զ������ָ��
//����ֵ:0�ɹ�������ʧ��
typedef int (*cb_event_func)(unsigned int flow,void * arg1,void * arg2,void * arg3);

//worker���ͨѶ�¼��ص���������
//serv_type:��������
//flow: ���ݰ�Ψһ��ʶ
//arg1: ͨ�ò���ָ��1��һ��ָ������blob
//arg2:ͨ�ò���ָ��2,ָ�������������
//arg3: ͨ�ò���ָ��3���û�ע��ص�����������Զ������ָ��
//����ֵ:0�ɹ�������ʧ��
typedef int (*cb_back_event_func)(unsigned int serv_type,unsigned int flow,void * arg1,void * arg2,void * arg3);

//////////////////////////////////////////////////////////////
#if 0
typedef struct tagTContext
{
    unsigned int timestamp_;//ʱ���
    unsigned int timeout_;//��ʱʱ��
    unsigned int data_len;//data�ĳ���
    void * data;//�û�����ָ�룬�ڴ���ʹ���߷��估�ͷ�
}TContext;

/*********************************************************
//�����ĳ�ʱ�ص�����
//v:�����г�ʱ���������б���Ϣ��KEY
//arg1:Ŀǰ����
//arg2:��������������(CServerBase *)
//����0��ʾ�ɹ�����0ʧ��
*********************************************************/
typedef int (*cb_context_timeout_list)(const std::vector<string> & v,void * arg1,void * arg2);

//ͨ�õ������Ķ���
class CContextQueue
{
public:
    CContextQueue(){max_len_ = 10000;}
    ~CContextQueue(){}

    int SetMaxQueueLen(unsigned int len){max_len_ = len;return 0;}

    //������ز���
    int AddItem(const string & key,const TContext * context);
    TContext * GetItem(const string & key,bool bRemove = false);

    //ȡ�ó�ʱ���б�
    int GetTimeOutList(std::vector<string> & v);


protected:
    typedef std::map<std::string,TContext * > MapContext;
    typedef MapContext::iterator Iter;
    MapContext ctx_map_;

    unsigned int max_len_;

};

#endif

/////////////////////////////////////////////////////////////

//��������������࣬�������л�����ʼ������־��ͳ�ơ���ض���
class CServerBase
{
public:
    CServerBase();
    virtual ~CServerBase();
    virtual void ShowVerInfo();
    virtual void run(int argc, char* argv[]);
    virtual void startup(bool bg_run = true);
    //ʵ�ʵ����к���
    virtual void realrun(int argc, char* argv[]){}
    //���ط�������������
    virtual int servertype(){return SERVER_TYPE_UNKNOWN;}


    /***********************************************************
    //�������ݸ���˷�����
    //serv_type:��������
    //route_key:��·�ɵ�key,���ͨ����ֵ��ѡ��ͨѶ��·
    //arg1:ͨ�ò���ָ�룬һ��ָ��blob����
    //arg2::�Զ������,Ŀǰ����
    //����ֵ:>=0�ɹ���ʾ���͵��ֽ�����<0ʧ��
    ************************************************************/
	virtual int spp_sendto(unsigned int& to_flow, unsigned int serv_type, unsigned int route_key,void * arg1, void * arg2) {return -1;}

//    virtual int spp_sendto(unsigned int serv_type,unsigned int route_key,void * arg1,void * arg2) {return -1;}

    /***********************************************************
    //����:ͨ��flow�������ݸ���˷�����
    //flow:ͨѶΨһ��ʶ
    //arg1:ͨ�ò���ָ�룬һ��ָ��blob����
    //arg2::�Զ������,Ŀǰ����
    //����ֵ:>=0�ɹ���ʾ���͵��ֽ�����<0ʧ��
    ************************************************************/
    virtual int spp_sendbyflow(unsigned int flow,void * arg1,void * arg2){return -1;}

    /***********************************************************
    //����:ȡ��Group����
    //����:��
    //����ֵ:
    //>=0:�ɹ���< 0:ʧ��
    ************************************************************/
    virtual int spp_get_group_num(){return -1;}

    /***********************************************************
    //����:ȡ�õ�ǰ�������������
    //����:��
    //����ֵ:
    //>=0:�ɹ���< 0:ʧ��
    ************************************************************/
    virtual int spp_get_group_id(){return -1;}

    /*****************************************************************
    //����������
    //key:�����ĵ�Ψһ��ʶ
    //context:�û��Զ���������
    //����ֵ:0�ɹ���<0ʧ��
    ******************************************************************/
    //virtual int spp_set_context(const string & key,const TContext * context);

    /*****************************************************************
    //��ȡ������
    //key:�����ĵ�Ψһ��ʶ
    //bRemove:�Ƿ�Ӷ������Ƴ���������Ϣ
    //����ֵ���ɹ���key��ָ���������ָ�룬ʧ�ܣ�NULL
    ******************************************************************/
    //TContext * spp_get_context(const string & key,bool bRemove);

    //ע��timer
    //interval:���ü��ʱ��,��λΪ����
    //func: �ص�����
    //args: �û��Զ������ָ��, ��Ϊ�ص�������ͨ�ò�������
    //����ֵ:0�ɹ�������ʧ��
    int  reg_timer_proc(int interval,cb_timer_func func,void * args = NULL);

    //ע���¼�֪ͨ�ص�
    //et:�¼�����
    //func:�ص�����
    //args: �û��Զ������ָ��, ��Ϊ�ص������ĵ�3��ͨ�ò�������
    //����ֵ:0�ɹ�����0ʧ��
    int reg_event_proc(event_type et,cb_event_func func,void * args = NULL);

    //ע����ͨѶ�¼�֪ͨ�ص�
    //et:�¼�����
    //func:�ص�����
    //args: �û��Զ������ָ��, ��Ϊ�ص������ĵ�3��ͨ�ò�������
    //����ֵ:0�ɹ�����0ʧ��
    int reg_back_event_proc(back_event_type et,cb_back_event_func func,void * args = NULL);

    //��־
    CLog log_;
    //ͳ��
    CTStat stat_;
    //���
    CTProcMonCli moncli_;	
    //�ڴ�������������ȵ�������ʩ	
	
protected:

    virtual void do_timer_callback(const struct timeval & nowtime) = 0;

    struct TInternal;
    struct TInternal* ix_;

    //���ʹ����־
    CLog m_log_internal;

    typedef struct tagTimerInfo
    {
        int interval_;
        cb_timer_func func_;
        void * args_;
        struct timeval proctime_;//�ϴε��õ�ʱ��
    }TimerInfo,*LPTimerInfo;

    typedef std::list<LPTimerInfo> TimerList;
    typedef TimerList::iterator TimerListItr;
    TimerList timerlist_;

    cb_event_func  event_func_list_[ET_TIMEOUT + 1];
    void *	event_func_args_[ET_TIMEOUT + 1];

    cb_back_event_func back_event_func_list_[EVENT_TIMEOUT + 1];
    void * back_event_func_args_[EVENT_TIMEOUT + 1];
public:
    ///////////////////////////////////////////////////////////////////////
    static bool reload();
    static bool quit();
    static bool reloadmoduleconfig();
    static void sigusr1_handle(int signo);
    static void sigusr2_handle(int signo);
    static void sigusr3_handle(int signo);
    static unsigned char flag_;
    static char version_desc[64];
};

#ifdef _DEBUG_STD_
#define FW_DBG_STD(fmt, args...)  do{printf(fmt, ##args);printf("\n");}while(0)
#else
#define FW_DBG_STD(fmt, args...) 
#endif

#define FW_LOG_DBG(fmt, args...) do{m_log_internal.LOG_P_LEVEL(LOG_DEBUG,fmt,##args);FW_DBG_STD(fmt,##args);}while(0)
#define FW_LOG_INFO(fmt, args...) do{m_log_internal.LOG_P_LEVEL(LOG_NORMAL,fmt,##args);FW_DBG_STD(fmt,##args);}while(0)
#define FW_LOG_ERR(fmt, args...) do{m_log_internal.LOG_P_LEVEL(LOG_ERROR,fmt,##args);FW_DBG_STD(fmt,##args);}while(0)
#define FW_LOG_FATAL(fmt, args...) do{m_log_internal.LOG_P_LEVEL(LOG_FATAL,fmt,##args);FW_DBG_STD(fmt,##args);}while(0)
#define FW_LOG_BIN                      m_log_internal.LOG_P_BIN

}
}
#endif

