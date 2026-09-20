#ifndef _TIME_UTIL_H
#define _TIME_UTIL_H

#ifndef _MSC_VER
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
extern "C" 
{
	#include <time.h>
}
#include <string.h>
#else
#include <string.h>
#include <ctime>
#endif
namespace nsUtil
{
struct timespec subTimespec(const struct  timespec & time1, const struct  timespec & time2);
const char * getTimeStr(const struct timespec & _rstTs, char * _pszStr, unsigned int _unMaxLen, bool _bInitial = true);
class SyncTime
{
	public :
		SyncTime();
		~SyncTime();
		void setTime(const struct timespec & _rstSetTime);
		struct timespec setTime(unsigned long _ulAfterMilliSec = 0);
		inline void clearTime()
		{
			if( m_cWriteFlag == 0 )
			{
				memset(&m_stTimeA, 0x00, sizeof(struct timespec));
				m_cWriteFlag = 1;
			}
			else
			{
				memset(&m_stTimeB, 0x00, sizeof(struct timespec));
				m_cWriteFlag = 0;
			}
		}
		inline const struct timespec & getTime() const
		{
			if( m_cWriteFlag == 0 )
				return m_stTimeB;
			else
				return m_stTimeA;
		}
		inline void reset()
		{
			memset(&m_stTimeA, 0x00, sizeof(struct timespec));
			memset(&m_stTimeB, 0x00, sizeof(struct timespec));
			m_cWriteFlag = 0;
		}
	private :
		struct timespec m_stTimeA;
		struct timespec m_stTimeB;
		char m_cWriteFlag;		
		
};
struct DelayTest
{
	enum
	{
		E_CONST_MAX_DESC_LEN = 50,
		E_CONST_MAX_KIND_SIZE = 100,
		E_CONST_RESET_CNT = 1000000,
	};
	static unsigned int m_arrMaxDiffTime[E_CONST_MAX_KIND_SIZE];
	static unsigned int m_arrCnt[E_CONST_MAX_KIND_SIZE];
	static bool m_bUse;
	bool m_bStartFlag;
	char m_szDesc[E_CONST_MAX_DESC_LEN];
	struct timespec m_stStartRealTimespec;
	struct timespec m_stEndRealTimespec;
	struct timespec m_stStartCpuTimespec;
	struct timespec m_stEndCpuTimespec;
	void reset()
	{
		m_bStartFlag = false;
		m_szDesc[0] = 0x00;
		memset(&m_stStartRealTimespec, 0x00, sizeof(struct timespec));
		memset(&m_stEndRealTimespec, 0x00, sizeof(struct timespec));
		memset(&m_stStartCpuTimespec, 0x00, sizeof(struct timespec));
		memset(&m_stEndCpuTimespec, 0x00, sizeof(struct timespec));
	}
	DelayTest()
	{
		reset();
	}
	void setStartDelayTest(const char * _pszDesc);
	void setEndDelayTest(unsigned int _unKind, unsigned int _unResetCnt, unsigned int _unLimitDelayMs = 100);
};
void g_fnMilliSleep(unsigned int _unMsec);
}

#endif

