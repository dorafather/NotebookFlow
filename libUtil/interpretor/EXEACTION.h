#ifndef EXE_ACTION_H
#define EXE_ACTION_H
#include "ACTIONPARSER.h"

namespace nsUtil
{
class ExeAction
{
	public:
		ExeAction(ActionParser & _dsl);
		~ExeAction();
		bool EXE(QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _rcvMsg);
		void SEND(QTHREAD & _wk,
					POOL::POOLDATA & _rPool,
					RestMsg & _msg);
		bool EXE_SEND(QTHREAD & _wk,
					POOL::POOLDATA & _rPool,
					RestMsg & _req);
		bool EXE_TIMER(QTHREAD & _wk,
					POOL::POOLDATA & _rPool,
					RestMsg & _req);
		ActionParser * m_dsl;
};
}
#endif
