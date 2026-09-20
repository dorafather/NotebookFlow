#ifndef D_FILE_H
#define D_FILE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef _MSC_VER
#include "PosixCompat.h"
#else
#include <unistd.h>
#include <dirent.h>
#endif
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <time.h>
#ifdef _LINUX_
#include <sys/time.h>
#endif
#ifndef _MSC_VER
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include "KSTRING.h"

namespace nsUtil
{
#define DEF_CFG_1K 1024
#define DEF_CFG_1M 1048576
#define DEF_CFG_1G 1073741824
#define DEF_MAX_CFG_FILE_SIZE    DEF_CFG_1M
#define DEF_CFG_MAGIC_CODE    "wldudehdbsfkdhs"
#define DEF_CFG_TRUE_SELECT(a,b,c)    ((a) ? (b) : (c))
class FileReader
{
	public:
		FileReader();
		~FileReader();
		void init(const char * _pszPath);
		bool writefile(KCSTR _pszData, KUINT _len);
		bool updatetime();   
		bool updatecrc();   
		void changelasttime(KUINT _unTime);
		void changelasttime(const char * _pszTime);
		KUINT getfilesize();
		void enablecheckchanged(bool _bEnhanced = false);    
		bool checkchanged();
		static char * gettimestring(KUINT _unTime,char * _pszSource, KUINT _nBufLen);
		static KUINT gettimeint(char * _pszSource);
		static void changetouchtime(const char * _pszPath,KUINT _unTime);
		static void changetouchtime(const char * _pszPath,const char * _pszTime);
		static unsigned long getcrc(KUINT _unCrc, char * _pszSrc, KUINT _unSize);
		bool m_bIsDir;
		bool m_bEnhanced;
		KSTRING m_clsPath;
		struct stat m_stFileInfo;
		tm m_stTm;
		char m_szTime[30];    
		KINT m_nYr;
		KINT m_nMon;
		KINT m_nDay;
		KINT m_nHr;
		KINT m_nMin;
		KINT m_nSec;	
		char * m_pszRawData;      
		KUINT m_unReadSz;  
		KUINT m_unCurrentModiTime;
		unsigned long m_ulCurrentCrc; 
		KUINT m_unCurrentSize;   
	private:
		void clear();
		KUINT m_unPrevModiTime;
		KULONG m_unPrevCrc;
		KUINT m_unPrevSize;
		bool m_bCheckInit;
};

}
#endif
