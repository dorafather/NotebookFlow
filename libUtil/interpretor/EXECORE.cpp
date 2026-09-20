#include "EXECORE.h"
#include "INIPARSER.h"
#include "GPOLLING.h"

namespace nsUtil
{
ExeCore::Session::Session()
{
	m_pMy = NULL;m_callCnt=0;
}
ExeCore::Session::~Session()
{
}
void ExeCore::Session::FINISH(QTHREAD & _wk)
{
}
bool ExeCore::Session::exeCnt()
{
	if(m_callCnt >= 200) return false;
	m_callCnt++;
	return true;
}
void ExeCore::Session::exeClear()
{
	m_callCnt = 0;
}
ExeCore * ExeCore::m_pInst = NULL;
ExeCore::ExeCore()
{
	m_bParse = true;
}
ExeCore::~ExeCore(){}
void ExeCore::INIT(KCSTR _sysPath)
{
	Gpolling::OBJ();
	m_ini.Read(_sysPath);
}
void ExeCore::ADD(KCSTR _name, KCSTR _scePath)
{
	ExeScen *pNew = new ExeScen;
	if(!pNew->INIT(_name,_scePath))
	{
		delete pNew;
		return;
	}
	m_listSce.pushback(pNew);
}
void ExeCore::IMPORT(KCSTR _name, RestMsg & _msg)
{
	ExeScen *pFind = FINDSCEN(_name);
	if(pFind == NULL) return;
	pFind->IMPORT(_msg);
}
void ExeCore::JSON(KCSTR _name, RestMsg & _msg)
{
	ExeScen *pFind = FINDSCEN(_name);
	if(pFind == NULL) return;
	pFind->JSON(_msg);
}
void ExeCore::STR(KCSTR _name, KSTRING & _buf)
{
	ExeScen *pFind = FINDSCEN(_name);
	if(pFind == NULL) return;
	pFind->STR(_buf);
}
ExeCore & ExeCore::OBJ()
{
	if(m_pInst==NULL)
	{
		m_pInst = new ExeCore;
	}
	return *m_pInst;
}
bool ExeCore::EXE(QTHREAD & _wk,
				POOL::POOLDATA & _rPool,
				RestMsg & _rcvMsg)
{
	KCSTR sceName = (KCSTR)_rPool.GET(DEF_SCE_NAME).VAL();
	ExeScen * pScen = FINDSCEN(sceName);
	if(pScen==NULL) return false;
	return pScen->EXE(_wk,_rPool,_rcvMsg);
}
bool ExeCore::GOTO(KCSTR _stateName,
					QTHREAD & _wk,
					POOL::POOLDATA & _rPool,
					RestMsg & _rcvMsg)
{
	KCSTR sceName = (KCSTR)_rPool.GET(DEF_SCE_NAME).VAL();
	ExeScen * pScen = FINDSCEN(sceName);
	if(pScen==NULL) return false;
	return pScen->GOTO(_stateName,_wk,_rPool,_rcvMsg);
}
ExeScen * ExeCore::FINDSCEN(KCSTR _name)
{
	return (ExeScen *)m_listSce.find(_name);
}
KCSTR ExeCore::GETINI(KCSTR _category, KCSTR _key, KSTRING & _ret)
{
	return m_ini.GET(_category,_key,_ret);
}
bool ExeCore::isMatchDomain(KCSTR _namespace, RestMsg & _req)
{
	RestParam & addr = _req.GET(DEF_DSL_K_ADDR_kor);
	if(addr.NUMS()==0) return false;
	RestParam & domain = addr.GET(DEF_DSL_K_DOMAIN_kor);
	if(domain.VAL().LENGTH()==0) return false;
	m_ini.m_lock.RLOCK();
	IniObject * pFind = m_ini.m_parser.findLine((KCSTR)_namespace, "domain");
	if(pFind == NULL)
	{
		m_ini.m_lock.UNLOCK();
		return false;
	}
	bool bMatch = STRNCMP((KCSTR)domain.VAL(),(KCSTR)pFind->VAL());
	m_ini.m_lock.UNLOCK();
	return bMatch;
}
bool ExeCore::isNamedDomain(KCSTR _namespace, KCSTR _domain)
{
	if(_domain == NULL || KSTRING::m_fnStrLen(_domain) == 0) return false;
	m_ini.m_lock.RLOCK();
	IniObject * pFind = m_ini.m_parser.findLine((KCSTR)_namespace, "domain");
	if(pFind == NULL)
	{
		m_ini.m_lock.UNLOCK();
		return false;
	}
	bool bMatch = STRNCMP(_domain,(KCSTR)pFind->VAL());
	m_ini.m_lock.UNLOCK();
	return bMatch;
}
StateParser * ExeCore::FINDSTATE(POOL::POOLDATA & _rPool,KCSTR _name)
{
	KCSTR sceName = (KCSTR)_rPool.GET(DEF_SCE_NAME).VAL();
	ExeScen * pScen = FINDSCEN(sceName);
	if(pScen==NULL) return NULL;
	return (StateParser*)pScen->m_listState.find(_name);
}
ProcParser * ExeCore::FINDPROC(POOL::POOLDATA & _rPool, KCSTR _name)
{
	KCSTR sceName = (KCSTR)_rPool.GET(DEF_SCE_NAME).VAL();
	ExeScen * pScen = FINDSCEN(sceName);
	if(pScen==NULL) return NULL;
	return (ProcParser*)pScen->m_listProc.find(_name);
}
ActionParser * ExeCore::FINDACTION(POOL::POOLDATA & _rPool,KCSTR _name)
{
	KCSTR sceName = (KCSTR)_rPool.GET(DEF_SCE_NAME).VAL();
	ExeScen * pScen = FINDSCEN(sceName);
	if(pScen==NULL) return NULL;
	return (ActionParser*)pScen->m_listAct.find(_name);
}
StmtParser * ExeCore::FINDSTMT(POOL::POOLDATA & _rPool,KCSTR _name)
{
	KCSTR sceName = (KCSTR)_rPool.GET(DEF_SCE_NAME).VAL();
	ExeScen * pScen = FINDSCEN(sceName);
	if(pScen==NULL) return NULL;
	return (StmtParser*)pScen->m_listStmt.find(_name);
}
bool ExeCore::EXEPROC(KCSTR _name, QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _rcvMsg)
{
	KCSTR sceName = (KCSTR)_rPool.GET(DEF_SCE_NAME).VAL();
	ExeScen * pScen = FINDSCEN(sceName);
	if(pScen==NULL) return false;
	return pScen->EXEPROC(_name,_wk, _rPool, _rcvMsg);
}
}
