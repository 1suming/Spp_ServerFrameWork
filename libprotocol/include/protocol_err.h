/********************************
*�ļ�����protocol_err.h
*���ܣ�Э������붨��
*���ߣ��ż���
*�汾��1.0
*������ʱ�䣺2009.05.20
**********************************/


#ifndef _PROTOCOL_ERR_H_
#define _PROTOCOL_ERR_H_

#ifdef _WIN32
#define _WIN32_WINNT 0x0400
#include <hash_map>
using namespace stdext;
#else
#include <ext/hash_map>
using namespace __gnu_cxx;
#endif

//#include <ext/hash_map>
//using namespace __gnu_cxx;

#include <string>
using std::string;

namespace protocol
{

const unsigned int RET_OK = 0;
/*Э�����*/
const unsigned int ERR_PROTOCOL = 1;
/*��֧�ֵļ��ܷ�ʽ*/
const unsigned int ERR_ENC_TYPE = 2;
/*��֧�ֵİ汾��*/
const unsigned int ERR_PROTOCOL_VER = 3;
/*�������ݿ����*/
const unsigned int ERR_DB = 4;
/*�������*/
const unsigned int ERR_NETWORK = 5;


//DB����������ش�����
//0x0000ff00 ~ 0x0000fffe
//DB��������ʱ�����ֲ���ʧ��
const unsigned int ERR_DB_PART = 0x0000ff00;



//δ֪����
const unsigned int IM_ERR_UNKNOWN = 0x0000FFFF;


/*
*������������ע�����ѯ
*/
class ErrInfoMng
{
public:

	/*���ݴ�������Ҷ�Ӧ�Ĵ�������*/
	static string GetErrInfo(unsigned int err_code)
	{
		hash_map<unsigned int, string>::iterator it = m_ErrInfo.find(err_code);		
		 return (it == m_ErrInfo.end()) ? string(""):it->second;
	}

	/*ע��������������Ϣ*/
	static int RegErrInfo(unsigned int err_code, const string &err_info)
	{
		m_ErrInfo[err_code] = err_info;
		return 0;
	}

    /*ע��������������Ϣ*/
	static int RegErrInfo(unsigned int err_code, const char* err_info)
	{
		m_ErrInfo[err_code] = err_info;
		return 0;
	}

private:
	//static __gnu_cxx::hash_map<unsigned int, string> m_ErrInfo;
    static hash_map<unsigned int, string> m_ErrInfo;
};


/*
example:
	ErrInfoMng::RegErrInfo(ERR_IM_AUTH, "Auth error");
	string err_info = ErrInfoMng::GetErrInfo(ERR_IM_AUTH);
*/

}

#endif



