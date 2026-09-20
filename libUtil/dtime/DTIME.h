#ifndef _D_TIME_H
#define _D_TIME_H
#include "AF.h"
#include "KSTRING.h"
#include "TIMEUTIL.h" 

namespace nsUtil
{
class TIME 
{
	public:
		TIME();
		TIME(KUINT _nTime);  
		TIME(KULONG _nTimeMs);
		TIME(struct timespec & _rClock);
		~TIME();
		TIME & operator=(TIME & _src);
		TIME & operator=(struct timespec & _rClock);
		void setCurTime();
		KINT Year();
		KINT Mon();
		KINT Week();
		KINT Day();
		KINT Hour();
		KINT Min();
		KINT Sec();
		KINT Msec();
		KCSTR Build();
		bool INSERT(KCSTR _format, KCSTR _strDate);
		KCSTR PRINT(KCSTR _format);
		static KULONG diffTime(TIME & _src,TIME & _dst);
		static KULONG getClockMs();
		static KUINT getTimeSec();
	private:
		KSTRING m_strTime;
		KUINT m_msec;
		time_t m_time;
		struct tm m_local;
		KULONG m_clockTime;
};
}
#endif
