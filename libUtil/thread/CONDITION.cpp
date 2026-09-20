#ifndef _MSC_VER
#include <sys/time.h>
#endif
#include "CONDITION.h"
#include "LOCK.h"

namespace nsUtil
{
Condition::Condition()
{
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
	
	int nRet = pthread_cond_init(&m_ConditionId, &attr);
	if( nRet != 0 )
	{
	}
}
Condition::~Condition()
{
	pthread_cond_destroy(&m_ConditionId);
}
void Condition::wait(Mutex & _rclsMutex)
{
	 int nRet = pthread_cond_wait(&m_ConditionId, _rclsMutex.getMutexId());
	 if( nRet != 0 )
 	{
 	}
}
bool Condition::wait(Mutex & _rclsMutex, unsigned int _unMs)
{
	if( _unMs == 0 )
	{
		wait(_rclsMutex);
		return true;
	}
	struct timespec stExpireTs;
	if( clock_gettime(CLOCK_MONOTONIC, &stExpireTs) != 0)
		return false;
	stExpireTs.tv_sec += _unMs / 1000;
	stExpireTs.tv_nsec += (long)(_unMs % 1000) * 1000000L;
	if( stExpireTs.tv_nsec >= 1000000000L )
	{
		stExpireTs.tv_sec++;
		stExpireTs.tv_nsec -= 1000000000L;
	}
	int nRet = pthread_cond_timedwait(&m_ConditionId, _rclsMutex.getMutexId(), &stExpireTs);
	if( nRet == 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}
void Condition::signal()
{
	int nRet = pthread_cond_signal(&m_ConditionId);
	if( nRet != 0 )
	{
	}
}
void Condition::broadcast()
{
	pthread_cond_broadcast(&m_ConditionId);
}
}

