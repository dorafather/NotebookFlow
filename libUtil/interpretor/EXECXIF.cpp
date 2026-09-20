#include "EXEPARAM.h"
#include "EXECORE.h"
#include "EXECXIF.h"
#include "EXEIF.h"

namespace nsUtil
{
ExeCxIf::ExeCxIf(CxIfParser & _dsl)
{
	m_dsl = &_dsl;
}
ExeCxIf::~ExeCxIf()
{
}
bool ExeCxIf::EXE(QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _req)
{
	CxIfParser & cx = *m_dsl;
	bool bCond = false;
	for(KUINT i=0;i<cx.NUMS();i++)
	{
		IfParser & ifP = cx[i];
		KSTRING & iftype = ifP.m_if;
		if(IS_DSL_K_OR((KCSTR)iftype))
		{
			if(bCond) break;
		}
		else if(IS_DSL_K_AND((KCSTR)iftype))
		{
			if(!bCond) break;
		}
		ExeIf exeIf(ifP);
		bCond = exeIf.EXE(_wk, _rPool,_req);
	}
	return bCond;
}
}
