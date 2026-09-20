#ifndef LOG_PARSER_H
#define LOG_PARSER_H
#include "BASICPARSER.h"
#include "LIST.h"

namespace nsUtil
{
#define DEF_DSL_K_LOG_eng "LOG"
#define DEF_DSL_K_STD_eng "info"
#define DEF_DSL_K_LOG_kor "로그"
#define DEF_DSL_K_STD_kor "출력"

inline bool IS_DSL_K_LOG(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_LOG_eng||cmp==DEF_DSL_K_LOG_kor)return true;else return false;}
inline bool IS_DSL_K_STD(KCSTR _cmp){COMPSTR cmp;cmp=_cmp;if(cmp==DEF_DSL_K_STD_eng||cmp==DEF_DSL_K_STD_kor)return true;else return false;}
inline KCSTR STR_DSL_K_LOG(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_LOG_eng;else return DEF_DSL_K_STD_kor;}
inline KCSTR STR_DSL_K_STD(KCSTR _lang){COMPSTR cmp(_lang);if(cmp=="eng")return DEF_DSL_K_STD_eng;else return DEF_DSL_K_STD_kor;}

class LogParser
{
	public:
		typedef enum 
		{
			E_PARSE_NONE = 0,
			E_PARSE_LOG,
			E_PARSE_LOG_SP,
			E_PARSE_LEVEL,
			E_PARSE_LEVEL_SP,
			E_PARSE_PARAM,
			E_PARSE_PARAM_SP,
			E_PARSE_END,
			E_PARSE_MAX
		}EParse_t;
		LogParser();
		~LogParser();
		void CLEAR();
		bool PARSE(KCSTR _src);
		bool parsestep(const char _cInput);
		void ENCODE(RestMsg & _encMsg);
		void DEBUGGING(KSTRING & _debug);
		void JSON(RestParam & _item);
		void STR(KSTRING & _buf);
		EParse_t STATE();
		KCSTR STATUS();
		KSTRING m_name;
		KSTRING m_Level;
		ALIST m_listParam;
		KSTRING m_result;
	private:
		void CHANGE(EParse_t _eT);
		bool m_fnE_PARSE_NONE    (const char _cInput);   
		bool m_fnE_PARSE_LOG     (const char _cInput);
		bool m_fnE_PARSE_LOG_SP  (const char _cInput);
		bool m_fnE_PARSE_LEVEL   (const char _cInput);
		bool m_fnE_PARSE_LEVEL_SP(const char _cInput);
		bool m_fnE_PARSE_PARAM   (const char _cInput);
		bool m_fnE_PARSE_PARAM_SP(const char _cInput);
		bool m_fnE_PARSE_END     (const char _cInput);
		EParse_t m_eSt;
		KSTRING m_tmp;
		bool m_bDotSeen;
};
}
#endif
