/*
 * PosixCompat.hxx
 *
 * MSVC-only drop-in replacements for the POSIX/pthread APIs that esiputil's
 * Linux build (g++) relies on directly (pthread_*, sysconf, sleep/usleep,
 * clock_gettime, syscall(SYS_gettid), getpid, stat-family calls, bzero,
 * strerror_r ...).
 *
 * Consumer files keep including the real POSIX headers on Linux and switch
 * to this header under MSVC, e.g.:
 *
 *     #ifdef _MSC_VER
 *     #include "PosixCompat.hxx"
 *     #else
 *     #include <pthread.h>
 *     #include <unistd.h>
 *     #endif
 *
 * All the identifiers below keep their original POSIX names so call sites
 * do not need to change. This whole file is a no-op outside of _MSC_VER.
 *
 * NOTE (pthread_t identity): pthread_t here is just a Windows thread id
 * (DWORD), not a HANDLE. join()/detach()/affinity re-open a HANDLE by id
 * via OpenThread() on demand. This keeps pthread_t comparable/castable to
 * int the way the existing code already does (`m_threadId == 0`,
 * `(unsigned int)m_threadId`, ...), at the cost of a theoretical (and in
 * practice negligible for this codebase's usage) thread-id-reuse race if a
 * thread is joined long after it has already exited and its id got recycled.
 *
 * NOTE (clock_gettime): CLOCK_REALTIME and CLOCK_MONOTONIC both resolve to
 * the same wall-clock source (GetSystemTimeAsFileTime). This is not a true
 * monotonic clock, but every caller in this codebase only ever uses
 * clock_gettime() to compute a short relative timeout for
 * pthread_cond_timedwait(), so a shared time base is sufficient.
 */
#ifndef ESIPUTIL_POSIXCOMPAT_HXX
#define ESIPUTIL_POSIXCOMPAT_HXX

#ifdef _MSC_VER

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602  // Windows 8+: needed for WSAPoll() in winsock2.h
#endif
/* winsock2.h MUST come before windows.h (not WIN32_LEAN_AND_MEAN) to avoid
 * the old winsock.h/winsock2.h struct-redefinition clash: winsock2.h's own
 * include guard (_WINSOCKAPI_) stops windows.h from pulling in winsock.h.
 * WIN32_LEAN_AND_MEAN is deliberately NOT defined here - Directory.hxx's
 * DeepSearch (MFC's <afxwin.h>/ATL) needs the OLE/COM declarations
 * (SysAllocString, VARIANT, ...) that WIN32_LEAN_AND_MEAN strips out. */
#include <winsock2.h>  // WSAPoll/WSAPOLLFD (epoll shim), used by ePoll.hxx and socket code
#include <windows.h>
#include <ws2tcpip.h>
#include <process.h>   // _beginthreadex
#include <time.h>      // struct timespec (UCRT, VS2015+)
#include <errno.h>     // ETIMEDOUT
#include <sys/stat.h>
#include <sys/types.h>
#include <io.h>        // _access, _unlink
#include <direct.h>    // _mkdir, _rmdir
#include <string.h>    // memset
#include <stdio.h>     // _snprintf, _vscprintf
#include <stdlib.h>    // malloc/free
#include <stdarg.h>    // va_list/va_copy
#include <signal.h>    // SIGTERM (for kill() below)

/* Everything below lives at global scope, exactly like the real
 * <pthread.h>/<unistd.h> it stands in for, so it is visible the same way
 * regardless of whether the including code is inside namespace eSipUtil. */

/* ===================== threads ===================== */
typedef DWORD pthread_t;

struct pthread_attr_t_
{
	unsigned stackSize;
	int detachState;
};
typedef pthread_attr_t_ pthread_attr_t;
typedef int pthread_mutexattr_t;

struct pthread_condattr_t_
{
	int clockId;
};
typedef pthread_condattr_t_ pthread_condattr_t;

#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1

inline int pthread_attr_init(pthread_attr_t * _pAttr)
{
	_pAttr->stackSize = 0; _pAttr->detachState = PTHREAD_CREATE_JOINABLE; return 0;
}
inline int pthread_attr_destroy(pthread_attr_t *) { return 0; }
inline int pthread_attr_setstacksize(pthread_attr_t * _pAttr, size_t _stStackSize)
{
	_pAttr->stackSize = (unsigned)_stStackSize; return 0;
}
inline int pthread_attr_setdetachstate(pthread_attr_t * _pAttr, int _nState)
{
	_pAttr->detachState = _nState; return 0;
}

struct WinPthreadTrampolineCtx_
{
	void * (*pfnStart)(void *);
	void * pArg;
};

inline unsigned __stdcall winPthreadTrampoline_(void * _pParam)
{
	WinPthreadTrampolineCtx_ * pCtx = (WinPthreadTrampolineCtx_ *)_pParam;
	void * (*pfnStart)(void *) = pCtx->pfnStart;
	void * pArg = pCtx->pArg;
	delete pCtx;
	if (pfnStart) pfnStart(pArg);
	return 0;
}

inline int pthread_create(pthread_t * _pThread, const pthread_attr_t * _pAttr,
							void * (*_pfnStart)(void *), void * _pArg)
{
	WinPthreadTrampolineCtx_ * pCtx = new WinPthreadTrampolineCtx_();
	pCtx->pfnStart = _pfnStart; pCtx->pArg = _pArg;
	unsigned unStackSize = (_pAttr && _pAttr->stackSize > 0) ? _pAttr->stackSize : 0;
	unsigned unTid = 0;
	uintptr_t uHandle = _beginthreadex(NULL, unStackSize, winPthreadTrampoline_, pCtx, 0, &unTid);
	if (uHandle == 0)
	{
		delete pCtx;
		return -1;
	}
	if (_pThread) *_pThread = (pthread_t)unTid;
	CloseHandle((HANDLE)uHandle);
	return 0;
}

inline pthread_t pthread_self() { return (pthread_t)GetCurrentThreadId(); }

inline int pthread_join(pthread_t _thread, void **)
{
	HANDLE hThread = OpenThread(SYNCHRONIZE, FALSE, _thread);
	if (!hThread) return -1;
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	return 0;
}

inline int pthread_detach(pthread_t) { return 0; }

typedef DWORD_PTR cpu_set_t;
#define CPU_ZERO(setp) (*(setp) = 0)
#define CPU_SET(cpu, setp) (*(setp) |= ((DWORD_PTR)1 << (cpu)))

inline int pthread_setaffinity_np(pthread_t _thread, size_t, const cpu_set_t * _pSet)
{
	HANDLE hThread = OpenThread(THREAD_SET_INFORMATION | THREAD_QUERY_INFORMATION, FALSE, _thread);
	if (!hThread) return -1;
	DWORD_PTR uResult = SetThreadAffinityMask(hThread, *_pSet);
	CloseHandle(hThread);
	return uResult ? 0 : -1;
}

#ifndef _SC_NPROCESSORS_ONLN
#define _SC_NPROCESSORS_ONLN 1
#endif
inline long sysconf(int)
{
	SYSTEM_INFO stSi; GetSystemInfo(&stSi); return (long)stSi.dwNumberOfProcessors;
}

/* ===================== mutex / condvar ===================== */
typedef CRITICAL_SECTION pthread_mutex_t;

inline int pthread_mutex_init(pthread_mutex_t * _pMutex, const pthread_mutexattr_t *)
{
	InitializeCriticalSection(_pMutex); return 0;
}
inline int pthread_mutex_destroy(pthread_mutex_t * _pMutex) { DeleteCriticalSection(_pMutex); return 0; }
inline int pthread_mutex_lock(pthread_mutex_t * _pMutex) { EnterCriticalSection(_pMutex); return 0; }
inline int pthread_mutex_unlock(pthread_mutex_t * _pMutex) { LeaveCriticalSection(_pMutex); return 0; }
inline int pthread_mutex_trylock(pthread_mutex_t * _pMutex) { return TryEnterCriticalSection(_pMutex) ? 0 : 1; }

typedef CONDITION_VARIABLE pthread_cond_t;

inline int pthread_condattr_init(pthread_condattr_t * _pAttr) { _pAttr->clockId = 0; return 0; }
inline int pthread_condattr_destroy(pthread_condattr_t *) { return 0; }
inline int pthread_condattr_setclock(pthread_condattr_t * _pAttr, int _nClockId)
{
	_pAttr->clockId = _nClockId; return 0;
}
inline int pthread_cond_init(pthread_cond_t * _pCond, const pthread_condattr_t *)
{
	InitializeConditionVariable(_pCond); return 0;
}
inline int pthread_cond_destroy(pthread_cond_t *) { return 0; }
inline int pthread_cond_wait(pthread_cond_t * _pCond, pthread_mutex_t * _pMutex)
{
	return SleepConditionVariableCS(_pCond, _pMutex, INFINITE) ? 0 : -1;
}
inline int pthread_cond_signal(pthread_cond_t * _pCond) { WakeConditionVariable(_pCond); return 0; }
inline int pthread_cond_broadcast(pthread_cond_t * _pCond) { WakeAllConditionVariable(_pCond); return 0; }

/* ===================== clock / timedwait ===================== */
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

inline int clock_gettime(int, struct timespec * _pTs)
{
	static const unsigned long long ULL_EPOCH_DIFF = 116444736000000000ULL; // 1601->1970, 100ns units
	FILETIME stFt; GetSystemTimeAsFileTime(&stFt);
	unsigned long long ullT = (((unsigned long long)stFt.dwHighDateTime) << 32) | stFt.dwLowDateTime;
	ullT -= ULL_EPOCH_DIFF;
	_pTs->tv_sec = (time_t)(ullT / 10000000ULL);
	_pTs->tv_nsec = (long)((ullT % 10000000ULL) * 100);
	return 0;
}

inline int pthread_cond_timedwait(pthread_cond_t * _pCond, pthread_mutex_t * _pMutex,
									const struct timespec * _pAbsTime)
{
	struct timespec stNow; clock_gettime(CLOCK_REALTIME, &stNow);
	long long llDiffMs = ((long long)_pAbsTime->tv_sec - (long long)stNow.tv_sec) * 1000LL
						+ ((long long)_pAbsTime->tv_nsec - (long long)stNow.tv_nsec) / 1000000LL;
	if (llDiffMs < 0) llDiffMs = 0;
	BOOL bOk = SleepConditionVariableCS(_pCond, _pMutex, (DWORD)llDiffMs);
	if (bOk) return 0;
	return (GetLastError() == ERROR_TIMEOUT) ? ETIMEDOUT : -1;
}

/* ===================== misc unistd / sys-call shims ===================== */
inline unsigned int sleep(unsigned int _unSec) { Sleep(_unSec * 1000); return 0; }
inline int usleep(unsigned int _unUsec) { Sleep((_unUsec + 999) / 1000); return 0; }

#ifndef SYS_gettid
#define SYS_gettid 0
#endif
inline long syscall(long) { return (long)GetCurrentThreadId(); }

#define getpid() ((int)GetCurrentProcessId())
inline int getppid() { return 0; } // no portable parent-pid query on Windows; callers here only use it to detect "reparented to init(pid 1)" on Linux

/* ===================== signal()/kill() extras (asif's MainP.cpp) ===================== */
/* MSVC's <signal.h> only defines SIGINT/SIGILL/SIGFPE/SIGSEGV/SIGTERM/SIGBREAK/SIGABRT -
 * no SIGKILL/SIGHUP/SIGPIPE, and no real kill(). These values are picked to not collide
 * with the real Windows-defined signal numbers; signal(SIGHUP/SIGPIPE, handler) below is a
 * harmless no-op registration (Windows never raises these), preserving "compiles and runs
 * without crashing" rather than true POSIX signal semantics. */
#ifndef SIGKILL
#define SIGKILL 9
#endif
#ifndef SIGHUP
#define SIGHUP 1
#endif
#ifndef SIGPIPE
#define SIGPIPE 13
#endif
typedef int pid_t;
typedef unsigned int uint; // glibc <sys/types.h> non-standard shorthand, used by asif's UTIL.cpp
inline int kill(pid_t _pid, int _sig)
{
	if (_sig == SIGKILL || _sig == SIGTERM)
	{
		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, (DWORD)_pid);
		if (!hProcess) return -1;
		BOOL bOk = TerminateProcess(hProcess, (UINT)_sig);
		CloseHandle(hProcess);
		return bOk ? 0 : -1;
	}
	return 0; // SIGHUP/SIGPIPE/etc: nothing to deliver on Windows
}

/* ===================== setrlimit() (asif's MainP.cpp stack-size hint) ===================== */
struct rlimit { size_t rlim_cur; size_t rlim_max; };
#ifndef RLIMIT_STACK
#define RLIMIT_STACK 3
#endif
inline int setrlimit(int, const struct rlimit *) { return 0; } // no per-thread-stack-at-runtime equivalent; thread stack size is set at CreateThread() time instead

/* ===================== file APIs ===================== */
#define mkdir(path, mode) _mkdir(path)
#define access _access
#define unlink _unlink
#define rmdir _rmdir
#define stat _stat
#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & _S_IFDIR) != 0)
#endif
#ifndef S_ISREG
#define S_ISREG(m) (((m) & _S_IFREG) != 0)
#endif

#define bzero(ptr, len) memset((ptr), 0, (len))
#ifndef strcasecmp
#define strcasecmp _stricmp
#endif
#ifndef strncasecmp
#define strncasecmp _strnicmp
#endif

inline int strerror_r(int _nErrNo, char * _pszBuf, size_t _stBufLen)
{
	return strerror_s(_pszBuf, _stBufLen, _nErrNo);
}

#define popen _popen
#define pclose _pclose
#define strtok_r strtok_s // same (str, delim, context) parameter order
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif
#define random rand      // not cryptographically equivalent, but this codebase only uses these for non-crypto PRNG
#define srandom srand
#define chmod _chmod
#define S_IRUSR _S_IREAD
#define S_IWUSR _S_IWRITE
#define S_IRGRP _S_IREAD
#define S_IWGRP _S_IWRITE
#define S_IROTH _S_IREAD
#define S_IWOTH _S_IWRITE

inline struct tm * localtime_r(const time_t * _pTime, struct tm * _pResult)
{
	if (localtime_s(_pResult, _pTime) != 0) return NULL;
	return _pResult;
}

inline int gettimeofday(struct timeval * _pTv, void *)
{
	if (_pTv)
	{
		static const unsigned long long ULL_EPOCH_DIFF2 = 116444736000000000ULL;
		FILETIME stFt; GetSystemTimeAsFileTime(&stFt);
		unsigned long long ullT = (((unsigned long long)stFt.dwHighDateTime) << 32) | stFt.dwLowDateTime;
		ullT -= ULL_EPOCH_DIFF2;
		_pTv->tv_sec = (long)(ullT / 10000000ULL);
		_pTv->tv_usec = (long)((ullT % 10000000ULL) / 10);
	}
	return 0;
}

inline char * strndup(const char * _pszSrc, size_t _stMaxLen)
{
	size_t stLen = strnlen(_pszSrc, _stMaxLen);
	char * pszDup = (char *)malloc(stLen + 1);
	if (pszDup)
	{
		memcpy(pszDup, _pszSrc, stLen);
		pszDup[stLen] = 0;
	}
	return pszDup;
}

inline int vasprintf(char ** _ppszRet, const char * _pszFormat, va_list _args)
{
	va_list argsCopy;
	va_copy(argsCopy, _args);
	int nLen = _vscprintf(_pszFormat, argsCopy);
	va_end(argsCopy);
	if (nLen < 0) { *_ppszRet = NULL; return -1; }
	*_ppszRet = (char *)malloc((size_t)nLen + 1);
	if (!*_ppszRet) return -1;
	return vsnprintf(*_ppszRet, (size_t)nLen + 1, _pszFormat, _args);
}

/* ===================== strptime() (asif's TIME::INSERT, format string is scenario-authored at runtime) =====================
 * Minimal but broad implementation: numeric fields (%Y %y %m %d %H %M %S %j), %% and literal/whitespace matching.
 * Not a full POSIX strptime (no %a/%b month-and-weekday-name parsing, no locale support), but covers the numeric
 * date/time formats this codebase's DSL DATE/TIME statements actually construct. */
inline const char * strptimeReadNum_(const char * _pszStr, int _nMaxDigits, int * _pnOut)
{
	int nVal = 0, nDigits = 0;
	while (*_pszStr == ' ') ++_pszStr;
	while (*_pszStr >= '0' && *_pszStr <= '9' && nDigits < _nMaxDigits)
	{
		nVal = nVal * 10 + (*_pszStr - '0');
		++_pszStr; ++nDigits;
	}
	if (nDigits == 0) return NULL;
	*_pnOut = nVal;
	return _pszStr;
}
inline char * strptime(const char * _pszStr, const char * _pszFormat, struct tm * _pTm)
{
	int nVal = 0;
	while (*_pszFormat)
	{
		if (*_pszFormat == '%')
		{
			++_pszFormat;
			switch (*_pszFormat)
			{
				case 'Y': if (!(_pszStr = strptimeReadNum_(_pszStr, 4, &nVal))) return NULL; _pTm->tm_year = nVal - 1900; break;
				case 'y': if (!(_pszStr = strptimeReadNum_(_pszStr, 2, &nVal))) return NULL; _pTm->tm_year = (nVal < 69) ? nVal + 100 : nVal; break;
				case 'm': if (!(_pszStr = strptimeReadNum_(_pszStr, 2, &nVal))) return NULL; _pTm->tm_mon = nVal - 1; break;
				case 'd': case 'e': if (!(_pszStr = strptimeReadNum_(_pszStr, 2, &nVal))) return NULL; _pTm->tm_mday = nVal; break;
				case 'H': if (!(_pszStr = strptimeReadNum_(_pszStr, 2, &nVal))) return NULL; _pTm->tm_hour = nVal; break;
				case 'M': if (!(_pszStr = strptimeReadNum_(_pszStr, 2, &nVal))) return NULL; _pTm->tm_min = nVal; break;
				case 'S': if (!(_pszStr = strptimeReadNum_(_pszStr, 2, &nVal))) return NULL; _pTm->tm_sec = nVal; break;
				case 'j': if (!(_pszStr = strptimeReadNum_(_pszStr, 3, &nVal))) return NULL; _pTm->tm_yday = nVal - 1; break;
				case '%': if (*_pszStr != '%') return NULL; ++_pszStr; break;
				default: return NULL; // unsupported specifier (%a/%b/%Z/... - not needed by this codebase's usage)
			}
			++_pszFormat;
		}
		else if (*_pszFormat == ' ')
		{
			while (*_pszStr == ' ') ++_pszStr;
			++_pszFormat;
		}
		else
		{
			if (*_pszStr != *_pszFormat) return NULL;
			++_pszStr; ++_pszFormat;
		}
	}
	return (char *)_pszStr;
}

inline int nanosleep(const struct timespec * _pReq, struct timespec *)
{
	if (!_pReq) return -1;
	DWORD dwMs = (DWORD)(_pReq->tv_sec * 1000 + _pReq->tv_nsec / 1000000);
	Sleep(dwMs);
	return 0;
}

/* ===================== dirent (opendir/readdir/closedir) ===================== */
struct dirent
{
	char d_name[MAX_PATH];
	long d_ino;	//no inode concept on Windows; readdir() below always sets this to 1
			//(real entries only - FindNextFile never yields a "deleted, ino==0" entry
			//the way some Linux filesystems can), so `d_ino == 0` deleted-file checks
			//in ported code never misfire.
};

struct DIR
{
	HANDLE hFind;
	WIN32_FIND_DATAA stFindData;
	bool bFirst;
	struct dirent stEntry;
};

inline DIR * opendir(const char * _pszPath)
{
	char szPattern[MAX_PATH];
	_snprintf(szPattern, sizeof(szPattern) - 1, "%s\\*", _pszPath);
	szPattern[sizeof(szPattern) - 1] = 0;

	DIR * pDir = new DIR();
	pDir->bFirst = true;
	pDir->hFind = FindFirstFileA(szPattern, &pDir->stFindData);
	if (pDir->hFind == INVALID_HANDLE_VALUE)
	{
		delete pDir;
		return NULL;
	}
	return pDir;
}

inline struct dirent * readdir(DIR * _pDir)
{
	if (!_pDir) return NULL;
	if (_pDir->bFirst)
	{
		_pDir->bFirst = false;
	}
	else if (!FindNextFileA(_pDir->hFind, &_pDir->stFindData))
	{
		return NULL;
	}
	strncpy(_pDir->stEntry.d_name, _pDir->stFindData.cFileName, sizeof(_pDir->stEntry.d_name) - 1);
	_pDir->stEntry.d_name[sizeof(_pDir->stEntry.d_name) - 1] = 0;
	_pDir->stEntry.d_ino = 1;
	return &_pDir->stEntry;
}

inline int closedir(DIR * _pDir)
{
	if (!_pDir) return -1;
	if (_pDir->hFind != INVALID_HANDLE_VALUE) FindClose(_pDir->hFind);
	delete _pDir;
	return 0;
}

/* ===================== epoll (WSAPoll-backed shim, see ePoll.cxx) ===================== */
/* Values are internal to this shim (not real Linux epoll bit values) - they only need to
 * be self-consistent between ePoll.cxx (producer) and callers like transportMgr.cxx
 * (consumer), which only ever test `events & EPOLLIN` / `events & EPOLLOUT`. */
#define EPOLLIN  0x0001
#define EPOLLOUT 0x0004
#define EPOLLERR 0x0008
#define EPOLLHUP 0x0010
/* WSAPoll has no edge-triggered mode; accepted so callers that OR this flag
 * into addFD/modFD still compile, but ePoll.cxx's Windows path just ignores
 * it (falls back to WSAPoll's inherently level-triggered behavior). */
#define EPOLLET  0x20000000

#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_MOD 2
#define EPOLL_CTL_DEL 3

struct epoll_event
{
	unsigned int events;
	union
	{
		void * ptr;
		int fd;
		unsigned int u32;
		unsigned __int64 u64;
	} data;
};

#endif // _MSC_VER

#endif // ESIPUTIL_POSIXCOMPAT_HXX
