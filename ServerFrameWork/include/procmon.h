/********************************************
//�ļ���:procmon.h
//����:���SERVER�����CLIENT
//����:�Ӻ���
//����ʱ��:2009.06.11
//�޸ļ�¼:

*********************************************/
#ifndef _TBASE_TPROCMON_H_
#define _TBASE_TPROCMON_H_
#include <unistd.h>
#include <assert.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include "list.h"
#include "loadtable.h"

using namespace std;
using namespace comm::load;

namespace spp 
{
namespace procmon 
{

#define MAX_SERVER_FLAG_LEN 64                 //���������ĳ���	
#define MAX_FILEPATH_LEN	128			//����ļ���·��
#define BUCKET_SIZE			10			//ÿ������Ĺ�ϣͰ����
#define MAX_PROC_GROUP_NUM  128			//�����������
#define MAX_MSG_BUFF        100			//��Ϣ��������С
#define MSG_EXPIRE_TIME		120			//��Ϣ����ʱ��
#define MSG_VERSION         0x01		//��Ϣ�汾��
#define MSG_ID_SERVER       0x01        //SERVER���͵���ϢID��CLIENT���͵���ϢID����MSG_ID_SERVER
#define MSG_SRC_SERVER      0x00		//������ΪSERVER
#define MSG_SRC_CLIENT		0x01		//������ΪCLIENT

#define PROCMON_EVENT_PROCDEAD		1	//����ʧЧ
#define PROCMON_EVENT_OVERLOAD 	(1<<1)	//���ع���
#define PROCMON_EVENT_LOWSRATE	(1<<2)  //�ɹ��ʹ���
#define PROCMON_EVENT_LATENCY 	(1<<3)  //�ӳ�̫��
#define PROCMON_EVENT_OTFMEM   	(1<<4)	//ʹ���ڴ����
#define PROCMON_EVENT_PROCDOWN  (1<<5)  //����������
#define PROCMON_EVENT_PROCUP  	(1<<6)  //������̫��

#define PROCMON_CMD_KILL                  0x1		//ɱ������
#define PROCMON_CMD_LOAD                 0x2		//��������
#define PROCMON_CMD_FORK	        0x4		//��������

#define PROCMON_STATUS_OK          	0x0    //����״̬
#define PROCMON_STATUS_OVERLOAD     1      //����̫��
#define PROCMON_STATUS_LOWSRATE    (1<<1)  //�ɹ��ʹ���
#define PROCMON_STATUS_LATENCY     (1<<2)  //�ӳ�̫��
#define PROCMON_STATUS_OTFMEM      (1<<3)  //ʹ���ڴ�̫��



/////////////////////////////�ⲿ�ɼ����ݽṹ///////////////////////////////////////		
typedef struct 
{
    int groupid_;						//������ID 
    int adjust_proc_time;
    char basepath_[MAX_FILEPATH_LEN];   //����·��
    char exefile_[MAX_FILEPATH_LEN];	//���̿�ִ���ļ���
    char etcfile_[MAX_FILEPATH_LEN];	//���������ļ���
    char server_flag_[MAX_SERVER_FLAG_LEN];   //���̱��
    int exitsignal_;					//kill���̵��ź�
    unsigned maxprocnum_;				//��������
    unsigned minprocnum_;				//��С������
    unsigned heartbeat_;				//������ʱʱ��
    unsigned maxwatermark_;				//�����ˮλ
    unsigned short minsrate_;			//��ͳɹ���
    unsigned short maxdelay_;			//�����ʱ
    unsigned maxmemused_;				//����ڴ�ʹ����
    char reserve_[8];
}TGroupInfo;//��������Ϣ

typedef struct 
{
    int groupid_;			//���������
    int procid_;			//����ID
    long timestamp_;		//ʱ���
    unsigned watermark_;	//���̸���
    unsigned short srate_;	//�ɹ���
    unsigned short delay_;	//ƽ����ʱ
    unsigned memused_;      //ʹ���ڴ�
    unsigned curconnnum_;//��ǰ��������
    char reserve_[8];
}TProcInfo;//������Ϣ

typedef struct 
{
    int groupid_;		//���������
    int procid_;		//����ID
    unsigned cmd_;		//����ID
    int arg1_;			//����1
    int arg2_;			//����2
    char reserve_[8];
}TProcEvent;//�¼�֪ͨ

typedef struct
{
    long msgtype_;		//��Ϣ���ͣ�ָ��Ϣ�����ߣ�����ָ����ID��
    long msglen_; 		//��Ϣ����
    long srctype_;		//��Ϣ�������������λ0--server, ��0--client������λ��ʾ�汾��
    long timestamp_;	//ʱ���
    char msgcontent_[MAX_MSG_BUFF]; //�˴����TProcInfo����TProcEvent��Ϣ
}TProcMonMsg;//����������ͻ���ͨѶ��Ϣ��

//////////////////////////////�ͻ������������ͨѶ��ʽ///////////////////////////////////		
//ͨѶ���ࣨ�����Ǳ���ͨѶ����������С�����Ҫ��ʵ��Ϊ��������ʽ��
class CCommu
{
public:
    CCommu(){}
    virtual ~CCommu(){}
    //��ʼ��
    //args:	ָ�������ָ��
    //����ֵ��0�ɹ�������ʧ��
    virtual int init(void* args) = 0;
    //������Ϣ
    //msg:	��Ž��յ���Ϣ
    //msgtype:	��Ϣ���� 1�����������˵���Ϣ��>1�����ͻ��˵���Ϣ
    //����ֵ:  >0���յ���Ϣ��С������ʧ��
    virtual int recv(TProcMonMsg* msg, long msgtype = 1) = 0;
    //������Ϣ
    //msg:  Ҫ���͵���Ϣ
    //����ֵ:  0�ɹ�������ʧ��
    virtual int send(TProcMonMsg* msg) = 0;
};

//��Ϣ����ͨѶ�ࣨĬ��ʹ�÷�ʽ��
class CMQCommu : public CCommu
{
public:
    CMQCommu();
    CMQCommu(key_t mqkey);
    ~CMQCommu();
    int init(void* args);
    int recv(TProcMonMsg* msg, long msgtype = 0);
    int send(TProcMonMsg* msg);
protected:
    int mqid_;
};
//......��չ�����ͨѶ�࣬��udp, fifo, share memory�ȵ�.....


////////////////////////////////�ڲ�ʹ�����ݽṹ//////////////////////////////////////		
typedef struct 
{
    TGroupInfo groupinfo_;				//��������Ϣ
    list_head_t bucket_[BUCKET_SIZE];	//���̹�ϣͰ����
    int curprocnum_;				//��ǰ������
    int errprocnum_;				//����������
    int last_check_group_time_;          //�ϴμ��������н��̸�����ʱ��
}TProcGroupObj;//���������

typedef struct 
{
    TProcInfo procinfo_;		//������Ϣ
    int status_;				//����״̬
    int notifytime_;			//�ϴθ澯ʱ��
    list_head_t list_;
}TProcObj;//���̶���

/////////////////////////////////�������˽ӿ�///////////////////////////////////////	
typedef void (*monsrv_cb)(const TGroupInfo* groupinfo /*�����¼��Ľ����������������*/,
                                        const TProcInfo* procinfo   /*�����¼��Ľ��̶���*/, 
                                        int event /*�¼�*/, 
                                        void* arg /*�Զ������*/);
typedef struct 
{
    TProcGroupObj* group;  //�����飬���ָ�����������ָ��
    TProcObj**	proc;	   //�����Ľ��̼��ϣ����ָ��������̶����ָ�룬��NULL��Ϊ����
}TProcQueryObj;

class CTProcMonSrv
{
public:
    CTProcMonSrv();
    virtual ~CTProcMonSrv();
    //����ͨѶ����
    //commu:  ͨѶ����ָ��
    void set_commu(CCommu* commu);

    //��ʼ�������ڴ漰�ź���
    //shmkey:�����ڴ��KEY
    //shmsize:�����ڴ�Ĵ�С
    //semkey:�ź�����KEY
    void init_shm(int shmkey,int shmsize,int semkey);

    //���ù��������ļ�·��
    //comm_cfg_file:�����ļ�·��
    void SetCommFilePath(const char * comm_cfg_file );

    void SetGroupNum(unsigned int val){group_num_ = val;}

    void SetCheckGroupInterval(unsigned int val){check_group_interval_ = val;}

    //notify:  �¼�֪ͨ�ص�����
    void set_notify(monsrv_cb notify, void* arg);
    //�����������Ϣ
    //groupinfo:	��������Ϣ
    //����ֵ: 0�ɹ�������ʧ��
    int add_group(const TGroupInfo* groupinfo);
    //�޸Ľ�������Ϣ
    //groupid:		������ID
    //groupinfo:	��������Ϣ
    //����ֵ: 0�ɹ�������ʧ��
    int mod_group(int groupid, const TGroupInfo* groupinfo);
    //���з������˽������ݺͼ�������������ͽ�����
    void run();
    //ȡͳ����Ϣ
    //buf:	ͳ����Ϣ������
    //buf_len:  ͳ����Ϣ����
    void stat(char* buf, int* buf_len);	
    //�����б���صĽ��̷��ź�
    void killall(int signo);
    //��ѯ����������н�����Ϣ
    void query(TProcQueryObj*& result, int& num);  
protected:

    CLoadTable loadwriter_;
    string common_config_file_;
    unsigned int group_num_;//����������
    unsigned int check_group_interval_;//������ڽ�������ʱ����

    CCommu* commu_;	
    TProcGroupObj proc_groups_[MAX_PROC_GROUP_NUM];
    int cur_group_;
    TProcMonMsg msg_[2]; //0-�գ�1-��
    monsrv_cb notify_;   //ʹ���߻ص�����
    void* notify_arg_;   //�ص������ĸ��Ӳ���ָ��

    bool do_recv(long msgtype);
    bool do_check();
    int add_proc(int groupid, const TProcInfo* procinfo);
    TProcObj* find_proc(int groupid, int procid);
    void del_proc(int groupid, int procid);	
    //���·������Ը��ݾ�����Ҫ���أ�������û���Ҫ���ø����ͬ������
    virtual void check_group(TGroupInfo* group, int curprocnum);
    virtual bool check_proc(TGroupInfo* group, TProcInfo* proc);
    virtual bool do_event(int event, void* arg1, void* arg2);
    //virtual void do_fork(const char* basepath, const char* exefile, const char* etcfile, int num);
    //virtual void do_fork(const char * basepath,const char * exefile,const char * etcfile,const char * serv_flag,int num);
    virtual void do_fork(const char * basepath,const char * exefile,const char * etcfile,
               int groupid,int max_procnum,const char * serv_flag,int num);
    virtual void do_kill(int procid, int signo = SIGKILL);
    virtual void do_order(int groupid, int procid, int eventno, int cmd, int arg1 = 0, int arg2 = 0);
    bool check_groupbusy(int groupid);

};

//////////////////////////////////�ͻ��˽ӿ�///////////////////////////////////////////	
typedef void (*moncli_cb)(int event /*�¼�*/, 
						  void* arg /*�Զ������*/);
class CTProcMonCli
{
public:
    CTProcMonCli();
    virtual ~CTProcMonCli();
    //����ͨѶ����
    //commu:  ͨѶ����ָ��
    void set_commu(CCommu* commu);
    //����ͨѶ����
    //notify:  �¼�֪ͨ�ص�����
    void set_notify(moncli_cb notify, void* arg);
    //�������ݵ��������ˣ�����������
    void run();
    TProcMonMsg msg_[2]; //0--����1--��
protected:
    CCommu* commu_;	
    moncli_cb notify_;
    void*	notify_arg_;
};

#define CLI_SEND_INFO(cli)  ((TProcInfo*)(cli)->msg_[0].msgcontent_)  //TProcInfo��������Ϣ���ͻ���ʹ��
#define CLI_RECV_INFO(cli)  ((TProcEvent*)(cli)->msg_[1].msgcontent_) //TProcEvent,  ������Ϣ���ͻ���ʹ��

}
}

#endif

