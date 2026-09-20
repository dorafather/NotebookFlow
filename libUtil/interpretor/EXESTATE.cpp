#include "EXEPARAM.h"
#include "EXECORE.h"
#include "EXEPROC.h"

namespace nsUtil
{
ExeState::ExeState(StateParser & _dsl)
{
	m_dsl = &_dsl;
}
ExeState::~ExeState()
{
}
bool ExeState::EXE(QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _req)
{
	KSTRING & evName = _req.GET(DEF_ACTION_EVENT).VAL();
	if(evName.LENGTH()>0)
	{
		KSTRING evN = evName;
		_req.DEL(DEF_ACTION_EVENT);
		_req.SET(DEF_DSL_K_ACTION_EVENT_kor).VAL() = evN;
	}
	for(KUINT i=0;i<m_dsl->NUMS();i++)
	{
		StateParser::item & item = (*m_dsl)[i];
		if(item.m_name == "FLOW")
		{
			// ok
		}
		else
		{
			if(!ExeCore::OBJ().isMatchDomain((KCSTR)item.m_name , _req))
			{
				continue;
			}
		}
		RestParam & pVal = _req.GETR((KCSTR)item.m_param);
		if(item.m_event == (KCSTR)pVal.VAL())
		{
			ProcParser * pFindProc = ExeCore::OBJ().FINDPROC(_rPool,(KCSTR)item.m_proc);
			if(pFindProc)
			{
				ExeCore::Session * pSes = (ExeCore::Session *)_rPool.GETU();
				if(pSes)
				{
					pSes->exeClear();
				}
				ExeProc proc(*pFindProc);
				return proc.EXE(_wk,_rPool,_req);
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}
}
