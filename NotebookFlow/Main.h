#ifndef _NOTEBOOKFLOW_H
#define _NOTEBOOKFLOW_H
#include "MAINP.h"
#include "FLOW.h"
#include "INIFILE.h"

class HelpFileReader
{
	public:
		HelpFileReader();
		~HelpFileReader();
		static HelpFileReader & OBJ();
		void Read(KCSTR _path);
		static void jsonfiletimeout(nsUtil::Gpolling::info * _info);
		nsUtil::FileReader m_file;
		nsUtil::RestMsg m_msg;
		nsUtil::MUTEX m_lock;
		static HelpFileReader * m_pInst;
};
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
