#include "STRCONV.h"

namespace nsUtil
{
/********************** STRCONV****************************/
STRCONV::item::item(){m_eT=E_TYPE_NONE;}
STRCONV::item::~item(){}
void STRCONV::item::CONSTRUCT(void * _pvSrc)
{
	m_eT = ((item*)(_pvSrc))->m_eT;
	m_szData = (KCSTR)((item*)(_pvSrc))->m_szData;
}
void STRCONV::item::DEBUGGING(KSTRING & _buf)
{
	if(m_eT == E_TYPE_DATA)
		_buf.PRINT("type : DATA___, val : [%s]\n",(KCSTR)m_szData);
	else if(m_eT == E_TYPE_KEYWORD)
		_buf.PRINT("type : KEYWORD, val : [%s]\n",(KCSTR)m_szData);
	else
		_buf.PRINT("type : ERROR__, val : [%s]\n",(KCSTR)m_szData);
}
STRCONV::STRCONV()
{
	m_pszOrig=NULL;
	m_unPos=0;
	m_eSt=E_PARAM_IDLE;
	m_bDetecting = false;
	m_pCurItem = NULL;
}
STRCONV::~STRCONV(){}
STRCONV & STRCONV::operator=(STRCONV & _src)
{
	clear();
	Iterator itr;
	item * pFind = (item*)_src.m_listRaw.next(itr);
	while(pFind)
	{
		item * pNew = new item;
		*pNew = *pFind;
		m_listRaw.pushback(pNew);
		pFind = (item*)_src.m_listRaw.next(itr);
	}
	return *this;
}
void STRCONV::clear()
{
	m_pszOrig = NULL;
	m_bDetecting = false;
	m_clsBeginTmp = KNULL;
	m_unPos=0;
	m_eSt = E_PARAM_IDLE;
	m_pCurItem = NULL;
	m_listRaw.clear();
}
bool STRCONV::PARSE(KCSTR _pszData)
{
	if(_pszData==NULL) return false;
	clear();
	m_pszOrig = (char*)_pszData;
	bool bBreak = false;
	while(1)
	{
		if(m_eSt == E_PARAM_IDLE) bBreak= m_fnParseIdle();
		else if(m_eSt == E_PARAM_BEGIN) bBreak = m_fnParseBegin();
		else if(m_eSt == E_PARAM_END) bBreak = m_fnParseEnd();
		if(bBreak == false) break;
	}
	return m_bDetecting;
}
bool STRCONV::ISCONV()
{
	return m_bDetecting;
}
KUINT STRCONV::KEYNUMS()
{
	Iterator itr; KUINT cnt=0;
	item * pFind = (item*)m_listRaw.next(itr);
	while(pFind)
	{
		if(pFind->m_eT == item::E_TYPE_KEYWORD) cnt++;
		pFind = (item*)m_listRaw.next(itr);
	}
	return cnt;
}
KCSTR STRCONV::STR(ALIST & _param, KSTRING & _buf)
{
	Iterator itr;
	item * pFind = (item*)m_listRaw.next(itr);
	while(pFind)
	{
		if(pFind->m_eT == item::E_TYPE_DATA)
		{
			_buf<<pFind->m_szData;
		}
		else if(pFind->m_eT == item::E_TYPE_KEYWORD)
		{
			KSTRING & getItem = _param.GET((KCSTR)pFind->m_szData).VAL();
			if(getItem.LENGTH()>0)
			{
				_buf<<getItem;  
			}
			else
			{
				_buf<<"$$$";
				_buf<<pFind->m_szData; 
				_buf<<"$$$";
			}
		}
		pFind = (item*)m_listRaw.next(itr);
	}
	return (KCSTR)_buf;
}
KCSTR STRCONV::STR_(KCSTR _replaceWord, KSTRING & _buf)
{
	Iterator itr;
	item * pFind = (item*)m_listRaw.next(itr);
	while(pFind)
	{
		if(pFind->m_eT == item::E_TYPE_DATA)
		{
			_buf<<pFind->m_szData;
		}
		else if(pFind->m_eT == item::E_TYPE_KEYWORD)
		{
			_buf<<_replaceWord;
		}
		pFind = (item*)m_listRaw.next(itr);
	}
	return (KCSTR)_buf;
}
void STRCONV::LIST(ALIST & _getList)
{
	Iterator itr;
	item * pFind = (item*)m_listRaw.next(itr);
	while(pFind)
	{
		if(pFind->m_eT == item::E_TYPE_KEYWORD)
		{
			_getList.SET(pFind->m_szData).VAL() =pFind->m_szData;
		}
		pFind = (item*)m_listRaw.next(itr);
	}
}
void STRCONV::LISTUP(ALIST & _getList)
{
	Iterator itr;
	item * pFind = (item*)m_listRaw.next(itr);
	while(pFind)
	{
		if(pFind->m_eT == item::E_TYPE_KEYWORD)
		{
			KSTRING & val = _getList.SET((KCSTR)pFind->m_szData).VAL();
			val = pFind->m_szData;
		}
		pFind = (item*)m_listRaw.next(itr);
	}
}
KCSTR STRCONV::DEBUGGING(KSTRING & _buf)
{
	Iterator itr;
	item * pFind = (item*)m_listRaw.next(itr);
	while(pFind)
	{
		pFind->DEBUGGING(_buf);
		pFind = (item*)m_listRaw.next(itr);
	}
	return (KCSTR)_buf;
}
void STRCONV::test()
{
	STRCONV test;
	ALIST param;
	param.SET("key1").VAL() = "aa";
	param.SET("key2").VAL() = "bb";
	KCSTR sztest = "$$$key1$$123456789$$$$$$key1$$$$$$key2$$$123456789$$$key3$$$$$$key1$$";
	printf("ORG_ : %s\n",sztest);
	printf("KEY_ : key1=aa, key2=bb\n");
	test.PARSE(sztest);
	KSTRING buf;
	printf("LIST : \n%s\n",(KCSTR)test.DEBUGGING(buf));
	buf = KNULL;
	printf("CONV : %s\n",(KCSTR)test.STR(param,buf));
}
bool STRCONV::m_fnParseIdle()
{
	if(m_pszOrig[m_unPos]==0x00) return false;
	if(m_fnDetecting(&m_pszOrig[m_unPos]))
	{
		m_unPos+=3;
		m_clsBeginTmp = KNULL;
		m_eSt=E_PARAM_BEGIN;
		m_pCurItem = NULL;
	}
	else
	{
		if(m_pCurItem == NULL)
		{
			m_pCurItem = new item;
			m_pCurItem->m_eT = item::E_TYPE_DATA;
			m_listRaw.pushback(m_pCurItem);
		}
		m_pCurItem->m_szData<<(KUCHR)m_pszOrig[m_unPos++];
	}
	 return true;
}
bool STRCONV::m_fnParseBegin()
{
	if(m_pszOrig[m_unPos]==0x00) 
	{
		item * pItem = new item;
		pItem->m_eT = item::E_TYPE_DATA;
		m_listRaw.pushback(pItem);
		pItem->m_szData<<"$$$";
		pItem->m_szData<<m_clsBeginTmp;
		m_eSt=E_PARAM_IDLE;
		return false;
	}
	if(m_fnDetecting(&m_pszOrig[m_unPos]))
	{
		m_unPos+=3;
		item * pItem = new item;
		pItem->m_eT = item::E_TYPE_KEYWORD;
		m_listRaw.pushback(pItem);
		pItem->m_szData<<m_clsBeginTmp;
		m_clsBeginTmp = KNULL;
		m_eSt=E_PARAM_END;
		if(m_bDetecting==false) m_bDetecting = true;
	}
	else
	{
		m_clsBeginTmp<<(KUCHR)m_pszOrig[m_unPos++];
	}
	return true;
}
bool STRCONV::m_fnParseEnd()
{
	if(m_pszOrig[m_unPos]==0x00) 
	{
		m_eSt=E_PARAM_IDLE;
		return false;
	}
	if(m_fnDetecting(&m_pszOrig[m_unPos]))
	{
		m_unPos+=3;
		m_eSt=E_PARAM_BEGIN;
	}
	else
	{
		if(m_pCurItem == NULL)
		{
			m_pCurItem = new item;
			m_pCurItem->m_eT = item::E_TYPE_DATA;
			m_listRaw.pushback(m_pCurItem);
		}
		m_pCurItem->m_szData<<(KUCHR)m_pszOrig[m_unPos++];
		m_eSt=E_PARAM_IDLE;
	}
	return true;
}
bool STRCONV::m_fnDetecting(const char * _pszConfig)
{
	if(_pszConfig==NULL) return false;
	if(_pszConfig[0] == '$' && _pszConfig[1] == '$' && _pszConfig[2] == '$') return true;
	return false;
}
}
