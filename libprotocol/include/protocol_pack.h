/********************************
*�ļ�����protocol_pack.h
*���ܣ�Э���������
*���ߣ��ż���
*ʱ�䣺2009.05.20
**********************************/


#ifndef _PROTOCOL_PACK_H_
#define _PROTOCOL_PACK_H_

#ifdef _WIN32
#define _WIN32_WINNT 0x0400
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib,"wsock32.lib")
typedef unsigned __int64 uint64_t;
#else
#include <arpa/inet.h>
#include <stdint.h>
#endif


//#include <arpa/inet.h>
//#include <stdint.h>

#include <string>
#include <sstream>
using std::string;
using std::ostringstream;

namespace protocol
{
/*��Ϣͷ���*/
const unsigned char SOH = 'J';
/*��Ϣβ���*/
const unsigned char EOT = 'W';

/*Э�����汾��*/
const unsigned char MAIN_VER = 1;
/*Э���Ӱ汾��*/
const unsigned char SUB_VER = 0;

/*���ܷ�ʽ��������*/
const unsigned char ENC_TYPE_NONE = 0;

/*���ܷ�ʽ��TEA����*/
const unsigned char ENC_TYPE_TEA = 1;

/*��Դ���Ͷ���7bit,��1bitΪServer�ڲ�ʹ��*/
//========Client 1-31,0��ʱ����=============
const unsigned char SOURCE_TYPE_MIN_IM = 1;
const unsigned char SOURCE_TYPE_MAX_IM = 31;

const unsigned char SOURCE_TYPE_IM_CLIENT = 1;//IM

//========Web 32-63=======================
const unsigned char SOURCE_TYPE_MIN_WEB = 32;
const unsigned char SOURCE_TYPE_MAX_WEB = 63;

const unsigned char SOURCE_TYPE_WEB_SNS = 32;//SNS����
const unsigned char SOURCE_TYPE_WEB_HOME = 33;//788111�Ż�
const unsigned char SOURCE_TYPE_WEB_TAO = 34;//�Թ�����

//=========DYJ Client 64-127==================
const unsigned char SOURCE_TYPE_DYJ_MIN_CLIENT = 64;
const unsigned char SOURCE_TYPE_DYJ_MAX_CLIENT = 127;

const unsigned char SOURCE_TYPE_DYJ_CLIENT_XGW = 64;//ѡ����
const unsigned char SOURCE_TYPE_DYJ_CLIENT_PCW = 66;//������
const unsigned char SOURCE_TYPE_DYJ_CLIENT_GGW = 68;//��Ʊ�ܼ�
const unsigned char SOURCE_TYPE_DYJ_CLIENT_GBS = 70;//�ɲ�ʿ

//====================================

const unsigned int MSG_TOTAL_LEN_POS = 4;
const unsigned int MSG_MAIN_CMD_POS = 7;
const unsigned int MSG_SUB_CMD_POS = 9;
const unsigned int MSG_SEQ_POS = 11;
const unsigned int MSG_FROM_UIN_POS = 15;
const unsigned int MSG_TO_UIN_POS = 19;
const unsigned int MSG_ENC_TYPE_POS = 23;
const unsigned int MSG_SOURCE_TYPE_POS = 24;
const unsigned int MSG_REV_POS = 25;
const unsigned int MSG_HEADER_LEN = 29;


/*
*���ܣ�ȡ���ݰ�ͷ�еĳ����ֶΡ�
            ע�����ݰ���������
            ����Ҫ>=7
*IN p:���ݰ���ַ
*����ֵ�������ܳ���
*/
inline unsigned int GetPackLen(const unsigned char *p)
{
    unsigned int *q = (unsigned int *)(p+3);
    return ntohl(*q);
}

inline void SetPackLen(unsigned char *p, unsigned int len)
{
    unsigned int *q = (unsigned int *)(p+3);
    *q =  htonl(len);
}

/*
*���ܣ��������ݰ�ͷ�е��������ֶΡ�
            ע�����ݰ���������
            ����Ҫ>=11
*IN p:���ݰ���ַ
*IN main:��������
*IN sub:��������
*����ֵ��
*/
inline void SetPackCmd(const unsigned char *p, unsigned short main, unsigned short sub)
{
    unsigned short *q = (unsigned short *)(p+MSG_MAIN_CMD_POS);
    *q = htons(main);
    *(q+1) = htons(sub);
}

inline void GetPackCmd(const unsigned char *p, unsigned short &main, unsigned short &sub)
{
    unsigned short *q = (unsigned short *)(p+MSG_MAIN_CMD_POS);
    main = ntohs(*q);
    sub = ntohs(*(q+1));
}

inline void SetFromUin(const unsigned char *p, unsigned int from_uin)
{
    unsigned int *q = (unsigned int *)(p+MSG_FROM_UIN_POS);
    *q = htonl(from_uin);
}

inline unsigned int GetFromUin(const unsigned char *p)
{
    unsigned int *q = (unsigned int *)(p+MSG_FROM_UIN_POS);
    return ntohl(*q);
}

inline void SetToUin(const unsigned char *p, unsigned int to_uin)
{
    unsigned int *q = (unsigned int *)(p+MSG_TO_UIN_POS);
    *q = htonl(to_uin);
}

inline unsigned int GetToUin(const unsigned char *p)
{
    unsigned int *q = (unsigned int *)(p+MSG_TO_UIN_POS);
    return ntohl(*q);
}

inline void SetSequence(const unsigned char *p, unsigned int seq)
{
    unsigned int *q = (unsigned int *)(p+MSG_SEQ_POS);
    *q = htonl(seq);
}

inline unsigned int GetSequence(const unsigned char *p)
{
    unsigned int *q = (unsigned int *)(p+MSG_SEQ_POS);
    return  ntohl(*q);
}



/*
*����:���ñ����ֶ�
*����˵��:
*   IN p:���ݰ���ַ
*   IN rev:�����ֶ�
* ����ֵ:��
*/
inline void SetReserve(unsigned char *p, unsigned char rev)
{
    unsigned int *q = (unsigned int *)(p+MSG_REV_POS);
    unsigned int reserve = ntohl(*q);
    reserve = (rev<<24) | (reserve & 0x00FFFFFF);
    *q = htonl(reserve);
}

/*
*����:��ȡ�����ֶ�
*����˵��:
*   IN p:���ݰ���ַ
* ����ֵ:�����ֶ�����
*/
inline unsigned char GetReserve(const unsigned char *p)
{
    unsigned int *q = (unsigned int *)(p+MSG_REV_POS);
    return  (unsigned char ) (ntohl(*q) >> 24);
}

/*
*����:����Cookie��λ��
*����˵��:
*   IN p:���ݰ���ַ
*   IN offset:cookie��ƫ��λ��
* ����ֵ:��
*/
inline void SetCookiePos(unsigned char *p, unsigned int pos)
{
    unsigned int *q = (unsigned int *)(p+MSG_REV_POS);
    unsigned int reserve = ntohl(*q);
    reserve = (reserve & 0xFF000000 ) | (pos & 0x00FFFFFF);
    *q = htonl(reserve);
}

/*
*����:ȡ��Cookie��λ��
*����˵��:
*   IN p:���ݰ���ַ
* ����ֵ:
* 0:��cookie
* >0:Cookie��λ��
*/
inline unsigned int GetCookiePos(const unsigned char *p)
{
    unsigned int *q = (unsigned int *)(p+MSG_REV_POS);
    return  (ntohl(*q) & 0x00FFFFFF);
}

/*
* ����:����cookie��Ϣ
* ����˵��:
* IN p:���ݰ���ַ
* IN len:���ݰ�buf�ĳ���
* IN ck:cookie��Ϣ��ַ
* IN ckLen:cookie��Ϣ����
* ����ֵ: 
* �ɹ�:������Ϣ���ܳ���,ʧ��<0
*/
inline int SetCookie(unsigned char * p,unsigned int len,const void * ck,unsigned short ckLen )
{        
    unsigned int pack_len = GetPackLen(p);
    unsigned int ck_pos = GetCookiePos(p);
    unsigned short ori_ck_len = 0;//ԭ��cookie��Ϣ����
    unsigned short * l = (unsigned short *)(p + ck_pos);
    if(ck_pos == 0 )
    {
        //ԭ����cookie��Ϣ
        l =(unsigned short *) (p + pack_len - 1);//Pack Tail      
    }
    else
    {
        ori_ck_len = ntohs(*l);//ԭ��cookie�ĳ���
        pack_len -= (2 + ori_ck_len);//����pack_len
    }

    if( len < (pack_len + 2 + ckLen ) )
        return -1;//buf̫С��
    
    *l = htons(ckLen);
    unsigned char * v = (unsigned char *) (l + 1);
    memcpy(v,ck,ckLen);
    *(v+ckLen) = EOT;

    SetCookiePos(p,pack_len - 1);
    SetPackLen(p,pack_len + 2 + ckLen);    
    return (int)(pack_len + 2 + ckLen);
}

inline int SetCookie(unsigned char * p,unsigned int len,const string & cookie)
{
    if(cookie.size() > 0xFFFF )
        return -2;//cookie ̫��
        
    return SetCookie(p,len,cookie.data(),(unsigned short )cookie.size());
}

/*
* ����:ȡ����Ϣ���е�cookie��Ϣ
* ����˵��:
* IN p:���ݰ���ַ
* OUT ck:cookie��Ϣ
* IN OUT ckLen:
    IN ��ʾcookie���ջ������ĳ���
    OUT��ʾʵ��cookie����
* ����ֵ: 
*  0:�ɹ�
*  <0:ʧ��
*/
inline int GetCookie(const unsigned char *p,void * ck,unsigned short & ckLen)
{
    unsigned int ck_pos = GetCookiePos(p);
    if(ck_pos == 0 )
    {
        //û��cookie��Ϣ
        ckLen = 0;
        return 0;
    }
    
    unsigned short * l = (unsigned short * ) (p + ck_pos);
    unsigned short ck_len = ntohs(*l);
    if(ckLen < ck_len )
    {
        //ck����̫С,����ʵ��cookie����
        ckLen = ck_len;
        return -1;
    }
    
    unsigned char * v = (unsigned char *)( l + 1);
    memcpy(ck,v,ck_len);
    ckLen = ck_len;    
    return 0;
}

/*
* ����:ȡ����Ϣ���е�cookie��Ϣ
* ����˵��:
* IN p:��Ϣ��ͷָ��
* OUT ck:cookie��Ϣ
* ����ֵ: 
* �ɹ�:true,ʧ��:false
*/
inline int GetCookie(const unsigned char *p,string & ck)
{
    unsigned int ck_pos = GetCookiePos(p);
    if(ck_pos == 0 )
    {
        //û��cookie��Ϣ
        return 0;
    }
    
    unsigned short * l = (unsigned short * ) (p + ck_pos);
    unsigned short ck_len = ntohs(*l);
    if(ck_len == 0 )
    {
        ck = "";
        return 0;//cookie����Ϊ��
    }
        
    unsigned char * v = (unsigned char *)( l + 1);
    ck.assign((const char *)v,ck_len);
    return 0;
}

/*
* ����:ɾ����Ϣ���е�cookie��Ϣ
* ����˵��:
* IN p:��Ϣ��ͷָ��
* ����ֵ: 
* �ɹ�:>0��Ϣ���ĳ���,<=0ʧ��
*/
inline int DelCookie(unsigned char * p)
{
    unsigned  int  pack_len = GetPackLen(p);
    unsigned int ck_pos =GetCookiePos(p);
    if(ck_pos <= MSG_HEADER_LEN || ck_pos >= pack_len )
    {
        //û��cookie��Ϣ
        return pack_len;
    }
        
    unsigned short * l = (unsigned short * ) (p + ck_pos);
    unsigned short ck_len = ntohs(*l);
    pack_len -= (2 + ck_len);
    unsigned char * v = (unsigned char * ) l;
    *v = EOT;
    SetCookiePos(p,0);
    SetPackLen(p,pack_len);
    return (int)pack_len;
}


inline unsigned char GetEncType(const unsigned char *p){return p[MSG_ENC_TYPE_POS];}
inline void SetEncType(unsigned char *p, unsigned int type){p[MSG_ENC_TYPE_POS]=type;}


/*��Ϣͷ����*/
class MsgHeader
{
public:
	MsgHeader():soh(SOH), main_ver(MAIN_VER), sub_ver(SUB_VER),
		main_cmd(0), sub_cmd(0), seq(0), from_uin(0), to_uin(0),
		enc_type(ENC_TYPE_NONE), source_type(SOURCE_TYPE_IM_CLIENT),rev(0)
	{
	}
	
public:
	unsigned char soh;              /*��Ϣ��ʼ���*/
	unsigned char main_ver;     /*Э�����汾��*/
	unsigned char sub_ver;      /*Э���Ӱ汾��*/
	unsigned int pack_len;    /*�������ĳ��ȣ�������ͷ�����壬��β*/
	unsigned short main_cmd;    /*��������*/
	unsigned short sub_cmd;     /*��������*/
	unsigned int seq;                   /*���к�*/
	unsigned int from_uin;          /*���ͷ���uin*/
	unsigned int to_uin;              /*���շ���uin,û�о���0*/
	unsigned char enc_type;     /*���ܷ�ʽ*/
	unsigned char source_type;  /*��Ϣ��Դ*/
	unsigned int rev;                   /*�����ֶ�*/
};




/*
�����
*/
class Packer
{
public:
	Packer();
	virtual ~Packer();
        /*
        *       ���ܣ����ô����
        */
	void Reset();
        /*
        *       ���ܣ����8bit����
        *       IN v:Ҫ���������
        *       ����ֵ��*this����
        */
	Packer &PackByte(unsigned char v);
        /*
        *       ���ܣ����16bit����
        *       IN v:Ҫ���������
        *       ����ֵ��*this����
        */
	Packer &PackWord(unsigned short v);

        /*
        *       ���ܣ����32bit����
        *       IN v:Ҫ���������
        *       ����ֵ��*this����
        */
	Packer &PackDWord(unsigned int v);

        /*
        *       ���ܣ����64bit����
        *       IN v:Ҫ���������
        *       ����ֵ��*this����
        */
	Packer &PackQWord(uint64_t v);
        /*
        *       ���ܣ����������
        *       IN v:Ҫ���������
        *       ����ֵ��*this����
        */
	Packer &PackReal(double v);
    
        /*
        *       ���ܣ����������
        *       IN v:Ҫ���������
        *       IN len:���ݳ���
        *       ����ֵ��*this����
        */
	Packer &PackBinary(const void *v, unsigned int len);
        /*
        *       ���ܣ�����ַ���
        *       IN v:Ҫ���������
        *       ����ֵ��*this����
        */
	Packer &PackString(const string &v);

        /*
        *       ���ܣ�����ַ���
        *       IN v:Ҫ���������
        *       ����ֵ��*this����
        */
	Packer &PackString(const char *v);

        /*
        *       ���ܣ���ȡ���ɵİ���
        *       ����ֵ�����ɵ���Ϣ����
        */
	string GetBody();

        /*
        *       ���ܣ�������������Ϣ��
        *       IN h:��ͷ
        *       ����ֵ�����ɵ���Ϣ��
        */
	string GetPack(const MsgHeader &h);

        /*
        *       ���ܣ�������������Ϣ��
        *       IN h:��ͷ
        *       IN key:���ܵ���Կ
        *       IN key_len:��Կ�ĳ���
        *       ����ֵ�����ɵ���Ϣ��
        */
	string GetPack(const MsgHeader &h, const void *key, unsigned int key_len);

        /*
        *       ������������Ϣ��
        *       bufΪ��������������ɵİ�����buf���档
        *       lenΪ�������������
        *       ����ֵ��<=0:buf���Ȳ���, >0����Ϣ���ĳ���
        */
	//int GetPack(char *buf, unsigned int len, const MsgHeader &h);
	//int GetPack(char *buf, unsigned int len, const MsgHeader &h, const void *key, unsigned int key_len);


        /*
        *   ����:���ɴ�cookie��������Ϣ��
        *   IN h:��ͷ
        *   IN ck:cookie��Ϣ
        *   IN ck_len:cookie��Ϣ����
        *   ����ֵ:���ɵ���Ϣ��
        */
        string GetPack(const MsgHeader & h,unsigned short ck_len,const char * ck);

        
        /*
        *hbuf:���ɵ�header��
        *body_size:����ĳ��ȣ���������ͷ�Ͱ�β
        */
        static void PackHeader(char *hbuf, const MsgHeader &h, unsigned int body_size);

        /*
        pack�������������Ҳ�����������
            ������Ϣ����pack��һ����������Ϣ����
            ���ú�pack���Ѿ��Ǽ��ܺ�����ݡ�
            ���ܺ����ݳ��Ȳ���
        */
	static void EncryptPack(void *pack, unsigned int len, const void *key, unsigned int key_len=16);
protected:
	ostringstream m_oss;
};




class FixedPacker
{
public:
	FixedPacker(char *buf = NULL, unsigned int len = 0){Init(buf, len);}
	virtual ~FixedPacker(){}
    unsigned int GetErr(){return m_Err;}
    int Init(char *buf, unsigned int len);
    int LeftSpace(){ return m_BufLen - (m_pTail - m_pBuf);}
	FixedPacker &PackByte(unsigned char v);
	FixedPacker &PackWord(unsigned short v);
	FixedPacker &PackDWord(unsigned int v);
	FixedPacker &PackQWord(uint64_t v);
	FixedPacker &PackReal(double v);
    
	FixedPacker &PackBinary(const void *v, unsigned int len);
	FixedPacker &PackString(const string &v);
	FixedPacker &PackString(const char *v);
	string GetBody();

        /*
        *len:�������ɰ��ĳ��ȣ�<=0ʧ�ܣ�>0�ɹ�
        *����ֵ������ָ��
        */
	char *GetPack(int &len, const MsgHeader &h);
	char *GetPack(int &len, const MsgHeader &h, const void *key, unsigned int key_len);


        /*
        *   ����:���ɴ�cookie��������Ϣ��
        *   OUT len:�������ɰ��ĳ��ȣ�<=0ʧ�ܣ�>0�ɹ�
        *   IN h:��ͷ
        *   IN ck:cookie��Ϣ
        *   IN ck_len:cookie��Ϣ����
        *   ����ֵ:����ָ��
        */
        char * GetPack(int & len,const MsgHeader & h,unsigned short ck_len,const char * ck);

    
        /*
        *       ������������Ϣ��
        *       bufΪ��������������ɵİ�����buf���档
        *       lenΪ�������������
        *       ����ֵ��<=0:buf���Ȳ���, >0����Ϣ���ĳ���
        */
	//int GetPack(char *buf, unsigned int len, const MsgHeader &h);
	//int GetPack(char *buf, unsigned int len, const MsgHeader &h, const void *key, unsigned int key_len);

        /*
            pack�������������Ҳ�����������
            ������Ϣ����pack��һ����������Ϣ����
            ���ú�pack���Ѿ��Ǽ��ܺ�����ݡ�
            ���ܺ����ݳ��Ȳ���
        */
	static void EncryptPack(void *pack, unsigned int len, const void *key, unsigned int key_len=16);
protected:
        char *m_pBuf;
        char *m_pTail;
        int m_BufLen;
	unsigned int m_Err;
        
};









/*
�����
*/
class UnPacker
{
public:
	UnPacker();
	virtual ~UnPacker();

        /*
        ��ʼ��������0�ɹ��������������ʽ����
        */
	int Init(const void *data, unsigned int len);

	/*������ʽ�Ƿ���ȷ*/
	int CheckPack(const void *data, unsigned int len);
	
	void GetHeader(MsgHeader &h);

        /*
        ���ܰ���ԭ���İ������ݻᱻ�޸�Ϊ���ܺ�����ݡ�
        */
	int Decrypt(const void *key, unsigned int len=16);

        /*
        ���ܰ���ԭ���İ������ݲ��ᱻ�޸�
        */
	int DumpDecrypt(const void *key, unsigned int len=16);


        /*
        pack�������������Ҳ�����������
            ������Ϣ����pack��һ����������Ϣ����
            ���ú�pack���Ѿ��ǽ��ܺ�����ݡ�
            ���ܺ����ݳ��Ȳ���
        */
        static void DecryptPack(void *pack, unsigned int len, const void *key, unsigned int key_len=16);

	
	void Reset();
	unsigned char UnPackByte();
	unsigned short UnPackWord();
	unsigned int UnPackDWord();
	uint64_t UnPackQWord();
        double UnPackReal();

        /*
        * v:�������Ķ���������,�ڴ�Ҫ�ȷ���ã��㹻len����
        *len:���������ݵĳ���
        *����0����ɹ�
        */
	unsigned int UnPackBinary(void * v, unsigned int len);

        /*
        *len:���������ݵĳ���
        *�������ݵ�ָ��,����NULL�������
        */
	const char *UnPackBinary(unsigned int len);

        /*
        * data:�������Ķ���������
        *len:���������ݵĳ���
        *����0����ɹ�
        */
        unsigned int UnPackBinary(string &data, unsigned int len);

    
	string UnPackString();
	const char *UnPackString(unsigned int &len);
	
	unsigned int GetErr();
protected:
	const char *m_pData;
	unsigned int m_Len;
	unsigned int m_Err;
	string m_Body;
	const char *m_pBody;
	unsigned int m_BodyLen;
	unsigned int m_Offset;
};

}

#endif


