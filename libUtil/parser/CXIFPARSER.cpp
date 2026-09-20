#include "CXIFPARSER.h"
#include "BASICPARSER.h"
namespace nsUtil
{
CxIfParser::CxIfParser()
{
	m_eSt = E_PARSE_NONE;
	m_curIf=NULL;
}
CxIfParser::~CxIfParser()
{
}
CxIfParser & CxIfParser::operator=(CxIfParser & _src)
{
	CONSTRUCT((void *)&_src);
	return *this;
}
IfParser & CxIfParser::operator[](KUINT _idx)
{
	IfParser * pv = (IfParser*)m_listIf.index(_idx);
	if(pv == NULL)
	{
		return m_defIf;
	}
	return *pv;
}
void CxIfParser::CONSTRUCT(void * _pvSrc)
{
	CxIfParser * psrc = (CxIfParser*)_pvSrc;
	m_listIf.clear();
	Iterator itr;
	IfParser * pFind = (IfParser *)psrc->m_listIf.next(itr);
	while(pFind)
	{
		IfParser * pNew = new IfParser;
		*pNew = *pFind;
		m_listIf.pushback(pNew);
		pFind = (IfParser *)psrc->m_listIf.next(itr);
	}
}
CxIfParser::EParse_t CxIfParser::STATE()
{
	return m_eSt;
}
bool CxIfParser::PARSE(KCSTR _src)
{
	if(_src==NULL) return false;
	KUINT len = strlen(_src);
	for(KUINT i=0;i<len;i++)
	{
		if(!parsestep((const char)_src[i])) return false;
	}
	if(m_eSt != E_PARSE_COND_SP)
	{
		m_result.PRINT("if multi-condition incomplete expression, stopped at state(%d)",
					   (int)m_eSt);
		return false;
	}
	m_eSt = E_PARSE_MAX;
	return true;
}
void CxIfParser::JSON(RestParam & _item)
{
	Iterator itr;
	IfParser * pFind = (IfParser *)m_listIf.next(itr);
	while(pFind)
	{
		pFind->JSON(_item);
		pFind = (IfParser *)m_listIf.next(itr);
	}
}
void CxIfParser::STR(KSTRING & _buf)
{
	Iterator itr;
	IfParser * pFind = (IfParser *)m_listIf.next(itr);
	while(pFind)
	{
		pFind->STR(_buf);
		pFind = (IfParser *)m_listIf.next(itr);
	}
	_buf<<"\n";
}
void CxIfParser::IMPORT(RestParam & _item)
{
	m_listIf.clear();
	IfParser * pNew = new IfParser;
	pNew->IMPORT(_item);
	m_listIf.pushback(pNew);
}
bool CxIfParser::parsestep(const char _cInput)
{
	switch(m_eSt)
	{
		case E_PARSE_NONE: return m_fnE_PARSE_NONE(_cInput); 
		case E_PARSE_COND: return m_fnE_PARSE_COND(_cInput); 
		case E_PARSE_COND_SP: return m_fnE_PARSE_COND_SP(_cInput); 
		default: return m_fnE_PARSE_NONE(_cInput); 
	};
	return false;
}
KSTRING & CxIfParser::TYPE()
{
	Iterator itr;
	IfParser *pIf = (IfParser *)m_listIf.next(itr);
	if(pIf == NULL)
	{
		return m_def;
	}
	return pIf->m_if;
}
KUINT CxIfParser::NUMS(){return m_listIf.size();}
KCSTR CxIfParser::DEBUGGING(KSTRING & _buf)
{
	Iterator itr;
	IfParser * pFind = (IfParser *)m_listIf.next(itr);
	while(pFind)
	{
		pFind->DEBUGGING(_buf);
		pFind = (IfParser *)m_listIf.next(itr);
	}
	return (KCSTR)_buf;
}
void CxIfParser::CHANGE(EParse_t _eT)
{
	m_eSt = _eT;
}
bool CxIfParser::m_fnE_PARSE_NONE(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t\r\n"))
	{
		// skipp
	}
	else
	{
		m_curIf = new  IfParser;
		if(!m_curIf->parsestep(_cInput))
		{
			m_result = m_curIf->m_result;
			return false;
		}
		m_listIf.pushback(m_curIf);
		CHANGE(E_PARSE_COND);
	}
	return true;
}
bool CxIfParser::m_fnE_PARSE_COND(const char _cInput)
{
	if(m_curIf==NULL)
	{
		m_result.PRINT("multi compare error");
		return false;
	}
	if(!m_curIf->parsestep(_cInput))
	{
		m_result = m_curIf->m_result;
		return false;
	}
	if(m_curIf->STATE() == IfParser::E_PARSE_END)
	{
		if(!m_curIf->valid())
		{
			m_result.PRINT("if(%s) illegal param (%s.%s)/(%s.%s)",
							(KCSTR)m_curIf->m_if_a,
							(KCSTR)m_curIf->m_if_a,
							(KCSTR)m_curIf->m_if_b,
							(KCSTR)m_curIf->m_if_c,
							(KCSTR)m_curIf->m_if_d);
			return false;
		}
		CHANGE(E_PARSE_COND_SP);
	}
	return true;
}
bool CxIfParser::m_fnE_PARSE_COND_SP(const char _cInput)
{
	unsigned char uc = (unsigned char)_cInput;
	if(uc == 0xEC || uc == 0xEA || uc == 'O' || uc == 'A')
	{
		m_curIf = new  IfParser;
		if(!m_curIf->parsestep(_cInput))
		{
			m_result = m_curIf->m_result;
			return false;
		}
		m_listIf.pushback(m_curIf);
		CHANGE(E_PARSE_COND);
	}
	else if(!BasicParser::MATCH(_cInput," "))
	{
		m_result.PRINT("if(-) illegal multi compare char '%c'",_cInput);
		return false;
	}
	return true;
}
void CxIfParser::m_fnTest()
{
	const char m_test[]="IF(a.11 == b.22) OR (c.33 == d) AND (e.55 == f.66)";
	CxIfParser mLine;
	for(KUINT i=0; i<strlen(m_test);i++)
		mLine.parsestep(m_test[i]);
	KSTRING tmp;
	mLine.DEBUGGING(tmp);
	printf("Org Cx IF : %s\n",m_test);
	printf("%s",(KCSTR)tmp);
}
}
