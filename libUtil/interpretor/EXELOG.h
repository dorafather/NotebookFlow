#ifndef LOG_STATE_H
#define LOG_STATE_H
#include "LOGPARSER.h"
#include "DTIME.h"
#include "MATRIX.h"

namespace nsUtil
{
class ExeLog
{
	public:
		ExeLog(LogParser & _dsl);
		~ExeLog();
		bool EXE(QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _rcvMsg);
		LogParser * m_dsl;
};
}
#endif
