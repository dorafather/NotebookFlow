#include "EXESCEN.h"
#include "INIPARSER.h"
#include "GPOLLING.h"
#include "FLOW.h"

namespace nsUtil
{
static void timeout(Gpolling::info * _info)
{
	ExeScen * pScen = (ExeScen*)_info->m_pOwner;
	if(pScen==NULL) return;
	if(pScen->m_sceCfg.checkchanged())
	{
		if(pScen->PARSE())
		{
			if(pScen->m_name == "rest")
			{
				Flow::OBJ().bootStrap();
			}
		}
		else
		{
			Flow::OBJ().Logging("Parsing Failed!");
		}
	}
}
ExeScen::ExeScen()
{
	m_bParse = true;
}
ExeScen::~ExeScen(){}
bool ExeScen::INIT(KCSTR _name, KCSTR _scePath)
{
	setkey(_name);
	m_name = _name;
	m_sceCfg.init(_scePath);
	m_sceCfg.enablecheckchanged();
	if(!PARSE())
	{
		printf("Parsing Failed!\r\n");
		return false;
	}
	Gpolling::setTimer(this,"sce", 1000, timeout);
	return true;
}
bool ExeScen::PARSE()
{
	m_lock.WLOCK();
	m_listState.clear();
	m_listProc.clear();
	m_listAct.clear();
	m_listStmt.clear();
	SceParser sce;
	KSTRING buf;
	buf = m_sceCfg.m_pszRawData;
	if(!sce.PARSE(buf))
	{
		Flow::Logging("[ExeScen::PARSE] scenario parse FAILED: %s\r\n", (KCSTR)sce.m_result);
		m_lock.UNLOCK();
		return false;
	}
	for(KUINT i=0;i<sce.NUMS();i++)
	{
		ClassParser & classObj = sce[i];
		if(IS_DSL_CLASS_STATE((KCSTR)classObj.m_oType))
		{
			StateParser * pSt = new StateParser;
			if(!pSt->PARSE(classObj))
			{
				Flow::Logging("[ExeScen::PARSE] StateParser(%s) parse FAILED: %s\r\n", (KCSTR)classObj.m_oName, (KCSTR)pSt->m_result);
				delete pSt;
				m_lock.UNLOCK();
				return false;
			}
			m_listState.pushback(pSt);
		}
		else if(IS_DSL_CLASS_PROC((KCSTR)classObj.m_oType))
		{
			ProcParser * pProc = new ProcParser;
			if(!pProc->PARSE(classObj))
			{
				Flow::Logging("[ExeScen::PARSE] ProcParser(%s) parse FAILED: %s\r\n", (KCSTR)classObj.m_oName, (KCSTR)pProc->m_result);
				delete pProc;
				m_lock.UNLOCK();
				return false;
			}
			m_listProc.pushback(pProc);
		}
		else if(IS_DSL_CLASS_ACT_SEND((KCSTR)classObj.m_oType)
			|| IS_DSL_CLASS_ACT_TIMER((KCSTR)classObj.m_oType) )
		{
			ActionParser * pAct = new ActionParser;
			if(!pAct->PARSE(classObj))
			{
				Flow::Logging("[ExeScen::PARSE] ActionParser(%s) parse FAILED: %s\r\n", (KCSTR)classObj.m_oName, (KCSTR)pAct->m_result);
				delete pAct;
				m_lock.UNLOCK();
				return false;
			}
			m_listAct.pushback(pAct);
		}
		else if(IS_DSL_CLASS_STMT((KCSTR)classObj.m_oType))
		{
			StmtParser * pStmt = new StmtParser;
			if(!pStmt->PARSE(classObj))
			{
				Flow::Logging("[ExeScen::PARSE] StmtParser(%s) parse FAILED: %s\r\n", (KCSTR)classObj.m_oName, (KCSTR)pStmt->m_result);
				delete pStmt;
				m_lock.UNLOCK();
				return false;
			}
			m_listStmt.pushback(pStmt);
		}
	}
	Flow::Logging("%s Parsing Success",(KCSTR)m_name);
	m_lock.UNLOCK();
	return true;
}
void ExeScen::JSON(RestMsg & _msg)
{
	m_lock.RLOCK();
	_msg.SET("scenario-name").VAL() = m_name;
	RestParam & stList = _msg.SET("state-list");
	INITARR(stList);
	Iterator itrSt;
	StateParser * pSt = (StateParser*)m_listState.next(itrSt);
	while(pSt)
	{
		RestParam & stItem = stList.ARR();
		pSt->JSON(stItem);
		pSt = (StateParser*)m_listState.next(itrSt);
	}
	RestParam & prList = _msg.SET("procedure-list");
	INITARR(prList);
	Iterator itrPr;
	ProcParser* pPr = (ProcParser*)m_listProc.next(itrPr);
	while(pPr)
	{
		RestParam & stItem = prList.ARR();
		pPr->JSON(stItem);
		pPr = (ProcParser*)m_listProc.next(itrPr);
	}
	RestParam & actList = _msg.SET("action-list");
	INITARR(actList);
	Iterator itrAct;
	ActionParser* pAct = (ActionParser*)m_listAct.next(itrAct);
	while(pAct)
	{
		RestParam & stItem = actList.ARR();
		pAct->JSON(stItem);
		pAct = (ActionParser*)m_listAct.next(itrAct);
	}
	RestParam & stmtList = _msg.SET("stmt-list");
	INITARR(stmtList);
	Iterator itrStmt;
	StmtParser* pStmt = (StmtParser*)m_listStmt.next(itrStmt);
	while(pStmt)
	{
		RestParam & stItem = stmtList.ARR();
		pStmt->JSON(stItem);
		pStmt = (StmtParser*)m_listStmt.next(itrStmt);
	}
	m_lock.UNLOCK();
}
void ExeScen::IMPORT(RestMsg & _msg)
{
	m_lock.WLOCK();
	m_listState.clear();
	m_listProc.clear();
	m_listAct.clear();
	m_listStmt.clear();
	m_name = _msg.GET("scenario-name").VAL();
	setkey((KCSTR)m_name);

	RestParam & stList = _msg.GET("state-list");
	for(KUINT i=0;i<stList.NUMS();i++)
	{
		StateParser * pSt = new StateParser;
		pSt->IMPORT(stList[i]);
		m_listState.pushback(pSt);
	}
	RestParam & prList = _msg.GET("procedure-list");
	for(KUINT i=0;i<prList.NUMS();i++)
	{
		ProcParser * pPr = new ProcParser;
		pPr->IMPORT(prList[i]);
		m_listProc.pushback(pPr);
	}
	RestParam & actList = _msg.GET("action-list");
	for(KUINT i=0;i<actList.NUMS();i++)
	{
		ActionParser * pAct = new ActionParser;
		pAct->IMPORT(actList[i]);
		m_listAct.pushback(pAct);
	}
	RestParam & stmtList = _msg.GET("stmt-list");
	for(KUINT i=0;i<stmtList.NUMS();i++)
	{
		StmtParser * pStmt = new StmtParser;
		pStmt->IMPORT(stmtList[i]);
		m_listStmt.pushback(pStmt);
	}
	KSTRING wbuf;
	STR_NOLOCK(wbuf);
	std::string decoded;
	deserialPath((KCSTR)wbuf, decoded);
	m_sceCfg.writefile(decoded.c_str(),(KUINT)decoded.length());
	m_lock.UNLOCK();
}
void ExeScen::STR(KSTRING & _buf)
{
	m_lock.RLOCK();
	STR_NOLOCK(_buf);
	m_lock.UNLOCK();
}
void ExeScen::STR_NOLOCK(KSTRING & _buf)
{
	Iterator itrSt;
	StateParser * pSt = (StateParser*)m_listState.next(itrSt);
	while(pSt)
	{
		pSt->STR(_buf);
		pSt = (StateParser*)m_listState.next(itrSt);
	}
	Iterator itrPr;
	ProcParser* pPr = (ProcParser*)m_listProc.next(itrPr);
	while(pPr)
	{
		pPr->STR(_buf);
		pPr = (ProcParser*)m_listProc.next(itrPr);
	}
	Iterator itrAct;
	ActionParser* pAct = (ActionParser*)m_listAct.next(itrAct);
	while(pAct)
	{
		pAct->STR(_buf);
		pAct = (ActionParser*)m_listAct.next(itrAct);
	}
	Iterator itrStmt;
	StmtParser* pStmt = (StmtParser*)m_listStmt.next(itrStmt);
	while(pStmt)
	{
		pStmt->STR(_buf);
		pStmt = (StmtParser*)m_listStmt.next(itrStmt);
	}
}
bool ExeScen::EXE(QTHREAD & _wk,
				POOL::POOLDATA & _rPool,
				RestMsg & _rcvMsg)
{
	m_lock.RLOCK();
	StateParser * pFindState = FINDSTATE((KCSTR)_rPool.GET(DEF_DSL_K_STATE_eng).VAL());
	if(pFindState)
	{
		ExeState state(*pFindState);
		bool bret = state.EXE(_wk, _rPool,_rcvMsg);
		m_lock.UNLOCK();
		return bret;
	}
	m_lock.UNLOCK();
	return false;
}
bool ExeScen::GOTO(KCSTR _stateName,
					QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _rcvMsg)
{
	if(IS_DSL_K_BREAK(_stateName))
	{
		_rPool.SET(DEF_DSL_K_LOOP_BREAK_eng).VAL() = "1";
		return true;
	}
	_rPool.SET(DEF_DSL_K_STATE_eng).VAL() = _stateName;
	return true;
}
StateParser * ExeScen::FINDSTATE(KCSTR _name)
{
	return (StateParser*)m_listState.find(_name);
}
ProcParser * ExeScen::FINDPROC(KCSTR _name)
{
	return (ProcParser*)m_listProc.find(_name);
}
ActionParser * ExeScen::FINDACTION(KCSTR _name)
{
	return (ActionParser*)m_listAct.find(_name);
}
StmtParser * ExeScen::FINDSTMT(KCSTR _name)
{
	return (StmtParser*)m_listStmt.find(_name);
}
bool ExeScen::EXEPROC(KCSTR _name, QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _rcvMsg)
{
	ProcParser * pFindProc = FINDPROC(_name);
	if(pFindProc)
	{
		ExeProc proc(*pFindProc);
		return proc.EXE(_wk, _rPool, _rcvMsg);
	}
	return false;
}
}
