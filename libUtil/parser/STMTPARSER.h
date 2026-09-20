#ifndef STMT_PARSER_H
#define STMT_PARSER_H
#include "BASICPARSER.h"
#include "CLASSPARSER.h"
#include "STRCONV.h"

namespace nsUtil
{
class StmtParser : public StlObject
{
	public:
		StmtParser();
		~StmtParser();
		StmtParser & operator=(StmtParser & _src);
		void CONSTRUCT(void * _pvSrc);
		bool PARSE(ClassParser & _class);
		void JSON(RestParam & _item);
		void STR(KSTRING & _buf); 
		void IMPORT(RestParam & _item);
		KSTRING m_oType;
		KSTRING m_oName;
		KSTRING m_oSpace;
		KSTRING m_result;
		STRCONV m_conv;
};
}
#endif

