#ifndef __SESSION_H
#define __SESSION_H
#include "STL.h"
#include "KSTRING.h"
#include "COMPSTR.h"
#include "ID.h"
#include "LIST.h"

namespace nsUtil
{
/********************** pool *************************/
class MAP
{
	public:
		MAP();
		~MAP();
		ALIST * SET(KSTRING _szKey);
		ALIST * GET(KSTRING _szKey);
		void DEL(KSTRING _szKey);
		KUINT LEN();
	private:
		StlMap mMap;
};
class POOL
{
	public:
		class POOLOPTION
		{
			public:
				POOLOPTION();
				~POOLOPTION();
				KSTR DEBUGGING(KSTRING & _rDebug);
				KSTRING m_szName;
				KUINT m_nMax;
				KUINT m_nCheckNums;
				KUINT m_nCheckTimeMs;
				KUINT m_nAliveTimeSec;
				KUINT m_nQueueCheckMs;
		};
		class user : public StlObject
		{
			public:
				user(){}
				~user(){}
		};
		class POOLDATA
		{
			public:
				POOLDATA();
				~POOLDATA();
				KUINT & ID();
				ALIST::Kobj & SET(KCSTR _szKey);
				ALIST::Kobj & GET(KCSTR _szKey);
				void SETU(user * _pObj);
				user * GETU();
				void CLEAR();
				KBOOL DIFF(KUINT _unDiffT);
				KSTR  DEBUGGING(KSTRING & _rDebug);
				POOL * m_pOwner;
				KUINT m_unIndex;
				KUINT m_unTime;
				user * m_pUser;
				ALIST m_list;
		};
		POOL();
		~POOL();
		void INIT(POOL::POOLOPTION * _pOpt);
		POOLDATA * ALLOC();  
		POOLDATA * ALLOC_(KSTRING _szKey); 
		POOLDATA * GET(KUINT _nIdx);        
		POOLDATA * GET_(KSTRING _szKey);
		POOLDATA * SEQ(KUINT _nSeq);
		bool FREE(KUINT _nIdx);
		bool FREE_(KSTRING _szKey); 
		void CHECK();
		KUINT LENGTH();
		KUINT SIZE();
		virtual void GARBAGE(POOL::POOLDATA & _rData)=0;
	private:
		KUINT m_fnGetToken();
		const POOLOPTION * m_pOpt;
		IdMgr *m_pclsIdMgr;
		POOLDATA * m_arrId;
		KUINT m_unToken;
		MAP m_map;
		MAP m_mapUniq;
		KUINT m_unUniq;
};
}
#endif

