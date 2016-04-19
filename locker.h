#ifndef _LOCKER
#define _LOCKER
#include<pthread.h>
#include<semaphore.h>

//��װ�ź�������
class sem
{
public:
	sem()
	{
		if( sem_init(&msem,0,0) != 0 )
		{
			//throw std::exception();
		}
	}
	~sem(){ sem_destroy(&msem); }
	//�ȴ��ź���
	bool wait(){ return sem_wait(&msem) == 0; }
	//�����ź���
	bool post(){ return sem_post(&msem) == 0; }
private:
	sem_t msem;
};

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//��װ������
class locker
{
public:
	//��������ʼ��������
	locker()
	{
		if(pthread_mutex_init(&mmutex,NULL) != 0)
		{
			//throw std::exception();
		}
	}
	//���ٻ�����
	~locker()
	{
		pthread_mutex_destroy(&mmutex);
	}
	//��ȡ������
	bool lock()
	{
		return pthread_mutex_lock(&mmutex) == 0;
	}
	//�ͷŻ�����
	bool unlock()
	{
		return pthread_mutex_unlock(&mmutex) == 0;
	}
private:
	pthread_mutex_t mmutex;
};
#endif
