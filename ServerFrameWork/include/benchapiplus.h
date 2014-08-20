
#ifndef _BENCHAPIPLUS_H_
#define _BENCHAPIPLUS_H_

typedef int		(*spp_handle_init_t)		 	 (void*, void*);//�����ʼ��
typedef int		(*spp_handle_input_t)		        (unsigned int, void*, void*);//������������
typedef int		(*spp_handle_route_t)		 (unsigned int, void*, void*);//���ݰ�·��
typedef void		(*spp_handle_fini_t)		  	 (void*, void*);//������Դ

typedef int		(*spp_handle_local_process_t)(unsigned, void*, void*);//ҵ����

typedef int           (*spp_handle_event_route_t)(unsigned int,unsigned int,void *,void *);//�����¼�·������
typedef int           (*spp_handle_queue_full_t) (unsigned int ,unsigned int, void * , void * );//�ڴ�ܵ����Ĵ���
typedef int           (*spp_handle_preprocess_t)(unsigned int,unsigned int, void*, void*);//ҵ��Ԥ����
typedef int		(*spp_handle_process_t)	  	(unsigned int,unsigned int, void*, void*);//ҵ����
typedef int 		(*spp_handle_reloadconfig_t)(void *,void *,void *);//��������ҵ������������ļ�


typedef struct 
{ 
	void *handle;
	spp_handle_init_t		spp_handle_init;
	spp_handle_input_t		spp_handle_input;
	spp_handle_route_t		spp_handle_route;
	spp_handle_queue_full_t  spp_handle_queue_full;
	spp_handle_process_t	spp_handle_process;
	spp_handle_reloadconfig_t spp_handle_reloadconfig;
	spp_handle_event_route_t spp_handle_event_route;
	spp_handle_fini_t		spp_handle_fini;
}spp_dll_func_t;
extern spp_dll_func_t sppdll;

#endif

