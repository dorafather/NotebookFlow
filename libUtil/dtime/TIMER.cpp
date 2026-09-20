#include "TIMER.h"

namespace nsUtil
{
TIMER::TIMER()
{
	m_pclsTimer = NULL;
}
TIMER::~TIMER()
{
	delete m_pclsTimer;
}
void TIMER::RUN(const char * _pszName)
{
	m_pclsTimer= new TimerTable<TimerInfo>(m_fnCbkTimeOut,_pszName);
}
void TIMER::SET(ARG & _rclsData)
{
	TimerInfo * pclsNew = new TimerInfo();
	pclsNew->m_pclsTIMER = this;
	unsigned int unTimeOut = (KUINT)_rclsData.GET(API_P_TIME).VAL();
	pclsNew->m_clsArg<<_rclsData;
	pclsNew->m_clsArg.SET(API_P_EVENT).VAL() = API_E_TIMER;
	m_pclsTimer->setTimer(pclsNew,unTimeOut);
}
KULONG TIMER::GETMS()
{
	struct timespec stCurrentTs;
	clock_gettime(CLOCK_REALTIME, &stCurrentTs);
	return (KULONG)stCurrentTs.tv_sec*1000 + (KULONG)(stCurrentTs.tv_nsec/1000000);
}
void TIMER::m_fnCbkTimeOut(TimerInfo * _pclsData)
{	
	if(_pclsData->m_pclsTIMER)
	{
		if(_pclsData->m_clsArg.GET(API_P_TIMER_OPTION).VAL() == API_P_TIMER_PERIODIC)
		{
			ARG arg; arg = _pclsData->m_clsArg;
			_pclsData->m_pclsTIMER->SET(arg);
		}
		_pclsData->m_pclsTIMER->TIMEOUT(_pclsData->m_clsArg);
	}
	delete _pclsData;
}
}