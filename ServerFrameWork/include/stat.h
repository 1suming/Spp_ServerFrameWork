/********************************************
//�ļ���:stat.h
//����:ͳ�Ƴ���
//����:�Ӻ���
//����ʱ��:2009.06.11
//�޸ļ�¼:

*********************************************/
#ifndef _TBASE_TSTAT_H_
#define _TBASE_TSTAT_H_
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "stat_policy.h"

namespace spp
{
namespace stat
{
    
#define STAT_TYPE_SUM     1	            //�ۼӲ��� 
#define STAT_TYPE_AVG     1<<1	   //ƽ������
#define STAT_TYPE_MAX     1<<2	   //���ֵ����
#define STAT_TYPE_MIN     1<<3	   //��Сֵ����
#define STAT_TYPE_COUNT   1<<4	   //��������
#define STAT_TYPE_UPDATE 1<<5   //���²���
#define STAT_TYPE_ALL	  -1	            //ͨ��	
#define STAT_TYPE_NUM     6	            //ͳ�Ʋ��Ը���

#define TSTAT_MAGICNUM    83846584	    //ͳ�Ƴػ��� 
#define TSTAT_VERSION	  0x01                    //ͳ�ư汾��
#define BUCKET_NUM        100	            //ͳ�ƶ���Ͱ����
#define DEFAULT_STATOBJ_NUM  100	    //ͳ�ƶ����ܸ���
#define DEFAULT_STATVAL_NUM  (DEFAULT_STATOBJ_NUM*1)  //ͳ��ֵ�ܸ�����һ��ͳ�ƶ���������һ��ͳ��ֵ��
#define STAT_BUFF_SIZE	  1<<14          //ͳ�Ʊ���������С
#define STAT_MAX_VALSIZE  10                    //ͳ��ֵ���ά��
#define INVALID_HANDLE	  -1

#define ERR_STAT_EXIST    -2000             //ͳ�ƶ����Ѿ�����
#define ERR_STAT_NONE	  -2001	  //ͳ�ƶ��󲻴���
#define ERR_STAT_FULL	  -2010                //û�п����ڴ�洢ͳ�ƶ���
#define ERR_STAT_OPENFILE -2020	  //���ڴ�ӳ���ļ�ʧ��
#define ERR_STAT_TRUNFILE -2021	 //truncate�ļ�ʧ��
#define ERR_STAT_MAPFILE  -2030	 //ӳ���ļ�ʧ��
#define ERR_STAT_MEMERROR -2040	 //�����ڴ���������


///////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    char id_[STAT_ID_MAX_LEN];            //ͳ��ID
    char desc_[STAT_ID_MAX_LEN*2];   //ͳ������
    int type_;                                              //ͳ������
    int val_size_;                                        //ͳ��ֵά��
    TStatVal count_;                                 //����
    int val_offset_;                                    //ͳ��ֵƫ����
    int next_;                                              //��һ��TStatObj
}TStatObj; //ͳ�ƶ���

///////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    long long magic_;                        //����
    int ver_;                                        //�汾��
    //char buffer_[STAT_BUFF_SIZE];  //ͳ�Ʊ������� --��ʹ����
    int freelist_;                                  //����TStatObj����
    int bucket_[BUCKET_NUM];       //TStatObj��ϡͰ
    int statobjs_used_;                      //ʹ�õ�TStatObj��Ŀ
    TStatObj statobjs_[DEFAULT_STATOBJ_NUM];  //TStatObj����
    int statvals_used_;                        //ʹ�õ�TStatVal��Ŀ
    TStatVal statvals_[DEFAULT_STATVAL_NUM];  //TStatVal����
}TStatPool; //ͳ�ƶ����

///////////////////////////////////////////////////////////////////////////////////
//ͳ�Ƴ���
class CTStat
{
public:
    CTStat();
    ~CTStat();
    //��ʼ��ͳ�Ƴ�, ��mapfile != NULL����ʹ���ļ�ӳ��Ĺ����ڴ棬����ʹ�ý��̶��ڴ�
    int init_statpool(const char* mapfilepath = NULL);

    //��ʼ��ͳ�Ʋ���
    //policy:  ͳ�Ʋ�����
    //type:    ͳ�Ʋ������ͣ�2���������ݣ�
    int init_statpolicy(CTStatPolicy* policy, int type);

    //��ʼ��ͳ�ƶ���
    //id:	ͳ��ID
    //type:	ͳ�Ʋ������ͣ����Ը��ϣ�
    //desc: ͳ������
    //value_size: ͳ�ƶ���ͳ��ֵά��
    int init_statobj(const char* id, int type, const char* desc = NULL, int val_szie = 1);

    //һ��ͳ�ƣ��������
    //ids:	ͳ��ID����
    //num:	ͳ��ID����
    //val:  ͳ��ֵ����
    //value_idx: ͳ��ֵά��
    int step(const char** ids, int num, long val, int val_idx = 1);

    //һ��ͳ�ƣ���������
    //id:	ͳ��ID
    //val:  ͳ��ֵ����
    //value_idx: ͳ��ֵά��
    int step0(const char* id, long val, int val_idx = 1);

    //����ͳ�ƽ��
    //buffer: ָ��ͳ�Ʊ���������ָ��
    //len:      ͳ�Ʊ�����
    void result(char** buffer, int* len);

    //���ɼ���ϱ��������
    //buffer: ָ��ͳ�Ʊ���������ָ��
    //len:      ͳ�Ʊ�����
    void moni_result(char ** buffer,int * len );

    //��������ͳ�ƶ���
    void reset();

    //��ѯͳ�ƶ�����Ϣ
    //id:       ͳ��ID
    //wrapper:  ͳ�ƶ�����Ϣ
    int query(const char* id, TStatObjWrapper* wrapper);
protected:
    CTStatPolicy* policy_[STAT_TYPE_NUM];
    int policy_no_[STAT_TYPE_NUM];
    int policy_type_[1<<STAT_TYPE_NUM];
    TStatPool* statpool_;
    int policy_num_;
    bool mapfile_;
    
    inline unsigned hashid(const char* id)
    {
        int len = strlen(id);
        unsigned hash = 1315423911;
        for(int i = 0; i < len; ++i)
        {
            hash ^= ((hash << 5) + id[i] + (hash >> 2));
        }
        return hash % BUCKET_NUM;
    }
    int find_statobj(const char* id, int type = STAT_TYPE_ALL);
    int alloc_statobj(int val_size);
    void insert_statobj(int choice);
    void output_statpool(char* buffer, int* len);
    void moni_statpool(char * buffer,int *len);
    void output_statobj(const TStatObj* obj, long count, const long* values, int val_size, char* buffer, int* len);
    void newpool();
    typedef void (CTStat::*visit_func) (TStatObj*, void*);
    void travel(visit_func visitor, void* data = NULL);
    void do_reset(TStatObj* obj, void* data = NULL);
    void do_result(TStatObj* obj, void* data = NULL);
};

}
}
#endif

