#include "CLASSPARSER.h"
#include "BASICPARSER.h"

namespace nsUtil
{
SceParserClassInfoTbl_t g_SceParserClassInfo[E_DSL_CLASS_MAX]=
{
	{E_DSL_CLASS_SCE,DEF_DSL_CLASS_SCE_kor,DEF_DSL_CLASS_SCE_eng,IS_DSL_CLASS_SCE},
	{E_DSL_CLASS_STATE,DEF_DSL_CLASS_STATE_kor,DEF_DSL_CLASS_STATE_eng,IS_DSL_CLASS_STATE},
	{E_DSL_CLASS_PROC,DEF_DSL_CLASS_PROC_kor,DEF_DSL_CLASS_PROC_eng,IS_DSL_CLASS_PROC},
	{E_DSL_CLASS_ACT_SEND,DEF_DSL_CLASS_ACT_SEND_kor,DEF_DSL_CLASS_ACT_SEND_eng,IS_DSL_CLASS_ACT_SEND},
	{E_DSL_CLASS_ACT_TIMER,DEF_DSL_CLASS_ACT_TIMER_kor,DEF_DSL_CLASS_ACT_TIMER_eng,IS_DSL_CLASS_ACT_TIMER},
	{E_DSL_CLASS_STMT,DEF_DSL_CLASS_STMT_kor,DEF_DSL_CLASS_STMT_eng,IS_DSL_CLASS_STMT},
	{E_DSL_CLASS_INI,DEF_DSL_CLASS_INI_kor,DEF_DSL_CLASS_INI_eng,IS_DSL_CLASS_INI},
};
bool validclass(KCSTR _name)
{
	for(KUINT i=0;i< E_DSL_CLASS_MAX;i++)
	{
		if(g_SceParserClassInfo[i].m_pfn(_name)) return true;
	}
	return false;
}
ClassParser::PFuncParseState ClassParser::m_pfnParseHandle[ClassParser::E_PARSE_MAX]=
{
	ClassParser::m_fnE_PARSE_NONE,
	ClassParser::m_fnE_PARSE_OTYPE,
	ClassParser::m_fnE_PARSE_OTYPE_SP,
	ClassParser::m_fnE_PARSE_OTYPE_SP2,
	ClassParser::m_fnE_PARSE_OSPACE,
	ClassParser::m_fnE_PARSE_OSPACE_SP,
	ClassParser::m_fnE_PARSE_ONAME,
	ClassParser::m_fnE_PARSE_ONAME_SP,
	ClassParser::m_fnE_PARSE_BODY,
	ClassParser::m_fnE_PARSE_END
};
ClassParser::ClassParser(){m_eSt = E_PARSE_NONE;}
ClassParser::~ClassParser(){}
ClassParser & ClassParser::operator=(ClassParser & _src)
{
	CONSTRUCT((void *)&_src);
	return *this;
}
void ClassParser::CONSTRUCT(void * _pvSrc)
{
	ClassParser * psrc = (ClassParser*)_pvSrc;
	m_oType = psrc->m_oType;
	m_oName = psrc->m_oName;
	m_oBody = psrc->m_oBody;
}
bool ClassParser::PARSE(KCSTR _src)
{
	if(_src==NULL) return false;
	KUINT len = strlen(_src);
	for(KUINT i=0;i<len;i++)
	{
		if(!parsestep((const char)_src[i])) return false;
	}
	m_eSt = E_PARSE_MAX;
	return true;
}
bool ClassParser::parsestep(const char _cInput)
{
	return m_pfnParseHandle[m_eSt](this,_cInput);
}
void ClassParser::DEBUGGING(KSTRING & _debug)
{
	_debug.PRINT("TNS: [%s][%s][%s]\n",(KCSTR)m_oType,
									(KCSTR)m_oName,
									(KCSTR)m_oSpace);
	_debug.PRINT("oBody: [%s]\n",(KCSTR)m_oBody);
}
KCSTR ClassParser::STATUS()
{
	switch(m_eSt)
	{
		case E_PARSE_NONE    : return "PARSE_NONE";    
		case E_PARSE_OTYPE   : return "PARSE_OTYPE";
		case E_PARSE_OTYPE_SP: return "PARSE_OTYPE_SP";
		case E_PARSE_OTYPE_SP2: return "PARSE_OTYPE_SP2";
		case E_PARSE_OSPACE   : return "PARSE_OSPACE";
		case E_PARSE_OSPACE_SP: return "PARSE_OSPACE_SP";
		case E_PARSE_ONAME   : return "PARSE_ONAME";
		case E_PARSE_ONAME_SP: return "PARSE_ONAME_SP";
		case E_PARSE_BODY    : return "PARSE_BODY";
		case E_PARSE_END     : return "PARSE_END";
		case E_PARSE_MAX     : return "PARSE_MAX";
		default: return "PARSE_NONE";
	};
	return "PARSE_NONE";   
}
 bool ClassParser::VALIDKEY(KCSTR _key)
{
	return true;
}
bool ClassParser::m_fnE_PARSE_NONE(ClassParser *_pclsObj, const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \r\n\t"))
	{
	}
	else if(_cInput == ':')
	{
		_pclsObj->m_result = "class(-) object(-) illegal class name(empty)";
		return false;
	}
	else 
	{
		_pclsObj->m_eSt = E_PARSE_OTYPE;
		_pclsObj->m_oType<<_cInput;
	}
	return true;
}
bool ClassParser::m_fnE_PARSE_OTYPE(ClassParser *_pclsObj, const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t"))
	{
	}
	else if(BasicParser::MATCH(_cInput,":"))
	{
		if(_pclsObj->VALIDKEY((KCSTR)_pclsObj->m_oType))
		{
			_pclsObj->m_eSt = E_PARSE_OTYPE_SP;
			return true;
		}
		_pclsObj->m_result.PRINT("class(%s) obejct(-) undefined class",
										(KCSTR)_pclsObj->m_oType);
		return false;
	}
	else 
	{
		_pclsObj->m_oType<<_cInput;
	}
	return true;
}
bool ClassParser::m_fnE_PARSE_OTYPE_SP(ClassParser *_pclsObj, const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t"))
	{
	}
	else if(BasicParser::MATCH(_cInput,":"))
	{
		_pclsObj->m_eSt = E_PARSE_OTYPE_SP2;
	}
	else
	{
		_pclsObj->m_result.PRINT("class(%s) obejct(-) illegal token '%c'!=':'",
										(KCSTR)_pclsObj->m_oType,
										_cInput);
		return false;
	}
	return true;
}
bool ClassParser::m_fnE_PARSE_OTYPE_SP2(ClassParser *_pclsObj, const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t"))
	{
	}
	else if(BasicParser::MATCH(_cInput,":"))
	{
		_pclsObj->m_result.PRINT("class(%s) obejct(-) illegal token '%c'!=':'",
										(KCSTR)_pclsObj->m_oType,
										_cInput);
		return false;
	}
	else
	{
		_pclsObj->m_oSpace<<_cInput;
		_pclsObj->m_eSt = E_PARSE_OSPACE;
	}
	return true;
}
bool ClassParser::m_fnE_PARSE_OSPACE(ClassParser *_pclsObj, const char _cInput)
{
	if(BasicParser::MATCH(_cInput," .\t"))
	{
		_pclsObj->m_eSt = E_PARSE_OSPACE_SP;
	}
	else
	{
		_pclsObj->m_oSpace<<_cInput;
	}
	return true;
}
bool ClassParser::m_fnE_PARSE_OSPACE_SP(ClassParser *_pclsObj, const char _cInput)
{
	if(BasicParser::MATCH(_cInput," .\t"))
	{
	}
	else
	{
		_pclsObj->m_eSt = E_PARSE_ONAME;
		_pclsObj->m_oName<<_cInput;
	}
	return true;
}
bool ClassParser::m_fnE_PARSE_ONAME(ClassParser *_pclsObj, const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \r\n\t"))
	{
		_pclsObj->m_eSt = E_PARSE_ONAME_SP;
		_pclsObj->setkey((KCSTR)_pclsObj->m_oName);
	}
	else if(_cInput == '{')
	{
		_pclsObj->setkey((KCSTR)_pclsObj->m_oName);
		_pclsObj->m_eSt = E_PARSE_BODY;
	}
	else if(_cInput == '}')
	{
		_pclsObj->m_result.PRINT("class(%s) obejct(%s) illegal token '{' != '}'",
										(KCSTR)_pclsObj->m_oType,
										(KCSTR)_pclsObj->m_oName);
		return false;
	}
	else
	{
		_pclsObj->m_oName<<_cInput;
	}
	return true;
}
bool ClassParser::m_fnE_PARSE_ONAME_SP(ClassParser *_pclsObj, const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \r\n\t"))
	{
	}
	else if(_cInput == '{')
	{
		_pclsObj->m_eSt = E_PARSE_BODY;
	}
	else if(_cInput == '}')
	{
		_pclsObj->m_eSt = E_PARSE_END;
		_pclsObj->m_result.PRINT("class(%s) obejct(%s) empty body",
										(KCSTR)_pclsObj->m_oType,
										(KCSTR)_pclsObj->m_oName);
		return false;
	}
	else
	{
		_pclsObj->m_eSt = E_PARSE_BODY;
		_pclsObj->m_oBody<<_cInput;
	}
	return true;
}
bool ClassParser::m_fnE_PARSE_BODY(ClassParser *_pclsObj, const char _cInput)
{
	if(_cInput == '}') 
	{
		_pclsObj->m_eSt = E_PARSE_END;
	}
	else if(_cInput == '{')
	{
		_pclsObj->m_result.PRINT("class(%s) obejct(%s) illegal body data, included token '{'",
										(KCSTR)_pclsObj->m_oType,
										(KCSTR)_pclsObj->m_oName);
		return false;
	}
	else
	{
		_pclsObj->m_oBody<<_cInput;
	}
	return true;
}
bool ClassParser::m_fnE_PARSE_END(ClassParser *_pclsObj, const char _cInput)
{
	return true;
}
}
