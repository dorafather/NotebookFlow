#include "PROCPARSER.h"

namespace nsUtil
{
ProcParser::Procedure::ProcExe::ProcExe(){m_exe.TOKEN()=".";}
ProcParser::Procedure::ProcExe::~ProcExe(){}
bool ProcParser::Procedure::ProcExe::PARSE(KCSTR _data)
{
	if(STRNCMP(_data,DEF_DSL_K_FUNC_eng) ||
			STRNCMP(_data,DEF_DSL_K_FUNC_kor))
	{
		if(!m_func.PARSE(_data))
		{
			m_result = m_func.m_result;
			return false;
		}
		m_type = "FUNC";
	}
	else if(STRNCMP(_data,DEF_DSL_K_LOG_eng)
		||STRNCMP(_data,DEF_DSL_K_LOG_kor) )
	{
		m_type = "LOG";
		if(!m_log.PARSE(_data))
		{
			m_result = m_log.m_result;
			return false;
		}
	}
	else
	{
		if(!m_exe.PARSE(_data))
		{
			m_result = m_exe.m_result;
			return false;
		}
		if(m_exe.VAL().LENGTH()==0)
		{
			m_result.PRINT("class(%s) empty obj name",(KCSTR)m_exe.KEY());
			return false;
		}
		m_type = "PROC";
	}
	return true;
}
void ProcParser::Procedure::ProcExe::JSON(RestParam & _item)
{
	if(m_type == "FUNC")
	{
		m_func.JSON(_item);
	}
	else if(m_type == "LOG")
	{
		m_log.JSON(_item);
	}
	else if(m_type == "PROC")
	{
		_item.SET("exe-type").VAL() = m_exe.KEY();
		_item.SET("exe-name").VAL() = m_exe.VAL();
	}
}
void ProcParser::Procedure::ProcExe::STR(KSTRING & _buf)
{
	if(m_type == "FUNC")
	{
		m_func.STR(_buf);
	}
	else if(m_type == "LOG")
	{
		m_log.STR(_buf);
	}
	else if(m_type == "PROC")
	{
		_buf.PRINT("%s.%s\n",(KCSTR)m_exe.KEY(),
							(KCSTR)m_exe.VAL());
	}
}
void ProcParser::Procedure::ProcExe::IMPORT(RestParam & _item)
{
	KCSTR exeType = (KCSTR)_item.GET("exe-type").VAL();
	if(IS_DSL_K_FUNC(exeType))
	{
		m_type = "FUNC";
		m_func.IMPORT(_item);
	}
	else
	{
		m_type = "PROC";
		m_exe.KEY() = exeType;
		m_exe.VAL() = (KCSTR)_item.GET("exe-name").VAL();
	}
}
ProcParser::Procedure::Procedure(){}
ProcParser::Procedure::~Procedure(){}
bool ProcParser::Procedure::PARSE(KCSTR _data)
{
	ProcExe * pNew = new ProcExe;
	if(!pNew->PARSE(_data))
	{
		delete pNew;
		return false;
	}
	m_list.pushback(pNew);
	return true;
}
void ProcParser::Procedure::JSON(RestParam & _item)
{
	m_cond.JSON(_item.SET("condition"));
	KSTRING iftype;
	if(IS_DSL_K_IF((KCSTR)m_cond.TYPE()))
	{
		iftype = "if-list";
	}
	else if(IS_DSL_K_ELSEIF((KCSTR)m_cond.TYPE()))
	{
		iftype = "elseif-list";
	}
	else if(IS_DSL_K_ELSE((KCSTR)m_cond.TYPE()))
	{
		iftype = "else-list";
	}
	Procedure & my = (Procedure&)*this;
	RestParam & condList = _item.SET((KCSTR)iftype);
	INITARR(condList);
	for(KUINT i=0;i<my.NUMS();i++)
	{
		ProcExe & exeP = my[i];
		RestParam & exeItem = condList.ARR();
		exeP.JSON(exeItem);
	}
}
void ProcParser::Procedure::STR(KSTRING & _buf)
{
	_buf<<"  ";
	m_cond.STR(_buf);
	Procedure & my = (Procedure&)*this;
	for(KUINT i=0;i<my.NUMS();i++)
	{
		ProcExe & exeP = my[i];
		_buf<<"    ";
		exeP.STR(_buf);
	}
}
void ProcParser::Procedure::IMPORT(RestParam & _item)
{
	m_list.clear();
	m_cond.IMPORT(_item.GET("condition"));
	KSTRING iftype;
	if(IS_DSL_K_IF((KCSTR)m_cond.TYPE())) iftype = "if-list";
	else if(IS_DSL_K_ELSEIF((KCSTR)m_cond.TYPE())) iftype = "elseif-list";
	else if(IS_DSL_K_ELSE((KCSTR)m_cond.TYPE())) iftype = "else-list";
	RestParam & exeList = _item.GET((KCSTR)iftype);
	for(KUINT i=0;i<exeList.NUMS();i++)
	{
		ProcExe * pNew = new ProcExe;
		pNew->IMPORT(exeList[i]);
		m_list.pushback(pNew);
	}
}
ProcParser::Procedure::ProcExe & ProcParser::Procedure::operator[](KUINT _idx)
{
	ProcExe * pv = (ProcExe*)m_list.index(_idx);
	if(pv == NULL)
	{
		return m_defExe;
	}
	return *pv;
}
KUINT ProcParser::Procedure::NUMS()
{
	return m_list.size();
}
ProcParser::ProcParser()
{
}
ProcParser::~ProcParser()
{
}
ProcParser & ProcParser::operator=(ProcParser & _src)
{
	CONSTRUCT((void *)&_src);
	return *this;
}
ProcParser::Procedure & ProcParser::operator[](KUINT _idx)
{
	Procedure * pv = (Procedure*)m_list.index(_idx);
	if(pv==NULL)
	{
		return m_def;
	}
	return *pv;
}
void ProcParser::CONSTRUCT(void * _pvSrc)
{
	ProcParser * pSrc = (ProcParser*)_pvSrc;
	m_oType = pSrc->m_oType;
	m_oName = pSrc->m_oName;
	m_oSpace = pSrc->m_oSpace;
	setkey((KCSTR)pSrc->m_oName);
	m_conv = pSrc->m_conv;	
}
bool ProcParser::PARSE(ClassParser & _class)
{
	m_oType = _class.m_oType;
	m_oName = _class.m_oName;
	m_oSpace = _class.m_oSpace;
	setkey((KCSTR)m_oName);
	TOKSTR tokLine((KCSTR)_class.m_oBody); tokLine.TOK("\r\n");
	Procedure * pCur = NULL;
	for(KUINT i=0;i<tokLine.NUMS();i++)
	{
		KSTR pLine = KSTRING::m_fnGetOptimizeString((KSTR)tokLine[i]," \t");
		if(pLine==NULL) continue;
		if(pLine[0] == '/' || pLine[0]=='#')
		{
			continue;
		}
		std::string conv;
		serialPath(pLine, conv);
		BasicParser dslparam;
		dslparam.TOKEN()=".";
		dslparam.PARSE(conv.c_str());
		
		if(STRNCMP(conv.c_str(),DEF_DSL_K_IF_eng)
			||STRNCMP(conv.c_str(),DEF_DSL_K_IF_kor) )
		{
			pCur = new Procedure;
			if(!pCur->m_cond.PARSE(conv.c_str()))
			{
				m_result.PRINT("proc(%s) %s",(KCSTR)m_oName,
												(KCSTR)pCur->m_cond.m_result);
				delete pCur;
				return false;
			}		
			m_list.pushback(pCur);
		}
		else if(STRNCMP(conv.c_str(),DEF_DSL_K_ELSEIF_eng) 
			||STRNCMP(conv.c_str(),DEF_DSL_K_ELSEIF_kor)  )
		{
			pCur = new Procedure;
			if(!pCur->m_cond.PARSE(conv.c_str()))
			{
				m_result.PRINT("proc(%s) %s",(KCSTR)m_oName,
												(KCSTR)pCur->m_cond.m_result);
				delete pCur;
				return false;
			}		
			m_list.pushback(pCur);
		}
		else if(STRNCMP(conv.c_str(),DEF_DSL_K_ELSE_eng) 
			||STRNCMP(conv.c_str(),DEF_DSL_K_ELSE_kor)  )
		{
			if(!(dslparam.KEY() == DEF_DSL_K_ELSE_eng
				|| dslparam.KEY() == DEF_DSL_K_ELSE_kor))
			{
				m_result.PRINT("proc(%s) else(%s) ELSE error!",(KCSTR)m_oName,
												(KCSTR)dslparam.KEY());
				return false;
			}
			pCur = new Procedure;
			IfParser * pElseIf = new IfParser;
			pElseIf->m_if = (KCSTR)dslparam.KEY();
			pCur->m_cond.m_listIf.pushback(pElseIf);
			m_list.pushback(pCur);
		}
		else
		{
			if(!pCur->PARSE(conv.c_str()))
			{
				m_result.PRINT("proc(%s)  %s",	(KCSTR)pCur->m_result);
				return false;
			}
		}
	}
	return true;
}
void ProcParser::JSON(RestParam & _item)
{
	_item.SET("procedure-name").VAL() = m_oName;
	_item.SET("name-space").VAL() = m_oSpace;
	RestParam & exeList = _item.SET("exe-list");
	INITARR(exeList);
	Iterator itr;
	Procedure * pP = (Procedure*)m_list.next(itr);
	while(pP)
	{
		RestParam & newItem = exeList.ARR();
		pP->JSON(newItem);
		pP = (Procedure*)m_list.next(itr);
	}
}
void ProcParser::STR(KSTRING & _buf)
{
	_buf.PRINT("%s::%s.%s\n{\n",(KCSTR)m_oType,
								(KCSTR)m_oSpace,
								(KCSTR)m_oName);
	Iterator itr;
	Procedure * pP = (Procedure*)m_list.next(itr);
	while(pP)
	{
		pP->STR(_buf);
		pP = (Procedure*)m_list.next(itr);
	}
	_buf.PRINT("}\n");
}
void ProcParser::IMPORT(RestParam & _item)
{
	m_list.clear();
	m_oType = DEF_DSL_CLASS_PROC_kor;
	m_oName = _item.GET("procedure-name").VAL();
	m_oSpace = _item.GET("name-space").VAL();
	setkey((KCSTR)m_oName);
	RestParam & exeList = _item.GET("exe-list");
	for(KUINT i=0;i<exeList.NUMS();i++)
	{
		Procedure * pNew = new Procedure;
		pNew->IMPORT(exeList[i]);
		m_list.pushback(pNew);
	}
}
KUINT ProcParser::NUMS(){return m_list.size();}
}

