#ifndef DEF_GPOLLING_HXX
#define DEF_GPOLLING_HXX
#include "TIMERTHREAD.h"
#include "KSTRING.h"

namespace nsUtil
{
class Gpolling
{
	public:
		class info;
		typedef void (*PFuncGTimeout)(info * _info);
		class info
		{
			public:
				info(){m_pOwner=NULL;m_pfn=NULL;}
				~info(){}
				KSTRING m_type;
				KUINT m_unTimeout;
				void * m_pOwner;
				PFuncGTimeout m_pfn;
		};
		Gpolling();
		~Gpolling();
		static Gpolling & OBJ();
		static void setTimer(void * _pUser, KCSTR _type, 
							KUINT _timeout, PFuncGTimeout _pfn);
	private:
		void setTimerObj(info * _pInfo);
		static void timeout(info *_arg);
		static Gpolling * m_pInst;
		static TimerTable<info>  * m_pTimer;
};
}
#endif
