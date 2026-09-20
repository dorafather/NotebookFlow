#ifndef _QUEUE_H
#define _QUEUE_H
#include <string>
#ifdef _MSC_VER
#include "PosixCompat.h"
#include <time.h>
#else
#include <pthread.h>
extern "C" 
{
#include <time.h>
}
#endif
#include <string.h>

namespace nsUtil
{
template <class T>
class LimitedQueue
{
	public :
		LimitedQueue(unsigned int _unMaxQueueSize = 1000, bool _bMonitor = false, const char * _pszName = NULL, bool _bUseTbb = true);
		virtual ~LimitedQueue();
		void wake();
		void clear();
		bool put(T * _pData);
		T * get(int _nMs = -1);	
		void setMaxQueueSize(unsigned int _unMaxQueueSize);
		unsigned int getMaxQueueSize() const;
		unsigned int getCurrentQueueSize() const;
	private :
		void createQueue4Normal(unsigned int _unMaxQueueSize);
		void deleteQueue4Normal();
		void wake4Normal();
		void clear4Normal();
		bool put4Normal(T * _pData);
		T * get4Normal(int _nMs = -1);		
		void setMaxQueueSize4Normal(unsigned int _unMaxQueueSize);
		unsigned int getMaxQueueSize4Normal() const;
		unsigned int getCurrentQueueSize4Normal() const;			
		std::string m_clsName;
		int m_nQueueMonId;
		bool m_bUseTbb;
		T ** m_parrQueue;		
		unsigned int m_unMaxQueueSize;
		unsigned int m_unCurrentQueueSize;
		unsigned int m_unPutCurrentPos;
		unsigned int m_unGetCurrentPos;		
		pthread_mutex_t m_QueueMutex;
		pthread_cond_t m_QueueWakeCondition;
};

template<class T>
LimitedQueue<T>::LimitedQueue(unsigned int _unMaxQueueSize, bool _bMonitor, const char * _pszName, bool _bUseTbb)
	: m_nQueueMonId(-1), m_bUseTbb(_bUseTbb),
	m_parrQueue(NULL), 	m_unMaxQueueSize(0), m_unCurrentQueueSize(0), m_unPutCurrentPos(0), m_unGetCurrentPos(0)
{
	pthread_mutex_init(&m_QueueMutex, NULL);
	pthread_cond_init(&m_QueueWakeCondition, NULL);	
	createQueue4Normal(_unMaxQueueSize);
	if( _pszName && strlen(_pszName) > 0 )
		m_clsName = _pszName;
}

template<class T> 
LimitedQueue<T>::~LimitedQueue()
{
	deleteQueue4Normal();		
	pthread_cond_destroy(&m_QueueWakeCondition);
	pthread_mutex_destroy(&m_QueueMutex);
}

template<class T>
void LimitedQueue<T>::wake()
{
	wake4Normal();
}

template<class T>
void LimitedQueue<T>::clear()
{
	clear4Normal();
}

template<class T>
bool LimitedQueue<T>::put(T * _pData)
{
	return put4Normal(_pData);
}

template<class T>
T * LimitedQueue<T>::get(int _nMs)
{
	return get4Normal(_nMs);
}

template<class T>
void LimitedQueue<T>::setMaxQueueSize(unsigned int _unMaxQueueSize)
{
	setMaxQueueSize4Normal(_unMaxQueueSize);
}

template<class T>
unsigned int LimitedQueue<T>::getMaxQueueSize() const
{
	return getMaxQueueSize4Normal();
}

template<class T>
unsigned int LimitedQueue<T>::getCurrentQueueSize() const
{
	return getCurrentQueueSize4Normal();
}

template<class T>
void LimitedQueue<T>::createQueue4Normal(unsigned int _unMaxQueueSize)
{
	if( m_parrQueue )
		return;
	m_unMaxQueueSize = 0;
	m_unCurrentQueueSize = 0;
	m_unPutCurrentPos = 0;
	m_unGetCurrentPos = 0;
	unsigned int unMaxQueueSize = 1000;
	if( _unMaxQueueSize > 0 )
		unMaxQueueSize = _unMaxQueueSize;
	m_parrQueue = new T*[unMaxQueueSize];
	if (!m_parrQueue)
		return;	
	memset(m_parrQueue, 0x00, sizeof(T*)*unMaxQueueSize);
	m_unMaxQueueSize = unMaxQueueSize;
}

template<class T>
void LimitedQueue<T>::deleteQueue4Normal()
{
	if( m_parrQueue )
	{
		for( unsigned int i = 0; i < m_unMaxQueueSize; ++i )
		{
			delete m_parrQueue[i];
		}
		delete[] m_parrQueue;
		m_parrQueue = NULL;
	}
	m_unMaxQueueSize = 0;
	m_unCurrentQueueSize = 0;
	m_unPutCurrentPos = 0;
	m_unGetCurrentPos = 0;
}

template<class T>
void LimitedQueue<T>::wake4Normal()
{
	pthread_mutex_lock(&m_QueueMutex);
	pthread_cond_signal(&m_QueueWakeCondition);
	pthread_mutex_unlock(&m_QueueMutex);
}

template<class T>
void LimitedQueue<T>::clear4Normal()
{
	if( !m_parrQueue )
		return;
	pthread_mutex_lock(&m_QueueMutex);
	unsigned int unCurrentPos = m_unGetCurrentPos;
	unsigned int unDelCnt = 0;
	for( unsigned int i = 0; i < m_unMaxQueueSize; ++i )
	{
		if( unDelCnt >= m_unCurrentQueueSize )
			break;
		if( unCurrentPos >= m_unMaxQueueSize )
			unCurrentPos = 0;
		if( m_parrQueue[unCurrentPos] )
		{
			delete m_parrQueue[unCurrentPos];
			++unDelCnt;
		}
		++unCurrentPos;
	}
	memset(m_parrQueue, 0x00, sizeof(T*)*m_unMaxQueueSize);
	m_unCurrentQueueSize = 0;
	m_unPutCurrentPos = 0;
	m_unGetCurrentPos = 0;
	pthread_mutex_unlock(&m_QueueMutex);
}

template<class T>
bool LimitedQueue<T>::put4Normal(T * _pData)
{
	if( !m_parrQueue )
		return false;
	pthread_mutex_lock(&m_QueueMutex);
	if( m_unCurrentQueueSize >= m_unMaxQueueSize )
	{
		pthread_mutex_unlock(&m_QueueMutex);
		return false;
	}
	delete m_parrQueue[m_unPutCurrentPos];
	m_parrQueue[m_unPutCurrentPos] = _pData;
	++m_unCurrentQueueSize;
	++m_unPutCurrentPos;
	if( m_unPutCurrentPos >= m_unMaxQueueSize )
		m_unPutCurrentPos = 0;
	pthread_cond_signal(&m_QueueWakeCondition);	
	pthread_mutex_unlock(&m_QueueMutex);
	return true;
}

template<class T>
T * LimitedQueue<T>::get4Normal(int _nMs)
{
	if( !m_parrQueue )
		return NULL;
	T * pData = NULL;
	pthread_mutex_lock(&m_QueueMutex);
	if( m_unCurrentQueueSize == 0 )
	{
		if( _nMs == 0 )
		{
			pData = NULL;
			goto END_GET;
		}
		else if( _nMs < 0 )
		{
			pthread_cond_wait(&m_QueueWakeCondition, &m_QueueMutex);
		}
		else
		{
			struct timespec ts;
			clock_gettime(CLOCK_REALTIME, &ts);
			ts.tv_sec += _nMs / 1000;
			ts.tv_nsec += (_nMs % 1000)*1000*1000;
			ts.tv_sec += ts.tv_nsec / (1000*1000*1000);
			ts.tv_nsec = ts.tv_nsec % (1000*1000*1000);
			pthread_cond_timedwait(&m_QueueWakeCondition, &m_QueueMutex, &ts);
		}
	}
	if( m_unCurrentQueueSize == 0 )
	{
		pData = NULL;
		goto END_GET;
	}
	pData = m_parrQueue[m_unGetCurrentPos];
	m_parrQueue[m_unGetCurrentPos] = NULL;
	--m_unCurrentQueueSize;
	++m_unGetCurrentPos;
	if( m_unGetCurrentPos >= m_unMaxQueueSize )
		m_unGetCurrentPos = 0;
	END_GET:
	pthread_mutex_unlock(&m_QueueMutex);
	return pData;
}

template<class T>
void LimitedQueue<T>::setMaxQueueSize4Normal(unsigned int _unMaxQueueSize)
{
	pthread_mutex_lock(&m_QueueMutex);
	deleteQueue4Normal();
	createQueue4Normal(_unMaxQueueSize);
	pthread_mutex_unlock(&m_QueueMutex);
}

template<class T>
unsigned int LimitedQueue<T>::getMaxQueueSize4Normal() const
{
	return m_unMaxQueueSize;
}

template<class T> 
unsigned int LimitedQueue<T>::getCurrentQueueSize4Normal() const
{
	return m_unCurrentQueueSize;
}
}

#endif

