/********************************************
//�ļ���:shmcommu.h
//����:�����ڴ�ͨѶ��
//����:�Ӻ���
//����ʱ��:2009.06.11
//�޸ļ�¼:

*********************************************/
#ifndef _COMM_COMMUNICATION_SHMCOMMU_H_
#define _COMM_COMMUNICATION_SHMCOMMU_H_

#include <map>

#ifndef SLK_LINUX
#include <pthread.h>
#else					 //slackware��linuxthread�߳̿ⲻ֧���߳�������̹�������Ҫ�����ź���
#include <sys/sem.h>
union shmsemun
{
    int val;					//<= value for SETVAL
    struct semid_ds *buf;		//<= buffer for IPC_STAT & IPC_SET
    unsigned short int *array;	//<= array for GETALL & SETALL
    struct seminfo *__buf;		//<= buffer for IPC_INFO
};
#endif


#include "commu.h"


//#define LOAD_CHECK_ENABLE
#ifdef LOAD_CHECK_ENABLE
#include "load.h"
#endif


using namespace comm::commu;

namespace comm 
{
namespace commu 
{
namespace shmcommu 
{
#define LOCK_TYPE_NONE			0x0		//����
#define LOCK_TYPE_PRODUCER		0x1		//д��
#define LOCK_TYPE_COMSUMER		0x2		//����

#define COMMU_ERR_SHMGET	-101		//��ȡ�����ڴ����
#define COMMU_ERR_SHMNEW	-102		//���������ڴ����
#define COMMU_ERR_SHMMAP	-103		//�����ڴ�ӳ�����
#define COMMU_ERR_SEMGET	-104		//��ȡ�ź�������
#define COMMU_ERR_SEMLOCK	-105		//�ź�����ʧ��
#define COMMU_ERR_SEMUNLOCK	-106		//�ź�������ʧ��
#define COMMU_ERR_FILEOPEN 	-111 	//���ļ�����
#define COMMU_ERR_FILEMAP	-112		//�ļ�ӳ�����
#define COMMU_ERR_MQFULL	-121		//�ܵ���
#define COMMU_ERR_MQEMPTY  	-122		//�ܵ���
#define COMMU_ERR_OTFBUFF	-123		//���������

//�����ڴ�ܵ�ͳ��
typedef struct tagMQStat
{       
    unsigned usedlen_;
    unsigned freelen_;
    unsigned totallen_;
    unsigned shmkey_;
    unsigned shmid_;
    unsigned shmsize_;
}TMQStat;


//�����ڴ�ܵ�
class CShmMQ
{
public: 
    CShmMQ();
    ~CShmMQ();

    /******************************************
    //��ʼ��
    //[in] shmkey:�����ڴ�KEY
    //[in] shmsize:�����ڴ��С
    //����ֵ:
    //�ɹ�:0,ʧ��:-1
    *****************************************/
    int init(int shmkey, int shmsize);

    /********************************************
    //����:�����ݷ��빲���ڴ�ܵ�
    //����˵��:
    //[in] arg1:���ݿ�blob����
    //[in] flow:ͨѶΨһ��ʶ
    //[in] msgtype:��Ϣ����
    //����ֵ:
    //0�ɹ���<0:ʧ��
    ********************************************/
    int enqueue(const void * arg1, unsigned flow,unsigned char msgtype);

    /***************************************************
    //����:�ӹ����ڴ�ܵ��ж�ȡ����
    //����˵��:
    //[out] arg1:���ݿ�blob����
    //[out] flow:ͨѶΨһ��ʶ
    //[out] type:���ݰ�����
    //����ֵ:
    //0�ɹ���<0:ʧ��
    ***************************************************/
    int dequeue(void* arg1, unsigned int & flow,unsigned char &msgtype);
    int dequeue_i(void* arg1, unsigned int & flow,unsigned char &msgtype,struct timeval & time_stamp);

    /****************************************************
    //����:�����ڴ�ĵ�ַ
    //����˵��:��
    //����ֵ:�����ڴ����ʼ��ַ
    ****************************************************/
    inline void* memory() {return shmmem_;}

    /***************************************************
    //����:��ȡ�����ڴ�ܵ���ͳ����Ϣ
    //����˵��:
    //[out] mq_stat:���ع����ڴ�ܵ���ͳ����Ϣ
    //����ֵ:��
    ***************************************************/
    inline void getstat(TMQStat& mq_stat)
    {
        unsigned int head = *head_; 
        unsigned int tail = *tail_; 

        mq_stat.usedlen_ = (tail >= head) ? tail - head : tail + blocksize_ - head;
        mq_stat.freelen_ = head > tail ? head - tail: head + blocksize_ - tail;
        mq_stat.totallen_ = blocksize_;
        mq_stat.shmkey_ = shmkey_; 
        mq_stat.shmid_ = shmid_;
        mq_stat.shmsize_ = shmsize_;
    }
protected:
    int shmkey_;//�����ڴ��key
    int shmsize_;//�����ڴ�Ĵ�С
    int shmid_;//�����ڴ��ID
    void* shmmem_;//�����ڴ����ʼ��ַ	

    unsigned int * head_;//the head of data section
    unsigned int * tail_;//the tail of data section
    char* block_; //data section base address
    unsigned int blocksize_;//data section length

    /*********************************************
    //����:ȡ�ù����ڴ�
    //����˵��:
    //[in] shmkey:�����ڴ��key
    //[in] shmsize:�����ڴ�Ĵ�С
    //����ֵ:
    //0:��ʾ�Ѵ��ڵĹ����ڴ棬1:��ʾ�´����Ĺ����ڴ�
    // <0��ʾʧ��
    *********************************************/
    int getmemory(int shmkey, int shmsize);

     /*****************************************
     //����:������Դ
     //����˵��:��
     //����ֵ:��
     *****************************************/	
    void fini();

    public:
        int msg_timeout_;//��ʱʱ��        
};

//�����ߣ���������
class CShmProducer
{
public:
    CShmProducer();
    virtual ~CShmProducer();

    /******************************************
    //��ʼ��
    //[in] shmkey:�����ڴ�KEY
    //[in] shmsize:�����ڴ��С
    //����ֵ:
    //�ɹ�:0,ʧ��:-1
    *****************************************/
    virtual int init(int shmkey, int shmsize);

    /********************************************
    //����:�����ݷ��빲���ڴ�ܵ�
    //����˵��:
    //[in] arg1:���ݿ����
    //[in] flow:ͨѶΨһ��ʶ
    //[in] type:���ݰ�����
    //����ֵ:
    //0�ɹ���<0:ʧ��
    ********************************************/
    virtual int produce(const void* arg1, unsigned flow,unsigned char type);

    /***************************************************
    //����:��ȡ�����ڴ�ܵ���ͳ����Ϣ
    //����˵��:
    //[out] mq_stat:���ع����ڴ�ܵ���ͳ����Ϣ
    //����ֵ:��
    ***************************************************/
    virtual void getstat(TMQStat& mq_stat)
    {
    	mq_->getstat(mq_stat);
    }

     /*****************************************
         //����:������Դ
         //����˵��:��
         //����ֵ:��
         *****************************************/	
    virtual void fini();
public:
    CShmMQ* mq_;//�����ڴ�ܵ�����ָ��
};


//�����ߣ�������
class CShmProducerL : public CShmProducer
{
public:
    CShmProducerL();
    ~CShmProducerL();

    /******************************************
    //��ʼ��
    //[in] shmkey:�����ڴ�KEY
    //[in] shmsize:�����ڴ��С
    //����ֵ:
    //�ɹ�:0,ʧ��:-1
    *****************************************/
    int init(int shmkey, int shmsize);

    /********************************************
    //����:�����ݷ��빲���ڴ�ܵ�
    //����˵��:
    //[in] arg1:���ݿ�blob����
    //[in] flow:ͨѶΨһ��ʶ
    //[in] type:���ݰ�����
    //����ֵ:
    //0�ɹ���<0:ʧ��
    ********************************************/
    int produce(const void* arg1, unsigned flow,unsigned char type);
protected:
#ifndef SLK_LINUX			
    pthread_mutex_t* mutex_;
    int mfd_;
#else
    int semid_;//�ź���ID

    /*****************************************
    //����:�ź�����ʼ��
    //����˵��:
    //[in] key:�ź�����key
    //����ֵ
    //�ɹ�0,ʧ��-1
    *****************************************/
    int sem_init(int key);

    int sem_lock();

    int sem_unlock();
#endif	

    /*****************************************
    //����:������Դ
    //����˵��:��
    //����ֵ:��
    *****************************************/	
    void fini();
};

//�����ߣ���������	
class CShmComsumer
{
public:
    CShmComsumer();
    virtual ~CShmComsumer();

    /******************************************
    //��ʼ��
    //[in] shmkey:�����ڴ�KEY
    //[in] shmsize:�����ڴ��С
    //����ֵ:
    //�ɹ�:0,ʧ��:-1
    *****************************************/
    virtual int init(int shmkey, int shmsize);

    /***************************************************
    //����:�ӹ����ڴ�ܵ��ж�ȡ����
    //����˵��:
    //[out] arg1:���ݿ����
    //[out] flow:ͨѶΨһ��ʶ
    //[out] type:���ݰ�����
    //[in] block:������־��true:��������,flase:����������
    //����ֵ:
    //0�ɹ���<0:ʧ��
    ***************************************************/
    virtual int comsume(void* arg1, unsigned int& flow, unsigned char &msgtype,bool block = false);

    /***************************************************
    //����:��ȡ�����ڴ�ܵ���ͳ����Ϣ
    //����˵��:
    //[out] mq_stat:���ع����ڴ�ܵ���ͳ����Ϣ
    //����ֵ:��
    ***************************************************/
    virtual void getstat(TMQStat& mq_stat)
    {
        mq_->getstat(mq_stat);
    }

    /***************************************************
    //����:�ӹ����ڴ�ܵ��ж�ȡ����
    //����˵��:
    //[out] arg1:���ݿ�blob����
    //[out] flow:ͨѶΨһ��ʶ
    //[out] msgtype:���ݰ�����
    //����ֵ:
    //0�ɹ���<0:ʧ��
    ***************************************************/
    virtual inline int comsume_i(void* arg1, unsigned& flow, unsigned char & type);

    /*****************************************
    //����:������Դ
    //����˵��:��
    //����ֵ:��
    *****************************************/	
    virtual void fini();
public:
    CShmMQ* mq_;
	
};

//�����ߣ�������
class CShmComsumerL : public CShmComsumer
{
public:
    CShmComsumerL();
    ~CShmComsumerL();
    int init(int shmkey, int shmsize);
protected:
#ifndef SLK_LINUX			
    pthread_mutex_t* mutex_;
    int mfd_;
#else
    int semid_;
    int sem_init(int key);
    int sem_lock();
    int sem_unlock();
#endif

    /***************************************************
    //����:�ӹ����ڴ�ܵ��ж�ȡ����
    //����˵��:
    //[out] arg1:���ݿ�blob����
    //[out] flow:ͨѶΨһ��ʶ
    //[out] msgtype:��Ϣ����
    //����ֵ:
    //0�ɹ���<0:ʧ��
    ***************************************************/
    inline int comsume_i(void* arg1, unsigned& flow, unsigned char & msgtype);


    /*****************************************
    //����:������Դ
    //����˵��:��
    //����ֵ:��
    *****************************************/	
    void fini();
};

//�����ڴ�ͨѶ���
//һ�������ݹܵ�����Ϊ������producer
//һ�������ݹܵ�����Ϊ������comsumer
//�ص��������ͣ�typedef int (*cb_func) (unsigned flow, void* arg1, void* arg2);(�ο�commu.h)
//flow: ���ݰ�Ψһ��ʾ��arg1: ����blob��arg2: �û��Զ������
//����ע��CB_RECVDATA�ص�����
typedef struct 
{
    int shmkey_producer_;	           //�����ݹ����ڴ�key
    int shmsize_producer_;          //�����ݹ����ڴ�size
    int shmkey_comsumer_;        //�����ݹ����ڴ�key
    int shmsize_comsumer_;       //�����ݹ����ڴ�size
    int locktype_;		          //�����ͣ�������д��������
    int expiretime_;                       //��ʱʱ�䣬���Ϊ0��ʾ����鳬ʱ
    int maxpkg_;                            //ÿ��������, ���Ϊ0��ʾ�����
    int msg_timeout_;                      //��Ϣ�ڶ����ʱ��,expiretime_����!
}TShmCommuConf;

class CTShmCommu : public CTCommu
{
public:
    CTShmCommu();
    ~CTShmCommu();

    /***************************************
    //����:��ʼ�������ڴ�ͨѶ���
    //����˵��
    //[in] config:������Ϣ���ڴ��ַ
    //����ֵ:
    //0:�ɹ�,-1:ʧ��
    ***************************************/
    int init(const void* config);

    /**************************************
    //����:������
    //����˵��:
    //[in] block:������ǣ�true:�����գ�false:��������
    //����ֵ:0,�ɹ���<0:ʧ��
    ***************************************/
    int poll(bool block = false);

    /***************************************************
    //����:������
    //����˵��
    //[in] flow:���ݰ�Ψһ��ʶ
    //[in] arg1:����blobָ��
    //[in] arg2:��ʱ����
    ****************************************************/
    int sendto(unsigned flow, void* arg1, void* arg2 /*��ʱ����*/);

    int ctrl(unsigned flow, ctrl_type type, void* arg1, void* arg2);

    int get_msg_count();
protected:
    CShmProducer* producer_;//������
    CShmComsumer* comsumer_;//������
    unsigned int buff_size_;//���ݿ�buff��С
    unsigned short buff_ext_size_;//��չ���ݿ�buff��С
    blob_type buff_blob_;//���ݿ����
    int locktype_;//������
    std::map<unsigned, unsigned> expiremap_;
    int expiretime_;
    int lastchecktime_;
    //int msg_timeout_;    

#ifdef LOAD_CHECK_ENABLE
    CLoad myload_;
#endif

    /************************
    //����:������Դ
    //����˵��:��
    //����ֵ:��
    ****************************/
    void fini();

    /************************************
    //����:��鳬ʱ
    //����˵��:��
    //����ֵ:��
    *************************************/
    void check_expire();
};

}
}
}

#endif

