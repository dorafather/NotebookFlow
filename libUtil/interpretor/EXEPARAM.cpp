#include "EXEPARAM.h"
#include "EXESTMT.h"

namespace nsUtil
{
static bool s_fnIsAllDigits(KCSTR _psz)
{
	if(_psz == NULL || _psz[0] == '\0') return false;
	for(KCSTR p=_psz; *p; p++)
	{
		if(*p < '0' || *p > '9') return false;
	}
	return true;
}
static KCSTR s_fnGetObjBracketVal(ExeCore::Session * _pSes, POOL::POOLDATA & _rPool,
									KCSTR _tmp, KSTRING & _buf)
{
	KCSTR pBracket = strchr(_tmp, '[');
	if(pBracket == NULL) { _buf = KNULL; return (KCSTR)_buf; } // 호출부에서 이미 '[' 존재를 확인하지만 방어적으로 한 번 더 체크
	KCSTR pCloseBr = strchr(pBracket, ']');
	if(pCloseBr == NULL) { _buf = KNULL; return (KCSTR)_buf; }
	KSTRING objName; objName.PRINT("%.*s",(int)(pBracket-_tmp), _tmp);
	KSTRING idxTok;  idxTok.PRINT("%.*s",(int)(pCloseBr-pBracket-1), pBracket+1);
	KINT nIdx = -1;
	if(IS_DSL_K_LOOP_CNT((KCSTR)idxTok))
	{
		KSTRING & cntVal = _rPool.GET(DEF_DSL_K_LOOP_CNT_eng).VAL();
		nIdx = atoi((KCSTR)cntVal) - 1;
	}
	else if(s_fnIsAllDigits((KCSTR)idxTok))
	{
		nIdx = atoi((KCSTR)idxTok);
	}
	else
	{
		KCSTR pVarName = (KCSTR)idxTok;
		TOKSTR vtok(idxTok); vtok.TOK(".");
		if(vtok.NUMS() >= 2 && IS_DSL_K_SESSION((KCSTR)vtok[0]))
		{
			pVarName = (KCSTR)vtok[1];
		}
		KSTRING & varVal = _rPool.GET(pVarName).VAL();
		nIdx = atoi((KCSTR)varVal);
	}
	if(nIdx < 0) { _buf = KNULL; return (KCSTR)_buf; }
	RestParam & arrEntry = _pSes->m_subObj.GET((KCSTR)objName);
	if((KUINT)nIdx >= arrEntry.NUMS()) { _buf = KNULL; return (KCSTR)_buf; } // RestParam::operator[]도 안전하지만 명시적으로 한 번 더 방어 - 배열 끝(재귀 종료조건)에서 반드시 여기로 와야 함
	RestParam & item = arrEntry[(KUINT)nIdx];
	KCSTR pAfter = pCloseBr + 1;
	if(*pAfter == '.') pAfter++;
	if(*pAfter == '\0')
	{
		_buf = item.VAL();
		return (KCSTR)_buf;
	}
	TOKSTR ftok(pAfter); ftok.TOK(".");
	RestParam * pCur = &item;
	for(KUINT fi=0; fi<ftok.NUMS(); fi++)
	{
		pCur = &pCur->GET((KCSTR)ftok[fi]);
	}
	_buf = pCur->VAL();
	return (KCSTR)_buf;
}
ExeParam::ExeParam(){}
ExeParam::~ExeParam(){}
KCSTR ExeParam::PARAM(KSTRING & _val1, KSTRING & _val2,
					POOL::POOLDATA & _rPool, RestMsg & _req,
					KSTRING & _buf)
{
	if(_val1.LENGTH()==0) return NULL;
	if(_val2.LENGTH()==0)
	{
		if(IS_DSL_K_LOOP_CNT((KCSTR)_val1))
		{
			return (KCSTR)_rPool.GET(DEF_DSL_K_LOOP_CNT_eng).VAL();
		}
		return (KCSTR)_val1;
	}
	else
	{
		if(IS_DSL_K_SESSION((KCSTR)_val1))
		{
			TOKSTR tok(_val2); tok.TOK(".");
			if(IS_DSL_K_OBJ((KCSTR)tok[0]))
			{
				ExeCore::Session * pSes = (ExeCore::Session*)_rPool.GETU();
				if(pSes == NULL) return NULL;
				KSTRING tmp;
				for(KUINT i=1;i<tok.NUMS();i++)
				{
					if(i == (tok.NUMS()-1))
						tmp.PRINT("%s",(KCSTR)tok[i]);
					else
						tmp.PRINT("%s.",(KCSTR)tok[i]);
				}
				if(strchr((KCSTR)tmp,'[') != NULL)
				{
					return s_fnGetObjBracketVal(pSes, _rPool, (KCSTR)tmp, _buf);
				}
				_buf = (KCSTR)pSes->m_subObj.GETR((KCSTR)tmp).VAL();
				return _buf;
			}
			if(IS_DSL_K_LIST((KCSTR)tok[0]))
			{
				KCSTR pFirstDot = strchr((KCSTR)_val2, '.');
				KSTRING listPart; listPart = pFirstDot ? (pFirstDot + 1) : (KCSTR)_val2;
				KSTRING listName; KINT listIdx = -1;
				KCSTR pBracket = strchr((KCSTR)listPart, '[');
				bool bSizeQuery = false;
				KCSTR pSizeSuffix = strstr((KCSTR)listPart, DEF_DSL_PARSE_SIZE_eng);
				if(pSizeSuffix && pBracket == NULL)
				{
					KUINT nameLen = (KUINT)(pSizeSuffix - (KCSTR)listPart);
					listName = KNULL;
					for(KUINT ci=0;ci<nameLen;ci++) listName<<((KCSTR)listPart)[ci];
					bSizeQuery = true;
				}
				else
				if(pBracket)
				{
					KUINT nameLen = (KUINT)(pBracket - (KCSTR)listPart);
					listName = KNULL;
					for(KUINT ci=0;ci<nameLen;ci++) listName<<((KCSTR)listPart)[ci];
					KCSTR pIdxStr = pBracket+1;
					KSTRING idxTok;
					{
						KCSTR pCloseBr = strchr(pIdxStr, ']');
						if(pCloseBr) idxTok.PRINT("%.*s",(int)(pCloseBr-pIdxStr),pIdxStr);
						else idxTok = pIdxStr;
					}
					if(IS_DSL_K_LOOP_CNT((KCSTR)idxTok))
					{
						KSTRING & cntVal = _rPool.GET(DEF_DSL_K_LOOP_CNT_eng).VAL();
						listIdx = atoi((KCSTR)cntVal) - 1; 
					}
					else if(s_fnIsAllDigits((KCSTR)idxTok))
					{
						listIdx = atoi((KCSTR)idxTok);
					}
					else
					{
						KCSTR pVarName = (KCSTR)idxTok;
						TOKSTR vtok(idxTok); vtok.TOK(".");
						if(vtok.NUMS() >= 2 && IS_DSL_K_SESSION((KCSTR)vtok[0]))
						{
							pVarName = (KCSTR)vtok[1];
						}
						KSTRING & varVal = _rPool.GET(pVarName).VAL();
						listIdx = atoi((KCSTR)varVal);
					}
				}
				else
				{
					listName = (KCSTR)listPart;
				}
				ExeCore::Session * pSessL = (ExeCore::Session*)_rPool.GETU();
				if(pSessL)
				{
					RestParam & arrObjs = pSessL->m_arrObjs;
					for(KUINT ai=0; ai<arrObjs.NUMS(); ai++)
					{
						RestParam & entry = arrObjs[ai];
						if(entry.GET("key").VAL() == (KCSTR)listName)
						{
							RestParam & vals = entry.GET("values");
							if(bSizeQuery || listIdx < 0)
							{
								_buf = vals.NUMS();
								return (KCSTR)_buf;
							}
							if((KUINT)listIdx < vals.NUMS())
							{
								return (KCSTR)vals[(KUINT)listIdx].VAL();
							}
							_buf = KNULL;
							return (KCSTR)_buf;
						}
					}
				}
				_buf = KNULL;
				return (KCSTR)_buf;
			}
			if(IS_DSL_K_LOOP_CNT((KCSTR)_val2))
			{
				return (KCSTR)_rPool.GET(DEF_DSL_K_LOOP_CNT_eng).VAL();
			}
			return (KCSTR)_rPool.GET((KCSTR)_val2).VAL();
		}
		else if(IS_DSL_K_RCV_MSG((KCSTR)_val1))
		{
			return (KCSTR)_req.GETR((KCSTR)_val2).VAL();
		}
		else if(IS_DSL_K_INI((KCSTR)_val1))
		{
			BasicParser param;
			param.TOKEN()=".";
			param.PARSE((KCSTR)_val2);
			ExeCore::OBJ().GETINI((KCSTR)param.KEY(),
								(KCSTR)param.VAL(),
								_buf);
			return (KCSTR)_buf;
		}
		else if(IS_DSL_CLASS_STMT((KCSTR)_val1))
		{
			StmtParser *pStmt = ExeCore::OBJ().FINDSTMT(_rPool,(KCSTR)_val2);
			if(pStmt==NULL)
			{
				return "null";
			}
			ExeStmt stmt(*pStmt);
			if(stmt.EXE(_rPool,_req,_buf))
			{
				return (KCSTR)_buf;
			}
			else
			{
				return "null";
			}
			return "null";
		}
		else
		{
			_buf.PRINT("%s.%s",(KCSTR)_val1,(KCSTR)_val2);
			return (KCSTR)_buf;
		}
	}
	return NULL;
}
KUINT ExeParam::PARAMLength(KSTRING & _val1, KSTRING & _val2,
					POOL::POOLDATA & _rPool, RestMsg & _req,
					KSTRING & _buf)
{
	if(_val1.LENGTH()==0) return 0;
	if(_val2.LENGTH()==0)
	{
		return (KUINT)_val1;
	}
	else
	{
		KSTRING tmpVal; tmpVal = _val2;
		if(endsWithLength((KCSTR)_val2)==1)
		{
			BasicParser pm; pm.TOKEN()=".";pm.PARSE(_val2);
			tmpVal = pm.KEY();
		}
		if(IS_DSL_K_SESSION((KCSTR)_val1))
		{
			return _rPool.GET((KCSTR)tmpVal).VAL().LENGTH();
		}
		else if(IS_DSL_K_RCV_MSG((KCSTR)_val1))
		{
			TOKSTR tok; tok = _val2; tok.TOK(".");
			KSTRING tmp;
			for(KUINT i=0;i<tok.NUMS()-1;i++)
			{
				if(i==0)
				{
					tmp<<tok[i];
				}
				else
				{
					tmp.PRINT(".%s",(KCSTR)tok[i]);
				}
			}
			return _req.GETR((KCSTR)tmp).VAL().LENGTH();
		}
		else
		{
			return (KUINT)_val1;
		}
	}
	return 0;
}
}
