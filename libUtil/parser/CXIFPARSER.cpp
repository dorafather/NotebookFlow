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
	// claude-code 2026-09-27 (dorafather 승인): 한글 "또는"(OR)이 다중조건
	// 구분자로 전혀 인식되지 않던 버그 수정. "또는"의 UTF-8 첫 바이트는
	// 0xEB인데 원래 코드는 0xEC(어느 한글 단어에도 해당하지 않는 값)를
	// 검사하고 있었다 - "그리고"(0xEA)만 걸리고 "또는"은 이 목록 어디에도
	// 없어, 만약에(A) 또는(B)를 쓰면 그 즉시 파싱이 실패하며 그 지점부터
	// 파일 끝까지 처리::/전송:: 등록이 절반 가까이 누락되는 심각한 사고로
	// 이어졌다(rest.sce 실측 재현 - KRX/KMA/KECO 관심목록 addr.ini 영구
	// 저장 기능 검증 중 발견, CLAUDE.md 1.19절 참고). 실제 값(0xEB)으로
	// 교체.
#if 1
	if(uc == 0xEB || uc == 0xEA || uc == 'O' || uc == 'A')
#else
	if(uc == 0xEC || uc == 0xEA || uc == 'O' || uc == 'A')
#endif
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
