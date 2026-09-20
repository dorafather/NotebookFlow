#ifndef ACTION_PARSER_H
#define ACTION_PARSER_H
#include "BASICPARSER.h"
#include "CLASSPARSER.h"
#include "ASSIGNPARSER.h"
#include "KSTRING.h"

namespace nsUtil
{
class ActionParser : public StlObject
{
	public:
		ActionParser();
		~ActionParser();
		ActionParser & operator=(ActionParser & _src);
		AssignParser & operator[](KUINT _idx);
		void CONSTRUCT(void * _pvSrc);
		bool PARSE(ClassParser & _class);
		void JSON(RestParam & _item);
		void STR(KSTRING & _buf);
		void IMPORT(RestParam & _item);
		KCSTR EVENT(KSTRING & _val);
		KUINT NUMS();
		KSTRING m_oType;
		KSTRING m_oName;
		KSTRING m_oSpace;
		KSTRING m_result;
	private:
		StlList m_list;  
		AssignParser m_def;
};
}
#endif
