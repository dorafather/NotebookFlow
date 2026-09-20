#include "ASSIGNPARSER.h"
namespace nsUtil
{
AssignParser::AssignParser()
{
	m_eSt = E_PARSE_NONE;
	m_AB.TOKEN()=".";
	m_CD.TOKEN()=".";
}
AssignParser::~AssignParser()
{
}
AssignParser & AssignParser::operator=(AssignParser & _src)
{
	CONSTRUCT((void *)&_src);
	return *this;
}
void AssignParser::CONSTRUCT(void * _pvSrc)
{
	AssignParser * pSrc = (AssignParser*)_pvSrc;
	setkey(pSrc->m_pszKey);
	m_AB = pSrc->m_AB;
	m_CD = pSrc->m_CD;
}
bool AssignParser::PARSE(KCSTR _src)
{
	if(_src==NULL) return false;
	KUINT len = strlen(_src);
	for(KUINT i=0;i<len;i++)
	{
		if(!parsestep((const char)_src[i])) return false;
	}
	m_eSt = E_PARSE_MAX;
	return true;
}
void AssignParser::JSON(RestParam & _item)
{
/*
	{param-key:~~~,param-val:~~~}
*/
	KSTRING a; KSTRING b;
	m_AB.STR(a);
	m_CD.STR(b);
	_item.SET("param-key").VAL() = a;
	_item.SET("param-val").VAL() = b;	
}
void AssignParser::STR(KSTRING & _buf)
{
	KSTRING a; KSTRING b;
	m_AB.STR(a);
	m_CD.STR(b);
	_buf.PRINT("%s = %s\n",(KCSTR)a,(KCSTR)b);
}
void AssignParser::IMPORT(RestParam & _item)
{
	m_AB.IMPORT((KCSTR)_item.GET("param-key").VAL());
	m_CD.IMPORT((KCSTR)_item.GET("param-val").VAL());
	m_eSt = E_PARSE_MAX;
}
bool AssignParser::parsestep(const char _cInput)
{
	switch(m_eSt)
	{
		case E_PARSE_NONE: return m_fnE_PARSE_NONE(_cInput); 
		case E_PARSE_AB: return m_fnE_PARSE_AB(_cInput); 
		case E_PARSE_AB_SP: return m_fnE_PARSE_AB_SP(_cInput); 
		case E_PARSE_COND: return m_fnE_PARSE_COND(_cInput); 
		case E_PARSE_CD:	 return m_fnE_PARSE_CD(_cInput); 
		default: return m_fnE_PARSE_NONE(_cInput); 
	};
	return false;
}
AssignParser::EParse_t AssignParser::STATE()
{
	return m_eSt;
}
void AssignParser::CHANGE(EParse_t _eT)
{
	m_eSt = _eT;
}
bool AssignParser::m_fnE_PARSE_NONE(const char _cInput)
{
	if(!m_AB.parseStep(_cInput))
	{
		m_result = m_AB.m_result;
		return false;
	}
	CHANGE(E_PARSE_AB);
	return true;
}
bool AssignParser::m_fnE_PARSE_AB(const char _cInput)
{
	if(m_AB.STATE() == BasicParser::E_PARSE_VAL)
	{
		if(BasicParser::MATCH(_cInput," \t"))
		{
			CHANGE(E_PARSE_AB_SP);
			return true;
		}
		else if(BasicParser::MATCH(_cInput,"="))
		{
			CHANGE(E_PARSE_COND);
			return true;		
		}
	}
	if(!m_AB.parseStep(_cInput))
	{
		m_result = m_AB.m_result;
		return false;
	}
	return true;
}
bool AssignParser::m_fnE_PARSE_AB_SP(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t"))
	{
	}
	else if(BasicParser::MATCH(_cInput,"="))
	{
		CHANGE(E_PARSE_COND);
		return true;		
	}
	else
	{
		m_result.PRINT("line() illegal char '%c'",_cInput);
		return false;
	}
	return true;
}
bool AssignParser::m_fnE_PARSE_COND(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t"))
	{
	}
	else if(BasicParser::MATCH(_cInput,"="))
	{
		m_result.PRINT("line() duplicated char '%c'",_cInput);
		return false;		
	}
	else
	{
		if(!m_CD.parseStep(_cInput))
		{
			m_result = m_CD.m_result;
			return false;
		}
		CHANGE(E_PARSE_CD);
	}
	return true;
}
bool AssignParser::m_fnE_PARSE_CD(const char _cInput)
{
	if(!m_CD.parseStep(_cInput))
	{
		m_result = m_CD.m_result;
		return false;
	}
	return true;
}
}
