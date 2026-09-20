#include "SESSION.h"

namespace nsUtil
{
MAP::MAP(){}
MAP::~MAP(){}
ALIST * MAP::SET(KSTRING _szKey)
{
	ALIST * pNew = new ALIST;
	mMap.set((KCSTR)_szKey, pNew);
	return (ALIST*)pNew;
}
ALIST * MAP::GET(KSTRING _szKey)
{
	return (ALIST*)mMap.find((KCSTR)_szKey);
}
void MAP::DEL(KSTRING _szKey)
{
	mMap.del((KCSTR)_szKey);
}
KUINT MAP::LEN()
{
	return mMap.sizestr();
}
/********************** POOL ******************************/
POOL::POOLOPTION::POOLOPTION()
{
	m_nMax=100;
	m_nAliveTimeSec = 604800;
	m_nCheckNums=20;
	m_nCheckTimeMs = 200;
	m_nQueueCheckMs = 1000;
}
POOL::POOLOPTION::~POOLOPTION()
{
}
KSTR POOL::POOLOPTION::DEBUGGING(KSTRING & _rDebug)
{
	_rDebug.PRINT("m_szName:%s, ",(KCSTR)m_szName);
	_rDebug.PRINT("m_nMax:%u, ",m_nMax);
	_rDebug.PRINT("m_nCheckNums:%u, ",m_nCheckNums);
	_rDebug.PRINT("m_nCheckTimeMs:%u, ",m_nCheckTimeMs);
	_rDebug.PRINT("m_nAliveTimeSec:%u,",m_nAliveTimeSec);
	_rDebug.PRINT("m_nQueueCheckMs:%u",m_nQueueCheckMs);
	return (KSTR)_rDebug;
}			
POOL::POOLDATA::POOLDATA()
{
	m_pUser = NULL;
	m_pOwner = NULL;
	m_unIndex = 0;
	m_unTime = 0;
}
POOL::POOLDATA::~POOLDATA()
{
	if(m_pUser) delete m_pUser;
}
KUINT & POOL::POOLDATA::ID(){return m_unIndex;}
ALIST::Kobj & POOL::POOLDATA::SET(KCSTR _szKey)
{
	m_unTime = time(NULL);
	return m_list.SET(_szKey);
}
ALIST::Kobj & POOL::POOLDATA::GET(KCSTR _szKey)
{
	m_unTime = time(NULL);
	return m_list.GET(_szKey);
}
void POOL::POOLDATA::SETU(user * _pObj)
{  
   m_unTime = time(NULL);
   if(m_pUser) delete m_pUser;
   m_pUser = _pObj;
}
POOL::user * POOL::POOLDATA::GETU()
{  
   m_unTime = time(NULL);
   return m_pUser;
}
void POOL::POOLDATA::CLEAR()
{
	m_pOwner = NULL;
	m_unIndex = 0;
	m_unTime = 0;
	m_list.CLEAR();
	if(m_pUser)
	{
		delete m_pUser;
		m_pUser = NULL;
	}
}
KBOOL POOL::POOLDATA::DIFF(KUINT _unDiffT)
{
	if(_unDiffT == 0) return false;
	if(m_unTime==0) return false;
	unsigned int unCurrT = time(NULL);
	if((unCurrT - m_unTime) >= _unDiffT) return true;
	return false;
	
}
KSTR POOL::POOLDATA::DEBUGGING(KSTRING & _rDebug)
{
	return m_list.DEBUGGING(_rDebug);
}
POOL::POOL()
{
	m_pOpt = NULL;
	m_pclsIdMgr = NULL;
	m_arrId = NULL;
	m_unToken =0;
	m_unUniq = 0;
}
POOL::~POOL()
{
	if(m_pclsIdMgr) delete m_pclsIdMgr;
	if(m_arrId) delete [] m_arrId;
}
void POOL::INIT(POOL::POOLOPTION * _pOpt)
{
	m_pOpt = _pOpt;
	m_arrId = new POOLDATA[m_pOpt->m_nMax];
	m_pclsIdMgr = new  IdMgr(m_pOpt->m_nMax);
}
POOL::POOLDATA * POOL::ALLOC()
{
	if(m_pclsIdMgr->getAvailableIdNum() == 0)
	{
		return NULL;
	}
	int nId = m_pclsIdMgr->getAvailableId();
	if(nId == -1) return NULL;
	m_arrId[(unsigned int)nId].m_unTime = time(NULL);
	m_arrId[(unsigned int)nId].m_pOwner = this;
	m_arrId[(unsigned int)nId].m_unIndex = (KUINT)nId;
	m_arrId[(unsigned int)nId].SET(API_P_POOL_UNIQ).VAL() = (KUINT)m_unUniq++;
	return &m_arrId[(unsigned int)nId];
}
POOL::POOLDATA * POOL::ALLOC_(KSTRING _szKey)
{
	ALIST *pFind = m_map.GET(_szKey);
	if(pFind) return NULL;
	POOLDATA * pPool = ALLOC();
	if(pPool==NULL) return NULL;
	pFind = m_map.SET(_szKey);
	pPool->SET(API_P_POOL_KEY).VAL() = _szKey;
	KUINT nUniq = m_unUniq++;
	pPool->SET(API_P_POOL_UNIQ).VAL() = nUniq; 
	pFind->SET(API_P_POOL_ID).VAL() = pPool->ID();
	ALIST *pUniq = m_mapUniq.SET(nUniq);
	pUniq->SET(API_P_POOL_ID).VAL() = pPool->ID();
	return pPool;
}
POOL::POOLDATA * POOL::GET(KUINT _nId)
{
	if(_nId >= m_pOpt->m_nMax) return NULL;
	if(m_arrId[_nId].m_pOwner == NULL) return NULL;
	return &m_arrId[_nId];
}
POOL::POOLDATA * POOL::GET_(KSTRING _szKey)
{
	ALIST *pFind = m_map.GET(_szKey);
	if(pFind == NULL) return NULL;
	KSTRING & rKey = pFind->GET(API_P_POOL_ID).VAL();
	if(rKey.LENGTH() ==0) return NULL;
	return GET((KUINT)rKey);
}
POOL::POOLDATA * POOL::SEQ(KUINT _nSeq)
{
	ALIST *pFind = m_mapUniq.GET(_nSeq);
	if(pFind == NULL) return NULL;
	KSTRING & rKey = pFind->GET(API_P_POOL_ID).VAL();
	if(rKey.LENGTH() ==0) return NULL;
	return GET((KUINT)rKey);
}
bool POOL::FREE(KUINT _nId)
{
	if(m_pclsIdMgr->releaseId(_nId))
	{
		COMPSTR szKey = (KCSTR)m_arrId[_nId].GET(API_P_POOL_KEY).VAL();
		if(szKey.LENGTH() > 0)
		{
			m_map.DEL((KCSTR)szKey);	
		}
		KUINT nUniq =(KUINT)m_arrId[_nId].GET(API_P_POOL_UNIQ).VAL(); 
		m_mapUniq.DEL(nUniq);
		m_arrId[_nId].CLEAR();
		return true;
	}
	return false;
}
bool POOL::FREE_(KSTRING _szKey)
{
	ALIST *pFind = m_map.GET(_szKey);
	if(pFind == NULL) return false;
	KUINT nIdx = (KUINT)pFind->GET(API_P_POOL_ID).VAL();
	KUINT nUniq =(KUINT)m_arrId[nIdx].GET(API_P_POOL_UNIQ).VAL(); 
	m_map.DEL(_szKey);
	m_mapUniq.DEL(nUniq);
	return FREE(nIdx);
}
void POOL::CHECK()
{
	unsigned int unTok = 0;
	for(unsigned int i=0;i<m_pOpt->m_nCheckNums;i++)
	{
		unTok = m_fnGetToken();
		if(m_arrId[unTok].DIFF(m_pOpt->m_nAliveTimeSec))
		{
			GARBAGE(m_arrId[unTok]);
			FREE(unTok);
		}
	}
}
KUINT POOL::LENGTH()
{
	return (m_pOpt->m_nMax - m_pclsIdMgr->getAvailableIdNum());
}
KUINT POOL::SIZE()
{
	return m_pOpt->m_nMax;
}
KUINT POOL::m_fnGetToken()
{
	unsigned int unCur = m_unToken;
	if(unCur < (m_pOpt->m_nMax-1))
	{
		m_unToken = unCur +1;
	}
	else if(unCur == (m_pOpt->m_nMax-1))
	{
		m_unToken = 0;
	}
	return unCur;
}
}
