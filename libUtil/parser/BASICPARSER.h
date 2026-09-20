#ifndef BASICE_PARSER_H
#define BASICE_PARSER_H
#include "KSTRING.h"
#include "COMPSTR.h"
#include "RESTMSG.h"
#include <string>

namespace nsUtil
{
void serialPath(const char * _pszSrc, std::string & _conv);
void deserialPath(const char * _pszSrc, std::string & _conv);
typedef bool (*PFuncSceParserMatch)(KCSTR _cmp);
typedef struct SceParserInfoTbl_t
{
	KUINT m_Enum;
	KCSTR m_kor;
	KCSTR m_eng;
	PFuncSceParserMatch m_pfn;
}SceParserInfoTbl_t;
#define DEF_DSL_K_SCE_kor "시나리오"
#define DEF_DSL_K_INIT_kor "초기"
#define DEF_DSL_K_START_kor "시작"
#define DEF_DSL_PARSE_LENGTH_kor ".길이"
#define DEF_DSL_K_SESSION_kor "세션"	
#define DEF_DSL_K_RCV_MSG_kor "수신메시지"
#define DEF_DSL_K_SND_MSG_kor "전송메시지"
#define DEF_DSL_K_INI_kor "설정"
#define DEF_DSL_K_FUNC_kor "함수"
#define DEF_DSL_K_STATE_kor "상태"
#define DEF_DSL_K_GOTO_kor "상태변경"
#define DEF_DSL_K_BREAK_kor "중단"
#define DEF_DSL_K_STOP_kor "종료"
#define DEF_DSL_K_TYPE_kor "타입"
#define DEF_DSL_K_SVC_TYPE_kor "서비스_타입"
#define DEF_DSL_K_TIME_kor "시간"
#define DEF_DSL_K_LIST_kor "리스트"
#define DEF_DSL_K_TERM_kor "소멸"
#define DEF_DSL_K_ADDR_kor "주소"
#define DEF_DSL_K_OBJ_kor "객체"
#define DEF_DSL_K_METHOD_kor "메소드"
#define DEF_DSL_K_ACTION_EVENT_kor "이벤트명"
#define DEF_DSL_K_DOMAIN_kor "도메인"
#define DEF_DSL_K_RSP_CODE_kor "응답코드"

#define DEF_DSL_K_SCE_eng "SCE"
#define DEF_DSL_K_INIT_eng "INIT"
#define DEF_DSL_K_START_eng "START"
#define DEF_DSL_PARSE_LENGTH_eng ".LENGTH"
#define DEF_DSL_K_SESSION_eng "SESSION"	
#define DEF_DSL_K_RCV_MSG_eng "RCV_MSG"
#define DEF_DSL_K_SND_MSG_eng "SND_MSG"
#define DEF_DSL_K_INI_eng "INI"
#define DEF_DSL_K_FUNC_eng "FUNC"
#define DEF_DSL_K_STATE_eng "STATE"
#define DEF_DSL_K_GOTO_eng "GOTO"
#define DEF_DSL_K_BREAK_eng "BREAK"
#define DEF_DSL_K_STOP_eng "STOP"
#define DEF_DSL_K_TYPE_eng "TYPE"
#define DEF_DSL_K_SVC_TYPE_eng "SVC_TYPE"
#define DEF_DSL_K_TIME_eng "TIME"
#define DEF_DSL_K_LIST_eng "LIST"
#define DEF_DSL_K_TERM_eng "TERM"
#define DEF_DSL_K_ADDR_eng "ADDR"
#define DEF_DSL_K_OBJ_eng "OBJECT"
#define DEF_DSL_K_METHOD_eng "METHOD"
#define DEF_DSL_K_ACTION_EVENT_eng "ACTION_EVENT"
#define DEF_DSL_K_DOMAIN_eng "DOMAIN"
#define DEF_DSL_K_RSP_CODE_eng "RSP_CODE"

inline bool IS_DSL_K_SCE(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_SCE_eng||cmp==DEF_DSL_K_SCE_kor)return true;else return false;}
inline bool IS_DSL_K_INIT(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_INIT_eng||cmp==DEF_DSL_K_INIT_kor)return true;else return false;}
inline bool IS_DSL_K_START(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_START_eng||cmp==DEF_DSL_K_START_kor)return true;else return false;}
inline bool IS_DSL_K_SESSION	(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_SESSION_eng||cmp==DEF_DSL_K_SESSION_kor)return true;else return false;}
inline bool IS_DSL_K_RCV_MSG(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_RCV_MSG_eng||cmp==DEF_DSL_K_RCV_MSG_kor)return true;else return false;}
inline bool IS_DSL_K_SND_MSG(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_SND_MSG_eng||cmp==DEF_DSL_K_SND_MSG_kor)return true;else return false;}
inline bool IS_DSL_K_INI(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_INI_eng||cmp==DEF_DSL_K_INI_kor)return true;else return false;}
inline bool IS_DSL_K_FUNC(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_FUNC_eng||cmp==DEF_DSL_K_FUNC_kor)return true;else return false;}
inline bool IS_DSL_K_STATE(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_STATE_eng||cmp==DEF_DSL_K_STATE_kor)return true;else return false;}
inline bool IS_DSL_K_GOTO(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_GOTO_eng||cmp==DEF_DSL_K_GOTO_kor)return true;else return false;}
inline bool IS_DSL_K_BREAK(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_BREAK_eng||cmp==DEF_DSL_K_BREAK_kor)return true;else return false;}
inline bool IS_DSL_K_STOP(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_STOP_eng||cmp==DEF_DSL_K_STOP_kor)return true;else return false;}

inline bool IS_DSL_K_TYPE(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_TYPE_eng||cmp==DEF_DSL_K_TYPE_kor)return true;else return false;}
inline bool IS_DSL_K_SVC_TYPE(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_SVC_TYPE_eng||cmp==DEF_DSL_K_SVC_TYPE_kor)return true;else return false;}
inline bool IS_DSL_K_TIME(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_TIME_eng||cmp==DEF_DSL_K_TIME_kor)return true;else return false;}
inline bool IS_DSL_K_LIST(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_LIST_eng||cmp==DEF_DSL_K_LIST_kor)return true;else return false;}
inline bool IS_DSL_K_TERM(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_TERM_eng||cmp==DEF_DSL_K_TERM_kor)return true;else return false;}
inline bool IS_DSL_K_ADDR(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_ADDR_eng||cmp==DEF_DSL_K_ADDR_kor)return true;else return false;}
inline bool IS_DSL_K_OBJ(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_OBJ_eng||cmp==DEF_DSL_K_OBJ_kor)return true;else return false;}
inline bool IS_DSL_K_METHOD(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_METHOD_eng||cmp==DEF_DSL_K_METHOD_kor)return true;else return false;}
inline bool IS_DSL_K_ACTION_EVENT(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_ACTION_EVENT_eng||cmp==DEF_DSL_K_ACTION_EVENT_kor)return true;else return false;}
inline bool IS_DSL_K_DOMAIN(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_DOMAIN_eng||cmp==DEF_DSL_K_DOMAIN_kor)return true;else return false;}
inline bool IS_DSL_K_RSP_CODE(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_RSP_CODE_eng||cmp==DEF_DSL_K_RSP_CODE_kor)return true;else return false;}

inline KCSTR STR_DSL_K_SCE(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_SCE_eng;else return DEF_DSL_K_SCE_kor;}
inline KCSTR STR_DSL_K_INIT(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_INIT_eng;else return DEF_DSL_K_INIT_kor;}
inline KCSTR STR_DSL_K_START(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_START_eng;else return DEF_DSL_K_START_kor;}
inline KCSTR STR_DSL_K_SESSION(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_SESSION_eng;else return DEF_DSL_K_SESSION_kor;}
inline KCSTR STR_DSL_K_RCV_MSG(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_RCV_MSG_eng;else return DEF_DSL_K_RCV_MSG_kor;}
inline KCSTR STR_DSL_K_SND_MSG(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_SND_MSG_eng;else return DEF_DSL_K_SND_MSG_kor;}
inline KCSTR STR_DSL_K_INI(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_INI_eng;else return DEF_DSL_K_INI_kor;}
inline KCSTR STR_DSL_K_FUNC(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_FUNC_eng;else return DEF_DSL_K_FUNC_kor;}
inline KCSTR STR_DSL_K_STATE(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_STATE_eng;else return DEF_DSL_K_STATE_kor;}
inline KCSTR STR_DSL_K_GOTO(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_GOTO_eng;else return DEF_DSL_K_GOTO_kor;}
inline KCSTR STR_DSL_K_BREAK(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_BREAK_eng;else return DEF_DSL_K_BREAK_kor;}
inline KCSTR STR_DSL_K_STOP(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_STOP_eng;else return DEF_DSL_K_STOP_kor;}
inline KCSTR STR_DSL_K_LIST(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_LIST_eng;else return DEF_DSL_K_LIST_kor;}

inline KCSTR STR_DSL_K_TYPE(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_TYPE_eng;else return DEF_DSL_K_TYPE_kor;}
inline KCSTR STR_DSL_K_SVC_TYPE(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_SVC_TYPE_eng;else return DEF_DSL_K_SVC_TYPE_kor;}
inline KCSTR STR_DSL_K_TIME(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_TIME_eng;else return DEF_DSL_K_TIME_kor;}
inline KCSTR STR_DSL_K_TERM(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_TERM_eng;else return DEF_DSL_K_TERM_kor;}
inline KCSTR STR_DSL_K_ADDR(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_ADDR_eng;else return DEF_DSL_K_ADDR_kor;}
inline KCSTR STR_DSL_K_OBJ(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_OBJ_eng;else return DEF_DSL_K_OBJ_kor;}
inline KCSTR STR_DSL_K_METHOD(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_METHOD_eng;else return DEF_DSL_K_METHOD_kor;}
inline KCSTR STR_DSL_K_ACTION_EVENT(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_ACTION_EVENT_eng;else return DEF_DSL_K_ACTION_EVENT_kor;}
inline KCSTR STR_DSL_K_DOMAIN(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_DOMAIN_eng;else return DEF_DSL_K_DOMAIN_kor;}
inline KCSTR STR_DSL_K_RSP_CODE(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_RSP_CODE_eng;else return DEF_DSL_K_RSP_CODE_kor;}

class BasicParser : public StlObject
{
	public:
		typedef enum 
		{
			E_PARSE_NONE = 0,
			E_PARSE_KEY,
			E_PARSE_KEY_STATIC,
			E_PARSE_KEY_SP,
			E_PARSE_VAL_PRE,
			E_PARSE_VAL,
			E_PARSE_MAX
		}EParse_t;
		BasicParser();
		~BasicParser();
		BasicParser & operator=(BasicParser & _src);
		void CONSTRUCT(void * _pvSrc);
		bool PARSE(KCSTR _src);
		bool parseStep(const char _cInput);
		KSTRING & KEY();
		KSTRING & VAL();
		KSTRING & TOKEN();
		void STR(KSTRING & _buf);
		void IMPORT(KCSTR _flat);
		EParse_t STATE();
		KCSTR STATUS();
		virtual bool VALIDKEY(KCSTR _key);
		static bool MATCH(const char _cInput,KCSTR _chrList);
		KSTRING m_result;
	private:
		KSTRING m_key;
		KSTRING m_val;
		bool m_bTrim;
		KSTRING m_token;
		void CHANGE(EParse_t _eT);
		bool m_fnE_PARSE_NONE(const char _cInput);
		bool m_fnE_PARSE_KEY(const char _cInput);
		bool m_fnE_PARSE_KEY_STATIC(const char _cInput);
		bool m_fnE_PARSE_KEY_SP(const char _cInput);
		bool m_fnE_PARSE_VAL_PRE(const char _cInput);
		bool m_fnE_PARSE_VAL(const char _cInput);
		EParse_t m_eSt;
};
inline static int endsWithLength_kor(KCSTR _str)
{
    if(_str==NULL) return 0;
    const char *suffix = DEF_DSL_PARSE_LENGTH_kor;
    size_t str_len = strlen(_str);
    size_t suf_len = strlen(suffix);
    if (str_len < suf_len)
        return 0;
    return strcmp(_str + str_len - suf_len, suffix) == 0;
}
inline static int endsWithLength_eng(KCSTR _str)
{
    if(_str==NULL) return 0;
    const char *suffix = DEF_DSL_PARSE_LENGTH_eng;
    size_t str_len = strlen(_str);
    size_t suf_len = strlen(suffix);
    if (str_len < suf_len)
        return 0;
    return strcmp(_str + str_len - suf_len, suffix) == 0;
}
inline static int endsWithLength(KCSTR _str)
{
	if(_str==NULL) return 0;
	if(strstr(_str,DEF_DSL_PARSE_LENGTH_kor))
	{
		return endsWithLength_kor(_str);
	}
	else if(strstr(_str,DEF_DSL_PARSE_LENGTH_eng))
	{
		return endsWithLength_eng(_str);		
	}
	return 0;
}
inline static KCSTR STROBJ(KSTRING & _val)
{
	if(_val.LENGTH()==0) return "";
	return (KCSTR)_val;
}
inline static bool STRNCMP(KCSTR _val1, KCSTR _val2)
{
	if(_val1 == NULL || _val2 == NULL) return false;
	if(strncmp(_val1,_val2,strlen(_val2))==0) return true;
	return false;
}
inline static bool STRSTR(KCSTR _val1, KCSTR _val2)
{
	if(_val1 == NULL || _val2 == NULL) return false;
	if(strlen(_val1)==0 || strlen(_val2)==0) return false;
	if(strstr(_val1,_val2)) return true;
	return false;
}
inline static void CPJPARAM(RestParam & _src,
							RestParam & _dst,
							KCSTR _keyName)
{
	_dst = _src;
	_dst.KEY() = _keyName;
	_dst.setkey(_keyName);
}
inline static void INITARR(RestParam & _list)
{
	_list.ARR(); _list.DEL((KUINT)0);
}
}
#endif
