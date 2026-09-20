#include "GPOLLING.h"

namespace nsUtil
{
Gpolling * Gpolling::m_pInst = NULL;
TimerTable<Gpolling::info>  * Gpolling::m_pTimer = NULL;
Gpolling::Gpolling()
{
	if(m_pTimer==NULL)
		m_pTimer= new TimerTable<info>(timeout,"Gpolling");
}
Gpolling::~Gpolling(){}
Gpolling & Gpolling::OBJ()
{
	if(m_pInst == NULL)
	{
		m_pInst = new Gpolling;
	}
	return *m_pInst;
}
void Gpolling::setTimer(void * _pUser, KCSTR _type, KUINT _timeout, PFuncGTimeout _pfn)
{
	info * pstTime = new info;
	pstTime->m_pOwner = _pUser;
	pstTime->m_type = _type;
	pstTime->m_pfn = _pfn;
	pstTime->m_unTimeout = _timeout;
	m_pTimer->setTimer(pstTime,_timeout);
}
void Gpolling::setTimerObj(info * _pInfo)
{
	info * pstTime = new info;
	pstTime->m_pOwner = _pInfo->m_pOwner;
	pstTime->m_type = _pInfo->m_type;
	pstTime->m_pfn = _pInfo->m_pfn ;
	pstTime->m_unTimeout = _pInfo->m_unTimeout;
	m_pTimer->setTimer(pstTime,pstTime->m_unTimeout);
}
void Gpolling::timeout(info *_arg)
{
	if(_arg->m_pfn) _arg->m_pfn(_arg);
	m_pInst->setTimerObj(_arg);
	delete _arg;
}
}