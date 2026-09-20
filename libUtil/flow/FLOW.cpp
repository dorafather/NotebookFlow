#include "FLOW.h"
#include "DTIME.h"

namespace nsUtil
{
Flow * Flow::m_pInst = NULL;
static QTHREAD::QOPTION s_opt;
static void printMsg(bool _bSnd, KCSTR _msg)
{
	TIME dTime;
	KSTRING dir;
	if(_bSnd) dir = "<--- FLOW";
	else dir = "---> FLOW";
	printf("FLOW %04u.%02u.%02u:%02u.%02u.%02u-%03u> %s\n%s\n",
								dTime.Year(),
								dTime.Mon(),
								dTime.Day(),
								dTime.Hour(),
								dTime.Min(),
								dTime.Sec(),
								dTime.Msec(),
								(KCSTR)dir,
								(KCSTR)_msg);	
}
Flow::Flow()
{
	m_pInst = this;
	m_pmain = NULL;
}
Flow::~Flow()
{
	if(m_pmain) delete m_pmain;
}
Flow & Flow::OBJ()
{
	return *m_pInst;
}
void Flow::applyAddrConfig(RestMsg & _msg)
{
	ExeCore & core = ExeCore::OBJ();
	core.m_ini.IMPORT(_msg);
}
void Flow::applyFlowConfig(RestMsg & _msg)
{
	ExeCore & core = ExeCore::OBJ();
	core.IMPORT("user",  _msg);
}
void Flow::applyRestConfig(RestMsg & _msg)
{
	ExeCore & core = ExeCore::OBJ();
	core.IMPORT("rest",  _msg);
}
void Flow::queryAddrConfig(RestMsg & _msg)
{
	ExeCore & core = ExeCore::OBJ();
	core.m_ini.JSON(_msg);
}
void Flow::queryFlowConfig(RestMsg & _msg)
{
	ExeCore & core = ExeCore::OBJ();
	core.JSON("user", _msg);
}
void Flow::queryRestConfig(RestMsg & _msg)
{
	ExeCore & core = ExeCore::OBJ();
	core.JSON("rest", _msg);
}
void Flow::bootStrap()
{
	RestMsg bootMsg;
	bootMsg.SET(DEF_ACTION_EVENT).VAL() = STR_DSL_K_START("kor");
	Flow::OBJ().PUT("", (KCSTR)bootMsg.STR());
}
void Flow::RUNFLOW(int argc, char ** argv)
{
	m_pmain = new MainP(argc, argv);
	ExeCore & core = ExeCore::OBJ();
	core.INIT("./addr.ini");
	core.ADD("user", "./user.sce");
	core.ADD("rest", "./rest.sce");
	m_gPool.SET(DEF_SCE_NAME).VAL() = "rest";
	m_gPool.SET(DEF_SCE_ID).VAL() = "NonProto"; 
	m_gPool.SET(DEF_DSL_K_STATE_eng).VAL() = STR_DSL_K_INIT("kor");
	m_pInst->RUN(&s_opt,0);
	bootStrap();
	m_pmain->m_fnRun();
}
void Flow::PUT(KCSTR _addr, KCSTR _msg)
{
	ARG * arg = new ARG;
	arg->SET(API_P_EVENT).VAL() = API_E_SOCKET_RCV;
	arg->SET(API_P_IF_NAME).VAL() = API_I_SCEIF;
	arg->SET(API_P_PACKET).VAL() = _msg;
	arg->SET(DEF_DSL_K_ADDR_eng).VAL() = _addr;
	ADD(arg);
}
void Flow::NOTIFY(QTHREAD & _wk, POOL::POOLDATA & _rPool, RestMsg & _msg)
{
	printMsg(true,_msg.STR());
	ACTION(_wk,_rPool,_msg);
}
void Flow::PROC(ARG & _arg)
{
	proc(*this,_arg);
}
void Flow::Logging(	KCSTR _pszFormat, ...)
{
	TIME dTime;
	char tmp[5120];
	memset(tmp,0x00,sizeof(tmp));
	va_list ap;
	va_start(ap, _pszFormat);
	vsnprintf(tmp, sizeof(tmp)-1,_pszFormat, ap);
	va_end(ap);
	printf("FLOW %04u.%02u.%02u:%02u.%02u.%02u-%03u> %s\n",
								dTime.Year(),
								dTime.Mon(),
								dTime.Day(),
								dTime.Hour(),
								dTime.Min(),
								dTime.Sec(),
								dTime.Msec(),
								(KCSTR)tmp);	
}
void Flow::GARBAGE(POOL::POOLDATA & _rData)
{
	ExeCore::Session * pSes = (ExeCore::Session*)_rData.GETU();
	if(pSes)
	{
		RestMsg apiMsg;
		apiMsg.SET(DEF_SCE_EVENT).VAL() = DEF_SCE_STOP_REQ;
		apiMsg.SET(DEF_SCE_ID).VAL() = (KCSTR)_rData.GET(DEF_SCE_ID).VAL();
		apiMsg.SET(DEF_AS_ID).VAL() = (KCSTR)_rData.GET(DEF_AS_ID).VAL();
		apiMsg.SET("REASON").VAL() = "GARBAGE";
		apiMsg.SET("RESULT").VAL() = "1";
		if(pSes->m_appHdr.NUMS()>0)
		{
			RestParam & dstAppHdr = apiMsg.SET("app-hdr");
			dstAppHdr = pSes->m_appHdr;
		}
		printMsg(true,(KCSTR)apiMsg.STR());
		ACTION(*this,_rData,apiMsg);
	}	
	termSession((QTHREAD&)*this,_rData,"garbage");
}
void Flow::termSession(QTHREAD & _wk, POOL::POOLDATA  & _rPool,
							KCSTR _reason)
{
	ExeCore::Session * pSes = (ExeCore::Session*)_rPool.GETU();
	if(pSes)
	{
		RestMsg termMsg;
		termMsg.SET(DEF_SCE_EVENT).VAL() = DEF_SCE_TERM_INFO;
		termMsg.SET(DEF_ACTION_EVENT).VAL() = STR_DSL_K_TERM((KCSTR)_rPool.GET("LANG").VAL());
		termMsg.SET(DEF_SCE_ID).VAL() = (KCSTR)_rPool.GET(DEF_SCE_ID).VAL();
		termMsg.SET(DEF_AS_ID).VAL() = (KCSTR)_rPool.GET(DEF_AS_ID).VAL();
		termMsg.SET(DEF_DSL_K_ADDR_eng).VAL() = _rPool.GET(DEF_DSL_K_ADDR_eng).VAL(); 
		termMsg.SET(DEF_SCE_TERM_REASON).VAL() = _reason;
		ExeCore::OBJ().EXE(_wk,_rPool, termMsg);
		pSes->FINISH(_wk);
	}
}
void Flow::termTimer(QTHREAD & _wk, POOL::POOLDATA  & _rPool, KCSTR _id)
{
	ARG argT;
	argT.SET(API_P_EVENT).VAL() = API_E_TIMER;
	argT.SET(API_P_IF_NAME).VAL() = API_I_SCEIF;
	argT.SET(API_P_POOL_UNIQ).VAL() = (KUINT)_rPool.GET(API_P_POOL_UNIQ).VAL();
	argT.SET(API_P_TIMER_TYPE).VAL() = "SCE_TERM_TIMER";
	argT.SET(DEF_SCE_ID).VAL() = _id;
	argT.SET(API_P_TIME).VAL() = 1000;
	_wk.SETTIMER(argT);
}
void Flow::procTimeOut(QTHREAD & _wk, ARG & _arg, RestMsg & _rcvMsg)
{
	RestParam & srcAppHdr = _rcvMsg.GET("app-hdr");
	KSTRING szKey; 
	if(_arg.GET(API_P_TIMER_TYPE).VAL() == "SCE_TIMER")
	{
		szKey = (KSTR)_rcvMsg.GET(DEF_SCE_ID).VAL();
		if(szKey == "NonProto")
		{
			procNonTimeOut(_wk,_arg,_rcvMsg);
			return;
		}
		_rcvMsg.SET(DEF_SCE_EVENT).VAL() = DEF_SCE_ACTION;
		POOL::POOLDATA * pPool = NULL;
		if(KSTRING::m_fnStrnCmp((KCSTR)szKey,"IC", 2)==0)
		{
			pPool = _wk.GET_((KCSTR)szKey);
		}
		else
		{
			pPool = _wk.GET_((KCSTR)szKey);
		}
		if(pPool==NULL)
		{
			return;
		}
		ExeCore::Session * pSes = (ExeCore::Session*)pPool->GETU();
		if(pSes==NULL) 
		{
			return;
		}
		if(pPool->GET(API_P_POOL_UNIQ).VAL() != 
					(KCSTR)_arg.GET(API_P_POOL_UNIQ).VAL())
		{
			return;
		}
		ExeCore::OBJ().EXE(_wk, *pPool,_rcvMsg);
		if(IS_DSL_K_STOP((KCSTR)pPool->GET(DEF_DSL_K_STATE_eng).VAL() ))
		{
			RestMsg rspMsg;
			rspMsg.SET(DEF_SCE_EVENT).VAL() = DEF_SCE_STOP_REQ;
			rspMsg.SET(DEF_SCE_ID).VAL() = (KCSTR)szKey;
			rspMsg.SET(DEF_AS_ID).VAL() = (KCSTR)_rcvMsg.SET(DEF_AS_ID).VAL();
			rspMsg.SET(DEF_DSL_K_ADDR_eng).VAL() = pPool->GET(DEF_DSL_K_ADDR_eng).VAL(); 
			rspMsg.SET("RESULT").VAL() = "0";
			rspMsg.SET("REASON").VAL() = "Success";
			if(srcAppHdr.NUMS()>0)
			{
				RestParam & dstAppHdr = rspMsg.SET("app-hdr");
				dstAppHdr = srcAppHdr;
			}
			printMsg(true,(KCSTR)rspMsg.STR());
			ACTION(_wk,*pPool,rspMsg);
			termTimer(_wk,*pPool, (KCSTR)szKey);
		}
	}
	else if(_arg.GET(API_P_TIMER_TYPE).VAL() == "SCE_TERM_TIMER")
	{
		szKey = (KSTR)_arg.GET(DEF_SCE_ID).VAL();
		POOL::POOLDATA * pPool = NULL;
		pPool = _wk.GET_((KCSTR)szKey);
		if(pPool==NULL)
		{
			return;
		}
		if(pPool->GET(API_P_POOL_UNIQ).VAL() != 
					(KCSTR)_arg.GET(API_P_POOL_UNIQ).VAL())
		{
			return;
		}
		else
		{
			termSession(_wk, *pPool,"normal");
			_wk.FREE_((KCSTR)szKey);
		}
	}
}
void Flow::procStartReq(QTHREAD & _wk, ARG & _arg, RestMsg & _rcvMsg)
{
	RestParam & srcAppHdr = _rcvMsg.GET("app-hdr");
	KSTRING szKey; szKey.PRINT("IC-%s",(KCSTR)_rcvMsg.SET(DEF_AS_ID).VAL());
	POOL::POOLDATA * pPool = _wk.ALLOC_((KCSTR)szKey);									
	if(pPool==NULL)
	{
		RestMsg rspMsg;
		rspMsg.SET(DEF_SCE_EVENT).VAL() = DEF_SCE_START_RSP;
		rspMsg.SET(DEF_SCE_ID).VAL() = (KCSTR)szKey;
		rspMsg.SET(DEF_AS_ID).VAL() = (KCSTR)_rcvMsg.SET(DEF_AS_ID).VAL();
		rspMsg.SET(DEF_DSL_K_ADDR_eng).VAL() = _arg.GET(DEF_DSL_K_ADDR_eng).VAL();
		rspMsg.SET("RESULT").VAL() = "1";
		rspMsg.SET("REASON").VAL() = "Alloc Failed, dup ID";
		POOL::POOLDATA lPool;
		if(_rcvMsg.GET("reply-to").VAL().LENGTH()>0)
		{
			lPool.SET("REPLY-TO").VAL() = (KCSTR)_rcvMsg.GET("reply-to").VAL();
		}
		if(srcAppHdr.NUMS()>0)
		{
			RestParam & dstAppHdr = rspMsg.SET("app-hdr");
			dstAppHdr = srcAppHdr;
		}
		printMsg(true,(KCSTR)rspMsg.STR());
		ACTION(_wk,lPool ,rspMsg);
		return;
	}
	ExeCore::Session * pNew = new ExeCore::Session;
	pPool->SETU((POOL::user*)pNew);
	pNew->m_pMy = pPool;
	pPool->SET(DEF_SCE_FN_CNT).VAL() = 1;
	pPool->SET(DEF_SCE_ID).VAL() = (KCSTR)szKey;
	pPool->SET(DEF_AS_ID).VAL() = (KCSTR)_rcvMsg.SET(DEF_AS_ID).VAL();
	pPool->SET("WORKER-INDEX").VAL() = _wk.IDX();
	pPool->SET("POOL-INDEX").VAL() = pPool->ID();
	if(srcAppHdr.NUMS()>0)
	{
		pNew->m_appHdr = srcAppHdr;
	}
	if(_rcvMsg.GET(DEF_SCE_NAME).VAL().LENGTH()>0)
	{
		pPool->SET(DEF_SCE_NAME).VAL() = (KCSTR)_rcvMsg.GET(DEF_SCE_NAME).VAL();
	}
	else if(_rcvMsg.GET("sce_name").VAL().LENGTH()>0)
	{
		pPool->SET(DEF_SCE_NAME).VAL() = (KCSTR)_rcvMsg.GET("sce_name").VAL();
	}
	else
	{
		pPool->SET(DEF_SCE_NAME).VAL() = "user";
	}
	pPool->SET("LANG").VAL() ="kor";
	pPool->SET(DEF_DSL_K_STATE_eng).VAL() = STR_DSL_K_INIT("kor");
	pPool->SET(DEF_DSL_K_ADDR_eng).VAL() = _arg.GET(DEF_DSL_K_ADDR_eng).VAL(); 
	pPool->SET(DEF_DSL_K_ADDR_kor).VAL() = _arg.GET(DEF_DSL_K_ADDR_eng).VAL(); 
	if(_rcvMsg.GET("reply-to").VAL().LENGTH()>0)
	{
		pPool->SET("REPLY-TO").VAL() = (KCSTR)_rcvMsg.GET("reply-to").VAL();
	}
	if(ExeCore::OBJ().m_bParse==false)
	{
		RestMsg rspMsg;
		rspMsg.SET(DEF_SCE_EVENT).VAL() = DEF_SCE_STOP_REQ;
		rspMsg.SET(DEF_SCE_ID).VAL() = (KCSTR)szKey;
		rspMsg.SET(DEF_AS_ID).VAL() = (KCSTR)_rcvMsg.SET(DEF_AS_ID).VAL();
		rspMsg.SET(DEF_DSL_K_ADDR_eng).VAL() = _arg.GET(DEF_DSL_K_ADDR_eng).VAL(); 
		rspMsg.SET("RESULT").VAL() = "1";
		rspMsg.SET("REASON").VAL() = "scenario syntax error";
		if(srcAppHdr.NUMS()>0)
		{
			RestParam & dstAppHdr = rspMsg.SET("app-hdr");
			dstAppHdr = srcAppHdr;
		}
		ACTION(_wk,*pPool,rspMsg);
		printMsg(true,(KCSTR)rspMsg.STR());
		_wk.FREE_((KCSTR)szKey);
		return;
	}
	RestMsg rspMsg;
	rspMsg.SET(DEF_SCE_EVENT).VAL() = DEF_SCE_START_RSP;
	rspMsg.SET(DEF_SCE_ID).VAL() = (KCSTR)szKey;
	rspMsg.SET(DEF_AS_ID).VAL() = (KCSTR)_rcvMsg.SET(DEF_AS_ID).VAL();
	rspMsg.SET(DEF_DSL_K_ADDR_eng).VAL() = _arg.GET(DEF_DSL_K_ADDR_eng).VAL(); 
	rspMsg.SET("RESULT").VAL() = "0";
	rspMsg.SET("REASON").VAL() = "Success";
	if(srcAppHdr.NUMS()>0)
	{
		RestParam & dstAppHdr = rspMsg.SET("app-hdr");
		dstAppHdr = srcAppHdr;
	}
	ACTION(_wk,*pPool,rspMsg);
	printMsg(true,(KCSTR)rspMsg.STR());
	ExeCore::OBJ().EXE(_wk, *pPool, _rcvMsg);
	if(IS_DSL_K_STOP((KCSTR)pPool->GET(DEF_DSL_K_STATE_eng).VAL()))
	{
		RestMsg rspMsg;
		rspMsg.SET(DEF_SCE_EVENT).VAL() = DEF_SCE_STOP_REQ;
		rspMsg.SET(DEF_SCE_ID).VAL() = (KCSTR)szKey;
		rspMsg.SET(DEF_AS_ID).VAL() = (KCSTR)_rcvMsg.SET(DEF_AS_ID).VAL();
		rspMsg.SET(DEF_DSL_K_ADDR_eng).VAL() = _arg.GET(DEF_DSL_K_ADDR_eng).VAL(); 
		rspMsg.SET("RESULT").VAL() = "0";
		rspMsg.SET("REASON").VAL() = "Sucess";
		if(srcAppHdr.NUMS()>0)
		{
			RestParam & dstAppHdr = rspMsg.SET("app-hdr");
			dstAppHdr = srcAppHdr;
		}
		printMsg(true,(KCSTR)rspMsg.STR());
		ACTION(_wk,*pPool,rspMsg);
		termTimer(_wk,*pPool, (KCSTR)szKey);
	}
}
void Flow::procStartRsp(QTHREAD & _wk, ARG & _arg, RestMsg & _rcvMsg)
{
	KSTRING skey; skey = (KSTR)_rcvMsg.SET(DEF_SCE_ID).VAL();
	_wk.GET_((KCSTR)skey);
}
void Flow::procStopReq(QTHREAD & _wk, ARG & _arg, RestMsg & _rcvMsg)
{
	KSTRING skey; skey = (KSTR)_rcvMsg.SET(DEF_SCE_ID).VAL();
	POOL::POOLDATA * pPool = NULL;
	pPool = _wk.GET_((KCSTR)skey);
	RestMsg rspMsg;
	rspMsg.SET(DEF_SCE_EVENT).VAL() = DEF_SCE_STOP_RSP;
	rspMsg.SET(DEF_SCE_ID).VAL() = (KCSTR)skey;
	rspMsg.SET(DEF_AS_ID).VAL() = (KCSTR)_rcvMsg.SET(DEF_AS_ID).VAL();
	rspMsg.SET(DEF_DSL_K_ADDR_eng).VAL() = _arg.GET(DEF_DSL_K_ADDR_eng).VAL(); 
	if(pPool==NULL)
	{
		rspMsg.SET("RESULT").VAL() = "1";
		rspMsg.SET("REASON").VAL() = "not found session";
		if(_rcvMsg.GET("app-hdr").NUMS()>0)
		{
			RestParam & dstAppHdr = rspMsg.SET("app-hdr");
			dstAppHdr = _rcvMsg.GET("app-hdr");
		}
		POOL::POOLDATA mpol;
		printMsg(true,(KCSTR)rspMsg.STR());
		ACTION(_wk, mpol, rspMsg);
		return;
	}
	ExeCore::Session * pSes = (ExeCore::Session*)pPool->GETU();
	if(pSes==NULL)
	{
		rspMsg.SET("RESULT").VAL() = "1";
		rspMsg.SET("REASON").VAL() = "not found session";
		if(_rcvMsg.GET("app-hdr").NUMS()>0)
		{
			RestParam & dstAppHdr = rspMsg.SET("app-hdr");
			dstAppHdr = _rcvMsg.GET("app-hdr");
		}
		POOL::POOLDATA mpol;
		printMsg(true,(KCSTR)rspMsg.STR());
		ACTION(_wk, mpol, rspMsg);
		return;
	}
	ExeCore::OBJ().EXE(_wk, *pPool, _rcvMsg);
	rspMsg.SET("RESULT").VAL() = "0";
	rspMsg.SET("REASON").VAL() = "Success";
	if(pSes->m_appHdr.NUMS()>0)
	{
		RestParam & dstAppHdr = rspMsg.SET("app-hdr");
		dstAppHdr = pSes->m_appHdr;
	}
	printMsg(true,(KCSTR)rspMsg.STR());
	ACTION(_wk,*pPool,rspMsg);
	termTimer(_wk,*pPool, (KCSTR)skey);
}
void Flow::procStopRsp(QTHREAD & _wk, ARG & _arg, RestMsg & _rcvMsg)
{
	KSTRING skey; skey = (KSTR)_rcvMsg.SET(DEF_SCE_ID).VAL();
	POOL::POOLDATA * pPool = NULL;
	pPool = _wk.GET_((KCSTR)skey);
	if(pPool==NULL)
	{
		return;
	}
}
void Flow::procAction(QTHREAD & _wk, ARG & _arg, RestMsg & _rcvMsg)
{
	KSTRING skey; skey = (KSTR)_rcvMsg.SET(DEF_SCE_ID).VAL();
	POOL::POOLDATA * pPool = NULL;
	pPool = _wk.GET_((KCSTR)skey);
	if(pPool==NULL)
	{
		return;
	}
	ExeCore::Session * pSes = (ExeCore::Session*)pPool->GETU();
	if(pSes==NULL)
	{
		return;
	}
	if(ExeCore::OBJ().m_bParse==false)
	{
		RestMsg rspMsg;
		rspMsg.SET(DEF_SCE_EVENT).VAL() = DEF_SCE_STOP_REQ;
		rspMsg.SET(DEF_SCE_ID).VAL() = (KCSTR)skey;
		rspMsg.SET(DEF_AS_ID).VAL() = (KCSTR)_rcvMsg.SET(DEF_AS_ID).VAL();
		rspMsg.SET(DEF_DSL_K_ADDR_eng).VAL() = _arg.GET(DEF_DSL_K_ADDR_eng).VAL(); 
		rspMsg.SET("RESULT").VAL() = "1";
		rspMsg.SET("REASON").VAL() = "scenario syntax error";
		if(pSes->m_appHdr.NUMS()>0)
		{
			RestParam & dstAppHdr = rspMsg.SET("app-hdr");
			dstAppHdr = pSes->m_appHdr;
		}
		printMsg(true,(KCSTR)rspMsg.STR());
		ACTION(_wk,*pPool,rspMsg);
		termTimer(_wk,*pPool, (KCSTR)skey);
		return;
	}
	ExeCore::OBJ().EXE(_wk, *pPool, _rcvMsg);
	if( IS_DSL_K_STOP((KCSTR)pPool->GET(DEF_DSL_K_STATE_eng).VAL()))
	{
		RestMsg rspMsg;
		rspMsg.SET(DEF_SCE_EVENT).VAL() = DEF_SCE_STOP_REQ;
		rspMsg.SET(DEF_SCE_ID).VAL() = (KCSTR)skey;
		rspMsg.SET(DEF_AS_ID).VAL() = (KCSTR)_rcvMsg.SET(DEF_AS_ID).VAL();
		rspMsg.SET(DEF_DSL_K_ADDR_eng).VAL() = _arg.GET(DEF_DSL_K_ADDR_eng).VAL(); 
		rspMsg.SET("RESULT").VAL() = "0";
		rspMsg.SET("REASON").VAL() = "Success";
		if(pSes->m_appHdr.NUMS()>0)
		{
			RestParam & dstAppHdr = rspMsg.SET("app-hdr");
			dstAppHdr = pSes->m_appHdr;
		}
		printMsg(true,(KCSTR)rspMsg.STR());
		ACTION(_wk,*pPool,rspMsg);
		termTimer(_wk,*pPool, (KCSTR)skey);
	}
}
void Flow::procNonTimeOut(QTHREAD & _wk, ARG & _arg, RestMsg & _rcvMsg)
{
	ExeCore::Session * pSes = (ExeCore::Session*)m_gPool.GETU();
	if(pSes==NULL)
	{
		return;
	}
	if(ExeCore::OBJ().m_bParse==false)
	{
		return;
	}
	if(m_gPool.GET("UNIQ_ID").VAL() != (KCSTR)_arg.GET("UNIQ_ID").VAL())
	{
		Logging("TimeOut Mismatch Id %s, %s",(KCSTR)m_gPool.GET("UNIQ_ID").VAL(),
									(KCSTR)_arg.GET("UNIQ_ID").VAL());
		return;
	}
	ExeCore::OBJ().EXE(_wk, m_gPool,_rcvMsg);
	if( IS_DSL_K_STOP((KCSTR)m_gPool.GET(DEF_DSL_K_STATE_eng).VAL()))
	{
		m_gPool.CLEAR();
	}
}
static void s_fnMoveTopKey_(RestMsg & _msg, KCSTR _kor, KCSTR _eng)
{
	RestParam & src = _msg.GET(_kor);
	if(src.EMPTY()) return;
	_msg.SET(_eng).VAL() = src.VAL();
	_msg.DEL(_kor);
}
void Flow::procNonProto(QTHREAD & _wk, ARG & _arg, RestMsg & _rcvMsg)
{
	if(_rcvMsg.GET(DEF_ACTION_EVENT).VAL() == DEF_DSL_K_START_kor)
	{
		m_gPool.CLEAR();
		ExeCore::Session * pSes = new ExeCore::Session;
		m_gPool.SETU((POOL::user*)pSes);
		m_gPool.SET(DEF_SCE_NAME).VAL() = "rest";
		m_gPool.SET(DEF_SCE_ID).VAL() = "NonProto"; 
		m_gPool.SET(DEF_DSL_K_STATE_eng).VAL() = STR_DSL_K_INIT("kor");
		m_gPool.SET("UNIQ_ID").VAL() = TIME::getTimeSec();
	}
	s_fnMoveTopKey_(_rcvMsg,DEF_DSL_K_METHOD_eng,
							DEF_DSL_K_METHOD_kor);
	ExeCore::OBJ().EXE(_wk, m_gPool, _rcvMsg);
	if( IS_DSL_K_STOP((KCSTR)m_gPool.GET(DEF_DSL_K_STATE_eng).VAL()))
	{
		m_gPool.CLEAR();
	}
}
void Flow::proc(QTHREAD & _wk, ARG & _arg)
{
	KSTRING & pk = _arg.GET(API_P_PACKET).VAL();
	RestMsg msg; 
	if(pk.LENGTH() > 0)
	{
		msg.PARSE((KSTR)pk);
	}
	if(_arg.GET(API_P_EVENT).VAL() == API_E_TIMER)
	{
		Logging("TimeOut timer-type:%s time-ms:%s",
							(KCSTR)_arg.GET(API_P_TIMER_TYPE).VAL(),
							(KCSTR)_arg.GET(API_P_TIME).VAL());
		printMsg(false,(KCSTR)pk);
		procTimeOut(_wk,_arg, msg);
	}
	else if(_arg.GET(API_P_EVENT).VAL() == API_E_SOCKET_RCV)
	{
		printMsg(false,(KCSTR)pk);
		if(msg.SET(DEF_SCE_EVENT).VAL() == DEF_SCE_START_REQ)
		{
			procStartReq(_wk,_arg, msg);
		}
		else if(msg.SET(DEF_SCE_EVENT).VAL() == DEF_SCE_START_RSP)
		{
			procStartRsp(_wk,_arg, msg);
		}
		else if(msg.SET(DEF_SCE_EVENT).VAL() == DEF_SCE_STOP_REQ)
		{
			procStopReq(_wk,_arg, msg);
		}
		else if(msg.SET(DEF_SCE_EVENT).VAL() == DEF_SCE_STOP_RSP)
		{
			procStopRsp(_wk,_arg, msg);
		}
		else if(msg.SET(DEF_SCE_EVENT).VAL() == DEF_SCE_ACTION)
		{
			procAction(_wk,_arg, msg);
		}
		else
		{
			procNonProto(_wk,_arg, msg);
		}
	}
}

}
