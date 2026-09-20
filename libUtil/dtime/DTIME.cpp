#include "DTIME.h"
#include "TIMER.h"

namespace nsUtil
{
TIME::TIME()
{
	memset(&m_local,0x00,sizeof(m_local));
	m_local.tm_mday  = 1;   
	m_local.tm_isdst = -1;  
	m_time = time(NULL);
	localtime_r(&m_time, &m_local);
	m_clockTime = TIMER::GETMS();
	m_msec = (m_clockTime%1000);
}
TIME::TIME(KUINT _nTime)
{
	memset(&m_local,0x00,sizeof(m_local));
	m_local.tm_mday  = 1;   
	m_local.tm_isdst = -1;  
	m_time = _nTime;
	localtime_r(&m_time, &m_local);
	m_clockTime = _nTime*1000;
	m_msec = 0;
}
TIME::TIME(KULONG _nTimeMs)
{
	memset(&m_local,0x00,sizeof(m_local));
	m_local.tm_mday  = 1;   
	m_local.tm_isdst = -1;  
	m_time = (_nTimeMs/1000);
	localtime_r(&m_time, &m_local);
	m_clockTime = _nTimeMs;
	m_msec = (m_clockTime%1000);
}
TIME::TIME(struct timespec & _rClock)
{
	memset(&m_local,0x00,sizeof(m_local));
	m_local.tm_mday  = 1;   
	m_local.tm_isdst = -1;  
	unsigned long ulSec = (unsigned long)(_rClock.tv_sec*1000);
	unsigned long ulMsec = (unsigned long)(_rClock.tv_nsec/1000000);
	m_time = (KUINT)ulSec;
	localtime_r(&m_time, &m_local);
	m_msec = (KUINT)ulMsec;
}
TIME::~TIME(){}
TIME & TIME::operator=(TIME & _src)
{
	memset(&m_local,0x00,sizeof(m_local));
	m_local.tm_mday  = 1;   
	m_local.tm_isdst = -1;  
	m_time = (KUINT)_src.m_time;
	localtime_r(&m_time, &m_local);
	m_msec = _src.Msec();
	return *this;
}
TIME & TIME::operator=(struct timespec & _rClock)
{
	memset(&m_local,0x00,sizeof(m_local));
	m_local.tm_mday  = 1;   
	m_local.tm_isdst = -1;  
	unsigned long ulSec = (unsigned long)(_rClock.tv_sec*1000);
	unsigned long ulMsec = (unsigned long)(_rClock.tv_nsec/1000000);
	m_time = (KUINT)ulSec;
	localtime_r(&m_time, &m_local);
	m_msec = (KUINT)ulMsec;
	return *this;
}
void TIME::setCurTime()
{
	memset(&m_local,0x00,sizeof(m_local));
	m_local.tm_mday  = 1;   
	m_local.tm_isdst = -1;  
	m_time = time(NULL);
	localtime_r(&m_time, &m_local);
	m_clockTime = TIMER::GETMS();
	m_msec = (m_clockTime%1000);
}
static unsigned long s_ABS(unsigned long _a, unsigned long _b)
{
	if(_a >= _b) return (_a - _b);
	else return (_b - _a);
}
KULONG TIME::diffTime(TIME & _src,TIME & _dst)
{
	return s_ABS(_src.m_clockTime,_dst.m_clockTime);
}
KINT TIME::Year(){return m_local.tm_year +1900;}
KINT TIME::Mon(){return m_local.tm_mon +1;}
KINT TIME::Week(){return m_local.tm_wday+1;} 
KINT TIME::Day(){return m_local.tm_mday;}
KINT TIME::Hour(){return m_local.tm_hour;}
KINT TIME::Min(){return m_local.tm_min;}
KINT TIME::Sec(){return m_local.tm_sec;}
KINT TIME::Msec(){return m_msec;}
KULONG TIME::getClockMs()
{
	return TIMER::GETMS();
}
KUINT TIME::getTimeSec()
{
	return time(NULL);
}
KCSTR TIME::Build()
{
	m_strTime.PRINT("%04u.%02u.%02u %02u.%02u.%02u:%03u",
									Year(),
									Mon(),
									Day(),
									Hour(),
									Min(),
									Sec(),
									Msec());
	return (KCSTR)m_strTime;
}
bool TIME::INSERT(KCSTR _format, KCSTR _strDate)
{
	memset(&m_local,0x00,sizeof(m_local));
	m_local.tm_mday  = 1;   
	m_local.tm_isdst = -1;  
	const char* ret = strptime(_strDate, _format,&m_local);
	 if (ret == NULL) 
	 {        
	 	m_time = (time_t)-1;        
	 	return false;    
	 }
	m_time = mktime(&m_local);
	return true;
}
KCSTR TIME::PRINT(KCSTR _format)
{
	if(m_strTime.LENGTH()==0)
	{
		m_strTime.m_fnReSize(128);
		strftime((KSTR)m_strTime,128,_format,&m_local);
	}
	return (KCSTR)m_strTime;
}
}
