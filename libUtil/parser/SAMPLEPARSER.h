#ifndef SAMPLE_PARSER_H
#define SAMPLE_PARSER_H
#include "KSTRING.h"

namespace nsUtil
{
class parseLine
{
	public:
		typedef enum 
		{
			E_PARSE_NONE = 0,
			E_PARSE_ONE,
			E_PARSE_TWO,
			E_PARSE_MAX
		}EParse_t;
		parseLine(){m_eSt = E_PARSE_NONE;}
		~parseLine(){}
		bool PARSESTR(KCSTR _src)
		{
			if(_src==NULL) return false;
			KUINT len = strlen(_src);
			for(KUINT i=0;i<len;i++)
			{
				if(!PARSE((const char)_src[i])) return false;
			}
			m_eSt = E_PARSE_MAX;
			return true;
		}
		bool PARSE(const char _cInput)
		{
			switch(m_eSt)
			{
				case E_PARSE_NONE: return m_fnE_PARSE_NONE(_cInput); 
				case E_PARSE_ONE: return m_fnE_PARSE_ONE(_cInput); 
				case E_PARSE_TWO: return m_fnE_PARSE_TWO(_cInput); 
				default: return m_fnE_PARSE_NONE(_cInput); 
			};
			return false;
		}
		static bool MATCH(const char _cInput,ACSTR _chrList)
		{
			if(_chrList==NULL) return false;
			for(AUINT i=0;i<strlen(_chrList);i++)
			{
				if(_chrList[i] == _cInput) return true;
			}
			return false;
		}
		EParse_t STATE(){return m_eSt;}
	private:
		void CHANGE(EParse_t _eT){m_eSt = _eT;}
		bool m_fnE_PARSE_NONE(const char _cInput){return true;}
		bool m_fnE_PARSE_ONE(const char _cInput){return true;}
		bool m_fnE_PARSE_TWO(const char _cInput){return true;}
		EParse_t m_eSt;
};
}
#endif
