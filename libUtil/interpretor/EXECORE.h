#ifndef EXE_CORE_H
#define EXE_CORE_H
#include "EXESCEN.h"
#include "INIFILE.h"

namespace nsUtil
{
class ExeCore
{
	public:
		class Session: public StlObject
		{
			public:
				Session();
				~Session();
				void FINISH(QTHREAD & _wk);
				bool exeCnt();
				void exeClear();
				POOL::POOLDATA * m_pMy;
				ALIST m_arrBin;
				RestParam m_arrObjs;
				RestParam m_appHdr;
				RestMsg m_subObj;
				KUINT m_callCnt;
		};
		ExeCore();
		~ExeCore();
		static ExeCore & OBJ();
		void INIT(KCSTR _sysPath);
		void ADD(KCSTR _name, KCSTR _scePath);
		void IMPORT(KCSTR _name, RestMsg & _msg);
		void JSON(KCSTR _name, RestMsg & _msg);
		void STR(KCSTR _name, KSTRING & _buf);
		bool EXE(QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _rcvMsg);
		bool GOTO(KCSTR _stateName,
					QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _rcvMsg);
		ExeScen * FINDSCEN(KCSTR _name);
		StateParser * FINDSTATE(POOL::POOLDATA & _rPool,KCSTR _name);
		ProcParser * FINDPROC(POOL::POOLDATA & _rPool,KCSTR _name);
		ActionParser * FINDACTION(POOL::POOLDATA & _rPool,KCSTR _name);
		StmtParser * FINDSTMT(POOL::POOLDATA & _rPool,KCSTR _name);
		KCSTR GETINI(KCSTR _category, KCSTR _key, KSTRING & _ret);
		bool EXEPROC(KCSTR _name, QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _rcvMsg);
		bool isMatchDomain(KCSTR _namespace, RestMsg & _req);
		bool isNamedDomain(KCSTR _namespace, KCSTR _domain);
		static ExeCore * m_pInst;
		bool m_bParse;
		KSTRING m_result;
		IniFileReader m_ini;
		StlList m_listSce;  
};
}
#endif
