#ifndef _KMATRIX_H
#define _KMATRIX_H
#include "KSTRING.h"
namespace nsUtil
{
class KMatrix;
class KMatrixColum : public StlObject
{
	public:
		KMatrixColum(){m_pszVal = NULL;}
		~KMatrixColum(){}
		KSTRING m_szVal;
		char * m_pszVal;
};
class KMatrixColums : public StlObject
{
	public:
		typedef enum
		{
			IDLE,
			COLUM,
			MAX
		}EParse_t;
		typedef bool (*PFuncParseState)(KMatrixColums *_pclsObj, char * _cInput);
		KMatrixColums(KMatrix * _pclsOwner);
		~KMatrixColums();
		void m_fnClear();
		bool m_fnParse(char * _cInput);
		void m_fnDebug(KSTRING & _rDebug,KUINT _unDepth);
		KSTRING & operator[](KUINT _unIdx);
		KSTRING & INDEX(KUINT _unIdx);
		KSTRING & ADD();
		KUINT NUMS();
		static bool m_fnIDLE(KMatrixColums *_pclsObj,char * _cInput);
		static bool m_fnCOLUM(KMatrixColums *_pclsObj,char * _cInput);
		KMatrix * m_pclsOwner;
		StlList m_listColum;
		EParse_t m_eSt;
		static PFuncParseState m_pfnParseHandle[MAX];
		KMatrixColum * m_pclsCur;
		KMatrixColum m_def;
};
class KMatrix
{
	public:
		
		KMatrix();
		~KMatrix();
		typedef enum
		{
			IDLE,
			LINE,
			MAX
		}EParse_t;
		typedef bool (*PFuncParseState)(KMatrix *_pclsObj, char * _cInput);
		KMatrixColums & operator[](KUINT _unIdx);
		KMatrixColums & INDEX(KUINT _unIdx);
		KUINT NUMS();
		void m_fnClear();
		bool m_fnParse(const char * _pszSrc, const char * _pszDep1, const char * _pszDep2);  // "\r\n", " \t"
		bool m_fnParse(char * _pszSrc);
		KMatrixColums & ADD();
		void DEL(KUINT _unLine);
		void m_fnDebug(KSTRING & _rDebug, KUINT _unDepth=0);
		static bool m_fnIDLE(KMatrix *_pclsObj, char * _cInput);
		static bool m_fnLINE(KMatrix *_pclsObj, char * _cInput);
		static void m_fnTest();
		static void m_fnTest2(KSTRING & _szOrg, KSTRING & _szResult);
		bool m_fnIsDep1(char * _cInput);
		bool m_fnIsDep2(char * _cInput);
		KSTRING m_szDep1;
		KSTRING m_szDep2;
		KSTRING m_szSrc;
		StlList m_listLine;
		EParse_t m_eSt;
		static PFuncParseState m_pfnParseHandle[MAX];
		KMatrixColums * m_pclsCur;
		KMatrixColums m_def;
		char * m_pszRaw;
};
}
#endif