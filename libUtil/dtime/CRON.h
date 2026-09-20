#ifndef __CRON_H
#define __CRON_H
#include "STL.h"
#include "DTIME.h"
#include "THREAD.h"
#include "LOCK.h"
#include "RESTMSG.h"
#include "TIMER.h"
#include <string>
namespace nsUtil
{
class CheckSched
{
	public:
		class job : public StlObject
		{
			public:
				job(){}
				~job(){}
				RestMsg m_data;
				KSTRING m_cronStmt;
		};
		class mon : public THREAD
		{
			public:
				class jobGroup : public StlObject
				{
					public:
						jobGroup(){}
						~jobGroup(){}
						MUTEX m_lock;
						StlList m_jobs;
				};
				mon();
				~mon();
				void INIT(CheckSched * _pOwner);
				void PROC();
				void REG(KCSTR _cronStmt, RestMsg & _msg);
				void DEL(KCSTR _id);
				MUTEX m_groupLock;
				StlList m_groups;
				CheckSched * m_pOwner;
		};
		CheckSched();
		~CheckSched();
		void INIT();
		void REG(KCSTR _cronStmt, RestMsg & _msg);
		void DEL(RestMsg & _msg);
		virtual void JOB(job & _myJob)=0;
		static bool isMatch(KCSTR _cron, TIME & _time);
	private:
		static bool isMatch_(KCSTR _cron,
                  unsigned int _year, unsigned int _mon, unsigned int _day,
                  unsigned int _hour, unsigned int _mins);
		static void checkCronTest();
		mon * m_pMonitor;
};
}
#endif

