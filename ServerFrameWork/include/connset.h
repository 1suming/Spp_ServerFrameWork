/********************************************
//�ļ���:connset.h
//����:���ӳض���
//����:�Ӻ���
//����ʱ��:2009.06.11
//�޸ļ�¼:

*********************************************/
#ifndef _COMM_SOCKCOMMU_CONNSET_H_
#define _COMM_SOCKCOMMU_CONNSET_H_
#include <time.h>
#include <list>
#include "cache.h"
#include "mempool.h"

#define  E_NEED_CLOSE  10000
#define  E_NOT_FINDFD  20000
#define  C_NEED_SEND   30000

namespace comm 
{
namespace sockcommu  
{
class CConnSet
{
public:
    CConnSet(CMemPool& mp, int maxconn); 
    ~CConnSet();

    int addconn(unsigned& flow, int fd, int type);
    int fd(unsigned flow);

    int recv(unsigned flow);
    int send(unsigned flow, const char* data, size_t data_len);
    int sendfromcache(unsigned flow);

    int closeconn(unsigned flow);
    void check_expire(time_t access_deadline, std::list<unsigned>& timeout_flow);
    int canclose(unsigned flow);

    inline ConnCache* getcc(unsigned flow){ return ccs_[flow % maxconn_];}

    inline int getmaxconn(){return maxconn_;}
    inline int getcurconn(){return curconn_;}
private:
    ConnCache** ccs_;
    int maxconn_;//���������
    int curconn_;//��ǰ��������
};

}
}
#endif

