#include "STATEPARSER.h"
#include "BASICPARSER.h"

namespace nsUtil
{
StateParser::item::item()
{
}
StateParser::item::~item()
{
}
 void StateParser::item::CONSTRUCT(void * _pvSrc)
{
	item * psrc = (item*)_pvSrc;
	m_name = psrc->m_name;
	m_rcvmsg = psrc->m_rcvmsg;
	m_param = psrc->m_param;
	m_event = psrc->m_event;
	m_class = psrc->m_class;
	m_proc = psrc->m_proc;
	setkey(psrc->m_pszKey);
}
 void StateParser::item::JSON(RestParam & _item)
{
	_item.SET("name-space").VAL() = m_name;
	_item.SET("event-param").VAL() = m_param;
	_item.SET("event-name").VAL() = m_event;
	_item.SET("procedure-name").VAL() = m_proc;
}
void StateParser::item::STR(KSTRING & _buf) 
{
	_buf.PRINT("  %s.%s.%s == %s    %s.%s\n",
							(KCSTR)m_name,
							(KCSTR)m_rcvmsg,
							(KCSTR)m_param,
							(KCSTR)m_event,
							(KCSTR)m_class,
							(KCSTR)m_proc);
}
void StateParser::item::IMPORT(RestParam & _item)
{
	m_name = _item.GET("name-space").VAL();
	m_rcvmsg = DEF_DSL_K_RCV_MSG_kor;
	m_param = _item.GET("event-param").VAL();
	m_event = _item.GET("event-name").VAL();
	m_class = DEF_DSL_CLASS_PROC_kor;
	m_proc = _item.GET("procedure-name").VAL();
}
StateParser::StateParser()
{
	m_curr = NULL;
}
StateParser::~StateParser()
{
}
StateParser & StateParser::operator=(StateParser & _src)
{
	CONSTRUCT((void *)&_src);
	return *this;
}
StateParser::item & StateParser::operator[](KUINT _idx)
{
	item * pv = (item*)m_list.index(_idx);
	if(pv==NULL)
	{
		return m_def;
	}
	return *pv;
}
void StateParser::CONSTRUCT(void * _pvSrc)
{
	m_list.clear();
	StateParser * pSrc = (StateParser*)_pvSrc;
	m_oType = pSrc->m_oType;
	m_oName = pSrc->m_oName;
	setkey((KCSTR)pSrc->m_oName);
	Iterator itr;
	item * pFind = (item*)pSrc->m_list.next(itr);
	while(pFind)
	{
		item * pNew = new item;
		*pNew = *pFind;
		pFind = (item*)pSrc->m_list.next(itr);
	}
}
bool StateParser::PARSE(ClassParser & _class)
{
	m_oType = _class.m_oType;
	m_oName = _class.m_oName;
	m_oSpace = _class.m_oSpace;
	setkey((KCSTR)m_oName);
	TOKSTR tokline;
	tokline = _class.m_oBody;
	tokline.TOK("\r\n");
	for(KUINT i=0;i<tokline.NUMS();i++)
	{
		KSTR pLine = KSTRING::m_fnGetOptimizeString((KSTR)tokline[i]," \t");
		if(pLine==NULL) continue;
		if(pLine && (pLine[0] == '/' || pLine[0]=='#'))
		{
			continue; 
		}
		REINIT();
		std::string conv;
		serialPath(pLine,  conv);
		COMPSTR buf(conv.c_str());
		for(KUINT i=0;i<buf.LENGTH();i++)
		{
			if(!parseStep((const char)conv[i])) return false;
		}
		KSTRING tmp;
		m_curr->STR(tmp);
	}
	return true;
}
void StateParser::REINIT()
{
	m_eSt = E_PARSE_NONE;
	m_curr = NULL;
}
void StateParser::CHANGE(EParse_t _eT)
{
	m_eSt = _eT;
}
bool StateParser::parseStep(const char _cInput)
{
	switch(m_eSt)
	{
		case E_PARSE_NONE: return m_fnE_PARSE_NONE(_cInput); 
		case E_PARSE_NAME: return m_fnE_PARSE_NAME(_cInput); 
		case E_PARSE_NAME_SP: return m_fnE_PARSE_NAME_SP(_cInput); 
		case E_PARSE_RCV_MSG: return m_fnE_PARSE_RCV_MSG(_cInput);
		case E_PARSE_RCV_MSG_SP: return m_fnE_PARSE_RCV_MSG_SP(_cInput);
		case E_PARSE_PARAM: return m_fnE_PARSE_PARAM(_cInput);
		case E_PARSE_PARAM_SP: return m_fnE_PARSE_PARAM_SP(_cInput);
		case E_PARSE_EVENT: return m_fnE_PARSE_EVENT(_cInput);
		case E_PARSE_EVENT_SP: return m_fnE_PARSE_EVENT_SP(_cInput); 
		case E_PARSE_CLASS: return m_fnE_PARSE_CLASS(_cInput); 
		case E_PARSE_CLASS_SP: return m_fnE_PARSE_CLASS_SP(_cInput); 
		case E_PARSE_PROC: return m_fnE_PARSE_PROC(_cInput); 
		default: return m_fnE_PARSE_NONE(_cInput); 
	};
	return false;
}
bool StateParser::m_fnE_PARSE_NONE(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t"))
	{
		//skipp
	}
	else
	{
		m_curr = new item;
		m_curr->m_name<<_cInput;
		m_list.pushback(m_curr);
		CHANGE(E_PARSE_NAME);
	}
	return true;
}
bool StateParser::m_fnE_PARSE_NAME(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," .\t"))
	{
		CHANGE(E_PARSE_NAME_SP);
	}
	else
	{
		m_curr->m_name<<_cInput;
	}
	return true;
}
bool StateParser::m_fnE_PARSE_NAME_SP(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," .\t"))
	{
		// skipp
	}
	else
	{
		m_curr->m_rcvmsg<<_cInput;
		CHANGE(E_PARSE_RCV_MSG);
	}
	return true;
}
bool StateParser::m_fnE_PARSE_RCV_MSG(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," .\t"))
	{
		CHANGE(E_PARSE_RCV_MSG_SP);
	}
	else
	{
		m_curr->m_rcvmsg<<_cInput;
	}
	return true;
}
bool StateParser::m_fnE_PARSE_RCV_MSG_SP(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," .\t"))
	{
		// skipp
	}
	else
	{
		m_curr->m_param<<_cInput;
		CHANGE(E_PARSE_PARAM);
	}
	return true;
}
bool StateParser::m_fnE_PARSE_PARAM(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," =\t"))
	{
		CHANGE(E_PARSE_PARAM_SP);
	}
	else
	{
		m_curr->m_param<<_cInput;
	}
	return true;
}
bool StateParser::m_fnE_PARSE_PARAM_SP(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," =\t"))
	{
		// skipp
	}
	else
	{
		m_curr->m_event<<_cInput;
		CHANGE(E_PARSE_EVENT);
	}
	return true;
}
bool StateParser::m_fnE_PARSE_EVENT(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t"))
	{
		CHANGE(E_PARSE_EVENT_SP);
	}
	else
	{
		m_curr->m_event<<_cInput;
	}
	return true;
}
bool StateParser::m_fnE_PARSE_EVENT_SP(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t"))
	{
		// skipp
	}
	else
	{
		CHANGE(E_PARSE_CLASS);
		m_curr->m_class<<_cInput;
	}
	return true;
}
bool StateParser::m_fnE_PARSE_CLASS(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," .\t"))
	{
		CHANGE(E_PARSE_CLASS_SP);
	}
	else
	{
		m_curr->m_class<<_cInput;
	}
	return true;
}
bool StateParser::m_fnE_PARSE_CLASS_SP(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," .\t"))
	{
		// skipp
	}
	else
	{
		if(!IS_DSL_CLASS_PROC(m_curr->m_class))
		{
			m_result.PRINT("state() Illegal PROC keyword %s",m_oName,
													m_tmpProc);
			return false;
		}
		m_curr->m_proc<<_cInput;
		CHANGE(E_PARSE_PROC);
	}
	return true;
}
bool StateParser::m_fnE_PARSE_PROC(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," "))
	{
		// skipp
	}
	else
	{
		m_curr->m_proc<<_cInput;
	}
	return true;
}
void StateParser::JSON(RestParam & _item)
{
	StateParser & my = (StateParser&)*this;
	_item.SET("state-name-space").VAL() = m_oSpace;
	_item.SET("state-name").VAL() = m_oName;
	RestParam & t = _item.SET("event-list");
	INITARR(t);
	for(KUINT i=0;i<m_list.size();i++)
	{
		item & val = my[i];
		RestParam & newItem = t.ARR();
		val.JSON(newItem);
	}
}
void StateParser::STR(KSTRING & _buf)
{
	_buf.PRINT("%s::%s.%s\n{\n",(KCSTR)m_oType,
							   (KCSTR)m_oSpace,
							   (KCSTR)m_oName);
	StateParser & my = (StateParser&)*this;
	for(KUINT i=0;i<m_list.size();i++)
	{
		item & val = my[i];
		val.STR(_buf);
	}
	_buf.PRINT("}\n");
}
void StateParser::IMPORT(RestParam & _item)
{
	m_list.clear();
	m_oType = DEF_DSL_CLASS_STATE_kor;
	m_oSpace = _item.GET("state-name-space").VAL();
	m_oName = _item.GET("state-name").VAL();
	setkey((KCSTR)m_oName);
	RestParam & evList = _item.GET("event-list");
	for(KUINT i=0;i<evList.NUMS();i++)
	{
		item * pNew = new item;
		pNew->IMPORT(evList[i]);
		m_list.pushback(pNew);
	}
}
KUINT StateParser::NUMS(){return m_list.size();}
}
