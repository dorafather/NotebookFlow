#ifndef _QUEUE_THREAD_H
#define _QUEUE_THREAD_H
#ifdef _MSC_VER
#include "PosixCompat.h"
#include <time.h>
#else
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
extern "C" 
{
#include <time.h>
}
#endif
#include <stdio.h>
#include "THREAD.h"
#include "QUEUE.h"
#include "LOCK.h"
#include "SESSION.h"
#include "ARGV.h"
#include "TIMER.h"

namespace nsUtil
{
template<class T> class QueueThread;
typedef struct stDiffSpec_t
{
	unsigned int m_unSec;
	unsigned int m_unMsec;
}stDiffSpec_t;

/******************************** Queue Data Class ****************************************/
template <class T>
class InternalQData
{
	public:
		InternalQData(){m_bDogKick=false;m_pvData=NULL;m_unMax=0;m_unCurrent=0;}
		virtual ~InternalQData(){delete m_pvData;}
		bool m_bDogKick;
		T * m_pvData;
		unsigned int m_unMax;
		unsigned int m_unCurrent;
};

/******************************** Queue Thread *******************************************/
template <class T>
class QueueThread : public BaseThread
{
	public:
		typedef void(*PfuncProcess)(T *, void *);
		QueueThread(unsigned int _unMaxQueueSize = 1000,
										PfuncProcess _pfnProcessCb = NULL,
										int _nQueueWaitMs = -1,
										const char * _pszThreadName = NULL,
										bool _bQueueMonitor = false,
										bool _bUseTbb = true)
		: BaseThread(_pszThreadName),
			m_pObject(NULL),
			m_clsLimitQueue(_unMaxQueueSize, _bQueueMonitor, _pszThreadName, _bUseTbb), 
			m_pfnProcessCb(_pfnProcessCb), m_nQueueWaitMs(_nQueueWaitMs)
		{
		}
		virtual ~QueueThread()
		{
		}
		void shutdown(){BaseThread::shutdown();m_clsLimitQueue.wake();}
		bool run()
		{
			m_clsLimitQueue.clear();
			bool bRet =  BaseThread::run();			
			return bRet;
		}
		bool run_(int _nCoreID, int _nStackSize, bool _bDetached)
		{
			m_clsLimitQueue.clear();
			bool bRet = BaseThread::run_(_nCoreID, _nStackSize, _bDetached);			
			return bRet;
		}
		void clearQueue()
		{
			m_clsLimitQueue.clear();
		}
		void setObject(void * _pObject) { m_pObject = _pObject; }
		void setMaxQueueSize(unsigned int _unMaxQueueSize) 
		{ 
			m_clsLimitQueue.setMaxQueueSize(_unMaxQueueSize); 
		}
		void setProcessCb(PfuncProcess _pfnProcessCb) { m_pfnProcessCb = _pfnProcessCb; }
		void setQueueWaitMs(int _nQueueWaitMs) { m_nQueueWaitMs = _nQueueWaitMs; }
		unsigned int getMaxQueueSize() const { return m_clsLimitQueue.getMaxQueueSize(); }
		unsigned int getCurrentQueueSize() const { return m_clsLimitQueue.getCurrentQueueSize(); }
		bool put(T * _pclsData)
		{
			InternalQData<T> * pclsNew = new InternalQData<T>;
			pclsNew->m_pvData = _pclsData;
			bool bRet = m_clsLimitQueue.put(pclsNew);
			if(bRet == false)
			{
				pclsNew->m_pvData = NULL;
				delete pclsNew;
			}
			return bRet;
		}
	private:
		void process()
		{
			m_nKernelTid = syscall(SYS_gettid);			
			InternalQData<T> * pclsData = NULL;
			while( !isShutdown() )
			{
				pclsData = m_clsLimitQueue.get(m_nQueueWaitMs);
				if( isShutdown() )
				{
					delete pclsData;
					break;
				}
				if( m_pfnProcessCb )
				{
					if( pclsData )
					{
						m_pfnProcessCb(pclsData->m_pvData, m_pObject);
						pclsData->m_pvData = NULL;
					}
					else
						m_pfnProcessCb(NULL, m_pObject);
				}
				delete pclsData;
			}
		}
		void * m_pObject;
		LimitedQueue< InternalQData<T> > m_clsLimitQueue;
		PfuncProcess m_pfnProcessCb;
		int m_nQueueWaitMs;
};
class QTHREAD
{
	public:
		class QOPTION
		{
			public:
				QOPTION();
				~QOPTION();
				KSTR  DEBUGGING(KSTRING & _rDebug);
				KSTRING m_szName;
				KUINT m_unMaxQ;
				KINT m_nPollingTimeMs;
				POOL::POOLOPTION m_PoolOpt;
		};
		QTHREAD();
		virtual ~QTHREAD();
		const KSTRING & NAME();
		KUINT IDX();
		void RUN(QOPTION * _pOpt, KUINT _idx = 0);
		bool ADD(ARG * _pclsArg);
		POOL::POOLDATA * ALLOC();
		POOL::POOLDATA * ALLOC_(KSTRING _szKey);
		POOL::POOLDATA * GET(KUINT _nId);
		POOL::POOLDATA * GET_(KSTRING _szKey);
		bool FREE(KUINT _nId);
		bool FREE_(KSTRING _szKey);
		void SETTIMER(ARG & _arg);
		KUINT POOLSIZE();
		KUINT POOLLEN();
		KUINT QSIZE();
		KUINT QLEN();
		virtual void PROC(ARG & _arg)=0;
		virtual void GARBAGE(POOL::POOLDATA & _rData)=0;
		static KUINT INDEXING(KSTR _szKey, KUINT _nNums);
		static KUINT INDEXING(KUINT _nKey, KUINT _nNums);
		void CLEARQ();
		bool ISQFULL();
		bool ISSFULL();
	private:
		class QTIMER : public TIMER
		{
			public:
				QTIMER();
				~QTIMER();
				void TIMEOUT(ARG & _arg);
				QTHREAD * m_pOwner;
		};
		class QPOOL : public POOL
		{
			public:
				QPOOL();
				~QPOOL();
				void GARBAGE(POOL::POOLDATA & _rData);
				QTHREAD * m_pOwner;
		};
		static void m_fnCbkProc(ARG * _pclsArg, void * _pvObj);
		QueueThread<ARG> * m_pclsQ;
		QTIMER m_Timer;
		QPOOL m_Pool;
		QOPTION * m_pOpt;
		KUINT m_nIdx;
		KSTRING m_szName;
		KBOOL m_bInit;
};
}
#endif

