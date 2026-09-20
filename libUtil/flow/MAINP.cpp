#include "MAINP.h"
/******************************* GCC Include ********************************************/
#include <signal.h>
#ifdef _MSC_VER
#include "PosixCompat.h"
#else
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/resource.h>
#endif
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/types.h>

namespace nsUtil
{
KSTRING MainP::m_szProcName;
unsigned int MainP::m_unPid = 0;
KSTRING MainP::m_szBuildDate;
/**********************************Main Local Functions ************************************/
MainP::MainP(int argc, char ** argv)
{
	m_fnInitMemory();
	m_unPid = (unsigned int)getpid();
	m_szBuildDate<<(KCSTR)__DATE__<<" - "<<(KCSTR)__TIME__;
	m_fnSetProcName(argc,argv);
	m_fnSetSignal();
	m_bBackGround = false;
	KSTRING Cmd;
	for(unsigned int i=0;i<(unsigned int)argc;i++)
	{
		Cmd<<"["; Cmd<<argv[i]; Cmd<<"] ";
	}
	printf("%s\r\n",(KCSTR)Cmd);
	if(argc >= 2)  // ./proc & 
	{
		if(CmpString(argv[1]) == "bg")
		{
			m_bBackGround = true;
			printf("BackGround Mode..\r\n");
		}
		else if(CmpString(argv[1]) == "cli")
		{
			m_bCliMode = true;
		}
		else
		{
			m_bCliMode = false;
		}
	}
	else
	{
		m_bCliMode = false;
	}
}
MainP::~MainP(){}
int MainP::m_fnRun()
{
	pid_t ppid;
	while(1)
	{
		if(m_bBackGround==false)
		{
			ppid=getppid();
			if(ppid==1 )
			{
				printf("PPID is  1, we will exit\r\n");
				break;
			}
		}
		sleep(1);
	}
	return 0;
}
void MainP::m_fnRunCli(PFuncMainCli_t _pfnCli)
{
	if(m_bCliMode)
	{
		printf("CLI> ");
		while(1)
		{
			char cChr = getchar();
			if(cChr == '\n')
			{
				printf("CLI> ");
				if(m_szCliBuf.m_unRealLen > 0)
				{
					_pfnCli((KSTR)m_szCliBuf);
					m_szCliBuf = KNULL;
					printf("CLI> ");	
				}
			}
			else if(cChr == '\r')
			{
			}
			else
			{
				m_szCliBuf<<cChr;
			}
		}
	}
	else
	{
		m_fnRun();
	}
}
const char * MainP::m_fnStringSignal(int sig)
{
	switch(sig)
	{
		case SIGINT: return "SIGINIT";
		case SIGKILL: return "SIGKILL";
		case SIGTERM: return "SIGTERM";
		case SIGHUP: return "SIGHUP ";
		case SIGPIPE: return "SIGPIPE";
		default: return "NONE   ";
	};
	return "NONE   ";
}
void MainP::m_fnKillMyself()
{
	pid_t mypid=getpid();
	if( mypid )
	{
		kill(mypid,SIGKILL);
		usleep(2000*1000);
		kill(mypid,SIGKILL);
		usleep(2000*1000);
		exit(0);
	}
}
void MainP::m_fnCbkSignalHandle(int sig)
{
   printf("recv signal(%s)\r\n",m_fnStringSignal(sig));
   if((sig == SIGINT) || (sig == SIGKILL) || (sig == SIGTERM))
   {
      printf("Finished Process\r\n");
      m_fnKillMyself();
   }
}
void MainP::m_fnSetSignal()
{
	signal(SIGINT, m_fnCbkSignalHandle);	signal(SIGKILL, m_fnCbkSignalHandle);
	signal(SIGTERM, m_fnCbkSignalHandle); signal(SIGHUP, m_fnCbkSignalHandle);
	signal(SIGPIPE, m_fnCbkSignalHandle);
}
void MainP::m_fnSetProcName(int argc, char ** argv)
{
	TOKSTR tok(argv[0]);
#ifdef _MSC_VER
	tok.TOK("/\\");  
#else
	tok.TOK("/");
#endif
	if(tok.NUMS() > 0)
		m_szProcName = (KCSTR)tok[tok.NUMS()-1];
	else
		m_szProcName = (KCSTR)argv[0];
}
void MainP::m_fnInitMemory()
{
	struct rlimit rlim;
	rlim.rlim_cur = MAIN_PROCESS_STACK_SIZE;
	rlim.rlim_max = MAIN_PROCESS_STACK_SIZE;
	setrlimit(RLIMIT_STACK, &rlim);
	return ;
}
}
