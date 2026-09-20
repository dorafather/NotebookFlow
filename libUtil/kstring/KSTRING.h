
#ifndef __KSTRING_H
#define __KSTRING_H
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <string.h>
#include <ctype.h>
#ifdef _MSC_VER
#include "..\PosixCompat.h"
#else
#include <strings.h>
#endif
#include <errno.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include "STL.h"
#include "AF.h"
#define DEF_CFG_1K 1024
#define DEF_CFG_1M 1048576
#define DEF_CFG_1G 1073741824

namespace nsUtil
{
char * strncat(char * _pszSrc, unsigned int _unMaxSrcLen, const char * _pszFormat, ...);
bool isDisitStr(const char * _pszSrc, unsigned int _unMaxSrcLen);
void createRandomStr(char * _pszOutStr, unsigned int _unMaxLen);
void replaceAll(std::string & _rclsStr, const char * _pszOrigStr, const char * _pszChangeStr);
#define DEF_STR_TRUE_SELECT(a,b,c)    ((a) ? (b) : (c))
#define DEF_STR_1K 1024
#define DEF_STR_1M 1048576
#define DEF_STR_1G 1073741824
#define DEF_NUMERIC(c)      ((c) >= '0' && (c) <= '9')
#define DEF_MIN(a, b)       ((a) > (b) ? (b) : (a))
#define DEF_MAX(a, b)       ((a) < (b) ? (b) : (a))
#define DEF_ABS(x)          (((x) >= 0) ? (x) : -(x))
#define DEF_TRUE_SELECT(a,b,c)    ((a) ? (b) : (c))
#define DEF_AVERAGE(a,b) ((unsigned long int)((unsigned long int)(((unsigned long int)a*100)/(unsigned long int)b)))

//==========> Util String
/*
Unlimited String Buff;
*/
class KStrings;
class TokStrings;
class KSTRING;
class StrPrint
{
	public:
		StrPrint();
		~StrPrint();
		virtual void addPrint(char _cInput)=0;
		int PRINT(const char* format, ...);
		int VSPRINTF(const char* format, va_list arg);
	private:
		unsigned int strLen(const char * _pszSrc);
		void print_pad(int* ret, char p, int num);
		char get_negsign(int negative, int plus, int space);
		int print_dec(char* buf, int max, unsigned int value);
		int print_dec_l(char* buf, int max, unsigned long value);
		int print_dec_ll(char* buf, int max, unsigned long long value);
		int print_hex(char* buf, int max, unsigned int value);
		int print_hex_l(char* buf, int max, unsigned long value);
		int print_hex_ll(char* buf, int max, unsigned long long value);
		void spool_str_rev(int* ret, const char* buf, int len);
		void spool_str(int* ret, const char* buf, int len);
		void print_num(int* ret, int minw, int precision,
							int prgiven, int zeropad, int minus, int plus, int space,
							int zero, int negative, char* buf, int len);
		void print_num_d(int* ret, int value,
							int minw, int precision, int prgiven, int zeropad, int minus,
							int plus, int space);
		void print_num_ld(int* ret, long value,
							int minw, int precision, int prgiven, int zeropad, int minus,
							int plus, int space);
		void print_num_lld(int* ret, long long value,
							int minw, int precision, int prgiven, int zeropad, int minus,
							int plus, int space);
		void print_num_u(int* ret, unsigned int value,
							int minw, int precision, int prgiven, int zeropad, int minus,
							int plus, int space);
		void print_num_lu(int* ret, unsigned long value,
							int minw, int precision, int prgiven, int zeropad, int minus,
							int plus, int space);
		void print_num_llu(int* ret, unsigned long long value,
							int minw, int precision, int prgiven, int zeropad, int minus,
							int plus, int space);
		void print_num_x(int* ret, unsigned int value,
							int minw, int precision, int prgiven, int zeropad, int minus,
							int plus, int space);
		void print_num_lx(int* ret, unsigned long value,
							int minw, int precision, int prgiven, int zeropad, int minus,
							int plus, int space);
		void print_num_llx(int* ret, unsigned long long value,
							int minw, int precision, int prgiven, int zeropad, int minus,
							int plus, int space);
		void print_num_llp(int* ret, void* value,
							int minw, int precision, int prgiven, int zeropad, int minus,
							int plus, int space);
		int print_remainder(char* buf, int max, double r, int prec);
		int print_float(char* buf, int max, double value, int prec);
		void print_num_f(int* ret, double value,
							int minw, int precision, int prgiven, int zeropad, int minus,
							int plus, int space);
		int print_float_g(char* buf, int max, double value, int prec);
		void print_num_g(int* ret, double value,
							int minw, int precision, int prgiven, int zeropad, int minus,
							int plus, int space);
		int my_strnlen(const char* s, int max);
		void print_str(int* ret, char* s,
								int minw, int precision, int prgiven, int zeropad, int minus);
		void print_char(int* ret, int c,int minw, int minus);
};
class KSTRING : public StrPrint
{
public:
	enum { E_SZ_NUM = 256, 
			E_SZ_CHR_CAT = DEF_STR_1K, 
			E_SZ_STR_CAT = DEF_STR_1K * 10, 
			E_HEX_LINE = 32, };
	//==========> new Operator (Store)
	KSTRING();
	KSTRING(bool _bBool);
	KSTRING(char _cVal);
	KSTRING(unsigned char _ucVal);
	KSTRING(short _sVal);
	KSTRING(unsigned short _usVal);
	KSTRING(int _nVal);
	KSTRING(unsigned int _unVal);
	KSTRING(long _lVal);
	KSTRING(unsigned long _ulVal);
	KSTRING(char * _pszVal);
	KSTRING(const char * _pszVal);
	KSTRING(const KSTRING & _rclsVal);
	KSTRING(double _dVal);
	//==========> delete Operator
	~KSTRING();
	unsigned int SIZE(){return m_unLen;}
	unsigned int LENGTH(){return m_unRealLen;}
	//==========> Resize Memory
	void m_fnReSize(unsigned int _unSize);
	//==========> = Operator (Store)
	KSTRING & operator=(bool _bBool);
	KSTRING & operator=(char _cVal);
	KSTRING & operator=(unsigned char _ucVal);
	KSTRING & operator=(short _sVal);
	KSTRING & operator=(unsigned short _usVal);
	KSTRING & operator=(int _nVal);
	KSTRING & operator=(unsigned int _unVal);
	KSTRING & operator=(long _lVal);
	KSTRING & operator=(unsigned long _unVal);
	KSTRING & operator=(long long _llVal);
	KSTRING & operator=(unsigned long long _ullVal);
	KSTRING & operator=(char * _pszVal);
	KSTRING & operator=(const char * _pszVal);
	KSTRING & operator=(KSTRING & _rclsSrc);
	KSTRING & operator=(double _dVal);
	//==========> == Operator (Equal)
	bool operator==(bool _bBool);
	bool operator==(char _cVal);
	bool operator==(unsigned char _ucVal);
	bool operator==(short _sVal);
	bool operator==(unsigned short _usVal);
	bool operator==(int _nVal);
	bool operator==(unsigned int _unVal);
	bool operator==(long _lVal);
	bool operator==(unsigned long _ulVal);
	bool operator==(char * _szSrc);
	bool operator==(const char * _szSrc);
	bool operator==(KSTRING & _rclsCmp);
	bool operator==(double _dVal);
	//==========> != Operator (Mismatch)
	bool operator!=(bool _bBool);
	bool operator!=(char _cVal);
	bool operator!=(unsigned char _ucVal);
	bool operator!=(short _sVal);
	bool operator!=(unsigned short _usVal);
	bool operator!=(int _nVal);
	bool operator!=(unsigned int _unVal);
	bool operator!=(long _lVal);
	bool operator!=(unsigned long _ulVal);
	bool operator!=(char * _szSrc);
	bool operator!=(const char * _szSrc);
	bool operator!=(KSTRING & _rclsCmp);
	bool operator!=(double _dVal);
	//==========> << Operator (Add/Cat)
	KSTRING & operator<<(bool _bVal);
	KSTRING & operator<<(char _cVal);
	KSTRING & operator<<(unsigned char _ucVal);
	KSTRING & operator<<(short _sVal);
	KSTRING & operator<<(unsigned short _usVal);
	KSTRING & operator<<(int _nVal);
	KSTRING & operator<<(unsigned int _unVal);
	KSTRING & operator<<(long _lVal);
	KSTRING & operator<<(unsigned long _ulVal);
	KSTRING & operator<<(char * _pvVal);
	KSTRING & operator<<(const char * _pvVal);
	KSTRING & operator<<(KSTRING & _rclsSrc);
	KSTRING & operator<<(double _dVal);
	//==========> type casting operator
	operator KBOOL();
	operator KCHR();
	operator KUCHR();
	operator KSTR();
	operator KCSTR();
	operator KSHORT();
	operator KUSHORT();
	operator KINT();
	operator KUINT();
	operator KLONG();
	operator KULONG();
	operator KDOUBLE();
	//==========> [] Operator (Array Get/Set)
	char & operator[](unsigned int _unIndex);
	//==========> Cat Functions
	void m_fnCat(const char * _pszSrc);      // cat string
	void m_fnCat(const char _ucInput);    // cat charicter
	void m_fnCat(int _nSpace, const char * _szSrc);   // %-5s , %5s , string cat
	void m_fnHexCat(unsigned char _ucInput);         // char -> hex cat
	char * m_fnCatHexs(void * _pvData, unsigned int _unLen);
	void m_fnByteCat(const char * _szSrc, unsigned int _unLen);     // store byte
	//Unlimited Sprintf, type= %d, %c, %lu, %ul, %lf, %s
	void m_fnSprintf(const char *format, ...);
	void m_fnTrim(const char * _szTrim);
	void m_fnUpperCase();
	//==========> Util Functions
	static int m_fnStrLen(const char * _szString);
	static int m_fnStrCpy(char * _szDst, const char * _szSrc);
	static void m_fnStrnCatCpy(char * dst, const char * src, int _nMax); 
	static void m_fnStrCat(char *mstr, const char * pFormat, ...);
	static int m_fnStrnCat(char *_szSrc, unsigned int _unMaxSz, const char * _szVar, ...);
	static int m_fnStrCmp(const char * _sz1, const char * _sz2);
	static int m_fnStrnCmp(const char * _sz1, const char * _sz2, unsigned int _unLen);
	static int m_fnStrCaseCmp(const char * _sz1, const char * _sz2);
	static int m_fnStrnCaseCmp(const char * _sz1, const char * _sz2, unsigned int _unLen);
	static int m_fnAtoi(const char * _szString);
	static long m_fnAtol(const char * _szString);
	static double m_fnAtof(const char * _szString);
	static const char *m_fnStr(char *_szStr);
	static char * m_fnStrStr(const char * _pszSrc, const char * _pszFind);
	static char * m_fnTrimTailString(char * _pszSrc, const char * _pszTrim);
	static char * m_fnSkipString(const char * _pszSrc, const char * _pszTrim);
	static char * m_fnGetOptimizeString(char * _pszSrc, const char * _pszTrim);
	static void m_fnUnEscapeToString(const char * _pszSrc, KSTRING &_rclsResult);
	static void m_fnEscapeToString(const char * _pszSrc, const char *_pszChangeChr,
																			KSTRING &_rclsResult);
	static void m_fnEscapeToJson(const char * _pszSrc, KSTRING &_rclsResult);
	static bool m_fnTokenizeC(char *_pszOrig, const char *_pszTrimChr, TokStrings &_rclsResult);
	static bool m_fnTokenizeS(char *_pszOrig, const char *_pszTrimStr, TokStrings &_rclsResult);
	static bool m_fnTokenizeP(char *_pszOrig, const char *_pszPattern, TokStrings &_rclsResult);
	static int m_fnRmChr(char* dest, char *source,const char* ptrim);
	static int test_sprint_core();  // sprintf core test
	char *m_pszString;
	unsigned int m_unLen;
	unsigned int m_unRealLen;
private:
	void addPrint(char _cInput);  // sprintf core add function
	void clear();
	void m_fnCreateString(unsigned int _uiSize);
	void m_fnCreateString(const char * _szString);
	static unsigned int m_fnFixedStrCat(char * _pszDst, int _nSpace, const char * _pszSrc);
};
class StringObj : public StlObject
{
	public:
		StringObj();
		~StringObj();
		KSTRING m_clsStringData;
};
class KStrings
{
	public:
		KStrings();
		~KStrings();
		KStrings & operator+=(const char * _pszV);
		KSTRING & operator[](unsigned int _unIndex);
		unsigned int num();
		StringObj m_clsExept;
		StlList m_listString;
};
class TokString : public StlObject
{
	public:
		TokString();
		~TokString();
		char * m_pszLine;
};
class TokStrings
{
	public:
		TokStrings();
		~TokStrings();
		TokStrings & operator+=(const char * _pszV);
		char * operator[](unsigned int _unIndex);
		unsigned int num();
		TokString m_clsExept;
		StlList m_listString;
};
class TOKSTR;
class ARRSTR
{
	friend class TOKSTR;
	public:
		ARRSTR();
		~ARRSTR();
		ARRSTR & operator=(ARRSTR & _src);
		ARRSTR & operator+=(KCSTR _pszV);
		ARRSTR & operator+=(KSTRING _szV);
		KSTRING & operator[](KUINT _unIdx);
		KUINT NUMS();
		KCSTR STR(KSTRING & _rSrc);
		KSTRING m_szSep;
	private:
		void CLEAR();
		KStrings m_arr;
};
class TOKSTR
{
	public:
		TOKSTR();
		TOKSTR(KCSTR _pszSrc);
		TOKSTR(KSTRING _szSrc);
		~TOKSTR();
		void operator=(KCSTR _pszSrc);
		void operator=(KSTRING _szSrc);
		void TOK(KCSTR _pszTrim);
		KSTRING & operator[](KUINT _unIdx);
		KUINT NUMS();
		void CLEAR();
	private:
		ARRSTR m_tok;
		KSTRING m_szSrc;
};
class CmpString
{
	public: 
		CmpString();
		CmpString(const char * _pszSrc);
		~CmpString();
		CmpString & operator=(char * _pszSrc);
		CmpString & operator=(const char * _pszSrc);
		bool operator==(bool _bBool);
		bool operator==(char _cVal);
		bool operator==(unsigned char _ucVal);
		bool operator==(short _sVal);
		bool operator==(unsigned short _usVal);
		bool operator==(int _nVal);
		bool operator==(unsigned int _unVal);
		bool operator==(long _lVal);
		bool operator==(unsigned long _ulVal);
		bool operator==(char * _szSrc);
		bool operator==(const char * _szSrc);
		bool operator==(double _dVal);
		bool operator!=(bool _bBool);
		bool operator!=(char _cVal);
		bool operator!=(unsigned char _ucVal);
		bool operator!=(short _sVal);
		bool operator!=(unsigned short _usVal);
		bool operator!=(int _nVal);
		bool operator!=(unsigned int _unVal);
		bool operator!=(long _lVal);
		bool operator!=(unsigned long _ulVal);
		bool operator!=(char * _szSrc);
		bool operator!=(const char * _szSrc);
		bool operator!=(double _dVal);
		operator KBOOL();
		operator KCHR() ;
		operator KUCHR() ;
		operator KSTR() ;
		operator KCSTR() ;
		operator KSHORT() ;
		operator KUSHORT() ;
		operator KINT() ;
		operator KUINT() ;
		operator KLONG() ;
		operator KULONG() ;
		operator KDOUBLE() ;
	private:
		char * m_pszString;
};
}

#endif
