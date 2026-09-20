#ifndef EXE_IF_H
#define EXE_IF_H
#include "CXIFPARSER.h"

namespace nsUtil
{
class ExeIf
{
	public:
		ExeIf(IfParser & _dsl);
		~ExeIf();
		bool EXE(QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _rcvMsg);
		bool EXE_IFProc(KSTRING & ifA,
			 			KSTRING & ifB,
			 			KSTRING & ifCond,
			 			KSTRING & ifC,
			 			KSTRING & ifD,
						QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, 
						RestMsg & _req,
						RestMsg * _snd);
		IfParser * m_dsl;
};
}
#endif
