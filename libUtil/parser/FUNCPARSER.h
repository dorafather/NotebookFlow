#ifndef FUNC_PARSER_H
#define FUNC_PARSER_H
#include "KSTRING.h"
#include "COMPSTR.h"
#include "LIST.h"
#include "BASICPARSER.h"
namespace nsUtil
{
#define DEF_DSL_FUNC_SET_kor "저장"
#define DEF_DSL_FUNC_SUM_kor "더하기"
#define DEF_DSL_FUNC_MINUS_kor "빼기"
#define DEF_DSL_FUNC_DIV_kor "나누기"
#define DEF_DSL_FUNC_MOD_kor "나머지"
#define DEF_DSL_FUNC_CAT_kor "붙이기"
#define DEF_DSL_FUNC_STRSTR_kor "부분비교"
#define DEF_DSL_FUNC_STRNCMP_kor "앞자리비교"
#define DEF_DSL_FUNC_INSERT_kor "삽입"
#define DEF_DSL_FUNC_DELETE_kor "삭제"
#define DEF_DSL_FUNC_EXTRACT_kor "추출"
#define DEF_DSL_FUNC_TIME_kor "시간"
#define DEF_DSL_FUNC_CLOCK_kor "클럭"
#define DEF_DSL_FUNC_PRINT_kor "출력"
#define DEF_DSL_FUNC_DATE_kor "날짜"
#define DEF_DSL_FUNC_CPH_kor  "컴프리헨션"
#define DEF_DSL_FUNC_CPH_eng  "CPH"
#define DEF_DSL_FUNC_LOOP_kor "반복"
#define DEF_DSL_K_LOOP_CNT_kor "반복횟수"
#define DEF_DSL_FUNC_OBJ_kor "객체저장"
#define DEF_DSL_FUNC_ADDR_kor "주소저장"
#define DEF_DSL_FUNC_SPLIT_kor "쪼개기"
#define DEF_DSL_FUNC_WORD_EX_kor "단어분리"
#define DEF_DSL_FUNC_WORD_SUM_kor "단어합치기"

#define DEF_DSL_FUNC_SET_eng "SET"
#define DEF_DSL_FUNC_SUM_eng "SUM"
#define DEF_DSL_FUNC_MINUS_eng "MINUS"
#define DEF_DSL_FUNC_DIV_eng "DIV"
#define DEF_DSL_FUNC_MOD_eng "MOD"
#define DEF_DSL_FUNC_CAT_eng "CAT"
#define DEF_DSL_FUNC_STRSTR_eng "STRSTR"
#define DEF_DSL_FUNC_STRNCMP_eng "STRNCMP"
#define DEF_DSL_FUNC_INSERT_eng "INSERT"
#define DEF_DSL_FUNC_DELETE_eng "DELETE"
#define DEF_DSL_FUNC_EXTRACT_eng "EXTRACT"
#define DEF_DSL_FUNC_TIME_eng "TIME"
#define DEF_DSL_FUNC_CLOCK_eng "CLOCK"
#define DEF_DSL_FUNC_PRINT_eng "PRINT"
#define DEF_DSL_FUNC_DATE_eng "DATE"
#define DEF_DSL_FUNC_LOOP_eng "LOOP"
#define DEF_DSL_K_LOOP_CNT_eng "LOOP_CNT"
#define DEF_DSL_FUNC_OBJ_eng "OBJECT"
#define DEF_DSL_FUNC_ADDR_eng "ADDR"
#define DEF_DSL_FUNC_SPLIT_eng "SPLIT"
#define DEF_DSL_FUNC_WORD_EX_eng "WORDEX"
#define DEF_DSL_FUNC_WORD_SUM_eng "WORDSUM"

#define DEF_DSL_K_LOOP_BREAK_eng "LOOP_BREAK"
#define DEF_DSL_PARSE_SIZE_eng ".SIZE"

inline bool IS_DSL_K_LOOP_CNT(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_LOOP_CNT_eng||cmp==DEF_DSL_K_LOOP_CNT_kor)return true;else return false;}
inline bool IS_DSL_FUNC_SET(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_SET_eng||cmp==DEF_DSL_FUNC_SET_kor)return true;else return false;}
inline bool IS_DSL_FUNC_SUM(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_SUM_eng||cmp==DEF_DSL_FUNC_SUM_kor)return true;else return false;}
inline bool IS_DSL_FUNC_MINUS(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_MINUS_eng||cmp==DEF_DSL_FUNC_MINUS_kor)return true;else return false;}
inline bool IS_DSL_FUNC_DIV(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_DIV_eng||cmp==DEF_DSL_FUNC_DIV_kor)return true;else return false;}
inline bool IS_DSL_FUNC_MOD(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_MOD_eng||cmp==DEF_DSL_FUNC_MOD_kor)return true;else return false;}
inline bool IS_DSL_FUNC_CAT(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_CAT_eng||cmp==DEF_DSL_FUNC_CAT_kor)return true;else return false;}
inline bool IS_DSL_FUNC_STRSTR(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_STRSTR_eng||cmp==DEF_DSL_FUNC_STRSTR_kor)return true;else return false;}
inline bool IS_DSL_FUNC_STRNCMP(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_STRNCMP_eng||cmp==DEF_DSL_FUNC_STRNCMP_kor)return true;else return false;}
inline bool IS_DSL_FUNC_INSERT(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_INSERT_eng||cmp==DEF_DSL_FUNC_INSERT_kor)return true;else return false;}
inline bool IS_DSL_FUNC_DELETE(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_DELETE_eng||cmp==DEF_DSL_FUNC_DELETE_kor)return true;else return false;}
inline bool IS_DSL_FUNC_EXTRACT(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_EXTRACT_eng||cmp==DEF_DSL_FUNC_EXTRACT_kor)return true;else return false;}
inline bool IS_DSL_FUNC_TIME(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_TIME_eng||cmp==DEF_DSL_FUNC_TIME_kor)return true;else return false;}
inline bool IS_DSL_FUNC_CLOCK(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_CLOCK_eng||cmp==DEF_DSL_FUNC_CLOCK_kor)return true;else return false;}
inline bool IS_DSL_FUNC_PRINT(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_PRINT_eng||cmp==DEF_DSL_FUNC_PRINT_kor)return true;else return false;}
inline bool IS_DSL_FUNC_DATE(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_DATE_eng||cmp==DEF_DSL_FUNC_DATE_kor)return true;else return false;}
inline bool IS_DSL_FUNC_CPH(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_CPH_eng||cmp==DEF_DSL_FUNC_CPH_kor)return true;else return false;}
inline bool IS_DSL_FUNC_LOOP(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_LOOP_eng||cmp==DEF_DSL_FUNC_LOOP_kor)return true;else return false;}
inline bool IS_DSL_FUNC_OBJ(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_OBJ_eng||cmp==DEF_DSL_FUNC_OBJ_kor)return true;else return false;}
inline bool IS_DSL_FUNC_ADDR(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_ADDR_eng||cmp==DEF_DSL_FUNC_ADDR_kor)return true;else return false;}
inline bool IS_DSL_FUNC_SPLIT(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_SPLIT_eng||cmp==DEF_DSL_FUNC_SPLIT_kor)return true;else return false;}
inline bool IS_DSL_FUNC_WORDEX(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_WORD_EX_eng||cmp==DEF_DSL_FUNC_WORD_EX_kor)return true;else return false;}
inline bool IS_DSL_FUNC_WORDSUM(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_FUNC_WORD_SUM_eng||cmp==DEF_DSL_FUNC_WORD_SUM_kor)return true;else return false;}

inline KCSTR STR_DSL_FUNC_SET(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_SET_eng;else return DEF_DSL_FUNC_SET_kor;}
inline KCSTR STR_DSL_FUNC_SUM(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_SUM_eng;else return DEF_DSL_FUNC_SUM_kor;}
inline KCSTR STR_DSL_FUNC_MINUS(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_MINUS_eng;else return DEF_DSL_FUNC_MINUS_kor;}
inline KCSTR STR_DSL_FUNC_DIV(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_DIV_eng;else return DEF_DSL_FUNC_DIV_kor;}
inline KCSTR STR_DSL_FUNC_MOD(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_MOD_eng;else return DEF_DSL_FUNC_MOD_kor;}
inline KCSTR STR_DSL_FUNC_CAT(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_CAT_eng;else return DEF_DSL_FUNC_CAT_kor;}
inline KCSTR STR_DSL_FUNC_STRSTR(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_STRSTR_eng;else return DEF_DSL_FUNC_STRSTR_kor;}
inline KCSTR STR_DSL_FUNC_STRNCMP(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_STRNCMP_eng;else return DEF_DSL_FUNC_STRNCMP_kor;}
inline KCSTR STR_DSL_FUNC_INSERT(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_INSERT_eng;else return DEF_DSL_FUNC_INSERT_kor;}
inline KCSTR STR_DSL_FUNC_DELETE(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_DELETE_eng;else return DEF_DSL_FUNC_DELETE_kor;}
inline KCSTR STR_DSL_FUNC_EXTRACT(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_EXTRACT_eng;else return DEF_DSL_FUNC_EXTRACT_kor;}
inline KCSTR STR_DSL_FUNC_TIME(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_TIME_eng;else return DEF_DSL_FUNC_TIME_kor;}
inline KCSTR STR_DSL_FUNC_CLOCK(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_CLOCK_eng;else return DEF_DSL_FUNC_CLOCK_kor;}
inline KCSTR STR_DSL_FUNC_PRINT(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_PRINT_eng;else return DEF_DSL_FUNC_PRINT_kor;}
inline KCSTR STR_DSL_FUNC_DATE(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_DATE_eng;else return DEF_DSL_FUNC_DATE_kor;} 
inline KCSTR STR_DSL_FUNC_CPH(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_CPH_eng;else return DEF_DSL_FUNC_CPH_kor;}
inline KCSTR STR_DSL_FUNC_LOOP(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_LOOP_eng;else return DEF_DSL_FUNC_LOOP_kor;}
inline KCSTR STR_DSL_FUNC_OBJ(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_OBJ_eng;else return DEF_DSL_FUNC_OBJ_kor;}
inline KCSTR STR_DSL_FUNC_ADDR(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_ADDR_eng;else return DEF_DSL_FUNC_ADDR_kor;}
inline KCSTR STR_DSL_FUNC_SPLIT(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_SPLIT_eng;else return DEF_DSL_FUNC_SPLIT_kor;}
inline KCSTR STR_DSL_FUNC_WORD_EX(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_WORD_EX_eng;else return DEF_DSL_FUNC_WORD_EX_kor;}
inline KCSTR STR_DSL_FUNC_WORD_SUM(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_FUNC_WORD_SUM_eng;else return DEF_DSL_FUNC_WORD_SUM_kor;}

typedef enum 
{
    EXE_TYPE_SET = 0,
    EXE_TYPE_SUM,
    EXE_TYPE_MINUS,
    EXE_TYPE_DIV,
    EXE_TYPE_MOD,
    EXE_TYPE_CAT,
    EXE_TYPE_STRSTR,
    EXE_TYPE_STRNCMP,
    EXE_TYPE_INSERT,
    EXE_TYPE_DELETE,
    EXE_TYPE_EXTRACT,
    EXE_TYPE_TIME,
    EXE_TYPE_CLOCK,
    EXE_TYPE_PRINT,
    EXE_TYPE_DATE,
    EXE_TYPE_CPH,
    EXE_TYPE_LOOP,
    EXE_TYPE_OBJ,
    EXE_TYPE_ADDR,
    EXE_TYPE_SPLIT,
    EXE_TYPE_WORDEX,
    EXE_TYPE_WORDSUM,
    EXE_TYPE_MAX
}eExeType;
class FuncParser
{
	public:
		typedef enum 
		{
			E_PARSE_NONE = 0,
			E_PARSE_CLASS,
			E_PARSE_CLASS_SP,
			E_PARSE_FUNC_NAME,
			E_PARSE_FUNC_NAME_SP,
			E_PARSE_FUNC_ARG_INIT,
			E_PARSE_FUNC_ARG,
			E_PARSE_FUNC_ARG_SP,
			E_PARSE_FUNC_END,
			E_PARSE_MAX
		}EParse_t;
		FuncParser();
		~FuncParser();
		FuncParser & operator=(FuncParser & _src);
		void CONSTRUCT(void * _pvSrc);
		KSTRING & operator[](KUINT _idx);
		KUINT ARGNUMS();
		void CLEAR();
		bool PARSE(KCSTR _src);
		void JSON(RestParam & _msg);
		void STR(KSTRING & _buf);
		void IMPORT(RestParam & _item);
		bool parsestep(const char _cInput);
		KCSTR DEBUGGING(KSTRING & _debug);
		EParse_t STATE();
		KCSTR STATUS();
		void CHANGE(EParse_t _eT);
		ALIST & ARG();
		KSTRING & CLASSNAME();
		KSTRING & FUNCNAME();
		static bool validfunc(KCSTR _name);
		static eExeType getfunctype(KCSTR _name);
		static void m_fnTest();
		KSTRING m_result;
		KSTRING m_curArg;
		KSTRING m_className;
		KSTRING m_funcName;
		ALIST m_argList;
	private:
		bool m_fnE_PARSE_NONE(const char _cInput);
		bool m_fnE_PARSE_CLASS(const char _cInput);
		bool m_fnE_PARSE_CLASS_SP(const char _cInput);
		bool m_fnE_PARSE_FUNC_NAME(const char _cInput);
		bool m_fnE_PARSE_FUNC_NAME_SP(const char _cInput);
		bool m_fnE_PARSE_FUNC_ARG_INIT(const char _cInput);
		bool m_fnE_PARSE_FUNC_ARG(const char _cInput);
		bool m_fnE_PARSE_FUNC_ARG_SP(const char _cInput);
		bool m_fnE_PARSE_FUNC_END(const char _cInput);
		EParse_t m_eSt;
		bool m_bAfterComma;
};
}
#endif
