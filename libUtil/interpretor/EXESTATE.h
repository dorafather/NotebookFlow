#ifndef EXE_STATE_H
#define EXE_STATE_H
#include "STATEPARSER.h"

namespace nsUtil
{
class ExeState
{
	public:
		ExeState(StateParser & _dsl);
		~ExeState();
		bool EXE(QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _rcvMsg);
		StateParser * m_dsl;
};
}
#endif
