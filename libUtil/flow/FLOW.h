#ifndef FLOW_H
#define FLOW_H
#include "QUEUETHREAD.h"
#include "EXECORE.h"
#include "MAINP.h"
#include "SESSION.h"

namespace nsUtil
{
class Flow : public QTHREAD
{
	public:
		Flow();
		~Flow();
		static Flow & OBJ();
		void bootStrap();
		void RUNFLOW(int argc, char ** argv);
		void applyAddrConfig(RestMsg & _msg);
		void applyFlowConfig(RestMsg & _msg);
		void applyRestConfig(RestMsg & _msg);
		void queryAddrConfig(RestMsg & _msg);
		void queryFlowConfig(RestMsg & _msg);
		void queryRestConfig(RestMsg & _msg);
		void PROC(ARG & _arg);
		void GARBAGE(POOL::POOLDATA & _rData);
		void PUT(KCSTR _addr, KCSTR  _msg);
		void NOTIFY(QTHREAD & _wk, POOL::POOLDATA & _rPool, RestMsg & _msg);
		virtual void ACTION(QTHREAD & _wk, POOL::POOLDATA & _rPool, RestMsg & _msg)=0;
		void termSession(QTHREAD & _wk, POOL::POOLDATA  & _rPool,KCSTR _reason);
		void termTimer(QTHREAD & _wk, POOL::POOLDATA  & _rPool, KCSTR _id);
		void procTimeOut(QTHREAD & _wk, ARG & _arg, RestMsg & _rcvMsg);
		void procStartReq(QTHREAD & _wk, ARG & _arg, RestMsg & _rcvMsg);
		void procStartRsp(QTHREAD & _wk, ARG & _arg, RestMsg & _rcvMsg);
		void procStopReq(QTHREAD & _wk, ARG & _arg, RestMsg & _rcvMsg);
		void procStopRsp(QTHREAD & _wk, ARG & _arg, RestMsg & _rcvMsg);
		void procAction(QTHREAD & _wk, ARG & _arg, RestMsg & _rcvMsg);
		void procNonTimeOut(QTHREAD & _wk, ARG & _arg, RestMsg & _rcvMsg);
		void procNonProto(QTHREAD & _wk, ARG & _arg, RestMsg & _rcvMsg);
		void proc(QTHREAD & _wk, ARG & _arg);
		static void Logging(	KCSTR _pszFormat, ...);
	private:
		POOL::POOLDATA m_gPool;
		MainP * m_pmain;
		static Flow * m_pInst;
		
};
}
#endif
