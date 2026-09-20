#ifndef _NOTEBOOKFLOW_H
#define _NOTEBOOKFLOW_H
#include "MAINP.h"
#include "FLOW.h"

class App : public nsUtil::Flow
{
	public:
		App();
		~App();
		void ACTION(nsUtil::QTHREAD & _wk, 
					nsUtil::POOL::POOLDATA & _rPool, 
					nsUtil::RestMsg & _msg);
};
#endif
