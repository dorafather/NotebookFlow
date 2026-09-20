#ifndef EXE_PROC_H
#define EXE_PROC_H
#include "PROCPARSER.h"

namespace nsUtil
{
class ExeProc
{
	public:
		ExeProc(ProcParser & _dsl);
		~ExeProc();
		bool EXE(QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _rcvMsg);
		ProcParser * m_dsl;
};
}
#endif
