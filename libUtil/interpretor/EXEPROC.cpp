#include "EXEPARAM.h"
#include "EXECORE.h"
#include "EXEPROC.h"
#include "EXECXIF.h"
#include "EXEFUNC.h"
#include "EXELOG.h"
#include "FLOW.h"

namespace nsUtil
{
ExeProc::ExeProc(ProcParser & _dsl)
{
	m_dsl = &_dsl;
}
ExeProc::~ExeProc()
{
}
bool ExeProc::EXE(QTHREAD & _wk,
					POOL::POOLDATA & _rPool,
					RestMsg & _req)
{
	ExeCore::Session * pSes = (ExeCore::Session *)_rPool.GETU();
	if(pSes)
	{
		if(!pSes->exeCnt())
		{
			Flow::Logging("Too Many Function Call!");
			return false;
		}
	}
	bool bCond = false;
	bool bExe = false;
	ProcParser & doc = *m_dsl;
	ExeCore & exe = ExeCore::OBJ();
	for(KUINT i=0;i<doc.NUMS();i++)
	{
		ProcParser::Procedure & proc = doc[i];
		KSTRING & ifType = proc.m_cond.TYPE();
		if(IS_DSL_K_IF((KCSTR)ifType) || IS_DSL_K_ELSEIF((KCSTR)ifType))
		{
			ExeCxIf cx(proc.m_cond);
			bCond = cx.EXE(_wk,_rPool,_req);
			if(bCond)
			{
				bExe = true;
			}
			else
			{
				bExe = false;
			}
		}
		else if(IS_DSL_K_ELSE((KCSTR)ifType))
		{
			if(!bCond) bExe = true;
			else bExe = false;
			bCond = false;
		}
		else
		{
			return false;
		}
		if(!bExe) continue;
		for(KUINT j=0;j<proc.NUMS();j++)
		{
			ProcParser::Procedure::ProcExe & procexe = proc[j];
			KSTRING & exeType = procexe.m_type;
			if(exeType == "FUNC")
			{
				ExeFunc func(procexe.m_func);
				if(!func.EXE(_wk,_rPool,_req)) return false;
			}
			else if(exeType == "LOG")
			{
				ExeLog log(procexe.m_log);
				if(!log.EXE(_wk, _rPool,_req)) return false;
			}
			else
			{
				KSTRING & procType = procexe.m_exe.KEY();
				KSTRING & procName = procexe.m_exe.VAL();
				if(IS_DSL_CLASS_ACT_SEND((KCSTR)procType) ||
				IS_DSL_CLASS_ACT_TIMER((KCSTR)procType))
				{
					ActionParser * pFindAct = ExeCore::OBJ().FINDACTION(_rPool,(KCSTR)procName);
					if(pFindAct)
					{
						ExeAction action(*pFindAct);
						if(!action.EXE(_wk,_rPool,_req)) return false;
					}
					else
					{
						return false;
					}
				}
				else if(IS_DSL_K_GOTO((KCSTR)procType))
				{
					exe.GOTO(procName,_wk,_rPool, _req);
					return true;
				}
				else if(IS_DSL_CLASS_PROC((KCSTR)procType))
				{
					ProcParser * pFindProc = ExeCore::OBJ().FINDPROC(_rPool,(KCSTR)procName);
					if(pFindProc)
					{
						ExeProc subProc(*pFindProc);
						if(!subProc.EXE(_wk,_rPool,_req)) return false;						
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
		}
		return true;
	}
	return true;
}
}
