#ifndef STATE_PARSER_H
#define STATE_PARSER_H
#include "KSTRING.h"
#include "COMPSTR.h"
#include "CLASSPARSER.h"
#include "BASICPARSER.h"

namespace nsUtil
{
class StateParser : public StlObject
{
	public:
		typedef enum 
		{
			E_PARSE_NONE = 0,
			E_PARSE_NAME,
			E_PARSE_NAME_SP,	
			E_PARSE_RCV_MSG,
			E_PARSE_RCV_MSG_SP,
			E_PARSE_PARAM,
			E_PARSE_PARAM_SP,
			E_PARSE_EVENT,
			E_PARSE_EVENT_SP,
			E_PARSE_CLASS,
			E_PARSE_CLASS_SP,
			E_PARSE_PROC,
			E_PARSE_MAX
		}EParse_t;
		class item : public StlObject
		{
			public:
				item();
				~item();
				void CONSTRUCT(void * _pvSrc);
				void JSON(RestParam & _item);
				void STR(KSTRING & _buf); 
				void IMPORT(RestParam & _item);
				KSTRING m_name;
				KSTRING m_rcvmsg;
				KSTRING m_param;
				KSTRING m_event;
				KSTRING m_class;
				KSTRING m_proc;
		};
		StateParser();
		~StateParser();
		StateParser & operator=(StateParser & _src);
		item & operator[](KUINT _idx);
		void CONSTRUCT(void * _pvSrc);
		bool PARSE(ClassParser & _class);
		bool parseStep(const char _cInput);
		void REINIT();
		void JSON(RestParam & _item);
		void STR(KSTRING & _buf);
		void IMPORT(RestParam & _item);
		KUINT NUMS();
		void CHANGE(EParse_t _eT);
		bool m_fnE_PARSE_NONE(const char _cInput);
		bool m_fnE_PARSE_NAME(const char _cInput);
		bool m_fnE_PARSE_NAME_SP(const char _cInput);
		bool m_fnE_PARSE_RCV_MSG(const char _cInput);
		bool m_fnE_PARSE_RCV_MSG_SP(const char _cInput);
		bool m_fnE_PARSE_PARAM(const char _cInput);
		bool m_fnE_PARSE_PARAM_SP(const char _cInput);
		bool m_fnE_PARSE_EVENT(const char _cInput);
		bool m_fnE_PARSE_EVENT_SP(const char _cInput);
		bool m_fnE_PARSE_CLASS(const char _cInput);
		bool m_fnE_PARSE_CLASS_SP(const char _cInput);
		bool m_fnE_PARSE_PROC(const char _cInput);
		EParse_t m_eSt;
		KSTRING m_oType;
		KSTRING m_oSpace;
		KSTRING m_oName;
		KSTRING m_result;
		KSTRING m_tmpProc;
		item *m_curr;
	private:
		StlList m_list;
		item m_def;
};
}
#endif
