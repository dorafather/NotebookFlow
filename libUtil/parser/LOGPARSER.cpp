#include "LOGPARSER.h"
namespace nsUtil
{
LogParser::LogParser()
{
	m_eSt = E_PARSE_NONE; 
	m_bDotSeen = false;
}
LogParser::~LogParser()
{
}
void LogParser::CLEAR()
{
	m_eSt = E_PARSE_NONE;
	m_listParam.CLEAR();
	m_tmp=KNULL;
	m_bDotSeen=false;
}
bool LogParser::PARSE(KCSTR _src)
{
	if(_src==NULL) return false;
	KUINT len = strlen(_src);
	for(KUINT i=0;i<len;i++)
	{
		if(!parsestep((const char)_src[i])) return false;
	}
	if(m_eSt != E_PARSE_END)
	{
		m_result.PRINT("log() illegal parse state(%s)", STATUS());
		return false;
	}
	return true;
}
bool LogParser::parsestep(const char _cInput)
{
	switch(m_eSt)
	{
		case E_PARSE_NONE     : return m_fnE_PARSE_NONE    (_cInput);   
		case E_PARSE_LOG      : return m_fnE_PARSE_LOG     (_cInput);
		case E_PARSE_LOG_SP   : return m_fnE_PARSE_LOG_SP  (_cInput);
		case E_PARSE_LEVEL    : return m_fnE_PARSE_LEVEL   (_cInput);
		case E_PARSE_LEVEL_SP : return m_fnE_PARSE_LEVEL_SP(_cInput);
		case E_PARSE_PARAM    : return m_fnE_PARSE_PARAM   (_cInput);
		case E_PARSE_PARAM_SP : return m_fnE_PARSE_PARAM_SP(_cInput);
		case E_PARSE_END      : return m_fnE_PARSE_END     (_cInput);
		default: return m_fnE_PARSE_NONE(_cInput); 
	};
	return false;
}
void LogParser::ENCODE(RestMsg & _encMsg)
{
	m_Level = _encMsg.GET("log-level").VAL();
	m_listParam.CLEAR();		
	RestParam & logs = _encMsg.GET("logs");
	for(KUINT i=0;i<logs.NUMS();i++)
	{
		m_listParam.PUSH().VAL() = logs[i].VAL();
	}
}
void LogParser::DEBUGGING(KSTRING & _debug)
{
	_debug.PRINT("%s: [",(KCSTR)m_Level);
	for(KUINT i=0;i<m_listParam.NUMS();i++)
	{
		_debug.PRINT("%s,",(KCSTR)m_listParam[i].VAL());
	}
	_debug<<"]\n";
}
void LogParser::JSON(RestParam & _item)
{
	_item.SET("exe-type").VAL() = (KCSTR)m_name;
	KSTRING & func = _item.SET("exe-name").VAL();
	func.PRINT("%s(",(KCSTR)m_Level);
	for(KUINT i=0;i<m_listParam.NUMS();i++)
	{
		KSTRING & aVal = m_listParam[i].VAL();
		if(i == (m_listParam.NUMS()-1))
		{
			func.PRINT("%s)",(KCSTR)aVal);
		}
		else
		{
			func.PRINT("%s ",(KCSTR)aVal);
		}
	}
}
void LogParser::STR(KSTRING & _buf)
{
	_buf.PRINT("%s.%s(",(KCSTR)m_name,(KCSTR)m_Level);
	for(KUINT i=0;i<m_listParam.NUMS();i++)
	{
		KSTRING & aVal = m_listParam[i].VAL();
		if(i == (m_listParam.NUMS()-1))
		{
			_buf.PRINT("%s)",(KCSTR)aVal);
		}
		else
		{
			_buf.PRINT("%s ",(KCSTR)aVal);
		}
	}
	_buf<<"\n";
}
LogParser::EParse_t LogParser::STATE()
{
	return m_eSt;
}
KCSTR LogParser::STATUS()
{
	switch(m_eSt)
	{
		case E_PARSE_NONE    : return "PARSE_NONE";
		case E_PARSE_LOG     : return "PARSE_LOG";
		case E_PARSE_LOG_SP  : return "PARSE_LOG_SP";
		case E_PARSE_LEVEL   : return "PARSE_LEVEL";
		case E_PARSE_LEVEL_SP: return "PARSE_LEVEL_SP";
		case E_PARSE_PARAM   : return "PARSE_PARAM";
		case E_PARSE_PARAM_SP: return "PARSE_PARAM_SP";
		case E_PARSE_END     : return "PARSE_END";
		case E_PARSE_MAX     : return "PARSE_MAX";
		default: return "PARSE_NONE";
	};
	return "PARSE_NONE";
}
void LogParser::CHANGE(EParse_t _eT)
{
	m_eSt = _eT;
}
bool LogParser::m_fnE_PARSE_NONE    (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \r\n\t"))
	{
	}
	else
	{
		m_name<<_cInput;
		CHANGE(E_PARSE_LOG);
	}
	return true;
}
bool LogParser::m_fnE_PARSE_LOG     (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \r\n\t."))
	{
		if(_cInput == '.') m_bDotSeen = true;
		CHANGE(E_PARSE_LOG_SP);
	}
	else
	{
		m_name<<_cInput;
	}
	return true;
}
bool LogParser::m_fnE_PARSE_LOG_SP  (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \r\n\t"))
	{
	}
	else if(_cInput == '.')
	{
		if(m_bDotSeen)
		{
			m_result.PRINT("log duplicate dot accessor");
			return false;
		}
		m_bDotSeen = true;
	}
	else
	{
		m_Level<<_cInput;
		CHANGE(E_PARSE_LEVEL);
	}
	return true;
}
bool LogParser::m_fnE_PARSE_LEVEL   (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \r\n\t("))
	{
		CHANGE(E_PARSE_LEVEL_SP);
		return true;
	}
	else
	{
		m_Level<<_cInput;
	}
	return true;
}
bool LogParser::m_fnE_PARSE_LEVEL_SP(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \r\n\t"))
	{
	}
	else if(_cInput == '(')
	{
		m_result.PRINT("log(%s) duplicate open paren",(KCSTR)m_Level);
		return false;
	}
	else
	{
		m_tmp<<_cInput;
		CHANGE(E_PARSE_PARAM);
	}
	return true;
}
bool LogParser::m_fnE_PARSE_PARAM   (const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \r\n\t"))
	{
		m_listParam.PUSH().VAL() = (KCSTR)m_tmp;
		m_tmp = KNULL;
		CHANGE(E_PARSE_PARAM_SP);
	}
	else if(_cInput == ')')
	{
		m_listParam.PUSH().VAL() = (KCSTR)m_tmp;
		m_tmp = KNULL;
		CHANGE(E_PARSE_END);
	}
	else
	{
		m_tmp<<_cInput;
	}
	return true;
}
bool LogParser::m_fnE_PARSE_PARAM_SP(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \r\n\t"))
	{
	}
	else if(_cInput == ')')
	{
		m_tmp = KNULL;
		CHANGE(E_PARSE_END);
	}
	else
	{
		m_tmp<<_cInput;
		CHANGE(E_PARSE_PARAM);
	}
	return true;
}
bool LogParser::m_fnE_PARSE_END     (const char _cInput)
{
	if(!BasicParser::MATCH(_cInput," \r\n\t"))
	{
		m_result.PRINT("log(%s) unexpected char after %c",(KCSTR)m_Level,_cInput);
		return false;
	}
	return true;
}
}