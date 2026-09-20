#ifndef _TIMER_H
#define _TIMER_H
#include "TIMERTHREAD.h"
#include "ARGV.h"

namespace nsUtil
{
class TIMER;
class TimerInfo
{
	public:
		TimerInfo()
		{
			m_pclsTIMER=NULL;
		}
		~TimerInfo(){}
		TIMER * m_pclsTIMER;      
		ARG m_clsArg;   
};
class TIMER
{
	public:
		TIMER();
		virtual ~TIMER();
		void RUN(const char * _pszName);
		void SET(ARG & _rclsData);
		virtual void TIMEOUT(ARG & _rclsData)=0;
		static KULONG GETMS();
	private:
		static void m_fnCbkTimeOut(TimerInfo * _pclsData);  
		KSTRING m_szName;
		TimerTable<TimerInfo>  *m_pclsTimer;
};
}
#endif
