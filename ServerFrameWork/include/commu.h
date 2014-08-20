/********************************************
//�ļ���:commu.h
//����:ͨѶ�����ӿ�
//����:�Ӻ���
//����ʱ��:2009.06.11
//�޸ļ�¼:

*********************************************/
#ifndef _COMM_COMMUNICATION_COMMU_H_
#define _COMM_COMMUNICATION_COMMU_H_
#include <unistd.h>
#include <string.h>

#define COMMU_ERR_OVERLOAD_PKG 	-100
#define COMMU_ERR_OVERLOAD_CONN -200

namespace comm 
{
namespace commu 
{

//�ص���������
typedef enum 
{
    CB_CONNECT = 0,
    CB_CONNECTED ,
    CB_DISCONNECT,
    CB_RECVDATA,
    CB_RECVERROR,
    CB_RECVDONE,
    CB_SENDDATA,
    CB_SENDERROR,
    CB_SENDDONE,
    CB_HANGUP, 
    CB_OVERLOAD,
    CB_TIMEOUT,
}cb_type;

//��������
typedef enum 
{
    CT_DISCONNECT = 0,  //�Ͽ����ӣ������أ�
    CT_CLOSE,		 //������Դ�������أ�
    CT_STAT,			//ͳ����Ϣ�������أ�
    CT_LOAD,			//������Ϣ (������)
    CT_GET_CONN_EXT_INFO, //������չ��Ϣ(������)
}ctrl_type;

//�ص���������
//flow: ���ݰ�Ψһ��ʾ
//arg1: ͨ�ò���ָ��1��һ��ָ������blob
//arg2: ͨ�ò���ָ��2���û�ע��ص�����������Զ������ָ��
typedef int (*cb_func) (unsigned flow, void* arg1, void* arg2);

//����blob
typedef struct tagBlobType
{
    tagBlobType()
    {
        memset(this,0,sizeof(tagBlobType));
    }
    int len;//���ݳ���
    char* data;	//���ݻ�����
    void* owner;	//���ָ��
    unsigned char ext_type;//��չ��������
    unsigned short ext_len;//��չ���ݳ���
    void* extdata;  //��չ����
}blob_type;

const int MAX_BLOB_DATA_LEN = (1<<24);//16M
const unsigned short MAX_BLOB_EXTDATA_LEN = 0xffff;//64k

//��չ�������Ͷ���
const unsigned char EXT_TYPE_NONE = 0x00;//δʹ��
const unsigned char EXT_TYPE_CONNECTED= 0x01;//���������¼�֪ͨ
const unsigned char EXT_TYPE_OVERLOAD = 0x02;//�������ӹ���֪ͨ
const unsigned char EXT_TYPE_TIMEOUT = 0x03;//�������ӳ�ʱ֪ͨ
const unsigned char EXT_TYPE_DISCONNECT = 0x04;//����Ͽ�֪ͨ


const unsigned char EXT_TYPE_CONNEXTINFO = 0x10;//������չ��Ϣ
const unsigned char EXT_TYPE_CONNNODEINFO = 0x11;//���ӽڵ���Ϣ

const unsigned char EXT_TYPE_USER_BASE = 0x80;//�û��Զ�������


//������չ��Ϣ(��Ϊblob_type��extdata����)	
typedef struct
{
    int fd_;					//fd 
    int type_;					//fd type (SOCK_STREAM\SOCK_DGRAM\...)
    unsigned localip_;			//local ip
    unsigned short localport_;	//local port
    unsigned remoteip_;		//remote ip
    unsigned short remoteport_;	//remote port

}TConnExtInfo;


//ͨѶ�����ӿ�
class CTCommu
{
public:
    CTCommu()
    { 
        memset(func_list_, 0, sizeof(cb_func) * (CB_TIMEOUT + 1));
        memset(func_args_, 0, sizeof(void*) * (CB_TIMEOUT + 1));
    }
    virtual ~CTCommu(){}

    //��ʼ��
    //[in] config�������ļ����������ò����ڴ�ָ��
    //����ֵ:
    //	0:�ɹ���-1:ʧ��
    virtual int init(const void* config) = 0;

    //��ʼ��
    //[in] config�������ļ����������ò����ڴ�ָ��
    //	0:�ɹ���-1:ʧ��
    virtual int InitExt(const void * config){return -1;}

    //��ѯ���շ�����
    //block: true��ʾʹ������ģʽ�����������ģʽ
    virtual int poll(bool block = false) = 0;

    //���������ύ
    //flow: ���ݰ�Ψһ��ʾ
    //arg1: ͨ�ò���ָ��1�� һ��ָ������blob
    //arg2: ͨ�ò���ָ��2������
    virtual int sendto(unsigned flow, void* arg1, void* arg2) = 0;

    //���ƽӿ�
    //flow: ���ݰ�Ψһ��ʾ
    //type: ��������
    //arg1: ͨ�ò���ָ��1����������о���ĺ���
    //arg2: ͨ�ò���ָ��2����������о���ĺ���
    virtual int ctrl(unsigned flow, ctrl_type type, void* arg1, void* arg2) = 0;

    //ע��ص�
    //type: �ص��������� 
    //func: �ص�����
    //args: �û��Զ������ָ��, ��Ϊ�ص������ĵ�2��ͨ�ò�������
    virtual int reg_cb(cb_type type, cb_func func, void* args = NULL)
    {
        if(type <= CB_TIMEOUT)
        {
            func_list_[type] = func;
            func_args_[type] = args;
            return 0;
        }
        else
        {
            return -1;
        }
    }
protected:
    cb_func func_list_[CB_TIMEOUT + 1];
    void* func_args_[CB_TIMEOUT + 1];

    //�ͷ���Դ
    virtual void fini() = 0;
};

}
}
#endif 

