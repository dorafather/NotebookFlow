#include "IFPARSER.h"
#include "BASICPARSER.h"
#include "FUNCPARSER.h"
namespace nsUtil
{
const char * s_compList[E_DSL_COMP_MAX]=
{
"TRUE",
"FALSE",
">",
"<",
">=",
"<=",
"==",
"!=",
"===",
"!!!"
};
const char * s_cmpString[E_DSL_COMP_MAX]=
{
	"TRUE",
	"FALSE",
	"BIGGER",
	"SMALLER",
	"BIGGER_MATCH",
	"SMALLER_MATCH",
	"FULL_MATCH",
	"NOT_MATCH",
	"PFX_MATCH",
	"PARTIAL_MATCH",
};
KCSTR stringCmp(ESceParserComp_t _eT)
{
	switch(_eT)
	{
		case E_DSL_COMP_TRUE         : return s_cmpString[E_DSL_COMP_TRUE         ];                 
		case E_DSL_COMP_FALSE        : return s_cmpString[E_DSL_COMP_FALSE        ]; 
		case E_DSL_COMP_BIGGER       : return s_cmpString[E_DSL_COMP_BIGGER       ]; 
		case E_DSL_COMP_SMALLER      : return s_cmpString[E_DSL_COMP_SMALLER      ]; 
		case E_DSL_COMP_BIGGER_MATCH : return s_cmpString[E_DSL_COMP_BIGGER_MATCH ]; 
		case E_DSL_COMP_SMALLER_MATCH: return s_cmpString[E_DSL_COMP_SMALLER_MATCH]; 
		case E_DSL_COMP_FULL_MATCH   : return s_cmpString[E_DSL_COMP_FULL_MATCH   ]; 
		case E_DSL_COMP_NOT_MATCH    : return s_cmpString[E_DSL_COMP_NOT_MATCH    ]; 
		case E_DSL_COMP_PFX_MATCH    : return s_cmpString[E_DSL_COMP_PFX_MATCH    ]; 
		case E_DSL_COMP_PARTIAL_MATCH: return s_cmpString[E_DSL_COMP_PARTIAL_MATCH]; 
		default: return "UNKNOWN";
	}
	return "UNKNOWN";
}
ESceParserComp_t enumCmp(KCSTR _val)
{
	if(COMPSTR(_val) == "TRUE")         return E_DSL_COMP_TRUE          ;   
	else if(COMPSTR(_val) == "FALSE")        return E_DSL_COMP_FALSE         ;
	else if(COMPSTR(_val) == "BIGGER")       return E_DSL_COMP_BIGGER        ;
	else if(COMPSTR(_val) == "SMALLER")      return E_DSL_COMP_SMALLER       ;
	else if(COMPSTR(_val) == "BIGGER_MATCH") return E_DSL_COMP_BIGGER_MATCH  ;
	else if(COMPSTR(_val) == "SMALLER_MATCH")return E_DSL_COMP_SMALLER_MATCH ;
	else if(COMPSTR(_val) == "FULL_MATCH")   return E_DSL_COMP_FULL_MATCH    ;
	else if(COMPSTR(_val) == "NOT_MATCH")    return E_DSL_COMP_NOT_MATCH     ;
	else if(COMPSTR(_val) == "PFX_MATCH")    return E_DSL_COMP_PFX_MATCH     ;
	else if(COMPSTR(_val) == "PARTIAL_MATCH")return E_DSL_COMP_PARTIAL_MATCH ;
	else return E_DSL_COMP_MAX;
}
KCSTR convCmp(KCSTR _val)
{
	for(KUINT i=0;i<E_DSL_COMP_MAX;i++)
	{
		if(COMPSTR(s_compList[i]) == _val)
		{
			return s_cmpString[i];
		}
	}
	return "UNKNOWN";
}
bool validCompare(KCSTR _comp)
{
	for(KUINT i = 0; i < E_DSL_COMP_MAX; i++)
	{
		if(COMPSTR(s_compList[i]) == _comp) return true;
	}
	return false;
}
IfParser::IfParser()
{
	m_eSt = E_PARSE_NONE; 
	m_bDotSeen = false;
}
IfParser::~IfParser()
{
}
IfParser & IfParser::operator=(IfParser & _src)
{
	CONSTRUCT((void *)&_src);
	return *this;
}
void IfParser::CONSTRUCT(void * _pvSrc)
{
	IfParser * psrc = (IfParser*)_pvSrc;
	m_if        = psrc->m_if     ;
	m_if_a      = psrc->m_if_a   ;
	m_if_b      = psrc->m_if_b   ;
	m_if_cond   = psrc->m_if_cond;
	m_if_c      = psrc->m_if_c   ;
	m_if_d      = psrc->m_if_d   ;
	m_bDotSeen  = psrc->m_bDotSeen;
}
void IfParser::CLEAR()
{
	m_eSt = E_PARSE_NONE;
	m_if = KNULL;
	m_if_a = KNULL;
	m_if_b = KNULL;
	m_if_cond = KNULL;
	m_if_c = KNULL;
	m_if_d = KNULL;
	m_bDotSeen = false;
}
bool IfParser::compList(KCSTR _comp, ALIST & _a, ALIST & _b)
{
	COMPSTR cmp(_comp);
	for(KUINT i=0;i<_a.NUMS();i++)
	{
		for(KUINT j=0;j<_b.NUMS();j++)
		{
			if(cmp == "==")
			{
				if(_a[i].VAL() == _b[j].VAL()) return true;
			}
			else if(cmp == "!!!")
			{
				if(STRSTR((KCSTR)_a[i].VAL(),(KCSTR)_b[j].VAL())) return true;
			}
			else if(cmp == "===")
			{
				if(STRNCMP((KCSTR)_a[i].VAL(),(KCSTR)_b[j].VAL())) return true;
			}
		}
	}
	return false;
}
bool IfParser::PARSE(KCSTR _src)
{
	if(_src==NULL) return false;
	KUINT len = strlen(_src);
	for(KUINT i=0;i<len;i++)
	{
		if(!parsestep((const char)_src[i])) return false;
	}
	if(m_eSt != E_PARSE_END)
	{
		m_result.PRINT("if(%s) incomplete expression, stopped at state(%d)",
					   (KCSTR)m_if_a,(int)m_eSt);
		return false;
	}
	m_eSt = E_PARSE_MAX;
	return true;
}
void IfParser::JSON(RestParam & _item)
{
	KSTRING & val = _item.VAL();
	KSTRING a; KSTRING b;
	if(m_if_b.LENGTH()>0 )
	{
		a.PRINT("%s.%s",(KCSTR)m_if_a,(KCSTR)m_if_b);
	}
	else
	{
		a = m_if_a;
	}
	if(m_if_d.LENGTH()>0 )
	{
		b.PRINT("%s.%s",(KCSTR)m_if_c,(KCSTR)m_if_d);
	}
	else
	{
		b = m_if_c;
	}
	if(m_if_cond.LENGTH()>0)
	{
		val.PRINT("%s(%s %s %s)",(KCSTR)m_if,(KCSTR)a,
					(KCSTR)m_if_cond,(KCSTR)b);
	}
	else
	{
		val.PRINT("%s(%s)",(KCSTR)m_if,(KCSTR)a);
	}
}
void IfParser::STR(KSTRING & _buf)
{
	KSTRING a; KSTRING b;
	if(m_if_b.LENGTH()>0 )
	{
		a.PRINT("%s.%s",(KCSTR)m_if_a,(KCSTR)m_if_b);
	}
	else
	{
		a = m_if_a;
	}
	if(m_if_d.LENGTH()>0 )
	{
		b.PRINT("%s.%s",(KCSTR)m_if_c,(KCSTR)m_if_d);
	}
	else
	{
		b = m_if_c;
	}
	if(m_if_cond.LENGTH()>0)
	{
		_buf.PRINT("%s(%s %s %s)",(KCSTR)m_if,(KCSTR)a,
					(KCSTR)m_if_cond,(KCSTR)b);
	}
	else if(IS_DSL_K_ELSE((KCSTR)m_if) && !IS_DSL_K_ELSEIF((KCSTR)m_if) && a.LENGTH()==0)
	{
		_buf.PRINT("%s",(KCSTR)m_if);
	}
	else
	{
		_buf.PRINT("%s(%s)",(KCSTR)m_if,(KCSTR)a);
	}
}
void IfParser::IMPORT(RestParam & _item)
{
	CLEAR();
	KCSTR flat = (KCSTR)_item.VAL();
	if(flat==NULL) return;
	KCSTR paren = strchr(flat,'(');
	KSTRING prefix;
	if(paren) prefix.PRINT("%.*s",(int)(paren-flat),flat);
	else prefix = flat;
	if(IS_DSL_K_ELSE((KCSTR)prefix) && !IS_DSL_K_ELSEIF((KCSTR)prefix))
	{
		m_if = prefix;
		m_eSt = E_PARSE_END;
		return;
	}
	if(!PARSE(flat))
	{
		m_result.PRINT("IfParser::IMPORT PARSE 실패(%s): %s",flat,(KCSTR)m_result);
	}
}
bool IfParser::parsestep(const char _cInput)
{
	switch(m_eSt)
	{
		case E_PARSE_NONE       : return m_fnE_PARSE_NONE      (_cInput);    
		case E_PARSE_IF         : return m_fnE_PARSE_IF        (_cInput);
		case E_PARSE_IF_TAIL      : return m_fnE_PARSE_IF_TAIL     (_cInput);
		case E_PARSE_IF_SP      : return m_fnE_PARSE_IF_SP     (_cInput);
		case E_PARSE_IF_A       : return m_fnE_PARSE_IF_A      (_cInput);
		case E_PARSE_IF_A_SP    : return m_fnE_PARSE_IF_A_SP   (_cInput);
		case E_PARSE_IF_B       : return m_fnE_PARSE_IF_B      (_cInput);
		case E_PARSE_IF_B_SP    : return m_fnE_PARSE_IF_B_SP   (_cInput);
		case E_PARSE_IF_COND    : return m_fnE_PARSE_IF_COND   (_cInput);
		case E_PARSE_IF_COND_SP : return m_fnE_PARSE_IF_COND_SP(_cInput);
		case E_PARSE_IF_C       : return m_fnE_PARSE_IF_C      (_cInput);
		case E_PARSE_IF_C_SP    : return m_fnE_PARSE_IF_C_SP   (_cInput);
		case E_PARSE_IF_D       : return m_fnE_PARSE_IF_D      (_cInput);
		case E_PARSE_END       : return m_fnE_PARSE_END      (_cInput);
		default: return m_fnE_PARSE_NONE(_cInput); 
	};
	return false;
}
bool IfParser::valid()
{
	return true;
}
bool IfParser::m_fnE_PARSE_NONE      (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \r\n\t"))
	{
		// skipp
	}
	else if(BasicParser::MATCH(_cInput,"()"))
	{
		m_result.PRINT("if(-) illegal char '%c'",_cInput);
		return false;
	}
	else
	{
		m_if<<_cInput;
		CHANGE(E_PARSE_IF);
	}
	return true;
}
bool IfParser::m_fnE_PARSE_IF        (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t"))
	{
		CHANGE(E_PARSE_IF_TAIL);
	}
	else if(BasicParser::MATCH(_cInput,"("))
	{
		if(IS_DSL_K_IF((KCSTR)m_if) 
			|| IS_DSL_K_ELSEIF((KCSTR)m_if) 
			|| IS_DSL_K_OR((KCSTR)m_if) 
			|| IS_DSL_K_AND((KCSTR)m_if))
		{
			CHANGE(E_PARSE_IF_SP);
		}
		else
		{
			m_result.PRINT("if(%s) Illegal IF",(KCSTR)m_if);
			return false;
		}
	}
	else
	{
		m_if<<_cInput;
	}
	return true;
}
bool IfParser::m_fnE_PARSE_IF_TAIL     (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t"))
	{
		// skipp
	}
	else if(BasicParser::MATCH(_cInput,"("))
	{
		if(IS_DSL_K_IF((KCSTR)m_if) 
			|| IS_DSL_K_ELSEIF((KCSTR)m_if) 
			|| IS_DSL_K_OR((KCSTR)m_if)
			|| IS_DSL_K_AND((KCSTR)m_if))
		{
			CHANGE(E_PARSE_IF_A);
		}
		else
		{
			m_result.PRINT("if(%s) illegal IF",(KCSTR)m_if);
			return false;
		}
	}
	else
	{
		m_result.PRINT("if(-) illegal char '%c'",_cInput);
		return false;
	}
	return true;
}
bool IfParser::m_fnE_PARSE_IF_SP     (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t"))
	{
		// skipp
	}
	else if(BasicParser::MATCH(_cInput,"("))
	{
		m_result.PRINT("if(%s) duplicate '('",(KCSTR)m_if_a);
		return false;
	}
	else
	{
		m_if_a<<_cInput;
		CHANGE(E_PARSE_IF_A);
	}
	return true;
}
bool IfParser::m_fnE_PARSE_IF_A      (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," .\t"))
	{
		if(IS_DSL_K_RCV_MSG((KCSTR)m_if_a)
			||IS_DSL_K_SESSION((KCSTR)m_if_a)
			|| IS_DSL_K_INI((KCSTR)m_if_a))
		{
			if(BasicParser::MATCH(_cInput,".")) m_bDotSeen = true;
			CHANGE(E_PARSE_IF_A_SP);
		}
		else if(IS_DSL_K_LOOP_CNT((KCSTR)m_if_a))
		{
			CHANGE(E_PARSE_IF_A_SP);
		}
		else
		{
			m_result.PRINT("if(%s) illegal left value",(KCSTR)m_if_a);
			return false;
		}
	}
	else if(BasicParser::MATCH(_cInput,"!<>="))
	{
		m_if_cond<<_cInput;
		CHANGE(E_PARSE_IF_COND);
	}
	else if(BasicParser::MATCH(_cInput,"("))
	{
		m_result.PRINT("if(%s) illegal char '%c'",(KCSTR)m_if_a, _cInput);
		return false;
	}
	else if(BasicParser::MATCH(_cInput,")"))
	{
		if(IS_DSL_K_TRUE((KCSTR)m_if_a)
			|| IS_DSL_K_FALSE((KCSTR)m_if_a))
		{
			CHANGE(E_PARSE_END);
		}
		else
		{
			m_result.PRINT("if(%s) illegal condition",(KCSTR)m_if_a);
			return false;
		}
	}
	else
	{
		m_if_a<<_cInput;
	}
	return true;
}
bool IfParser::m_fnE_PARSE_IF_A_SP   (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t"))
	{
	}
	else if(BasicParser::MATCH(_cInput,"."))
	{
		if(m_bDotSeen)
		{
			m_result.PRINT("if(%s) duplicate dot accessor",(KCSTR)m_if_a);
			return false;
		}
		m_bDotSeen = true;
	}
	else if(BasicParser::MATCH(_cInput,"!<>="))
	{
		m_if_cond<<_cInput;
		CHANGE(E_PARSE_IF_COND);
	}
	else if(BasicParser::MATCH(_cInput,")"))
	{
		if(IS_DSL_K_TRUE((KCSTR)m_if_a) 
			|| IS_DSL_K_FALSE((KCSTR)m_if_a))
		{
			CHANGE(E_PARSE_END);
		}
		else
		{
			m_result.PRINT("if(%s) illegal condition",(KCSTR)m_if_a);
			return false;
		}
	}
	else
	{
		m_if_b<<_cInput;
		CHANGE(E_PARSE_IF_B);
	}
	return true;
}
bool IfParser::m_fnE_PARSE_IF_B      (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t"))
	{
		CHANGE(E_PARSE_IF_B_SP);
	}
	else if(BasicParser::MATCH(_cInput,")"))
	{
		m_result.PRINT("if(%s.%s) missing comparison operator",
					   (KCSTR)m_if_a,(KCSTR)m_if_b);
		return false;
	}
	else
	{
		m_if_b<<_cInput;
	}
	return true;
}
bool IfParser::m_fnE_PARSE_IF_B_SP   (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t"))
	{
	}
	else if(BasicParser::MATCH(_cInput,"!=<>"))
	{
		m_if_cond<<_cInput;
		CHANGE(E_PARSE_IF_COND);
	}
	else
	{
		m_result.PRINT("if(%s.%s) illegal char after operand('%c')",
					   (KCSTR)m_if_a,(KCSTR)m_if_b,_cInput);
		return false;
	}
	return true;
}
bool IfParser::m_fnE_PARSE_IF_COND   (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t"))
	{
		if(validCompare((KCSTR)m_if_cond))
		{
			CHANGE(E_PARSE_IF_COND_SP);
		}
		else
		{
			m_result.PRINT("if(%s) illegal condition", (KCSTR)m_if_cond);
			return false;
		}
	}
	else if(BasicParser::MATCH(_cInput,"!=<>"))
	{
		m_if_cond<<_cInput;
	}
	else
	{
		m_result.PRINT("if(%s) illegal compare char(%c)",(KCSTR)m_if_a,_cInput);
		return false;
	}
	return true;
}
bool IfParser::m_fnE_PARSE_IF_COND_SP(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t"))
	{
	}
	else if(BasicParser::MATCH(_cInput,")"))
	{
		m_result.PRINT("if(%s %s) missing right operand",
					   (KCSTR)m_if_a,(KCSTR)m_if_cond);
		return false;
	}
	else
	{
		m_if_c<<_cInput;
		CHANGE(E_PARSE_IF_C);
	}
	return true;
}
bool IfParser::m_fnE_PARSE_IF_C      (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," .\t"))
	{
		CHANGE(E_PARSE_IF_C_SP);
	}
	else if(BasicParser::MATCH(_cInput,")"))
	{
		CHANGE(E_PARSE_END);
	}
	else
	{
		m_if_c<<_cInput;
	}
	return true;
}
bool IfParser::m_fnE_PARSE_IF_C_SP   (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," .\t"))
	{
	}
	else if(BasicParser::MATCH(_cInput,")"))
	{
		CHANGE(E_PARSE_END);
	}
	else
	{
		m_if_d<<_cInput;
		CHANGE(E_PARSE_IF_D);
	}		
	return true;
}
bool IfParser::m_fnE_PARSE_IF_D      (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," )\t"))
	{
		CHANGE(E_PARSE_END);
	}
	else
	{
		m_if_d<<_cInput;
	}
	return true;
}
bool IfParser::m_fnE_PARSE_END      (const char _cInput)
{
	return true;
}
KCSTR IfParser::DEBUGGING(KSTRING & _debug)
{
	_debug.PRINT("%s<%s/%s{%s}%s/%s>\n",
				(KCSTR)m_if,
				(KCSTR)m_if_a,
				(KCSTR)m_if_b,
				(KCSTR)m_if_cond,
				(KCSTR)m_if_c,
				(KCSTR)m_if_d);
	return (KCSTR)_debug;
}
IfParser::EParse_t IfParser::STATE()
{
	return m_eSt;
}
void IfParser::CHANGE(EParse_t _eT)
{
	m_eSt = _eT;
}
void IfParser::m_fnTest()
{
	IfParser mLine;
	KSTRING debug;
	const char * a = "IF(SESSION.CUR_STATE == RCV_DATA.bFlag) ";
	for(KUINT i=0;i<strlen(a);i++) mLine.parsestep(a[i]);
	mLine.DEBUGGING(debug);
	printf("%s ---> %s\n",a,(KCSTR)debug);
	
	const char * b = "IF(TRUE)              ";
	debug = KNULL; mLine.CLEAR();
	for(KUINT i=0;i<strlen(b);i++) mLine.parsestep(b[i]);
	mLine.DEBUGGING(debug);
	printf("%s ---> %s\n",b,(KCSTR)debug);

	const char * c = "IF(SESSION.CUR_STATE >= bFlag)              ";
	debug = KNULL; mLine.CLEAR();
	for(KUINT i=0;i<strlen(c);i++) mLine.parsestep(c[i]);
	mLine.DEBUGGING(debug);
	printf("%s ---> %s\n",c,(KCSTR)debug);

	const char * d = "IF(1234 >= bFlag)              ";
	debug = KNULL; mLine.CLEAR();
	for(KUINT i=0;i<strlen(d);i++) mLine.parsestep(d[i]);
	mLine.DEBUGGING(debug);
	printf("%s ---> %s\n",d,(KCSTR)debug);
	
}
}
