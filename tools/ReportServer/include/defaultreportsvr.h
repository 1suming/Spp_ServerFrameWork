/***************************************
*�ļ���:defaultreportsvr.h
*����:�����ϱ�����Ӧ����
*������:�Ӻ���
*����ʱ��:2009.06.10
*�޸ļ�¼
****************************************/
#ifndef _SPP_DEF_RPT_SVR_
#define _SPP_DEF_RPT_SVR_

#include "serverbase.h"
#include "loadtable.h"
#include "base_socket.h"

using namespace comm::load;
using namespace comm::basesock;

namespace spp
{
namespace reportsvr
{

//���ò���
class CReportServerConf
{
public:
	CReportServerConf();
	~CReportServerConf();


	std::string serverip_;//LoadBalance Server Ip Address
	unsigned short serverport_;//LoadBalance Server Port
	
	unsigned int ipaddress_;//IP��ַ
	unsigned short tcpport_;//TCP PORT
	unsigned short udpport_;//UDP PORT
	unsigned short server_type_;//SERVER TYPE
	unsigned char net_type_;//NET TYPE
	unsigned int maxconn_num_;//���������	
};

class CDefaultReportServer:public CServerBase
{
public:
	CDefaultReportServer();
	virtual ~CDefaultReportServer();

	virtual void realrun(int argc, char * argv [ ]);

	//��ʼ������
	int initconf(bool reload = false);


protected:

        int InitLoadTable(const char * comm_config);

	CLoadTable loadreader_;

	//TcpClient���
	CTcpClient  * TcpClient_;

	CReportServerConf conf_;

};


}
}

#endif

