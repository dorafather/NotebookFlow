#ifndef EXE_PARAM_H
#define EXE_PARAM_H
#include "EXECORE.h"

namespace nsUtil
{
class ExeParam
{
	public:
		ExeParam();
		~ExeParam();
		static KCSTR PARAM(KSTRING & _val1, KSTRING & _val2, 
					POOL::POOLDATA & _rPool, RestMsg & _req, 
					KSTRING & _buf);
		static KUINT PARAMLength(KSTRING & _val1, KSTRING & _val2, 
					POOL::POOLDATA & _rPool, RestMsg & _req, 
					KSTRING & _buf);
};
}
#endif
