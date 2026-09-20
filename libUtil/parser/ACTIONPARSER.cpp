#include "ACTIONPARSER.h"

namespace nsUtil
{
ActionParser::ActionParser()
{
}
ActionParser::~ActionParser()
{
}
ActionParser & ActionParser::operator=(ActionParser & _src)
{
	CONSTRUCT((void *)&_src);
	return *this;
}
AssignParser & ActionParser::operator[](KUINT _idx)
{
	AssignParser * pv = (AssignParser*)m_list.index(_idx);
	if(pv == NULL)
	{
		return m_def;
	}
	return *pv;
}
void ActionParser::CONSTRUCT(void * _pvSrc)
{
	m_list.clear();
	ActionParser * pSrc = (ActionParser*)_pvSrc;
	m_oType = pSrc->m_oType;
	m_oName = pSrc->m_oName;
	m_oSpace = pSrc->m_oSpace;
	setkey((KCSTR)pSrc->m_oName);
	Iterator itr;
	AssignParser * pFind = (AssignParser*)pSrc->m_list.next(itr);
	while(pFind)
	{
		AssignParser * pNew = new AssignParser;
		*pNew = *pFind;
		pFind = (AssignParser*)pSrc->m_list.next(itr);
	}
}
bool ActionParser::PARSE(ClassParser & _class)
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
		std::string conv;
		serialPath(pLine,  conv);
		AssignParser *pAssign = new AssignParser;
		if(!pAssign->PARSE(conv.c_str()))
		{
			m_result.PRINT("act(%s) %s",(KCSTR)m_oName,(KCSTR)pAssign->m_result);
			delete pAssign;
			return false;
		}
		m_list.pushback(pAssign);
	}
	return true;
}
void ActionParser::JSON(RestParam & _item)
{
	ActionParser & my = (ActionParser&)*this;
	_item.SET("action-type").VAL() = m_oType;
	_item.SET("name-space").VAL() = m_oSpace;
	_item.SET("action-name").VAL() = m_oName;
	RestParam & list = _item.SET("param-list");
	INITARR(list);
	for(KUINT i=0;i<my.NUMS();i++)
	{
		AssignParser & asign = my[i];
		RestParam & pitem = list.ARR();
		asign.JSON(pitem);
	}
}
void ActionParser::STR(KSTRING & _buf)
{
	_buf.PRINT("%s::%s.%s\n{\n",(KCSTR)m_oType,
								(KCSTR)m_oSpace,
								(KCSTR)m_oName);
	ActionParser & my = (ActionParser&)*this;
	for(KUINT i=0;i<my.NUMS();i++)
	{
		AssignParser & asign = my[i];
		_buf<<"  ";
		asign.STR(_buf);
	}
	_buf.PRINT("}\n");
}
void ActionParser::IMPORT(RestParam & _item)
{
	m_list.clear();
	m_oType = _item.GET("action-type").VAL();
	m_oSpace = _item.GET("name-space").VAL();
	m_oName = _item.GET("action-name").VAL();
	setkey((KCSTR)m_oName);
	RestParam & list = _item.GET("param-list");
	for(KUINT i=0;i<list.NUMS();i++)
	{
		AssignParser * pNew = new AssignParser;
		pNew->IMPORT(list[i]);
		m_list.pushback(pNew);
	}
}
KCSTR ActionParser::EVENT(KSTRING & _val)
{
	ActionParser & my = (ActionParser&)*this;
	for(KUINT i=0;i<my.NUMS();i++)
	{
		AssignParser & asign = my[i];
		if(IS_DSL_K_SND_MSG((KCSTR)asign.m_AB.KEY()))
		{
			if(IS_DSL_K_TYPE((KCSTR)asign.m_AB.VAL()))
			{
				_val = asign.m_CD.KEY();
				return (KCSTR)_val;
			}
		}
	}
	return (KCSTR)_val;
}
KUINT ActionParser::NUMS(){return m_list.size();}
}
