/***************************************
*�ļ���:default_update_client.h
*����:���ø��³���Ӧ����
*������:�Ӻ���
*����ʱ��:2009.06.10
*�޸ļ�¼
****************************************/
#ifndef _SPP_DEF_UPDATE_SVR_
#define _SPP_DEF_UPDATE_SVR_

#include <map>

#include "serverbase.h"
#include "base_socket.h"

using namespace std;
using namespace comm::basesock;

namespace spp
{
namespace base
{

typedef struct 
{
    int len;
    char * data;
    void * owner;
}blob_type;


class CDefaultUpdateClient:public CServerBase
{
public:
	CDefaultUpdateClient();
	virtual ~CDefaultUpdateClient();

	virtual void realrun(int argc, char * argv [ ]);

	//��ʼ������
	int initconf(bool reload = false);

	int SendBlob(blob_type * blob);

protected:
        int OnProcess(blob_type * blob);
        int DoCheckVersion(blob_type * blob);
        int OnUpdateConfigNotify(blob_type * blob);
        int OnCheckVersionRsp(blob_type * blob);
        int OnGetConfigInfoRsp(blob_type * blob);

        int GetRouteTableVersion(unsigned int & version, const char * file);


protected:	

        std::string m_strServerip;// Server Ip Address
	unsigned short m_nServerport;//Server Port
        unsigned int m_nCheckVerInterval;//����°汾��ʱ���� 
        std::string m_strRouteTableBasePath;//·�ɱ�洢��Ŀ¼
        std::string m_strRouteTablePrefix;//·�ɱ��ļ���ǰ׺

        //first:serv_type,second:version
        typedef  std::map<unsigned int,unsigned int> MapConfigVer;
        typedef MapConfigVer::iterator ITER;
        MapConfigVer  m_mapConfigVersion;

        //TcpClient���
	CTcpClient  * TcpClient_;
    

};


}
}

#endif

