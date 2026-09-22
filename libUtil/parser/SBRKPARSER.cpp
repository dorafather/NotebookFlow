#include "SBRKPARSER.h"
#include "FUNCPARSER.h"
namespace nsUtil
{
parseSbrk::parseSbrk() {}
parseSbrk::~parseSbrk() {}
void parseSbrk::parse(POOL::POOLDATA & _rPool, const char* _src)
{
	dst.clear();
	if (!_src) return;
	const char* p = _src;
	while (*p)
	{
		if (*p == '[')
		{
			const char* start = p;
			++p; 
			const char* end = std::strchr(p, ']');
			if (end)
			{
				std::string detect(p, end - p);
				cbk(_rPool,detect);
				dst += '[';
				dst += detect;
				dst += ']';
				p = end + 1; 
				continue;
			}
			else
			{
				dst += *start;
				++p;
				continue;
			}
		}
		dst += *p;
		++p;
	}
}
void parseSbrk::cbk(POOL::POOLDATA & _rPool,std::string& _detect)
{
	TOKSTR tok(_detect.c_str()); tok.TOK(".");
	KUINT nIdx = 0;
	if(IS_DSL_K_LOOP_CNT((KCSTR)tok[0]))
	{
		KUINT cntVal = (KUINT)_rPool.GET(DEF_DSL_K_LOOP_CNT_eng).VAL();
		if(cntVal >0)
			nIdx = cntVal - 1;
		else
			nIdx = 0;
	}
	else if(s_fnIsAllDigits((KCSTR)tok[0]))
	{
		nIdx = (KUINT)tok[0];
	}
	else if(tok.NUMS()==2)
	{
		if(IS_DSL_K_SESSION((KCSTR)tok[0]))
		{
			KSTRING & varVal = _rPool.GET((KCSTR)tok[1]).VAL();
			nIdx = (KUINT)varVal;
		}
	}
	KSTRING tmp; tmp.PRINT("%u",nIdx);
	_detect = (KCSTR)tmp;
}
bool parseSbrk::s_fnIsAllDigits(KCSTR _psz)
{
	if(_psz == NULL || _psz[0] == '\0') return false;
	for(KCSTR p=_psz; *p; p++)
	{
		if(*p < '0' || *p > '9') return false;
	}
	return true;
}
}
