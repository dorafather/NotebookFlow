#ifndef _RESTFUL_API_H
#define _RESTFUL_API_H
#include "JSONK.h"
namespace nsUtil
{
class RestParam : public JsonObject
{
	public:
		RestParam();
		virtual ~RestParam();
		RestParam & operator=(RestParam & _rclsSrc);
		bool EMPTY();
		KSTRING & KEY();
		KSTRING & VAL();
		RestParam & GET(const char * _pszParam);
		RestParam & SET(const char * _pszParam);
		RestParam & SETR(TokStrings & _tok, KUINT &_cnt);
		void DEL(const char * _pszParam);
		void DEL(unsigned int _idx);  
		RestParam & ARR();
		RestParam & operator[](unsigned int _unIndex);
		RestParam & INDEX(unsigned int _unIndex);
		unsigned int NUMS();
		char * STR(KSTRING & _build);
		char * BODY(KSTRING & _builds);
};
class RestMsg : public JsonK
{
	public:
		RestMsg();
		virtual ~RestMsg();
		RestMsg & operator=(RestMsg & _rclsSrc);
		bool PARSE(const char * _pszJson);
		RestParam & GET(const char * _pszParam);
		RestParam & GETR(const char * _pszParam);
		RestParam & SET(const char * _pszParam);
		RestParam & SETR(const char * _pszParam);
		void DEL(const char * _pszParam);
		RestParam & operator[](unsigned int _unIndex);
		unsigned int NUMS();
		char * STR();
		static void TEST();
		static void TESTR(); 
		KSTRING m_szMsg;
};
void g_TestJsonMsg();
}
#endif