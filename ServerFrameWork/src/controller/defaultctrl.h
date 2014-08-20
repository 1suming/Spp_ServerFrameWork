#ifndef _SPP_CTRL_DEFAULT_H_
#define _SPP_CTRL_DEFAULT_H_
#include "serverbase.h"
#include "MarkupSTL.h"
using namespace spp::procmon;
using namespace spp::base;
using namespace comm::base;

namespace spp
{
namespace ctrl
{
class CDefaultCtrl : public CServerBase
{
public:
    CDefaultCtrl();
    ~CDefaultCtrl();
    
    //ʵ�����к���
    void realrun(int argc, char* argv[]);
    //������������
    int servertype() {return SERVER_TYPE_CTRL;}

    //��ʼ������
    int initconf(bool reload = false);
    
    //���srv�ص�����	
    static void notify(const TGroupInfo* groupinfo, const TProcInfo* procinfo, int event, void* arg);    
protected:
    int WritePidFile(const char * pszName);
    int WriteStatResult(const char * pszName,const char * buffer,unsigned int buf_len);

    int do_fork_ex(const char * basepath,const char * proc_name,const char * etcfile,const char * flag);

    virtual void do_timer_callback(const struct timeval & nowtime){};

    int InitProcMon(CMarkupSTL &conf,bool reload = false);

    //���srv	
    CTProcMonSrv monsrv_;

        
    //ͳ�ƽ�����·��
    string stat_output_file_;

    //ͳ�ƽ�������ʱ����
    unsigned int stat_output_interval_;

    //ͳ��ֵ����ʱ��[00:00:00 ~ 23:59:59]
    //string stat_reset_time_;
    int stat_reset_hour_;
    int stat_reset_minute_;
    int stat_reset_second_;

    //ͳ��ֵ���õ�����
    unsigned int stat_reset_days_;

};

}
}
#endif

