#ifndef EXE_SCEN_H
#define EXE_SCEN_H
#include "RESTMSG.h"
#include "SESSION.h"
#include "QUEUETHREAD.h"
#include "EXESTMT.h"
#include "EXEPROC.h"
#include "EXEACTION.h"
#include "EXESTATE.h"
#include "SCEPARSER.h"
#include "INIPARSER.h"
#include "LOCK.h"
#include "DFILE.h"

namespace nsUtil
{
//=========> Msg Key Words
#define DEF_SCE_EVENT "SCE_EVENT"
#define DEF_ACTION_EVENT "ACTION_EVENT"
#define DEF_SCE_SVC_TYPE "SERVICE_TYPE"
#define DEF_AS_ID "AS_ID"
#define DEF_SCE_ID "SCE_ID"
#define DEF_PARAMS "PARAMS"
#define DEF_SCE_NAME "SCE_NAME"
#define DEF_SCE_FN_CNT "FN_CNT"

//==========> Msg Types
#define DEF_SCE_3RD_GEN    "GEN_REQ"         // <-- 3rd Gen Session
#define DEF_SCE_START_REQ "START_REQ"     // DIR DON'T CARE
#define DEF_SCE_START_RSP "START_RSP"      // DIR DON'T CARE
#define DEF_SCE_ACTION "ACTION"                // DIR DON'T CARE
#define DEF_SCE_STOP_REQ "STOP_REQ"        // DIR DON'T CARE
#define DEF_SCE_STOP_RSP "STOP_RSP"         // DIR DON'T CARE
#define DEF_SCE_CHG_REQ "CHG_REQ"
#define DEF_SCE_CHG_RSP "CHG_RSP"
#define DEF_SCE_TERM_INFO "TERM_INFO"
#define DEF_SCE_TERM_REASON "TERM_REASON"
class ExeScen : public StlObject
{
	public:
		ExeScen();
		~ExeScen();
		bool INIT(KCSTR _name, KCSTR _scePath);
		bool PARSE();
		void JSON(RestMsg & _msg);
		void STR(KSTRING & _buf); 
		void IMPORT(RestMsg & _msg);
		bool EXE(QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _rcvMsg);
		bool GOTO(KCSTR _stateName,
					QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _rcvMsg);
		StateParser * FINDSTATE(KCSTR _name);
		ProcParser * FINDPROC(KCSTR _name);
		ActionParser * FINDACTION(KCSTR _name);
		StmtParser * FINDSTMT(KCSTR _name);
		bool GETINI(KCSTR _name,KCSTR _category, KCSTR _key, KSTRING & _ret);
		bool EXEPROC(KCSTR _name, QTHREAD & _wk, POOL::POOLDATA & _rPool, RestMsg & _rcvMsg);
	private:
		void STR_NOLOCK(KSTRING & _buf);
	public:
		StlList m_listState;
		StlList m_listProc;
		StlList m_listAct;
		StlList m_listStmt;
		StlList m_listIni;
		bool m_bParse;
		KSTRING m_result;
		KSTRING m_name;
		FileReader m_sceCfg;
		MUTEX m_lock;
};
}
#endif
