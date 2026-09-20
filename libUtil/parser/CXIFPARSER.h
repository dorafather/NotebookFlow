#ifndef CXIF_PARSER_H
#define CXIF_PARSER_H
#include "KSTRING.h"
#include "COMPSTR.h"
#include "IFPARSER.h"

namespace nsUtil
{
class CxIfParser
{
	public:
		typedef enum 
		{
			E_PARSE_NONE = 0,
			E_PARSE_COND,
			E_PARSE_COND_SP,
			E_PARSE_MAX
		}EParse_t;
		CxIfParser();
		~CxIfParser();
		CxIfParser & operator=(CxIfParser & _src);
		IfParser & operator[](KUINT _idx);
		void CONSTRUCT(void * _pvSrc);
		EParse_t STATE();
		bool PARSE(KCSTR _src);
		void JSON(RestParam & _item);
		void STR(KSTRING & _buf);
		void IMPORT(RestParam & _item);
		bool parsestep(const char _cInput);
		KCSTR DEBUGGING(KSTRING & _buf);
		KSTRING & TYPE();
		KUINT NUMS();
		static void m_fnTest();
		KSTRING m_result;
		StlList m_listIf;
	private:
		IfParser * m_curIf;
		void CHANGE(EParse_t _eT);
		bool m_fnE_PARSE_NONE(const char _cInput);
		bool m_fnE_PARSE_COND(const char _cInput);
		bool m_fnE_PARSE_COND_SP(const char _cInput);
		EParse_t m_eSt;
		KSTRING m_def;
		IfParser m_defIf;
};
}
#endif
