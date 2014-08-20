/*************************************
*�ļ���:serverbase.h
*����:�������
*����:�Ӻ���
*����ʱ��:2009.06.10
***************************************/
#ifndef _SPP_SERVER_BASE_H_
#define _SPP_SERVER_BASE_H_

#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <string.h>
#include "log.h"

using namespace std;
using namespace comm::log;

namespace spp
{
namespace base
{

const unsigned char  RUN_FLAG_QUIT	= 0x01;
const unsigned char  RUN_FLAG_RELOAD	=0x02;
const unsigned char  RUN_FLAG_RELOAD_MODULE = 0x04;


class CServerBase
{
public:
	CServerBase();
	virtual ~CServerBase();

	virtual void ShowVerInfo();

	virtual void run(int argc, char* argv[]);

	//bg_run:�Ƿ��ں�̨����
	virtual void startup(bool bg_run = true);
	
	//ʵ�ʵ����к���
	virtual void realrun(int argc, char* argv[]){}

	//��־
	CLog m_log_internal;


protected:

	//�ڲ�ʹ�õ����ݽṹ
	struct TInternal
	{
		//main����ֵ
		int argc_;
		char** argv_;
	};
	struct TInternal* ix_;	

public:

	///////////////////////////////////////////////////////////////////////
	static bool reload();
	static bool quit();
	static bool reloadmoduleconfig();
	static void sigusr1_handle(int signo);
	static void sigusr2_handle(int signo);
	static void sigusr3_handle(int signo);
	static unsigned char flag_;//���б��
	static char version_desc[64];//�汾����

};

}
}
#endif

