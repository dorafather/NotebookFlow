#ifndef CLASS_PARSER_H
#define CLASS_PARSER_H
#include "KSTRING.h"
#include "COMPSTR.h"
namespace nsUtil
{
typedef bool (*PFuncClassMatch)(KCSTR _cmp);
typedef struct SceParserClassInfoTbl_t
{
	KUINT m_Enum;
	KCSTR m_kor;
	KCSTR m_eng;
	PFuncClassMatch m_pfn;
}SceParserClassInfoTbl_t;
#define DEF_DSL_CLASS_SCE_kor "시나리오"
#define DEF_DSL_CLASS_STATE_kor "상태"
#define DEF_DSL_CLASS_PROC_kor "처리"
#define DEF_DSL_CLASS_ACT_SEND_kor "전송"
#define DEF_DSL_CLASS_ACT_TIMER_kor "타이머"
#define DEF_DSL_CLASS_STMT_kor "문장"
#define DEF_DSL_CLASS_INI_kor "설정"
#define DEF_DSL_CLASS_SCE_eng "SCE"
#define DEF_DSL_CLASS_STATE_eng "STATE"
#define DEF_DSL_CLASS_PROC_eng "PROC"
#define DEF_DSL_CLASS_ACT_SEND_eng "ACT_SEND"
#define DEF_DSL_CLASS_ACT_TIMER_eng "ACT_TIMER"
#define DEF_DSL_CLASS_STMT_eng "STMT"
#define DEF_DSL_CLASS_INI_eng "INI"
typedef enum
{
	E_DSL_CLASS_SCE=0,
	E_DSL_CLASS_STATE,
	E_DSL_CLASS_PROC,
	E_DSL_CLASS_ACT_SEND,
	E_DSL_CLASS_ACT_TIMER,
	E_DSL_CLASS_STMT,
	E_DSL_CLASS_INI,
	E_DSL_CLASS_MAX,
}ESceParserClass_t;
inline bool IS_DSL_CLASS_SCE(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_CLASS_SCE_eng||cmp==DEF_DSL_CLASS_SCE_kor)return true;else return false;}
inline bool IS_DSL_CLASS_STATE(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_CLASS_STATE_eng||cmp==DEF_DSL_CLASS_STATE_kor)return true;else return false;}
inline bool IS_DSL_CLASS_PROC(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_CLASS_PROC_eng||cmp==DEF_DSL_CLASS_PROC_kor)return true;else return false;}
inline bool IS_DSL_CLASS_ACT_SEND(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_CLASS_ACT_SEND_eng||cmp==DEF_DSL_CLASS_ACT_SEND_kor)return true;else return false;}
inline bool IS_DSL_CLASS_ACT_TIMER(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_CLASS_ACT_TIMER_eng||cmp==DEF_DSL_CLASS_ACT_TIMER_kor)return true;else return false;}
inline bool IS_DSL_CLASS_STMT(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_CLASS_STMT_eng||cmp==DEF_DSL_CLASS_STMT_kor)return true;else return false;}
inline bool IS_DSL_CLASS_INI(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_CLASS_INI_eng||cmp==DEF_DSL_CLASS_INI_kor)return true;else return false;}

inline KCSTR STR_DSL_CLASS_SCE(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_CLASS_SCE_eng;else return DEF_DSL_CLASS_SCE_kor;}
inline KCSTR STR_DSL_CLASS_STATE(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_CLASS_STATE_eng;else return DEF_DSL_CLASS_STATE_kor;}
inline KCSTR STR_DSL_CLASS_PROC(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_CLASS_PROC_eng;else return DEF_DSL_CLASS_PROC_kor;}
inline KCSTR STR_DSL_CLASS_ACT_SEND(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_CLASS_ACT_SEND_eng;else return DEF_DSL_CLASS_ACT_SEND_kor;}
inline KCSTR STR_DSL_CLASS_ACT_TIMER(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_CLASS_ACT_TIMER_eng;else return DEF_DSL_CLASS_ACT_TIMER_kor;}
inline KCSTR STR_DSL_CLASS_STMT(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_CLASS_STMT_eng;else return DEF_DSL_CLASS_STMT_kor;}
inline KCSTR STR_DSL_CLASS_INI(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_CLASS_INI_eng;else return DEF_DSL_CLASS_INI_kor;}

class ClassParser : public StlObject
{
	public:
		typedef enum 
		{
			E_PARSE_NONE = 0,
			E_PARSE_OTYPE,
			E_PARSE_OTYPE_SP,  
			E_PARSE_OTYPE_SP2, 
			E_PARSE_OSPACE,
			E_PARSE_OSPACE_SP, 
			E_PARSE_ONAME,
			E_PARSE_ONAME_SP, 
			E_PARSE_BODY,
			E_PARSE_END,   
			E_PARSE_MAX
		}EParse_t;
		typedef bool (*PFuncParseState)(ClassParser *_pclsObj, const char _cInput);
		ClassParser();
		~ClassParser();
		ClassParser & operator=(ClassParser & _src);
		void CONSTRUCT(void * _pvSrc);
		bool PARSE(KCSTR _src);
		bool parsestep(const char _cInput);
		void DEBUGGING(KSTRING & _debug);
		KCSTR STATUS();
		virtual bool VALIDKEY(KCSTR _key);
		KSTRING m_oType;
		KSTRING m_oSpace;
		KSTRING m_oName;
		KSTRING m_oBody;
		EParse_t m_eSt;
		KSTRING m_result;
		static bool m_fnE_PARSE_NONE(ClassParser *_pclsObj, const char _cInput);
		static bool m_fnE_PARSE_OTYPE(ClassParser *_pclsObj, const char _cInput);
		static bool m_fnE_PARSE_OTYPE_SP(ClassParser *_pclsObj, const char _cInput);
		static bool m_fnE_PARSE_OTYPE_SP2(ClassParser *_pclsObj, const char _cInput);
		static bool m_fnE_PARSE_OSPACE(ClassParser *_pclsObj, const char _cInput);
		static bool m_fnE_PARSE_OSPACE_SP(ClassParser *_pclsObj, const char _cInput);
		static bool m_fnE_PARSE_ONAME(ClassParser *_pclsObj, const char _cInput);
		static bool m_fnE_PARSE_ONAME_SP(ClassParser *_pclsObj, const char _cInput);
		static bool m_fnE_PARSE_BODY(ClassParser *_pclsObj, const char _cInput);
		static bool m_fnE_PARSE_END(ClassParser *_pclsObj, const char _cInput);
		static PFuncParseState m_pfnParseHandle[E_PARSE_MAX];
};
bool validclass(KCSTR _name);
}
#endif
