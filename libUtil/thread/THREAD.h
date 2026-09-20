#ifndef _THREAD_H
#define _THREAD_H
#ifdef _MSC_VER
#include "PosixCompat.h"
#else
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#endif
#include "STL.h"
#include "KSTRING.h"
#include "LOCK.h"
namespace nsUtil
{
class BaseThread
{
	public :
		BaseThread(const char * _pszThreadName = NULL);
		virtual ~BaseThread();
		virtual bool run();
		bool run_(int _nCoreID, int _nStackSize, bool _bDetached = false);
		virtual void shutdown();
		virtual bool waitForShutdown(int _nMs) const;
		void join();
		void detach();
		bool isShutdown() const;
		void setName(const char * _pszThreadName);
		const char * getName() const;
		unsigned long getId();
		int getKernelTid() const;
	protected :
		static void * threadWrapper(void * _data);
		virtual void process() = 0;
		pthread_t m_threadId;
		int m_nKernelTid;
		char m_szThreadName[32];
		bool m_bShutdown;
		mutable pthread_mutex_t m_shutdownMutex;
		mutable pthread_cond_t m_shutdownCondition;
	private:
		int m_fnSetCore(int _nCoreID);
		
};

template <class T>
class NormalThread : public BaseThread
{
	public :
		typedef void (* PfuncProcess)(T * _pclsT);
		NormalThread(const char * _pszThreadName = NULL);
		virtual ~NormalThread();
		void setProcessCb(PfuncProcess _pfnProcessCb);
		void setObject(T * _pclsT);
	protected :
		virtual void process();
		PfuncProcess m_pfnProcessCb;
		T * m_pclsObject;
};

template <class T>
NormalThread<T>::NormalThread(const char * _pszThreadName)
	: BaseThread(_pszThreadName), m_pfnProcessCb(NULL), m_pclsObject(NULL)
{
}

template <class T>
NormalThread<T>::~NormalThread()
{
}

template <class T>
void NormalThread<T>::setProcessCb(PfuncProcess _pfnProcessCb)
{
	m_pfnProcessCb = _pfnProcessCb;
}

template <class T>
void NormalThread<T>::setObject(T * _pclsT)
{
	m_pclsObject = _pclsT;
}

template <class T>
void NormalThread<T>::process()
{
	m_nKernelTid = syscall(SYS_gettid);
	if( m_pfnProcessCb )
	{	
		while( !isShutdown() )
		{
			m_pfnProcessCb(m_pclsObject);	
		}
	}
}
class THREAD
{
	public:
		THREAD();
		virtual ~THREAD();
		const KSTRING & NAME();
		void RUN(KCSTR _pszName);
		bool ISDOWN();
		virtual void PROC()=0;
	private:
		static void m_fnCbkProc(THREAD * _pclsObj);
		NormalThread<THREAD> * m_pclsThread;
		KSTRING m_szName;
};
}
#endif

