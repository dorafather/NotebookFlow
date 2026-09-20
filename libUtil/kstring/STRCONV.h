#ifndef STR_CONV_H
#define STR_CONV_H
#include "LIST.h"

namespace nsUtil
{
/********************** STRCONV****************************/
class STRCONV
{
	public:
		typedef enum
		{
			E_PARAM_IDLE=0,
			E_PARAM_BEGIN,
			E_PARAM_END,
			E_PARAM_MAX
		}EParse_t;
		class item : public StlObject
		{
			public:
				typedef enum
				{
					E_TYPE_NONE=0,
					E_TYPE_DATA,
					E_TYPE_KEYWORD,
					E_TYPE_MAX
				}EType_t;
				item();
				~item();
				void CONSTRUCT(void * _pvSrc);
				void DEBUGGING(KSTRING & _buf);
				EType_t m_eT;
				KSTRING m_szData;
				
		};
		STRCONV();
		~STRCONV();
		STRCONV & operator=(STRCONV & _src);
		bool PARSE(KCSTR _pszData);
		KCSTR STR(ALIST & _param, KSTRING & _buf);
		KCSTR STR_(KCSTR _replaceWord, KSTRING & _buf);
		bool ISCONV();
		KUINT KEYNUMS();
		KCSTR DEBUGGING(KSTRING & _buf);
		void LIST(ALIST & _getList);
		void LISTUP(ALIST & _getList);
		static void test();
	private:
		void clear();
		bool m_fnParseIdle();
		bool m_fnParseBegin();
		bool m_fnParseEnd();
		bool m_fnDetecting(const char * _pszConfig);
		char * m_pszOrig;
		bool m_bDetecting;
		KSTRING m_clsBeginTmp;
		unsigned int m_unPos;
		EParse_t m_eSt;
		item * m_pCurItem;
		StlList m_listRaw;
};
}
#endif

