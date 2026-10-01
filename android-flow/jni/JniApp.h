#ifndef JNIAPP_H
#define JNIAPP_H
#include "FLOW.h"

namespace nsUtil
{
class JniApp : public Flow
{
	public:
		JniApp();
		~JniApp();
		void ACTION(QTHREAD & _wk, POOL::POOLDATA & _rPool, RestMsg & _msg);
};
}
#endif
