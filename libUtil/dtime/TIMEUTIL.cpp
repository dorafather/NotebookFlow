#ifdef _MSC_VER
#include "PosixCompat.h"
#ifndef CLOCK_THREAD_CPUTIME_ID
#define CLOCK_THREAD_CPUTIME_ID 2
#endif
#else
#include <sys/syscall.h>
#endif
#include "TIMEUTIL.h"
#include "KSTRING.h"

namespace nsUtil
{
struct timespec subTimespec(const struct  timespec & time1, const struct  timespec & time2)
{
	struct timespec stResultTime;

	if( time1.tv_sec < time2.tv_sec
		|| ((time1.tv_sec == time2.tv_sec) && 	(time1.tv_nsec <= time2.tv_nsec)))
	{
		stResultTime.tv_sec = stResultTime.tv_nsec = 0;
	}
	else
	{
		stResultTime.tv_sec = time1.tv_sec - time2.tv_sec;
		if( time1.tv_nsec < time2.tv_nsec )
		{
			stResultTime.tv_nsec = time1.tv_nsec + 1000000000L - time2.tv_nsec;
			stResultTime.tv_sec--;
		}
		else
		{
			stResultTime.tv_nsec = time1.tv_nsec - time2.tv_nsec;
		}
	}
	return stResultTime;
}
const char * getTimeStr(const struct timespec & _rstTs, char * _pszStr, unsigned int _unMaxLen, bool _bInitial)
{
	if( !_pszStr || _unMaxLen == 0 )
		return "";
	if( _bInitial )
		_pszStr[0] = 0x00;
	struct tm stTm;
	localtime_r(&(_rstTs.tv_sec), &stTm);
	strncat(_pszStr, _unMaxLen, "%4d%02d%02d-%02d:%02d:%02d.%03d", 
														stTm.tm_year+1900, stTm.tm_mon+1, stTm.tm_mday, stTm.tm_hour, stTm.tm_min, stTm.tm_sec, _rstTs.tv_nsec/1000000);
	return _pszStr;
}
SyncTime::SyncTime()
{
	reset();
}
SyncTime::~SyncTime()
{
}
void SyncTime::setTime(const struct timespec & _rstSetTime)
{
	if( m_cWriteFlag == 0 )
	{
		m_stTimeA = _rstSetTime;
		m_cWriteFlag = 1;
	}
	else
	{
		m_stTimeB = _rstSetTime;
		m_cWriteFlag = 0;
	}
}
struct timespec SyncTime::setTime(unsigned long _ulAfterMilliSec)
{
	unsigned long ulAfterSec = _ulAfterMilliSec / 1000;
	unsigned long long ullAfterNanoSec = (_ulAfterMilliSec%1000)*1000*1000;

	if( m_cWriteFlag == 0 )
	{
		clock_gettime(CLOCK_REALTIME, &m_stTimeA);
		m_stTimeA.tv_sec += ulAfterSec;
		m_stTimeA.tv_nsec += ullAfterNanoSec;
		m_stTimeA.tv_sec += m_stTimeA.tv_nsec / (1000*1000*1000);
		m_stTimeA.tv_nsec = m_stTimeA.tv_nsec % (1000*1000*1000);
		m_cWriteFlag = 1;
		return m_stTimeA;
	}
	else
	{
		clock_gettime(CLOCK_REALTIME, &m_stTimeB);
		m_stTimeB.tv_sec += ulAfterSec;
		m_stTimeB.tv_nsec += ullAfterNanoSec;
		m_stTimeB.tv_sec += m_stTimeB.tv_nsec / (1000*1000*1000);
		m_stTimeB.tv_nsec = m_stTimeB.tv_nsec % (1000*1000*1000);
		m_cWriteFlag = 0;
		return m_stTimeB;
	}
}
unsigned int DelayTest::m_arrMaxDiffTime[DelayTest::E_CONST_MAX_KIND_SIZE];
unsigned int DelayTest::m_arrCnt[DelayTest::E_CONST_MAX_KIND_SIZE];
bool DelayTest::m_bUse = false;
void DelayTest::setStartDelayTest(const char * _pszDesc)
{
	m_bStartFlag = false;
	
	if( m_bUse )
	{
		reset();
		if( _pszDesc )
		{
			strncpy(m_szDesc, _pszDesc, DelayTest::E_CONST_MAX_DESC_LEN-1);
			m_szDesc[DelayTest::E_CONST_MAX_DESC_LEN-1] = 0x00;
		}
		m_bStartFlag = true;
		clock_gettime(CLOCK_MONOTONIC, &m_stStartRealTimespec);
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &m_stStartCpuTimespec);
	}
}
void DelayTest::setEndDelayTest(unsigned int _unKind, unsigned int _unResetCnt, unsigned int _unLimitDelayMs)
{
	if( m_bStartFlag )
	{	
		if( _unKind < 0 || _unKind >= E_CONST_MAX_KIND_SIZE )
			return;

		if( m_arrCnt[_unKind] >= _unResetCnt )
		{
			m_arrMaxDiffTime[_unKind] = 0;
			m_arrCnt[_unKind] = 0;
		}
		clock_gettime(CLOCK_MONOTONIC, &m_stEndRealTimespec);
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &m_stEndCpuTimespec);
		struct timespec stRealDiffTimespec = subTimespec(m_stEndRealTimespec, m_stStartRealTimespec);
		unsigned int unRealDiff = stRealDiffTimespec.tv_sec*1000+stRealDiffTimespec.tv_nsec/1000000;
		if( unRealDiff > m_arrMaxDiffTime[_unKind] || unRealDiff > _unLimitDelayMs )
		{
			if( unRealDiff > m_arrMaxDiffTime[_unKind] )
				m_arrMaxDiffTime[_unKind] = unRealDiff;
		}
		++(m_arrCnt[_unKind]);
	}
}
void g_fnMilliSleep(unsigned int _unMsec)
{
	#ifdef _MSC_VER
	Sleep(_unMsec);
	#else
	#ifdef USING_NANOSLEEP
	timespec req;
	req.tv_sec = 0;
	req.tv_nsec = _unMsec * 1000000;
	nanosleep(&req, NULL);
	#else
	struct timeval val;
	val.tv_sec = _unMsec / 1000;
	val.tv_usec = (_unMsec % 1000) * 1000;
	select(0, 0, 0, 0, &val);
	#endif
	#endif
}
}
