#ifndef _MSC_VER
extern "C" 
{
#include <time.h>
}
#else
#include <time.h>
#endif
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "THREAD.h"

namespace nsUtil
{
void * BaseThread::threadWrapper(void * _data)
{
	BaseThread * pclsThread = (BaseThread *)_data;
	if( pclsThread )
		pclsThread->process();

	return NULL;
}

BaseThread::BaseThread(const char * _pszThreadName)
	: m_threadId(0), m_nKernelTid(-1), m_bShutdown(true)
{
	m_szThreadName[0] = 0x00;
	if( _pszThreadName )
	{
		strncpy(m_szThreadName, _pszThreadName, sizeof(m_szThreadName)-1);
		m_szThreadName[sizeof(m_szThreadName)-1] = 0x00;
	}
	pthread_mutex_init(&m_shutdownMutex, NULL);
	pthread_cond_init(&m_shutdownCondition, NULL);
}

BaseThread::~BaseThread()
{
	pthread_cond_destroy(&m_shutdownCondition);
    pthread_mutex_destroy(&m_shutdownMutex);
}

bool BaseThread::run()
{
	if( !m_bShutdown )
	{
		return true;
	}
	m_bShutdown = false;
	int nResult = pthread_create(&m_threadId, 0, threadWrapper, this);
	if( nResult != 0 )
	{
		m_threadId = 0;
		m_bShutdown = true;
		return false;
	}
	else
	{
		m_bShutdown = false;
		return true;
	}
}
bool BaseThread::run_(int _nCoreID, int _nStackSize, bool _bDetached)
{
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);
	if(_nStackSize > 0) pthread_attr_setstacksize(&tattr, _nStackSize);
	if (_bDetached)
		pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
	int nResult = pthread_create(&m_threadId,&tattr,threadWrapper,this);
	if( nResult != 0 )
	{
		m_threadId = 0;
		m_bShutdown = true;
		pthread_attr_destroy(&tattr);
		return false;
	}
	else
	{
		m_bShutdown = false;
		m_fnSetCore(_nCoreID);
		pthread_attr_destroy(&tattr);
		return true;
	}
}
void BaseThread::shutdown()
{
	pthread_mutex_lock(&m_shutdownMutex);
	
	if( !m_bShutdown )
	{
		m_bShutdown = true;
		pthread_cond_signal(&m_shutdownCondition);
	}

	pthread_mutex_unlock(&m_shutdownMutex);
}

bool BaseThread::waitForShutdown(int _nMs) const
{
	if( m_bShutdown == 0 )
		return true;
	
	pthread_mutex_lock(&m_shutdownMutex);
	
	if( !m_bShutdown )
	{
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += _nMs / 1000;
		ts.tv_nsec += (_nMs % 1000)*1000*1000;
		ts.tv_sec += ts.tv_nsec / (1000*1000*1000);
		ts.tv_nsec = ts.tv_nsec % (1000*1000*1000);
		pthread_cond_timedwait(&m_shutdownCondition, &m_shutdownMutex, &ts);
	}

	pthread_mutex_unlock(&m_shutdownMutex);
	
	return m_bShutdown;
}

void BaseThread::join()
{
	if( m_threadId == 0 )
	{
		return;
	}

	if( m_threadId != pthread_self() )
	{
		int nResult = pthread_join(m_threadId , NULL);
		if( nResult != 0 )
		{
		}
	}
}

void BaseThread::detach()
{
	if( m_threadId == 0 )
	{
		return;
	}

	pthread_detach(m_threadId);
}

bool BaseThread::isShutdown() const
{
	return m_bShutdown;
}

void BaseThread::setName(const char * _pszThreadName)
{
	if( _pszThreadName )
	{
		strncpy(m_szThreadName, _pszThreadName, sizeof(m_szThreadName)-1);
		m_szThreadName[sizeof(m_szThreadName)-1] = 0x00;
	}
}

const char * BaseThread::getName() const
{
	return m_szThreadName;
}

unsigned long BaseThread::getId()
{
	return (unsigned long)m_threadId;
}

int BaseThread::getKernelTid() const
{
	return m_nKernelTid;
}

int BaseThread::m_fnSetCore(int _nCoreID)
{
	if(_nCoreID < 0) return -1;
	int num_cores = sysconf(_SC_NPROCESSORS_ONLN);   
	if (_nCoreID >= num_cores)  
	return EINVAL;                                                                                    	
	cpu_set_t cpuset;                                                                                    
	CPU_ZERO(&cpuset);                                                                                   	
	CPU_SET(_nCoreID, &cpuset);
	return pthread_setaffinity_np(m_threadId, sizeof(cpu_set_t), &cpuset);
}
THREAD::THREAD()
{
	m_pclsThread=NULL;
}
THREAD::~THREAD(){delete m_pclsThread;}
const KSTRING & THREAD::NAME()
{
	return m_szName;
}
void THREAD::RUN(KCSTR _pszName)
{
	m_pclsThread = new NormalThread<THREAD>(_pszName);
	m_pclsThread->setProcessCb(m_fnCbkProc);
	m_pclsThread->setObject(this);
	m_pclsThread->run_(-1,-1,false);
}
bool THREAD::ISDOWN(){return m_pclsThread->isShutdown();}
void THREAD::m_fnCbkProc(THREAD * _pclsObj)
{
	_pclsObj->PROC();
}
}


