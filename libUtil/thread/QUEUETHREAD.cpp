#include <stdio.h>
#ifdef _MSC_VER
#include "PosixCompat.h"
#include <io.h>        
#include <fcntl.h>    
#include <sys/utime.h> 
#include <time.h>
#else
#include <fcntl.h>
#include <utime.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
extern "C" 
{
#include <time.h>
}
#endif

#include <list>

#include "QUEUETHREAD.h"
#include "TIMEUTIL.h"

namespace nsUtil
{
unsigned long s_abs(unsigned long _a, unsigned long _b)
{
	if(_a >= _b) return (_a - _b);
	else return (_b - _a);
}
void s_fnDiffTimeSpec(struct timespec * _pst1, struct timespec * _pst2, stDiffSpec_t * _pstRet)
{
	unsigned long ulA = _pst1->tv_sec*1000 + _pst1->tv_nsec/1000000;
	unsigned long ulB = _pst2->tv_sec*1000 + _pst2->tv_nsec/1000000;
#ifndef _MSC_VER
	unsigned long ulRet = s_abs(ulA,ulB); //abs(ulA-ulB);
#else
	unsigned long ulRet = labs(ulA-ulB);
#endif
	_pstRet->m_unSec =  ulRet/1000; _pstRet->m_unMsec = ulRet%1000;
}
unsigned long s_fnDiffTimeSpecNow4Mono(const struct timespec * _pstOld)
{
	struct timespec stNow;
	clock_gettime(CLOCK_MONOTONIC, &stNow);
	unsigned long ulA = stNow.tv_sec*1000 + stNow.tv_nsec/1000000;
	unsigned long ulB = _pstOld->tv_sec*1000 + _pstOld->tv_nsec/1000000;
#ifndef _MSC_VER
	unsigned long ulRet = s_abs(ulA,ulB); //abs(ulA-ulB);
#else
	unsigned long ulRet = labs(ulA-ulB);
#endif
	return ulRet;
}
unsigned long s_fnDiffTimeSpecNow(const struct timespec * _pstOld)
{
	struct timespec stNow;
	clock_gettime(CLOCK_REALTIME, &stNow);
	unsigned long ulA = stNow.tv_sec*1000 + stNow.tv_nsec/1000000;
	unsigned long ulB = _pstOld->tv_sec*1000 + _pstOld->tv_nsec/1000000;
#ifndef _MSC_VER
	unsigned long ulRet = s_abs(ulA,ulB); //abs(ulA-ulB);
#else
	unsigned long ulRet = labs(ulA-ulB);
#endif
	return ulRet;
}
/********************** QTHREAD ***************************/
QTHREAD::QOPTION::QOPTION(){m_unMaxQ = 100;m_nPollingTimeMs = -1;}
QTHREAD::QOPTION::~QOPTION(){}
KSTR  QTHREAD::QOPTION::DEBUGGING(KSTRING & _rDebug)
{
	_rDebug.PRINT("m_szName:%s, ",(KCSTR)m_szName);
	_rDebug.PRINT("m_nPollingTimeMs:%u, ",m_nPollingTimeMs);
	_rDebug.PRINT("m_unMaxQ:%u, <",m_unMaxQ);
	m_PoolOpt.DEBUGGING(_rDebug);
	_rDebug.PRINT("%s",">");
	return (KSTR)_rDebug;
}	
QTHREAD::QTHREAD()
{
	m_bInit = false;
	m_nIdx = 0;
	m_pOpt = NULL;
	m_Timer.m_pOwner = this;
	m_Pool.m_pOwner = this;
	m_pclsQ = new QueueThread<ARG>;
	m_pclsQ->setObject(this);
	m_pclsQ->setProcessCb(m_fnCbkProc);
}
QTHREAD::~QTHREAD()
{
	delete m_pclsQ;
}
const KSTRING & QTHREAD::NAME()
{
	return m_szName;
}
KUINT QTHREAD::IDX()
{
	return m_nIdx;
}
void QTHREAD::RUN(QOPTION * _pOpt, KUINT _idx)
{
	m_nIdx = _idx;
	m_pOpt = _pOpt;
	m_szName = _pOpt->m_szName;
	m_szName<<"_"; m_szName<<m_nIdx;
	KSTRING szTimer = m_szName; szTimer<<"Timer";
	m_Timer.RUN((KCSTR)szTimer);
	if(m_pOpt->m_PoolOpt.m_szName.m_unRealLen==0)
	{
		m_pOpt->m_PoolOpt.m_szName = m_szName;
	}
	m_pclsQ->setName((KSTR)m_szName);
	m_pclsQ->setMaxQueueSize(_pOpt->m_unMaxQ);
	m_Pool.INIT((POOL::POOLOPTION*)&m_pOpt->m_PoolOpt);
	m_pclsQ->setQueueWaitMs(_pOpt->m_nPollingTimeMs);
	m_pclsQ->run();
	ARG arg; 
	arg.SET(API_E_TIMER).VAL() = API_P_TIMER_GARBAGE;
	arg.SET(API_P_TIME).VAL() = m_pOpt->m_PoolOpt.m_nCheckTimeMs;
	m_Timer.SET(arg);
	m_bInit = true;
}
bool QTHREAD::ADD(ARG * _pclsArg)
{
	if(m_pclsQ->put(_pclsArg) == false)
	{
		delete _pclsArg;
		return false;
	}
	return true;
}
POOL::POOLDATA * QTHREAD::ALLOC(){return m_Pool.ALLOC();}
POOL::POOLDATA * QTHREAD::ALLOC_(KSTRING _szKey){return m_Pool.ALLOC_(_szKey);}
POOL::POOLDATA * QTHREAD::GET(KUINT _nId){return m_Pool.GET(_nId);}
POOL::POOLDATA * QTHREAD::GET_(KSTRING _szKey){return m_Pool.GET_(_szKey);}
bool QTHREAD::FREE(KUINT _nId){return m_Pool.FREE(_nId);}
bool QTHREAD::FREE_(KSTRING _szKey){return m_Pool.FREE_(_szKey);}
void QTHREAD::SETTIMER(ARG & _arg)
{
	m_Timer.SET(_arg);
}
KUINT QTHREAD::POOLSIZE(){return m_Pool.SIZE();}
KUINT QTHREAD::POOLLEN(){return m_Pool.LENGTH();}
KUINT QTHREAD::QSIZE()
{
	if(m_bInit==false) return 0;
	else return m_pclsQ->getMaxQueueSize();
}
KUINT QTHREAD::QLEN()
{
	if(m_bInit==false) return 0;
	else return m_pclsQ->getCurrentQueueSize();
}
void QTHREAD::m_fnCbkProc(ARG * _pclsArg, void * _pvObj)
{
	QTHREAD * pMy = (QTHREAD*)_pvObj;
	if(_pclsArg)
	{
		if(_pclsArg->GET(API_E_TIMER).VAL() == API_P_TIMER_GARBAGE)
		{
			pMy->m_Pool.CHECK();
		}
		else
		{
			pMy->PROC((ARG&)(*_pclsArg));
		}
		delete _pclsArg;
	}
	else
	{
		ARG arg; arg.GET(API_P_EVENT).VAL() = API_E_NULL;  // EMPTY_EVENT
		pMy->PROC(arg);
	}
}
KUINT QTHREAD::INDEXING(KSTR _szKey, KUINT _nNums)
{
	if(_szKey==NULL || _nNums==1 || KSTRING::m_fnStrLen(_szKey)==0) return 0;
	unsigned int unResult = 0;
	unsigned long key = 0;unsigned long ch = 0;
	key = 5381;
	for(unsigned int i = 0;(_szKey[i] != 0x00);i++)
	{
		ch = (unsigned long)_szKey[i];
		key = ((key<< 5) + key) + ch;
	}
	unResult = (unsigned int)(key%_nNums);
	return unResult;
}
KUINT QTHREAD::INDEXING(KUINT _nKey, KUINT _nNums)
{
	return _nKey%_nNums;
}
void QTHREAD::CLEARQ()
{
	if(m_bInit==false) return;
	m_pclsQ->clearQueue();
}
bool QTHREAD::ISQFULL()
{
	if(m_bInit==false) return false;
	if(m_pclsQ->getCurrentQueueSize() == m_pclsQ->getMaxQueueSize())
		return true;
	else return false;
}
bool QTHREAD::ISSFULL()
{
	if(POOLSIZE() == POOLLEN()) return true;
	return false;
}
QTHREAD::QTIMER::QTIMER(){m_pOwner=NULL;}
QTHREAD::QTIMER::~QTIMER(){}
void QTHREAD::QTIMER::TIMEOUT(ARG & _arg)
{
	if(_arg.GET(API_E_TIMER).VAL() == API_P_TIMER_GARBAGE)
	{
		ARG arg; arg<<_arg;
		SET(arg);
		ARG  *argNoti = new ARG; 
		argNoti->SET(API_E_TIMER).VAL() = API_P_TIMER_GARBAGE;
		m_pOwner->ADD(argNoti);
	}
	else
	{
		ARG * pEv = new ARG;
		*pEv << _arg;
		m_pOwner->ADD(pEv);	
	}
}
QTHREAD::QPOOL::QPOOL(){m_pOwner=NULL;}
QTHREAD::QPOOL::~QPOOL(){}
void QTHREAD::QPOOL::GARBAGE(POOL::POOLDATA & _rData)
{
	m_pOwner->GARBAGE(_rData);
}
}

