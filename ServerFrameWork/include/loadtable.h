/****************************************
*�ļ���:loadtable.h
*����:���ر����
*������:�Ӻ���
*�޸ļ�¼:
*****************************************/
#ifndef _COMM_LOAD_TAB_H_
#define _COMM_LOAD_TAB_H_

#include <string>
#include <map>
#include "lock.h"

using namespace comm::lock;

namespace comm
{
namespace load
{

const unsigned char MAX_KEY_LEN = 32;//KEY����󳤶�

typedef struct tagItemInfo
{
    unsigned short len_;//item len
    unsigned pos_;//item pos
}ItemInfo;
typedef  std::map<std::string,ItemInfo> MapItems;
typedef MapItems::iterator Iter;


//�����ڴ��еĸ�����Ϣ��
class CLoadTable
{
public:
    CLoadTable();
    ~CLoadTable();

        /*
    *����:��ʼ�������ڴ��
    *����˵��:
    *[in] shmkey:�����ڴ��key
    *[in] shmsize:�����ڴ�Ĵ�С
    *[in] semkey:�ź�����key
    *����ֵ:
    *  �ɹ�:0,ʧ��:��0
    */
    int init(int shmkey,int shmsize,int semkey = -1);

        /*
    *����:��ʼ��������
    *����˵��:
    *key:�������KEY
    *len:��������Ҫʹ�õ���󴢴�ռ䣬��λ�ֽ�
    *����ֵ:
    *�ɹ�:>0,��ʾ�ø�����ʵ�ʵĴ洢�ռ�,ʧ��:<=0
    */
    int Init_LoadItem(const char * key,unsigned short len);

    /****************************************
    //�ӹ����ڴ��ж�ȡ���ݵ�buf��
    //buf:�������ݵ�BUF
    //buf_len:buf�������ĳ���
    //key:
    //timeout:����ʱ�䣬��λ��
    //����ֵ:>=0:ʵ�ʶ�ȡ���ݵĳ��ȣ�<0��ȡʧ��
    *******************************************/
    int read(void * buf,unsigned short buf_len,const char * key,int timeout = 60);

        /*********************************************
    *д���ݵ������ڴ���
    *key:�������KEY,�����ȳ�ʼ����ʹ��
    *buf:��д���buf
    *len:buf�ĳ���
    *����ֵ:
    *>=0:ʵ��д��ĳ��ȣ�<0д��ʧ��	
    *********************************************/
    int write(const char * key,void * buf,unsigned short len);

protected:

    //���ҿ�block
    //key:
    //len:���ݿ鳤��
    //����:
    //�ɹ�:������ݿ��ƫ����
    unsigned findblock(const char * key,unsigned short &len);
    	
    //shmkey:
    //shmsize:
    //return:>=0�ɹ���<0:ʧ��
    int getmemory(int shmkey,int shmsize);
    void fini();

    int shmkey_;//IPC KEY
    int shmsize_;//�����ڴ��С
    int shmid_;
    void * shmmem_;//attached share memory
    unsigned * tail_;//���ݿ�β��
    char * block_;//���ݿ�
    unsigned blocksize_;//���ݿ��С

    MapItems items_;

    CLock *lock_;

};

}
}


#endif

