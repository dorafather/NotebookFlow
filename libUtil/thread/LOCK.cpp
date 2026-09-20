#include "LOCK.h"

namespace nsUtil
{

Mutex::Mutex()
{
	int nRet = pthread_mutex_init(&m_MutexId, NULL);
	if( nRet != 0 )
	{
	}
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&m_MutexId);
}
		
void Mutex::m_fnLock()
{
	pthread_mutex_lock(&m_MutexId);
}

void Mutex::m_fnUnlock()
{
	pthread_mutex_unlock(&m_MutexId);
}

RwMutex::RwMutex()
	: Lockable(), m_unReaderCount(0), m_bWriterHasLock(false), m_unPendingWriterCount(0)
{
}

RwMutex::~RwMutex()
{
}

void RwMutex::m_fnLock()
{
	m_fnWriteLock();
}

void RwMutex::m_fnReadLock()
{
	Lock clsLock(m_clsMutex);

	while( m_bWriterHasLock || m_unPendingWriterCount > 0 )
	{
		m_clsReadCondition.wait(m_clsMutex);
	}

	m_unReaderCount++;
}

void RwMutex::m_fnWriteLock()
{
	Lock clsLock(m_clsMutex);

	++m_unPendingWriterCount;

	while( m_bWriterHasLock || m_unReaderCount > 0 )
	{
		m_clsPendingWriteCondition.wait(m_clsMutex);
	}

	--m_unPendingWriterCount;

	m_bWriterHasLock = true;
}

void RwMutex::m_fnUnlock()
{
	Lock clsLock(m_clsMutex);

	if( m_bWriterHasLock )
	{
		m_bWriterHasLock = false;

		if( m_unPendingWriterCount > 0 )
		{
			m_clsPendingWriteCondition.signal();
		}
		else
		{
			m_clsReadCondition.broadcast();
		}
	}
	else
	{
		--m_unReaderCount;

		if( m_unReaderCount == 0 && m_unPendingWriterCount > 0 )
		{
			m_clsPendingWriteCondition.signal();
		}
	}
}

Lock::Lock(Lockable & _rclsLockable, ELockType_t _eLockType)
	: m_rclsLockable(_rclsLockable)
{
	if( _eLockType == E_LOCK_TYPE_READ )
		m_rclsLockable.m_fnReadLock();
	else if( _eLockType == E_LOCK_TYPE_WRITE )
		m_rclsLockable.m_fnWriteLock();
	else
		m_rclsLockable.m_fnLock();
}

Lock::~Lock()
{
	m_rclsLockable.m_fnUnlock();
}
MUTEX::MUTEX()
{
}
MUTEX::~MUTEX()
{
}
void MUTEX::WLOCK()
{
	mLock.m_fnWriteLock();
}
void MUTEX::RLOCK()
{
	mLock.m_fnReadLock();
}
void MUTEX::UNLOCK()
{
	mLock.m_fnUnlock();
}
}

