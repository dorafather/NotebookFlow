#include "EXEPARAM.h"
#include "EXECORE.h"

namespace nsUtil
{
ExeStmt::ExeStmt(StmtParser & _dsl)
{
	m_dsl = &_dsl;
}
ExeStmt::~ExeStmt()
{
}
bool ExeStmt::EXE(POOL::POOLDATA & _rPool, 
				RestMsg & _rcvMsg,
				KSTRING & _val)
{
	ALIST keyList;
	m_dsl->m_conv.LIST(keyList);
	for(KUINT i = 0;i<keyList.NUMS();i++)
	{
		KSTRING & item = keyList[i].VAL();
		if(item == "<")
		{
			item = "{";
		}
		else if(item == ">")
		{
			item  = "}";
		}
		else
		{
			KSTRING param1; KSTRING param2;
			KSTRING key = (KCSTR)item;
			BasicParser param; param.TOKEN()=".";
			param.PARSE((KCSTR)key);
			KSTRING buf;
			KCSTR rVal = ExeParam::PARAM(param.KEY(),param.VAL(),_rPool, _rcvMsg,buf);
			item  = rVal;
		}
	}
	_val = KNULL;
	m_dsl->m_conv.STR(keyList,_val);
	return true;
}
}
