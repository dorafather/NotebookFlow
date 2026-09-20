#ifndef EXE_CXIF_H
#define EXE_CXIF_H
#include "CXIFPARSER.h"

namespace nsUtil
{
class ExeCxIf
{
	public:
		ExeCxIf(CxIfParser & _dsl);
		~ExeCxIf();
		bool EXE(QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _rcvMsg);
		CxIfParser * m_dsl;
};
}
#endif
