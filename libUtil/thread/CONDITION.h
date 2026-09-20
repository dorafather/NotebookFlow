#ifndef _CONDITION_H
#define _CONDITION_H
#ifdef _MSC_VER
#include "PosixCompat.h"
#else
#include <pthread.h>
#endif

namespace nsUtil
{
class Mutex;
class Condition
{
	public:
		Condition();
		virtual ~Condition();
		void wait(Mutex & _rclsMutex);
		bool wait(Mutex & _rclsMutex, unsigned int _unMs);
		void signal();
		void broadcast();

	private:
		mutable  pthread_cond_t m_ConditionId;

};
}
#endif
