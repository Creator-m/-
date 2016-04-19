#include<list>
#include<cstdio>
#include<pthread.h>
#include<iostream>
#include "locker.h"
using namespace std;

class locker;
template<typename T>
class threadpool
{
public:
	threadpool(int threadNumber = 10,int maxRequest=1024);
	~threadpool();
	bool append(T*request);             //������������������
private:
	static void *worker(void *arg);
	void run();
private:
	int mthreadNumber;               //�̵߳��������
	int mmaxRequest;                 //��������е����������
	pthread_t* mthreads;             //�����̳߳ص�����,��СΪmthreadNumber
	std::list<T*> mrequestQueue;     //�������    
	locker mqueueLocker;             //����������еĻ�����
	sem mprosess;                    //�Ƿ���������Ҫ����
	bool mstop;                      //�Ƿ�����߳�
};
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
threadpool<T>::threadpool(int threadNumber,int maxRequest)
:mthreadNumber(threadNumber)
,mmaxRequest(maxRequest)
,mstop(false)
,mthreads(NULL)
{
	if( (mthreadNumber <= 0) || (mmaxRequest <= 0) )
	{
		throw"";
	}
	mthreads = new pthread_t[mthreadNumber];
	if (!mthreads)
	{
		throw"";
	}
	//����threadNumber���߳�,�������ó������߳�
	for(int i = 0; i < threadNumber; ++i)
	{
		if( pthread_create(mthreads+i,NULL,worker,this) != 0 )
		{
			delete [] mthreads;
			throw"";
		}
		if (pthread_detach(mthreads[i]) != 0)
		{
			delete [] mthreads;
			throw"";
		}
	}
}

template<typename T>
threadpool<T>::~threadpool()
{
	delete [] mthreads;
	mstop = true;
}

template<typename T>
bool threadpool<T>::append(T* request)
{
	//�����������ʱҪ����,��Ϊ���������̹߳���
	mqueueLocker.lock();                      
	if (mrequestQueue.size() > mmaxRequest)
	{
		mqueueLocker.unlock();
		return false;
	}
	mrequestQueue.push_back(request);
	mqueueLocker.unlock();
	mprosess.post();
	return true;
}



template<typename T>
void* threadpool<T>::worker(void *arg)
{
	threadpool *pool = (threadpool  *)arg;
	pool->run();
	return pool;
}



template<typename T>
void threadpool<T>::run()
{
	while(!mstop)
	{
		mprosess.wait();                //�ȴ��ź���
		mqueueLocker.lock();
		if ( mrequestQueue.empty())
		{
			mqueueLocker.unlock();
			continue;
		}
		T*request = mrequestQueue.front();
		mrequestQueue.pop_front();
		mqueueLocker.unlock();
		if (!request)
		{
			continue;
		}
		request->process();
	}
}
