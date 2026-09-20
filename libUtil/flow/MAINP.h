#ifndef MAIN_P_H
#define MAIN_P_H
#include "KSTRING.h"

namespace nsUtil
{
#define MAIN_SPEC_1K 1024
#define MAIN_SPEC_1M 1048576
#define MAIN_SPEC_1G 1073741824
#define MAIN_PROCESS_STACK_SIZE    8*MAIN_SPEC_1M
#define MAIN_THREAD_STACK_SIZE    80*MAIN_SPEC_1K
class MainP
{
	public:
		typedef void (*PFuncMainCli_t)(char * _pszInput);
		MainP(int argc, char ** argv);
		~MainP();
		int m_fnRun();
		void m_fnRunCli(PFuncMainCli_t _pfnCli);
		static KSTRING m_szProcName;
		static unsigned int m_unPid;
		static KSTRING m_szBuildDate;
		bool m_bBackGround;
		bool m_bCliMode;
	private:
		static void m_fnCbkSignalHandle(int sig);
		static const char *m_fnStringSignal(int sig);
		static void m_fnInitMemory();
		static void m_fnKillMyself();
		void m_fnSetSignal();
		void m_fnSetProcName(int argc, char ** argv);
		KSTRING m_szCliBuf;
};
}
#endif
