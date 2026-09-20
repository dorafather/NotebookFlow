#ifndef ASSIGN_PARSER_H
#define ASSIGN_PARSER_H
#include "KSTRING.h"
#include "COMPSTR.h"
#include "BASICPARSER.h"
namespace nsUtil
{
class AssignParser : public StlObject
{
	public:
		typedef enum 
		{
			E_PARSE_NONE = 0,
			E_PARSE_AB,
			E_PARSE_AB_SP,
			E_PARSE_COND,
			E_PARSE_CD,
			E_PARSE_MAX
		}EParse_t;
		AssignParser();
		~AssignParser();
		AssignParser & operator=(AssignParser & _src);
		void CONSTRUCT(void * _pvSrc);
		bool PARSE(KCSTR _src);
		void JSON(RestParam & _item);
		void STR(KSTRING & _buf);
		void IMPORT(RestParam & _item);
		bool parsestep(const char _cInput);
		EParse_t STATE();
		BasicParser m_AB;
		BasicParser m_CD;
		KSTRING m_result;
	private:
		void CHANGE(EParse_t _eT);
		bool m_fnE_PARSE_NONE(const char _cInput);
		bool m_fnE_PARSE_AB(const char _cInput);
		bool m_fnE_PARSE_AB_SP(const char _cInput);
		bool m_fnE_PARSE_COND(const char _cInput);;
		bool m_fnE_PARSE_CD(const char _cInput);
		EParse_t m_eSt;
};
}
#endif
