/********************************************
//�ļ���:sockcommu.h
//����:SOCKETͨѶ��
//����:�Ӻ���
//����ʱ��:2009.06.11
//�޸ļ�¼:

*********************************************/
#ifndef _COMM_SOCKCOMMU_SOCKCOMMU_H_
#define _COMM_SOCKCOMMU_SOCKCOMMU_H_
#include <asm/atomic.h>
#include <time.h>
#include "commu.h"
#include <vector>
#include <string>
#include <map>
using namespace std;

//#define LOAD_CHECK_ENABLE
#ifdef LOAD_CHECK_ENABLE
#include "load.h"
#endif



using namespace comm::commu;

namespace comm 
{
namespace sockcommu 
{

#define SOCK_TYPE_TCP		0x1
#define SOCK_TYPE_UDP		0x2
#define SOCK_TYPE_UNIX		0x4
#define SOCK_MAX_BIND			100

const unsigned char  SOCK_FLAG_UNKNOW = 0x00;
const unsigned char SOCK_FLAG_SERVER = 0x01;
const unsigned char SOCK_FLAG_CLIENT = 0x02;

typedef struct 
{
    unsigned int ip_;			//ip address
    unsigned short port_;		//ip port
}TIpport;

typedef struct 
{
    int type_;					//tcp or udp or unix socket
    union
    {
        TIpport ipport_;        //ip and port
        char path_[256];		//unix socket path
    };
}TSockBind;	

typedef struct tagTSockCommuConf
{
    tagTSockCommuConf()
    {
        maxconn_ = 0;
        maxpkg_ = 0;
        expiretime_ = 0;
        check_expire_interval_ = 0;	
    }
    TSockBind sockbind_[SOCK_MAX_BIND]; //����Ϣ
    int maxconn_;		//��������� > 0
    int maxpkg_; 		//�����������Ϊ0�򲻼��
    int expiretime_;		//��ʱʱ�䣬 > 0
    int check_expire_interval_;//��鳬ʱ��ʱ������Ĭ��60��,0�����
}TSockCommuConf;


//////////////////////////////////////
//·�ɽڵ���Ϣ
typedef struct
{
    void Show() const
    {
        printf("serv_type=%u,node_id=%u,begin_=%d,end=%d\n",serv_type_,node_id_,begin_,end_);
    }
    unsigned int serv_type_;//�����ķ�������
    unsigned int node_id_;//�ڵ�ID
    unsigned int begin_;//·�ɶε���ʼֵ
    unsigned int end_;//·�ɶεĽ���ֵ
    TSockBind bind_;//�������İ���Ϣ,ip,port
}TNodeInfo;

typedef std::map<unsigned int,TNodeInfo> MapNode;
typedef MapNode::iterator NodeIter;
//·�ɱ���Ϣ
typedef struct tagTRouteTable
{
    unsigned int serv_type_;//��������
    string version_;//��ǰ�汾��
    string route_;//·���㷨
    int route_val_;//��route_����Ĳ�����

    int GetNodeInfo(TNodeInfo & info,unsigned int node_id);	

    unsigned int GetNodeID(unsigned int node_key);	

    void Show();

    MapNode  map_node_;
}TRouteTable;

typedef std::map<unsigned int,TRouteTable> MapRouteTable;
typedef MapRouteTable::iterator RouteTableIter;

//��˷�������������Ϣ
typedef struct tagTRouteConfig
{
    tagTRouteConfig()
    {
        max_conn_ = 1000;
        maxpkg_ = 0;
        expiretime_ = 60;
        check_expire_interval_ = 60;
    }
    ~tagTRouteConfig(){}

    //��ȡ�ڵ���Ϣ
    //serv_type:��������
    //node_id:�ڵ�ID
    //����ֵ:�ɹ�,ָ��ڵ���Ϣ��ָ�룬ʧ��NULL
    int GetNodeInfo(TNodeInfo & node,unsigned int serv_type,unsigned int node_id);	

    int GetRouteTable(TRouteTable & tbl,unsigned int serv_type);

    int max_conn_;//���������
    int maxpkg_; 	//ÿ�������������Ϊ0�򲻼��
    int expiretime_;//��ʱʱ��>0
    int check_expire_interval_;//��鳬ʱʱ����

    //������Ϣ
    MapRouteTable map_route_table_;
}TRouteConfig;



//����ע��CB_RECVDATA�ص�����
class CTSockCommu : public CTCommu
{
public:
    CTSockCommu();
    ~CTSockCommu();

    /*
    * ����:��ʼ��
    * ����˵��:
    * [in] config:���ýṹ��Ϣ�ڴ�ָ��  
    * ����ֵ:
    * 0:�ɹ�,-1ʧ��
    */
    int init(const void* config);

    /*
    * ����:��ѯ������
    * ����˵��:
    * [in] block:�Ƿ�������Ŀǰ��ʱû���õ�
    * ����ֵ:
    * 0:�ɹ�,<0ʧ��
    */
    int poll(bool block = false);

    /*
    * ����:������
    * ����˵��:
    * [in] flow: ����Ψһ��ʶ
    * [in] arg1:���ݿ�blob����
    * [in] arg2:Ŀǰ����
    * ����ֵ:
    * 0:�ɹ�,<0ʧ��
    */
    int sendto(unsigned flow, void* arg1, void* arg2);

    /*
    * ����:����
    * ����˵��:
    * [in] flow:����Ψһ��ʶ
    * ����ֵ:
    * 0:�ɹ�,-1ʧ��
    */
    int ctrl(unsigned flow, ctrl_type type, void* arg1, void* arg2);


    /*
    *����:��ʼ���������
    *����˵��:
    *[in] config:���ýṹ���ڴ��ַ
    *����ֵ:
    * 0�ɹ�����0ʧ��
    */
    int InitExt(const void * config);

    /*
    *����:���ӷ�����
    *����˵��:
    *[in] pNode:���ӵĽڵ���Ϣ
    *����:=0 �ɹ�������ʧ��
    */
    int connect(const TNodeInfo * pNode);	


protected:

    TSockBind sockbind_[SOCK_MAX_BIND];

    int maxconn_;
    int expiretime_;
    int check_expire_interval_;

    int lastchecktime_;
    int maxbindfd_;
    unsigned flow_;
    blob_type buff_blob_;
    TConnExtInfo extinfo_;
#ifdef LOAD_CHECK_ENABLE			
    CLoad myload_;
#endif
    struct TInternal;
    struct TInternal* ix_;	
    		 	
    void fini();
    void check_expire();
    int create_sock(const TSockBind* s);
    int create_sock(int sock_type);

    void handle_accept(int fd);
    void handle_accept_udp(int fd);
};

}
}
#endif

