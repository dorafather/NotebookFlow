#include <string>
#include <stdexcept>
#include "EXEPARAM.h"
#include "EXECORE.h"
#include "EXEFUNC.h"
#include "DTIME.h"

namespace nsUtil
{
ExeEntry g_ExeTable[EXE_TYPE_MAX] =
{
    { EXE_TYPE_SET,       &ExeFunc::EXE_SET     },
    { EXE_TYPE_SUM,       &ExeFunc::EXE_SUM     },
    { EXE_TYPE_MINUS,     &ExeFunc::EXE_MINUS   },
    { EXE_TYPE_DIV,       &ExeFunc::EXE_DIV     },
    { EXE_TYPE_MOD,       &ExeFunc::EXE_MOD     },
    { EXE_TYPE_CAT,       &ExeFunc::EXE_CAT     },
    { EXE_TYPE_STRSTR,    &ExeFunc::EXE_STRSTR  },
    { EXE_TYPE_STRNCMP,   &ExeFunc::EXE_STRNCMP },
    { EXE_TYPE_INSERT,    &ExeFunc::EXE_INSERT  },
    { EXE_TYPE_DELETE,    &ExeFunc::EXE_DELETE  },
    { EXE_TYPE_EXTRACT,   &ExeFunc::EXE_EXTRACT },
    { EXE_TYPE_TIME,      &ExeFunc::EXE_TIME    },
    { EXE_TYPE_CLOCK,     &ExeFunc::EXE_CLOCK   },
    { EXE_TYPE_PRINT,     &ExeFunc::EXE_PRINT   },
    { EXE_TYPE_DATE,      &ExeFunc::EXE_DATE    },
    { EXE_TYPE_CPH,       &ExeFunc::EXE_CPH       },
    { EXE_TYPE_LOOP,      &ExeFunc::EXE_LOOP      },
    { EXE_TYPE_OBJ,      &ExeFunc::EXE_OBJ      },
    { EXE_TYPE_ADDR,      &ExeFunc::EXE_OBJ      },
    { EXE_TYPE_SPLIT,      &ExeFunc::EXE_SPLIT      },
    { EXE_TYPE_WORDEX,      &ExeFunc::EXE_WORDEX      },
    { EXE_TYPE_WORDSUM,      &ExeFunc::EXE_WORDSUM      },
};
KCSTR str_insert(KSTRING & _src,KCSTR _insert, size_t _pos)
{
	if (_pos > _src.LENGTH()) return (KCSTR)_src;
	std::string result = (KCSTR)_src;
	result.insert(_pos, _insert);
	_src = result.c_str();
	return (KCSTR)_src;
}
KCSTR str_delete(KSTRING & _src, size_t _pos, size_t _len)
{
	if (_pos > _src.LENGTH()) return (KCSTR)_src;
	std::string result = (KCSTR)_src;
	result.erase(_pos, _len);
	_src = (KCSTR)result.c_str();
	return (KCSTR)_src;
}
KCSTR str_extract(KSTRING & _src, size_t _pos, size_t _len, KSTRING & _dst)
{
	if (_pos > _src.LENGTH()) return (KCSTR)_src;
	std::string src = (KCSTR)_src;
	std::string dst	= src.substr(_pos, _len);
	_dst = (KCSTR)dst.c_str();
	return (KCSTR)_dst;
}
ExeFunc::ExeFunc(FuncParser & _dsl)
{
	m_dsl = &_dsl;
}
ExeFunc::~ExeFunc()
{
}
bool ExeFunc::EXE(QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _req)
{
	FuncParser & ifP = *m_dsl;
	const ExeEntry* pEntry = findfuncbyname((KCSTR)ifP.m_funcName);    
	if (pEntry == NULL)   
	{            
		return false;    
	}    
	if (pEntry->func == NULL)
	{             
		return false;    
	}      
	bool ret = pEntry->func(_wk, _rPool, _req, ifP.m_argList);
	return ret;
}
KCSTR ExeFunc::paramone(KSTRING & _value,POOL::POOLDATA & _rPool, RestMsg & _req, KSTRING & _buf)
{
	BasicParser pm;
	pm.TOKEN()=".";
	pm.PARSE((KSTR)_value);
	KCSTR pszRet = ExeParam::PARAM(pm.KEY(), pm.VAL(),_rPool,_req,_buf);
	return pszRet;
}
void ExeFunc::copyParam(ALIST & _dst, RestParam & _src)
{
	for(KUINT i=0;i<_src.NUMS();i++)
	{
		_dst.PUSH().VAL() = _src[i].VAL();
	}
}
const ExeEntry* ExeFunc::findfuncbyname(KCSTR _name)
{
	if (_name == NULL) return NULL;
	eExeType eT = FuncParser::getfunctype(_name);
	if(eT == EXE_TYPE_MAX) return NULL;
	return &g_ExeTable[eT];
}
const ExeEntry* ExeFunc::findfuncbytype(eExeType _type)
{
    if (_type < 0 || _type >= EXE_TYPE_MAX)
        return NULL;
    return &g_ExeTable[_type];
}
bool ExeFunc::EXE_SET( QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	KSTRING & sVal = _rPool.SET((KCSTR)_params[0].VAL()).VAL();
	KSTRING buf;
	sVal = paramone(_params[1].VAL(),_rPool,_req,buf);
	return true;
}
bool ExeFunc::EXE_SUM( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	KUINT tmp = 0;
	KSTRING & target = _rPool.SET((KCSTR)_params[0].VAL()).VAL();
	tmp = 0;
	KSTRING buf;
	for(KUINT j=1;j<_params.NUMS();j++)
	{
		COMPSTR szTmp(paramone(_params[j].VAL(),_rPool,_req,buf));
		tmp += (KUINT)szTmp;
	}
	target = tmp;
	return true;
}
bool ExeFunc::EXE_MINUS( QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	KSTRING & target = _rPool.SET((KCSTR)_params[0].VAL()).VAL();
	KUINT tmp = 0;
	KSTRING buf;
	for(KUINT j=1;j<_params.NUMS();j++)
	{
		COMPSTR szTmp(paramone(_params[j].VAL(),_rPool,_req,buf));
		if(j==1)
		{
			tmp = (KUINT)szTmp;
		}
		else
		{
			tmp -= (KUINT)szTmp;
		}
	}
	target = tmp;
	return true;
}
bool ExeFunc::EXE_DIV( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	if(_params.NUMS() != 3) return false;
	KSTRING & target = _rPool.SET((KCSTR)_params[0].VAL()).VAL();
	KSTRING bufA, bufB;
	COMPSTR a(paramone(_params[1].VAL(),_rPool,_req,bufA));
	COMPSTR b(paramone(_params[2].VAL(),_rPool,_req,bufB));
	if(((KUINT)b) ==0) return false;
	target = (KUINT)((KUINT)a/(KUINT)b);
	return true;
}
bool ExeFunc::EXE_MOD( QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	KSTRING & target = _rPool.SET((KCSTR)_params[0].VAL()).VAL();
	if(_params.NUMS() != 3) return false;
	KSTRING bufA, bufB;
	COMPSTR a(paramone(_params[1].VAL(),_rPool,_req,bufA));
	COMPSTR b(paramone(_params[2].VAL(),_rPool,_req,bufB));
	if(((KUINT)b) ==0) return false;
	target = (KUINT)(((KUINT)a)%((KUINT)b));
	return true;
}
bool ExeFunc::EXE_CAT( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	KSTRING & target = _rPool.SET((KCSTR)_params[0].VAL()).VAL();
	KSTRING buf;
	for(KUINT j=1;j<_params.NUMS();j++)
	{
		KCSTR pszTmp = paramone(_params[j].VAL(),_rPool,_req,buf);
		target<<pszTmp;
	}
	return true;
}
bool ExeFunc::EXE_STRSTR( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	KSTRING & target = _rPool.SET((KCSTR)_params[0].VAL()).VAL();
	if(_params.NUMS()!= 3)
	{
		return false;
	}
	KSTRING bufA, bufB;
	KCSTR pszA = paramone((KSTRING &)_params[1].VAL(),_rPool,_req,bufA);
	KCSTR pszB = paramone((KSTRING &)_params[2].VAL(),_rPool,_req,bufB);
	if(STRSTR(pszA,pszB))
	{
		target = "1";
	}
	else
	{
		target = "0";
	}
	return true;
}
bool ExeFunc::EXE_STRNCMP( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	KSTRING & target = _rPool.SET((KCSTR)_params[0].VAL()).VAL();
	if(_params.NUMS()!= 3)
	{
		return false;
	}
	KSTRING bufA, bufB;
	KCSTR pszA = paramone((KSTRING &)_params[1].VAL(),_rPool,_req,bufA);
	KCSTR pszB = paramone((KSTRING &)_params[2].VAL(),_rPool,_req,bufB);
	if(pszA == NULL || pszB == NULL)
	{
		return false;
	}
	if(strncmp(pszA,pszB,strlen(pszB))==0)
	{
		target = "1";
	}
	else
	{
		target = "0";
	}
	return true;
}
bool ExeFunc::EXE_INSERT( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	KSTRING & target = _rPool.SET((KCSTR)_params[0].VAL()).VAL();
	if(_params.NUMS()!= 4)
	{
		return false;
	}
	KSTRING a;KSTRING b;KSTRING c;
	KSTRING bufA, bufB, bufC;
	a= paramone((KSTRING &)_params[1].VAL(),_rPool,_req,bufA);
	b= paramone((KSTRING &)_params[2].VAL(),_rPool,_req,bufB);
	c= paramone((KSTRING &)_params[3].VAL(),_rPool,_req,bufC);
	KSTRING tmp; tmp = a;
	target = str_insert(a, (KCSTR)b, (size_t)(KUINT)c);
	return true;
}
bool ExeFunc::EXE_DELETE( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	KSTRING & target = _rPool.SET((KCSTR)_params[0].VAL()).VAL();
	if(_params.NUMS()!= 4)
	{
		return false;
	}
	KSTRING a;KSTRING b;KSTRING c;
	KSTRING bufA, bufB, bufC;
	a= paramone((KSTRING &)_params[1].VAL(),_rPool,_req,bufA);
	b= paramone((KSTRING &)_params[2].VAL(),_rPool,_req,bufB);
	c= paramone((KSTRING &)_params[3].VAL(),_rPool,_req,bufC);
	KSTRING tmp; tmp = a;
	target = str_delete(a, (size_t)(KUINT)b, (size_t)(KUINT)c);
	return true;
}
bool ExeFunc::EXE_EXTRACT( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	KSTRING & target = _rPool.SET((KCSTR)_params[0].VAL()).VAL();
	if(_params.NUMS()!= 4)
	{
		return false;
	}
	KSTRING a;KSTRING b;KSTRING c;KSTRING d;
	KSTRING bufA, bufB, bufC;
	a= paramone(_params[1].VAL(),_rPool,_req,bufA);
	b= paramone(_params[2].VAL(),_rPool,_req,bufB);
	c= paramone(_params[3].VAL(),_rPool,_req,bufC);
	KSTRING tmp; tmp = a;
	target = str_extract(a, (size_t)(KUINT)b, (size_t)(KUINT)c,d);
	return true;
}
bool ExeFunc::EXE_TIME( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	KSTRING & target = _rPool.SET((KCSTR)_params[0].VAL()).VAL();
	target = TIME::getTimeSec();
	return true;
}
bool ExeFunc::EXE_CLOCK( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	KSTRING & target = _rPool.SET((KCSTR)_params[0].VAL()).VAL();
	target = TIME::getClockMs();
	return true;
}
bool ExeFunc::EXE_PRINT( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	KSTRING & target = _rPool.SET((KCSTR)_params[0].VAL()).VAL();
	if(_params[1].VAL().LENGTH()>0 && _params[2].VAL().LENGTH()>0)
	{
		KCSTR a = (KCSTR)_params[1].VAL();
		KSTRING buf;
		COMPSTR b(paramone(_params[2].VAL(),_rPool,_req,buf));
		if(STRSTR(a,"s"))
		{
			target.PRINT((KCSTR)_params[1].VAL(),(KCSTR)b);
		}
		else if(STRSTR(a,"d"))
		{
			target.PRINT((KCSTR)_params[1].VAL(),(KINT)b);
		}
		else if(STRSTR(a,"u"))
		{
			target.PRINT((KCSTR)_params[1].VAL(),(KUINT)b);
		}
		else
		{
			target.PRINT("null");
		}
	}
	return true;
}
bool ExeFunc::EXE_DATE( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	TIME curT;
	KSTRING & target = _rPool.SET((KCSTR)_params[0].VAL()).VAL();
	if(_params[1].VAL().LENGTH()>0)
	{
		KSTRING buf;
		COMPSTR a(paramone(_params[1].VAL(),_rPool,_req,buf));
		TIME curT;
		target = curT.PRINT(a);
	}
	return true;
}
bool ExeFunc::EXE_CPH( QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	if(_params.NUMS() < 3) return false;
	ExeCore::Session * pSess = (ExeCore::Session*)_rPool.GETU();
	if(pSess == NULL) return false;
	KCSTR resultKey  = (KCSTR)_params[0].VAL();
	BasicParser pl; 
	pl.TOKEN() = "."; 
	pl.PARSE(_params[1].VAL());
	KSTRING srcKey;
	if(IS_DSL_K_RCV_MSG((KCSTR)pl.KEY()))
	{
		srcKey = (KCSTR)pl.VAL();
	}
	KUINT fieldIdx   = (KUINT)_params[2].VAL();
	RestParam & srcInMsg = _req.GETR((KCSTR)srcKey);
	if(srcInMsg.NUMS() == 0 || srcInMsg[0].NUMS() == 0)
	{
		return false;
	}
	for(KUINT ai=0; ai<pSess->m_arrObjs.NUMS(); ai++)
	{
		RestParam & entry = pSess->m_arrObjs[ai];
		if(entry.GET("key").VAL() == resultKey)
		{
			pSess->m_arrObjs.DEL(ai);
			break;
		}
	}
	RestParam & newEntry = pSess->m_arrObjs.ARR();
	newEntry.SET("key").VAL() = resultKey;
	RestParam & valList = newEntry.SET("values");
	for(KUINT ri=0; ri<srcInMsg.NUMS(); ri++)
	{
		RestParam & row = srcInMsg[ri];
		if(fieldIdx < row.NUMS())
		{
			valList.ARR().VAL() = (KCSTR)row[(KUINT)fieldIdx].VAL();
		}
		else
		{
			valList.ARR().VAL() = "null";
		}
	}
	return true;
}
bool ExeFunc::EXE_LOOP( QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	if(_params.NUMS() < 2)
	{
		return false;
	}
	KCSTR listName = (KCSTR)_params[0].VAL();
	KCSTR procName = (KCSTR)_params[1].VAL();
	ExeCore::Session * pSess = (ExeCore::Session*)_rPool.GETU();
	if(pSess == NULL)
	{
		return false;
	}
	KUINT listSize = 0;
	RestParam & arrObjs = pSess->m_arrObjs;
	for(KUINT ai=0; ai<arrObjs.NUMS(); ai++)
	{
		RestParam & entry = arrObjs[ai];
		if(entry.GET("key").VAL() == listName)
		{
			listSize = entry.GET("values").NUMS();
			break;
		}
	}
	if(listSize == 0)
	{
		return true;
	}
	_rPool.SET(DEF_DSL_K_LOOP_BREAK_eng).VAL() = "0";
	for(KUINT cnt = 0; cnt < listSize; cnt++)
	{
		_rPool.SET(DEF_DSL_K_LOOP_CNT_eng).VAL() = cnt+1;
		if(!ExeCore::OBJ().EXEPROC(procName, _wk, _rPool, _req))
		{
			_rPool.m_list.DEL(DEF_DSL_K_LOOP_CNT_eng);
			_rPool.m_list.DEL(DEF_DSL_K_LOOP_BREAK_eng);
			return false;
		}
		KSTRING & breakFlag = _rPool.GET(DEF_DSL_K_LOOP_BREAK_eng).VAL();
		if(breakFlag == "1")
		{
			break;
		}
	}
	_rPool.m_list.DEL(DEF_DSL_K_LOOP_CNT_eng);
	_rPool.m_list.DEL(DEF_DSL_K_LOOP_BREAK_eng);
	return true;
}
bool ExeFunc::EXE_OBJ( QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	if(_params.NUMS() != 2) 
	{
		return false;
	}
	BasicParser pl; 
	pl.TOKEN() = "."; 
	pl.PARSE(_params[1].VAL());
	if(!IS_DSL_K_RCV_MSG((KCSTR)pl.KEY()))
	{
		return false;
	}
	ExeCore::Session * pSess = (ExeCore::Session*)_rPool.GETU();
	if(pSess == NULL) 
	{
		return false;
	}
	pSess->m_subObj.DEL((KCSTR)_params[0].VAL());
	RestParam & target = pSess->m_subObj.SET((KCSTR)_params[0].VAL());
	RestParam & src = _req.GETR((KCSTR)pl.VAL());
	target = src;
	target.setkey((KCSTR)_params[0].VAL());
	target.KEY() =(KCSTR)_params[0].VAL();
	return true;
}
bool ExeFunc::EXE_SPLIT( QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	if(_params.NUMS() != 3)
	{
		return false;
	}
	KCSTR listName = (KCSTR)_params[0].VAL();
	KSTRING src;
	KCSTR szSrc = paramone(_params[1].VAL(), _rPool,_req,src);
	KCSTR trimchr = (KCSTR)_params[2].VAL();
	ExeCore::Session * pSess = (ExeCore::Session*)_rPool.GETU();
	if(pSess == NULL)
	{
		return false;
	}
	KUINT listSize = 0;
	RestParam & arrObjs = pSess->m_arrObjs;
	for(KUINT i=0; i<arrObjs.NUMS(); i++)
	{
		RestParam & entry = arrObjs[i];
		if(entry.GET("key").VAL() == listName)
		{
			pSess->m_arrObjs.DEL(i);
			break;
		}
	}
	TOKSTR tok((KCSTR)szSrc); tok.TOK((KCSTR)trimchr);
	if(tok.NUMS()>0)
	{
		RestParam & newArr = pSess->m_arrObjs.ARR();
		newArr.SET("key").VAL() =listName;
		RestParam & valArr = newArr.SET("values");
		for(KUINT i=0;i<tok.NUMS();i++)
		{
			RestParam & valItem = valArr.ARR();
			valItem.VAL() = (KCSTR)tok[i];
		}		
	}	
	return true;
}
bool ExeFunc::EXE_WORDEX( QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	if(_params.NUMS() != 2)
	{
		return false;
	}
	KCSTR listName = (KCSTR)_params[0].VAL();
	KSTRING src;
	KCSTR szSrc = paramone(_params[1].VAL(), _rPool,_req,src);
	ExeCore::Session * pSess = (ExeCore::Session*)_rPool.GETU();
	if(pSess == NULL)
	{
		return false;
	}
	KUINT listSize = 0;
	RestParam & arrObjs = pSess->m_arrObjs;
	for(KUINT i=0; i<arrObjs.NUMS(); i++)
	{
		RestParam & entry = arrObjs[i];
		if(entry.GET("key").VAL() == listName)
		{
			pSess->m_arrObjs.DEL(i);
			break;
		}
	}
	TOKSTR tok((KCSTR)szSrc); tok.TOK(" ");
	if(tok.NUMS()>0)
	{
		RestParam & newArr = pSess->m_arrObjs.ARR();
		newArr.SET("key").VAL() =listName;
		RestParam & valArr = newArr.SET("values");
		for(KUINT i=0;i<tok.NUMS();i++)
		{
			RestParam & valItem = valArr.ARR();
			valItem.VAL() = (KCSTR)tok[i];
		}		
	}	
	return true;
}
bool ExeFunc::EXE_WORDSUM( QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params)
{
	if(_params.NUMS() != 3)
	{
		return false;
	}
	KSTRING & target = _rPool.SET((KCSTR)_params[0].VAL()).VAL();
	target = KNULL;
	TOKSTR tok((KCSTR)_params[1].VAL()); tok.TOK(" .");
	KCSTR szListName = (KCSTR)tok[2];
	KUINT nStart = (KUINT)_params[2].VAL();
	ExeCore::Session * pSess = (ExeCore::Session*)_rPool.GETU();
	if(pSess == NULL)
	{
		return false;
	}
	KUINT listSize = 0;
	RestParam & arrObjs = pSess->m_arrObjs;
	for(KUINT i=0; i<arrObjs.NUMS(); i++)
	{
		RestParam & entry = arrObjs[i];
		if(entry.GET("key").VAL() == szListName)
		{
			RestParam & valArr = entry.GET("values");
			for(KUINT j=nStart;j<valArr.NUMS();j++)
			{
				RestParam & valItem = valArr[j];
				if(j == (valArr.NUMS()-1))
					target.PRINT("%s",(KCSTR)valItem.VAL());
				else
					target.PRINT("%s ",(KCSTR)valItem.VAL());
			}
		}
	}
	return true;
}
}
