/********************************************
//�ļ���:stat_policy.h
//����:ͳ�Ʋ�����
//����:�Ӻ���
//����ʱ��:2009.06.11
//�޸ļ�¼:

*********************************************/
#ifndef _TBASE_TSTAT_POLICY_H_
#define _TBASE_TSTAT_POLICY_H_

#include <asm/atomic.h>

namespace spp
{
namespace stat
{    

#define STAT_ID_MAX_LEN   64	        //ͳ��ID�ַ�����󳤶�

#ifdef __x86_64__
#define TStatVal             	        atomic64_t
#define STATVAL_INC(val)    	atomic64_inc(&(val))
#define STATVAL_DEC(val)	atomic64_dec(&(val))
#define STATVAL_ADD(val, lv)	atomic64_add(lv, &(val))
#define STATVAL_SET(val, lv)	atomic64_set(&(val), lv)
#define STATVAL_READ(val)	atomic64_read(&(val))
#else
#define TStatVal             	        atomic_t
#define STATVAL_INC(val)    	atomic_inc(&(val))
#define STATVAL_DEC(val)	atomic_dec(&(val))
#define STATVAL_ADD(val, lv)	atomic_add(lv, &(val))
#define STATVAL_SET(val, lv)	atomic_set(&(val), lv)
#define STATVAL_READ(val)	atomic_read(&(val))
#endif


///////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    char* id_;                //ͳ��ID
    char* desc_;          //ͳ������
    int type_;                //ͳ������
    int val_size_;          //ͳ��ֵά��
    TStatVal* count_;  //����
    TStatVal* value_;   //ͳ��ֵ
}TStatObjWrapper; //�ⲿ�ɼ���ͳ�ƶ���

///////////////////////////////////////////////////////////////////////////////////
//ͳ�Ʋ���
class CTStatPolicy
{
public:
    CTStatPolicy(){}
    virtual ~CTStatPolicy(){}
    
    //һ��ͳ��
    //target:  ͳ�ƶ���
    //val:       ͳ��ֵ����
    //val_idx: ͳ��ֵά��
    virtual void __step__(TStatObjWrapper* target, long val, int val_idx) = 0;
    
    //ȡͳ�ƽ��
    //target:  ͳ�ƶ���
    //val:       ͳ��ֵ
    //val_size: ͳ��ֵά��
    //����ֵ:   ͳ�ƴ���
    virtual long __result__(TStatObjWrapper* target, long* val, long* val_size)
    {
        assert(target != NULL && target->val_size_ > 0 && val != NULL);

        *val_size = target->val_size_;
        for(int i = 0; i < *val_size; ++i)    
            val[i] = STATVAL_READ(target->value_[i]);
        return STATVAL_READ(*(target->count_));
    }
    //����ͳ��ֵ
    virtual void __reset__(TStatObjWrapper* target)
    {
        assert(target != NULL && target->val_size_ > 0);

        for(int i = 0; i < target->val_size_; ++i)
            STATVAL_SET(target->value_[i], 0);
        STATVAL_SET(*(target->count_), 0);
    }
    //ͳ�Ʋ�����
    virtual const char* __tag__() = 0;
};

//�ۼӲ���	
class CTStatPolicySum : public CTStatPolicy
{
public:
    CTStatPolicySum(){}
    ~CTStatPolicySum(){}
    virtual void __step__(TStatObjWrapper* target, long val, int val_idx)
    {
        assert(target != NULL && target->val_size_ > 0);

        STATVAL_ADD(target->value_[val_idx - 1], val);
        if(val_idx == target->val_size_)
            STATVAL_INC(*(target->count_));
    }
    virtual const char* __tag__()
    {
        return "Sum";
    }
};

//ƽ������
class CTStatPolicyAvg : public CTStatPolicy
{
public:
    CTStatPolicyAvg(){}
    ~CTStatPolicyAvg(){}
    virtual void __step__(TStatObjWrapper* target, long val, int val_idx)
    {
        assert(target != NULL && target->val_size_ > 0);

        STATVAL_ADD(target->value_[val_idx - 1], val);
        if(val_idx == target->val_size_)
            STATVAL_INC(*(target->count_));
    }
    virtual long __result__(TStatObjWrapper* target, long* val, long* val_size)
    {
        assert(target != NULL && target->val_size_ > 0 && val != NULL);

        long temp = STATVAL_READ(*(target->count_));
        if(temp > 0)
        {    
            *val_size = target->val_size_;
            for(int i = 0; i < *val_size; ++i)
            val[i] = STATVAL_READ(target->value_[i]) / temp;

            return temp;
        }
        else
        {
            return 0;
        }
    }
    virtual const char* __tag__()
    {
        return "Avg";
    }
};

//���ֵ����	
class CTStatPolicyMax : public CTStatPolicy
{
public:
    CTStatPolicyMax(){}
    ~CTStatPolicyMax(){}
    virtual void __step__(TStatObjWrapper* target, long val, int val_idx)
    {
        assert(target != NULL && target->val_size_ > 0);

        long temp = STATVAL_READ(target->value_[val_idx - 1]);
        if(temp < val)
            STATVAL_SET(target->value_[val_idx - 1], val);    
        if(val_idx == target->val_size_)
            STATVAL_INC(*(target->count_));
    }
    virtual const char* __tag__()
    {
        return "Max";
    }
};

//��Сֵ����	
class CTStatPolicyMin : public CTStatPolicy
{
public:
    CTStatPolicyMin(){}
    ~CTStatPolicyMin(){}
    virtual void __step__(TStatObjWrapper* target, long val, int val_idx)
    {
        assert(target != NULL && target->val_size_ > 0);

        long temp = STATVAL_READ(target->value_[0]);
        if(temp > val || temp == 0)
            STATVAL_SET(target->value_[0], val);    
        STATVAL_INC(*(target->count_));
    }
    virtual const char* __tag__()
    {
        return "Min";
    }
};

//��������
class CTStatPolicyCount : public CTStatPolicy
{
public:
    CTStatPolicyCount(){}
    ~CTStatPolicyCount(){}
    virtual void __step__(TStatObjWrapper* target, long val, int val_idx)
    {
        assert(target != NULL && target->val_size_ > 0);

        STATVAL_ADD(target->value_[val_idx - 1], val);
        if(val_idx == target->val_size_)
            STATVAL_INC(*(target->count_));
    }
    virtual const char* __tag__()
    {
        return "Count";
    }
};

//���²���
class CTStatPolicyUpdate: public CTStatPolicy
{
public:
    CTStatPolicyUpdate(){}
    ~CTStatPolicyUpdate(){}

    virtual void __step__(TStatObjWrapper* target, long val, int val_idx)
    {
        assert(target != NULL && target->val_size_ > 0);
        STATVAL_SET(target->value_[val_idx - 1], val); 
        
    }

    virtual const char* __tag__()
    {
        return "Update";
    }
    

};

//...����ͳ�Ʋ��Դ���չ...

}
}
#endif

