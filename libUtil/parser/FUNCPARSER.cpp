#include "FUNCPARSER.h"
namespace nsUtil
{
 SceParserInfoTbl_t g_SceParserFuncInfo[EXE_TYPE_MAX]=
{
	{EXE_TYPE_SET,DEF_DSL_FUNC_SET_kor,DEF_DSL_FUNC_SET_eng,IS_DSL_FUNC_SET},
	{EXE_TYPE_SUM,DEF_DSL_FUNC_SUM_kor,DEF_DSL_FUNC_SUM_eng,IS_DSL_FUNC_SUM},
	{EXE_TYPE_MINUS,DEF_DSL_FUNC_MINUS_kor,DEF_DSL_FUNC_MINUS_eng,IS_DSL_FUNC_MINUS},
	{EXE_TYPE_DIV,DEF_DSL_FUNC_DIV_kor,DEF_DSL_FUNC_DIV_eng,IS_DSL_FUNC_DIV},
	{EXE_TYPE_MOD,DEF_DSL_FUNC_MOD_kor,DEF_DSL_FUNC_MOD_eng,IS_DSL_FUNC_MOD},
	{EXE_TYPE_CAT,DEF_DSL_FUNC_CAT_kor,DEF_DSL_FUNC_CAT_eng,IS_DSL_FUNC_CAT},
	{EXE_TYPE_STRSTR,DEF_DSL_FUNC_STRSTR_kor,DEF_DSL_FUNC_STRSTR_eng,IS_DSL_FUNC_STRSTR},
	{EXE_TYPE_STRNCMP,DEF_DSL_FUNC_STRNCMP_kor,DEF_DSL_FUNC_STRNCMP_eng,IS_DSL_FUNC_STRNCMP},
	{EXE_TYPE_INSERT,DEF_DSL_FUNC_INSERT_kor,DEF_DSL_FUNC_INSERT_eng,IS_DSL_FUNC_INSERT},
	{EXE_TYPE_DELETE,DEF_DSL_FUNC_DELETE_kor,DEF_DSL_FUNC_DELETE_eng,IS_DSL_FUNC_DELETE},
	{EXE_TYPE_EXTRACT,DEF_DSL_FUNC_EXTRACT_kor,DEF_DSL_FUNC_EXTRACT_eng,IS_DSL_FUNC_EXTRACT},
	{EXE_TYPE_TIME,DEF_DSL_FUNC_TIME_kor,DEF_DSL_FUNC_TIME_eng,IS_DSL_FUNC_TIME},
	{EXE_TYPE_CLOCK,DEF_DSL_FUNC_CLOCK_kor,DEF_DSL_FUNC_CLOCK_eng,IS_DSL_FUNC_CLOCK},
	{EXE_TYPE_PRINT,DEF_DSL_FUNC_PRINT_kor,DEF_DSL_FUNC_PRINT_eng,IS_DSL_FUNC_PRINT},
	{EXE_TYPE_DATE,DEF_DSL_FUNC_DATE_kor,DEF_DSL_FUNC_DATE_eng,IS_DSL_FUNC_DATE},
	{EXE_TYPE_CPH,DEF_DSL_FUNC_CPH_kor,DEF_DSL_FUNC_CPH_eng,IS_DSL_FUNC_CPH},
	{EXE_TYPE_LOOP,DEF_DSL_FUNC_LOOP_kor,DEF_DSL_FUNC_LOOP_eng,IS_DSL_FUNC_LOOP},
	{EXE_TYPE_OBJ,DEF_DSL_FUNC_OBJ_kor,DEF_DSL_FUNC_OBJ_eng,IS_DSL_FUNC_OBJ},
	{EXE_TYPE_ADDR,DEF_DSL_FUNC_ADDR_kor,DEF_DSL_FUNC_ADDR_eng,IS_DSL_FUNC_ADDR},
	{EXE_TYPE_SPLIT,DEF_DSL_FUNC_SPLIT_kor,DEF_DSL_FUNC_SPLIT_eng,IS_DSL_FUNC_SPLIT},
	{EXE_TYPE_WORDEX,DEF_DSL_FUNC_WORD_EX_kor,DEF_DSL_FUNC_WORD_EX_eng,IS_DSL_FUNC_WORDEX},
	{EXE_TYPE_WORDSUM,DEF_DSL_FUNC_WORD_SUM_kor,DEF_DSL_FUNC_WORD_SUM_eng,IS_DSL_FUNC_WORDSUM},
};
 bool FuncParser::validfunc(KCSTR _name)
{
	for(int i = 0; i < EXE_TYPE_MAX; i++)
	{
		if(g_SceParserFuncInfo[i].m_pfn(_name)) return true;
	}
	return false;
}
 eExeType FuncParser::getfunctype(KCSTR _name)
{
    if(_name == NULL) return EXE_TYPE_MAX;
    for(int i = 0; i < EXE_TYPE_MAX; i++)
    {
        if(g_SceParserFuncInfo[i].m_pfn(_name)) return (eExeType)i;
    }
    return EXE_TYPE_MAX;
}
FuncParser::FuncParser()
{
	m_eSt = E_PARSE_NONE; 
	m_bAfterComma = false;
}
FuncParser::~FuncParser()
{
}
FuncParser & FuncParser::operator=(FuncParser & _src)
{
	CONSTRUCT((void *)&_src);
	return *this;
}
void FuncParser::CONSTRUCT(void * _pvSrc)
{
	FuncParser * psrc = (FuncParser*)_pvSrc;
	m_className = psrc->m_className;
	m_funcName = psrc->m_funcName;
	m_argList.CLEAR();
	m_argList = psrc->m_argList;
}
KSTRING & FuncParser::operator[](KUINT _idx)
{
	return m_argList[_idx].VAL();
}
KUINT FuncParser::ARGNUMS()
{
	return m_argList.NUMS();
}
void FuncParser::CLEAR()
{
	m_result = KNULL; 
	m_curArg = KNULL; 
	m_argList.CLEAR(); 
	m_eSt = E_PARSE_NONE;
	m_bAfterComma = false;
}
bool FuncParser::PARSE(KCSTR _src)
{
	COMPSTR mLine(_src);
	for(KUINT i=0;i<mLine.LENGTH();i++)
	{
		if(!parsestep((const char)_src[i]))
		{		
			return false;
		}
	}
	if(m_eSt != E_PARSE_FUNC_END)
	{
		m_result.PRINT("func(%s) illegal func state(%s)",
											(KCSTR)m_funcName,
											(KCSTR)STATUS());
		return false;
	}
	return true;
}
void FuncParser::JSON(RestParam & _item)
{
	_item.SET("exe-type").VAL() = m_className;
	KSTRING & func = _item.SET("exe-name").VAL();
	func.PRINT("%s(",(KCSTR)m_funcName);
	for(KUINT i=0;i<m_argList.NUMS();i++)
	{
		KSTRING & aVal = m_argList[i].VAL();
		if(i == (m_argList.NUMS()-1))
		{
			func.PRINT("%s)",(KCSTR)aVal);
		}
		else
		{
			func.PRINT("%s,",(KCSTR)aVal);
		}
	}
}
void FuncParser::STR(KSTRING & _buf)
{
	_buf.PRINT("%s.%s(",(KCSTR)m_className,(KCSTR)m_funcName);
	for(KUINT i=0;i<m_argList.NUMS();i++)
	{
		KSTRING & aVal = m_argList[i].VAL();
		if(i == (m_argList.NUMS()-1))
		{
			_buf.PRINT("%s)",(KCSTR)aVal);
		}
		else
		{
			_buf.PRINT("%s,",(KCSTR)aVal);
		}
	}
	_buf<<"\n";
}
void FuncParser::IMPORT(RestParam & _item)
{
	CLEAR();
	KSTRING flat;
	flat.PRINT("%s.%s",(KCSTR)_item.GET("exe-type").VAL(),
						(KCSTR)_item.GET("exe-name").VAL());
	if(!PARSE((KCSTR)flat))
	{
		m_result.PRINT("FuncParser::IMPORT PARSE 실패(%s): %s",
						(KCSTR)flat,(KCSTR)m_result);
	}
}
bool FuncParser::parsestep(const char _cInput)
{
	switch(m_eSt)
	{
		case E_PARSE_NONE        : return m_fnE_PARSE_NONE(_cInput);        
		case E_PARSE_CLASS       : return m_fnE_PARSE_CLASS(_cInput);
		case E_PARSE_CLASS_SP    : return m_fnE_PARSE_CLASS_SP(_cInput);
		case E_PARSE_FUNC_NAME   : return m_fnE_PARSE_FUNC_NAME(_cInput);
		case E_PARSE_FUNC_NAME_SP: return m_fnE_PARSE_FUNC_NAME_SP(_cInput);
		case E_PARSE_FUNC_ARG_INIT: return m_fnE_PARSE_FUNC_ARG_INIT(_cInput);
		case E_PARSE_FUNC_ARG    : return m_fnE_PARSE_FUNC_ARG(_cInput);
		case E_PARSE_FUNC_ARG_SP : return m_fnE_PARSE_FUNC_ARG_SP(_cInput);
		case E_PARSE_FUNC_END    : return m_fnE_PARSE_FUNC_END(_cInput);
		default                  : return m_fnE_PARSE_NONE(_cInput); 
	};
	return false;
}
ALIST & FuncParser::ARG()
{
	return m_argList;
}
KSTRING & FuncParser::CLASSNAME()
{
	return m_className;
}
KSTRING & FuncParser::FUNCNAME()
{
	return m_funcName;
}
FuncParser::EParse_t FuncParser::STATE()
{
	return m_eSt;
}
KCSTR FuncParser::STATUS()
{
	switch(m_eSt)
	{
		case E_PARSE_NONE          : return "PARSE_NONE";        
		case E_PARSE_CLASS         : return "PARSE_CLASS";       
		case E_PARSE_CLASS_SP      : return "PARSE_CLASS_SP";    
		case E_PARSE_FUNC_NAME     : return "PARSE_FUNC_NAME";   
		case E_PARSE_FUNC_NAME_SP  : return "PARSE_FUNC_NAME_SP";
		case E_PARSE_FUNC_ARG_INIT  : return "PARSE_FUNC_ARG_INIT";    
		case E_PARSE_FUNC_ARG      : return "PARSE_FUNC_ARG";    
		case E_PARSE_FUNC_ARG_SP   : return "PARSE_FUNC_ARG_SP"; 
		case E_PARSE_FUNC_END      : return "PARSE_FUNC_END";    
		case E_PARSE_MAX           : return "PARSE_MAX";
		default: return "PARSE_NONE";         
	};
	return "PARSE_NONE";   
}
void FuncParser::CHANGE(EParse_t _eT)
{
	m_eSt = _eT;
}
bool FuncParser::m_fnE_PARSE_NONE(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t\r\n"))
	{
		// skipp
	}
	else
	{
		m_className<<_cInput;
		CHANGE(E_PARSE_CLASS);
	}
	return true;
}
bool FuncParser::m_fnE_PARSE_CLASS(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t\r\n"))
	{
		// skipp
	}
	else if(BasicParser::MATCH(_cInput,"."))
	{
		CHANGE(E_PARSE_CLASS_SP);
	}
	else
	{
		m_className<<_cInput;
		CHANGE(E_PARSE_CLASS);
	}
	return true;
}
bool FuncParser::m_fnE_PARSE_CLASS_SP(const char _cInput)
{
	if(BasicParser::MATCH(_cInput,". \t\r\n"))
	{
		// skipp
	}
	else
	{
		m_funcName<<_cInput;
		CHANGE(E_PARSE_FUNC_NAME);
	}
	return true;
}
bool FuncParser::m_fnE_PARSE_FUNC_NAME(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t\r\n"))
	{
		// skipp
	}
	else if(BasicParser::MATCH(_cInput,"("))
	{
		if(validfunc((KCSTR)m_funcName))
		{
			CHANGE(E_PARSE_FUNC_NAME_SP);
				return true;
		}
		m_result.PRINT("func(%s) illegal func name",(KCSTR)m_funcName);
		return false;
	}
	else
	{
		m_funcName<<_cInput;
	}
	return true;
}
bool FuncParser::m_fnE_PARSE_FUNC_NAME_SP(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t\r\n"))
	{
		// skipp
	}
	else if(BasicParser::MATCH(_cInput,"("))
	{
		// duplicate open paren
		m_result.PRINT("func(%s) duplicate '('",(KCSTR)m_funcName);
		return false;
	}
	else if(BasicParser::MATCH(_cInput,","))
	{
		m_result.PRINT("func(%s) empty first argument",(KCSTR)m_funcName);
		return false;
	}
	else
	{
		m_curArg = KNULL;
		m_curArg<<_cInput;
		CHANGE(E_PARSE_FUNC_ARG);
	}
	return true;
}
bool FuncParser::m_fnE_PARSE_FUNC_ARG_INIT(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t\r\n"))
	{
		// skipp
	}
	else if(BasicParser::MATCH(_cInput,"("))
	{
		CHANGE(E_PARSE_FUNC_NAME_SP);
	}
	else
	{
		m_result.PRINT("func(%s) illegal empty '('",(KCSTR)m_funcName);
		return false;
	}
	return true;
}
bool FuncParser::m_fnE_PARSE_FUNC_ARG(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t\r\n"))
	{
		if(m_curArg.LENGTH()> 0)
		{
			m_argList.PUSH().VAL() = m_curArg;
			m_curArg = KNULL;
			m_bAfterComma = false;
			CHANGE(E_PARSE_FUNC_ARG_SP);
		}
		else
		{
			m_result.PRINT("func(%s) argment is null",(KCSTR)m_funcName);
			return false;
		}
	}
	else if(BasicParser::MATCH(_cInput,","))
	{
		if(m_curArg.LENGTH()> 0)
		{
			m_argList.PUSH().VAL() = m_curArg;
			m_curArg = KNULL;
			m_bAfterComma = true;
			CHANGE(E_PARSE_FUNC_ARG_SP);
		}
		else
		{
			m_result.PRINT("func(%s) argment is null",(KCSTR)m_funcName);
			return false;
		}
	}
	else if(BasicParser::MATCH(_cInput,")"))
	{
		if(m_curArg.LENGTH()> 0)
		{
			m_argList.PUSH().VAL() = m_curArg;
			m_curArg = KNULL;
			CHANGE(E_PARSE_FUNC_END);
		}
		else
		{
			m_result.PRINT("func(%s) argment is null",(KCSTR)m_funcName);
			return false;
		}
	}
	else
	{
		m_curArg<<_cInput;
	}
	return true;
}
bool FuncParser::m_fnE_PARSE_FUNC_ARG_SP(const char _cInput)
{
	if(BasicParser::MATCH(_cInput," \t\r\n"))
	{
	}
	else if(BasicParser::MATCH(_cInput,","))
	{
		if(m_bAfterComma)
		{
			m_result.PRINT("func(%s) argment is null",(KCSTR)m_funcName);
			return false;
		}
		m_bAfterComma = true;
	}
	else if(BasicParser::MATCH(_cInput,")"))
	{
		if(m_bAfterComma)
		{
			m_result.PRINT("func(%s) argment is null",(KCSTR)m_funcName);
			return false;
		}
		CHANGE(E_PARSE_FUNC_END);
	}
	else
	{
		m_bAfterComma = false;
		m_curArg = KNULL;
		m_curArg<<_cInput;
		CHANGE(E_PARSE_FUNC_ARG);
	}
	return true;
}
KCSTR FuncParser::DEBUGGING(KSTRING & _debug)
{
	_debug.PRINT("class:%s, func:%s, Arg[",(KCSTR)m_className,(KCSTR)m_funcName);
	for(KUINT i=0;i<m_argList.NUMS();i++)
	{
		_debug.PRINT("%s,",(KCSTR)m_argList[i].VAL());
	}
	_debug<<"]\n";
	return (KCSTR)_debug;
}
bool FuncParser::m_fnE_PARSE_FUNC_END(const char _cInput)
{
	if(BasicParser::MATCH(_cInput,")"))
	{
		m_result.PRINT("func(%s) duplicate ')'",(KCSTR)m_funcName);
		return false;
	}
	else if(!BasicParser::MATCH(_cInput," "))
	{
		m_result.PRINT("func(%s) illegal char '%c'",(KCSTR)m_funcName,_cInput);
		return false;
	}
	return true;
}
void FuncParser::m_fnTest()
{
	const char m_test[]="DSL. . fnTest ((  a,, b, c , , )";
	FuncParser mLine;
	for(KUINT i=0; i<strlen(m_test);i++)
		mLine.parsestep(m_test[i]);
	KSTRING tmp;
	mLine.DEBUGGING(tmp);
	printf("Org Func Line : %s\n",m_test);
	printf("%s",(KCSTR)tmp);
}
}
