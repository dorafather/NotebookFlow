#ifndef SBRK_PARSER_H
#define SBRK_PARSER_H
#include "KSTRING.h"
#include "COMPSTR.h"
#include "BASICPARSER.h"
#include "SESSION.h"
#include <string>
#include <cstdio>
#include <cstring>
namespace nsUtil
{
class parseSbrk
{
	public:
		parseSbrk() ;
		~parseSbrk() ;
		void parse(POOL::POOLDATA & _rPool, const char* _src);
		void cbk(POOL::POOLDATA & _rPool,std::string& _detect);
		static bool s_fnIsAllDigits(KCSTR _psz);
		std::string dst;
};
}
#endif
