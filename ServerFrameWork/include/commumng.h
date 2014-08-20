/***************************************
*�ļ���:commumng.h
*����:���ͨѶ���ӹ���
*����:�Ӻ���
*����ʱ��:2009.06.11
***************************************/

#ifndef _COMMU_MNG_H_
#define _COMMU_MNG_H_
#include <string>
#include <vector>
#include <map>
#include <assert.h>

using namespace std;

namespace comm
{
namespace sockcommu
{

//���ͨѶ����
class CCommuMng
{
public:	
    typedef std::map<unsigned int,unsigned int> NodeList;//nodeid->flow
    typedef NodeList::iterator NodeListIter;

    typedef std::map<unsigned int,NodeList *> GroupList;//serv_type,Node list
    typedef GroupList::iterator GroupListIter;

    CCommuMng();
    ~CCommuMng();

    //����һ������
    //serv_type:��������
    //node_id:�ڵ�ID
    //flow:���Ӿ��
    //����ֵ:0:�ɹ�������ʧ��
    int AddConn(unsigned int serv_type,unsigned node_id,unsigned int flow);

    //�Ƴ�����
    //flow:���ӵ�Ψһ���
    //����ֵ:�ɹ�:0,����ʧ��
    int RMConn(unsigned int flow);

    //��ȡ���Ӿ��
    //serv_type:��������
    //node_id:�ڵ�ID
    //����ֵ:
    //0:ʧ��,>0��ʾ���Ӿ��flow
    unsigned int GetFlow(unsigned int serv_type,unsigned int node_id);


    //ͨ��flowȡ�ù�����server_type
    //[out] server_type:��������
    //[in] flow:ͨѶΨһ��ʶ
    //����ֵ:
    //0ʧ�ܣ�>0��ʾ��flow������server_type
    unsigned int GetServerTypeByFlow(unsigned int flow);


    /*
    *��ӶϿ����ӵĽڵ���Ϣ
    *[in] serv_type:��������
    *[in] node_id:�ڵ�ID
    */
    //int AddDropItem(unsigned int serv_type,unsigned int node_id);

    /*
    *�Ƴ��Ͽ����ӵĽڵ���Ϣ
    *[in] serv_type:��������
    *[in] node_id:�ڵ�ID
    */
    //int RMDropItem(unsigned int serv_type,unsigned int node_id);

    //int GetDropNodeList(std::vector<unsigned int > & NodeIDList,unsigned int serv_type);

    //int GetDropServerList(std::vector<unsigned int> & ServerIDList);

protected:

    GroupList grp_list_;//���Ӿ���б�	
    //GroupList drop_conn_list_;//�Ͽ��������б�

};

}
}


#endif

