#ifndef IF_PARSER_H
#define IF_PARSER_H
#include "KSTRING.h"
#include "COMPSTR.h"
#include "LIST.h"
#include "BASICPARSER.h"
namespace nsUtil
{
typedef enum
{
	E_DSL_COMP_TRUE          ,
	E_DSL_COMP_FALSE         ,
	E_DSL_COMP_BIGGER        ,
	E_DSL_COMP_SMALLER       ,
	E_DSL_COMP_BIGGER_MATCH  ,
	E_DSL_COMP_SMALLER_MATCH ,
	E_DSL_COMP_FULL_MATCH    ,
	E_DSL_COMP_NOT_MATCH     ,
	E_DSL_COMP_PFX_MATCH     ,
	E_DSL_COMP_PARTIAL_MATCH ,
	E_DSL_COMP_MAX  
}ESceParserComp_t; 

#define DEF_DSL_K_IF_eng "IF"
#define DEF_DSL_K_ELSE_eng "ELSE"
#define DEF_DSL_K_ELSEIF_eng "ELSEIF"
#define DEF_DSL_K_TRUE_eng "TRUE"
#define DEF_DSL_K_FALSE_eng "FALSE"
#define DEF_DSL_K_OR_eng "OR"
#define DEF_DSL_K_AND_eng "AND"

#define DEF_DSL_K_IF_kor "만약에"
#define DEF_DSL_K_ELSE_kor "그외"
#define DEF_DSL_K_ELSEIF_kor "그외그외"
#define DEF_DSL_K_TRUE_kor "참"
#define DEF_DSL_K_FALSE_kor "거짓"
#define DEF_DSL_K_OR_kor "또는"
#define DEF_DSL_K_AND_kor "그리고"

inline bool IS_DSL_K_IF(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_IF_eng||cmp==DEF_DSL_K_IF_kor)return true;else return false;}
inline bool IS_DSL_K_ELSE(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_ELSE_eng||cmp==DEF_DSL_K_ELSE_kor)return true;else return false;}
inline bool IS_DSL_K_ELSEIF(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_ELSEIF_eng||cmp==DEF_DSL_K_ELSEIF_kor)return true;else return false;}
inline bool IS_DSL_K_TRUE(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_TRUE_eng||cmp==DEF_DSL_K_TRUE_kor)return true;else return false;}
inline bool IS_DSL_K_FALSE(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_FALSE_eng||cmp==DEF_DSL_K_FALSE_kor)return true;else return false;}
inline bool IS_DSL_K_OR(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_OR_eng||cmp==DEF_DSL_K_OR_kor)return true;else return false;}
inline bool IS_DSL_K_AND(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_AND_eng||cmp==DEF_DSL_K_AND_kor)return true;else return false;}

inline KCSTR STR_DSL_K_IF(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_IF_eng;else return DEF_DSL_K_IF_kor;}
inline KCSTR STR_DSL_K_ELSE(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_ELSE_eng;else return DEF_DSL_K_ELSE_kor;}
inline KCSTR STR_DSL_K_ELSEIF(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_ELSEIF_eng;else return DEF_DSL_K_ELSEIF_kor;}
inline KCSTR STR_DSL_K_TRUE(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_TRUE_eng;else return DEF_DSL_K_TRUE_kor;}
inline KCSTR STR_DSL_K_OR(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_OR_eng;else return DEF_DSL_K_OR_kor;}
inline KCSTR STR_DSL_K_AND(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_AND_eng;else return DEF_DSL_K_AND_kor;}
KCSTR stringCmp(ESceParserComp_t _eT);
ESceParserComp_t enumCmp(KCSTR _val);
KCSTR convCmp(KCSTR _val);
bool validCompare(KCSTR _comp);
class IfParser : public StlObject
{
	public:
		typedef enum 
		{
			E_PARSE_NONE,
			E_PARSE_IF,
			E_PARSE_IF_TAIL,
			E_PARSE_IF_SP,
			E_PARSE_IF_A,
			E_PARSE_IF_A_SP,
			E_PARSE_IF_B,
			E_PARSE_IF_B_SP,
			E_PARSE_IF_COND,
			E_PARSE_IF_COND_SP,
			E_PARSE_IF_C,
			E_PARSE_IF_C_SP,
			E_PARSE_IF_D,
			E_PARSE_END,
			E_PARSE_MAX
		}EParse_t;
		IfParser();
		~IfParser();
		IfParser & operator=(IfParser & _src);
		void CONSTRUCT(void * _pvSrc);
		void CLEAR();
		bool PARSE(KCSTR _src);
		void JSON(RestParam & _item);
		void STR(KSTRING & _buf);
		void IMPORT(RestParam & _item);
		bool parsestep(const char _cInput);
		KCSTR DEBUGGING(KSTRING & _debug);
		bool compList(KCSTR _comp, ALIST & _a, ALIST & _b);
		EParse_t STATE();
		bool valid();
		static void m_fnTest();
		KSTRING m_if;
		KSTRING m_if_a;
		KSTRING m_if_b;
		KSTRING m_if_cond;
		KSTRING m_if_c;
		KSTRING m_if_d;
		KSTRING m_result;
	private:
		void CHANGE(EParse_t _eT);
		bool m_fnE_PARSE_NONE      (const char _cInput);
		bool m_fnE_PARSE_IF        (const char _cInput);
		bool m_fnE_PARSE_IF_TAIL        (const char _cInput);
		bool m_fnE_PARSE_IF_SP     (const char _cInput);
		bool m_fnE_PARSE_IF_A      (const char _cInput);
		bool m_fnE_PARSE_IF_A_SP   (const char _cInput);
		bool m_fnE_PARSE_IF_B      (const char _cInput);
		bool m_fnE_PARSE_IF_B_SP   (const char _cInput);
		bool m_fnE_PARSE_IF_COND   (const char _cInput);
		bool m_fnE_PARSE_IF_COND_SP(const char _cInput);
		bool m_fnE_PARSE_IF_C      (const char _cInput);
		bool m_fnE_PARSE_IF_C_SP   (const char _cInput);
		bool m_fnE_PARSE_IF_D      (const char _cInput);
		bool m_fnE_PARSE_END      (const char _cInput);
		EParse_t m_eSt;
		bool m_bDotSeen;
};
}
#endif
