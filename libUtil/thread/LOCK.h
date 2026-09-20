#ifndef _LOCK_H
#define _LOCK_H
#ifdef _MSC_VER
#include "PosixCompat.h"
#else
#include <pthread.h>
#endif
#include "CONDITION.h"

namespace nsUtil
{
class Lockable
{
	public:
		Lockable(){};
		virtual ~Lockable(){};
		virtual void m_fnLock() = 0;
		virtual void m_fnReadLock(){ m_fnLock(); }
		virtual void m_fnWriteLock(){ m_fnLock() ; }
		virtual void m_fnUnlock() = 0;
};
class Mutex : public Lockable
{
	public:
		Mutex();
		virtual ~Mutex();
		virtual void m_fnLock();
		virtual void m_fnUnlock();
		inline pthread_mutex_t * getMutexId(){ return &m_MutexId; }
	private:
		pthread_mutex_t m_MutexId;

};
class RwMutex : public Lockable
{
	public:
		RwMutex();
		virtual ~RwMutex();
		virtual void m_fnLock();
		virtual void m_fnReadLock();
		virtual void m_fnWriteLock();
		virtual void m_fnUnlock();
	private:
		Mutex m_clsMutex;
		Condition m_clsReadCondition;
		Condition m_clsPendingWriteCondition;
		unsigned int m_unReaderCount;
		bool m_bWriterHasLock;
		unsigned int m_unPendingWriterCount;

};
class Lock
{
	public:
		typedef enum
		{
			E_LOCK_TYPE_NORMAL = 0,
			E_LOCK_TYPE_READ,
			E_LOCK_TYPE_WRITE,

			E_LOCK_TYPE_NUM,
		}ELockType_t;
		Lock(Lockable & _rclsLockable, ELockType_t _eLockType=E_LOCK_TYPE_NORMAL);
		~Lock();
	private:
		Lockable & m_rclsLockable;
};
class MUTEX
{
	public: 
		MUTEX();
		~MUTEX();
		void WLOCK();
		void RLOCK();
		void UNLOCK();
	private:
		RwMutex mLock;
};
}

#endif
