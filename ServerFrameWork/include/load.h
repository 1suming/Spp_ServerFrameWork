/********************************************
//�ļ���:load.h
//����:���ض���
//����:�Ӻ���
//����ʱ��:2009.06.11
//�޸ļ�¼:

*********************************************/
#ifndef _COMM_COMMU_LOAD_H_
#define _COMM_COMMU_LOAD_H_

#include <asm/atomic.h>
#include <time.h>
#include <sys/time.h>

namespace comm
{
namespace commu 
{

#define DEFAULT_MAX_LOAD	(1<<16)		//Ĭ��ÿ�����loadֵ
#define DEFAULT_INTERVAL	5			//�������ڣ��룩
#define LOADGRID_NUM     	2			//ʹ�õļ�����Ԫ��������ֹ���


class CLoad
{
public:
    CLoad();
    CLoad(int maxload);
    ~CLoad();

    //���������(ÿ��)	
    int maxload(int n);	
    //���ӻ��߼��ٸ���
    void grow_load(int n);
    //����Ƿ����
    bool check_load();
    //ȡ��ǰ����(ÿ��)
    int peek_load(struct timeval* nowtime = NULL);
protected:
    void reset();
    void trans();
    	
    int maxload_;
    int curtime_;
    bool direct_;
    atomic_t* curloadgrid_;
    atomic_t loadgrid_[LOADGRID_NUM];
    atomic_t loadgrid2_[LOADGRID_NUM];
};

}
}
#endif 

