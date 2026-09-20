#ifndef SCE_PARSER_H
#define SCE_PARSER_H
#include "BASICPARSER.h"
#include "CLASSPARSER.h"

namespace nsUtil
{
class SceParser
{
	public:
		SceParser();
		~SceParser();
		bool PARSE(KSTRING & _buf);
		ClassParser & operator[](KUINT _idx);
		KUINT NUMS();
		void DEBUGGING(KSTRING & _debug);
		KCSTR NAME();
		static void m_fnTest();
		ClassParser * m_curr;
		StlList m_listSceParser;
		KSTRING m_result;
		KSTRING m_langType;    
		ClassParser m_def;
};
}
#endif

