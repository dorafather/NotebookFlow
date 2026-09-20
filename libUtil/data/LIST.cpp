#include "LIST.h"

namespace nsUtil
{
ALIST::Kobj ALIST::m_clsNull;
ALIST::Kobj::Kobj(){}
ALIST::Kobj::~Kobj(){}
ALIST::Kobj & ALIST::Kobj::operator=(Kobj & _rSrc)
{
	SET(_rSrc.m_pszKey);
	m_val = _rSrc.m_val;
	return *this;
}
void ALIST::Kobj::SET(KCSTR _key)
{
	setkey(_key);
	m_key = _key;
}
KSTRING & ALIST::Kobj::KEY()
{
	return m_key;
}
KSTRING & ALIST::Kobj::VAL()
{
	return m_val;
}
KCSTR ALIST::Kobj::DEBUGGING(KSTRING & _debug)
{
	_debug.PRINT("(%s:%s) ",(KCSTR)m_key,(KCSTR)m_val);
	return _debug;
}
ALIST::ALIST()
{
}
ALIST::ALIST(ALIST & _src)
{
	setkey(_src.m_pszKey);
	CLEAR();
	Iterator itr;
	Kobj * pFind = (Kobj *)_src.m_list.next(itr);
	while(pFind)
	{
		Kobj * pNew = new Kobj;
		*pNew = *pFind;
		m_list.pushback(pNew);
		pFind = (Kobj *)_src.m_list.next(itr);
	}
}
ALIST::~ALIST()
{
}
ALIST & ALIST::operator=(ALIST & _src)
{
	CLEAR();
	setkey(_src.m_pszKey);
	Iterator itr;
	Kobj * pFind = (Kobj *)_src.m_list.next(itr);
	while(pFind)
	{
		Kobj * pNew = new Kobj;
		*pNew = *pFind;
		m_list.pushback(pNew);
		pFind = (Kobj *)_src.m_list.next(itr);
	}
	return *this;
}
ALIST::Kobj &  ALIST::operator[](KUINT _unIdx)
{
	Kobj * pFind = (Kobj*)m_list.index(_unIdx);
	if(pFind == NULL) return m_clsNull;
	return *pFind;
}
ALIST::Kobj & ALIST::SET(KSTRING _szKey)
{
	Kobj * pFind = (Kobj*)m_list.find((KCSTR)_szKey);
	if(pFind)
	{
		return *pFind;
	}
	Kobj * pNew = new Kobj;
	pNew->SET((KCSTR) _szKey);
	m_list.pushback(pNew);
	return *pNew;
}
ALIST::Kobj & ALIST::GET(KSTRING _szKey)
{
	Kobj * pFind = (Kobj*)m_list.find((KCSTR)_szKey);
	if(pFind == NULL) return m_clsNull;
	return *pFind;	
}
ALIST::Kobj & ALIST::PUSH()
{
	Kobj * pNew = new Kobj;
	pNew->SET("1");
	m_list.pushback(pNew);
	return *pNew;
}
ALIST::Kobj & ALIST::FRONT()
{
	Kobj  * pNew = new Kobj ;
	pNew->SET("1");
	m_list.pushfront(pNew);
	return *pNew;
}
bool ALIST::POP(KSTRING & _get)
{
	Kobj * pFind = (Kobj*)m_list.index(0);
	if(pFind == NULL) return false;
	_get = pFind->VAL();
	m_list.delhead();
	return true;
}
void ALIST::DEL(KSTRING _szKey)
{
	m_list.del((KCSTR)_szKey);
}
KUINT ALIST::NUMS()
{
	return m_list.size();
}
void ALIST::CLEAR()
{
	m_list.clear();
}
KSTR ALIST::DEBUGGING(KSTRING & _debug)
{
	_debug<<"<";
	Iterator itr;
	Kobj * pFind = (Kobj *)m_list.next(itr);
	while(pFind)
	{
		pFind->DEBUGGING(_debug);
		pFind = (Kobj *)m_list.next(itr);
	}
	_debug<<">";
	return _debug;
}
}

