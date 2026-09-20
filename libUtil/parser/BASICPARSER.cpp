#include "BASICPARSER.h"
#include "CLASSPARSER.h"
namespace nsUtil
{
void serialPath(const char * _pszSrc, std::string & _conv)
{
    _conv.clear();
    if (!_pszSrc) return;

    for (const char* p = _pszSrc; *p; ++p)
    {
        if (*p == '/')
        {
            _conv += "^^^";
        }
        else
        {
            _conv += *p;
        }
    }
}
void deserialPath(const char * _pszSrc, std::string & _conv)
{
    _conv.clear();
    if (!_pszSrc) return;

    const char* p = _pszSrc;
    while (*p)
    {
        if (p[0] == '^' && p[1] == '^' && p[2] == '^')
        {
            _conv += '/';
            p += 3;
        }
        else
        {
            _conv += *p;
            ++p;
        }
    }
}
BasicParser::BasicParser()
{
	m_eSt = E_PARSE_NONE;
	m_token="=";
	m_bTrim=false;
}
BasicParser::~BasicParser()
{
}
BasicParser & BasicParser::operator=(BasicParser & _src)
{
	CONSTRUCT((void *)_src);
	return *this;
}
void BasicParser::CONSTRUCT(void * _pvSrc)
{
	BasicParser * pSrc = (BasicParser*)_pvSrc;
	setkey(pSrc->m_pszKey);
	m_key = pSrc->m_key;
	m_val = pSrc->m_val;
	m_bTrim = pSrc->m_bTrim;
	m_token = pSrc->m_token;
}
bool BasicParser::PARSE(KCSTR _src)
{
	COMPSTR buf(_src);
	if(buf.LENGTH()==0) return false;
	for(KUINT i=0;i<buf.LENGTH();i++)
	{
		if(!parseStep((const char)_src[i])) return false;
	}
	if(!((m_eSt == E_PARSE_VAL) || (m_eSt == E_PARSE_KEY_STATIC)))
	{
		m_result.PRINT("key(%s) val(%s) illegal state %s",
						(KCSTR)KEY(),(KCSTR)VAL(),STATUS());
		return false;
	}
	return true;
}
void BasicParser::STR(KSTRING & _buf)
{
	if(VAL().LENGTH()>0)
	{
		_buf.PRINT("%s.%s",(KCSTR)KEY(),(KCSTR)VAL());
	}
	else
	{
		_buf.PRINT("%s",(KCSTR)KEY());
	}
}
void BasicParser::IMPORT(KCSTR _flat)
{
	m_key = KNULL;
	m_val = KNULL;
	if(_flat==NULL) return;
	KCSTR dot = strchr(_flat,'.');
	if(dot)
	{
		KSTRING k; k.PRINT("%.*s",(int)(dot-_flat),_flat);
		m_key = k;
		m_val = (dot+1);
	}
	else
	{
		m_key = _flat;
	}
	m_eSt = E_PARSE_MAX;
}
bool BasicParser::parseStep(const char _cInput)
{
	switch(m_eSt)
	{
		case E_PARSE_NONE: return m_fnE_PARSE_NONE(_cInput); 
		case E_PARSE_KEY: return m_fnE_PARSE_KEY(_cInput); 
		case E_PARSE_KEY_STATIC: return m_fnE_PARSE_KEY_STATIC(_cInput); 
		case E_PARSE_KEY_SP: return m_fnE_PARSE_KEY_SP(_cInput); 
		case E_PARSE_VAL_PRE: return m_fnE_PARSE_VAL_PRE(_cInput); 
		case E_PARSE_VAL: return m_fnE_PARSE_VAL(_cInput); 
		default: return m_fnE_PARSE_NONE(_cInput); 
	};
	return false;
}
bool BasicParser::MATCH(const char _cInput,KCSTR _chrList)
{
	if(_chrList==NULL) return false;
	for(KUINT i=0;i<strlen(_chrList);i++)
	{
		if(_chrList[i] == _cInput) return true;
	}
	return false;
}
BasicParser::EParse_t BasicParser::STATE()
{
	return m_eSt;
}
KCSTR BasicParser::STATUS()
{
	switch(m_eSt)
	{
		case E_PARSE_NONE  : return "PARSE_NONE";
		case E_PARSE_KEY   : return "PARSE_KEY";
		case E_PARSE_KEY_STATIC   : return "PARSE_KEY_STATIC";
		case E_PARSE_KEY_SP: return "PARSE_KEY_SP";
		case E_PARSE_VAL_PRE: return "PARSE_VAL_PRE";
		case E_PARSE_VAL   : return "PARSE_VAL";
		case E_PARSE_MAX   : return "PARSE_MAX";
		default: return "PARSE_MAX";
	};
	return "PARSE_MAX";
}
KSTRING & BasicParser::KEY()
{
	return m_key;
}
KSTRING & BasicParser::VAL()
{
	if(m_bTrim==false)
	{
		m_bTrim = true;
		KSTRING tmp; tmp = m_val;
		KSTRING::m_fnTrimTailString((KSTR)tmp," \t\r\n");
		m_val = tmp;
	}
	return m_val;
}
KSTRING & BasicParser::TOKEN()
{
	return m_token;
}
void BasicParser::CHANGE(EParse_t _eT)
{
	m_eSt = _eT;
}
bool BasicParser::VALIDKEY(KCSTR _key)
{
	if(STRNCMP(_key,DEF_DSL_K_RCV_MSG_eng) ||
		STRNCMP(_key,DEF_DSL_K_RCV_MSG_kor) ||
		STRNCMP(_key,DEF_DSL_K_SESSION_eng) ||
		STRNCMP(_key,DEF_DSL_K_RCV_MSG_kor) ||
		STRNCMP(_key,DEF_DSL_CLASS_STMT_eng) ||
		STRNCMP(_key,DEF_DSL_CLASS_STMT_kor) )
		return true;
	else
		return false;
}
bool BasicParser::m_fnE_PARSE_NONE(const char _cInput)
{
	if(MATCH(_cInput," \r\n\t"))
	{
	}
	else
	{
		m_key<<_cInput;
		CHANGE(E_PARSE_KEY);
	}
	return true;
}
bool BasicParser::m_fnE_PARSE_KEY(const char _cInput)
{
	if(MATCH(_cInput," \r\n\t"))
	{
		if(VALIDKEY((KCSTR)m_key))
		{
			CHANGE(E_PARSE_KEY_SP);
		}
		else
		{
			m_key<<_cInput;
			CHANGE(E_PARSE_KEY_STATIC);
		}
	}
	else if(MATCH(_cInput,(KCSTR)m_token))
	{
		CHANGE(E_PARSE_VAL_PRE);
	}
	else
	{
		m_key<<_cInput;
	}
	return true;
}
bool BasicParser::m_fnE_PARSE_KEY_STATIC(const char _cInput)
{
	m_key<<_cInput;
	return true;
}
bool BasicParser::m_fnE_PARSE_KEY_SP(const char _cInput)
{
	if(MATCH(_cInput," \r\n\t"))
	{
	}
	else if(MATCH(_cInput,(KCSTR)m_token))
	{
		CHANGE(E_PARSE_VAL_PRE);
	}
	else
	{
		if(VALIDKEY((KCSTR)m_key))
			
		{
			m_result.PRINT("key(%s) illegal token '%c'",(KCSTR)KEY(),_cInput);
			return false;
		}
	}
	return true;
}
bool BasicParser::m_fnE_PARSE_VAL_PRE(const char _cInput)
{
	if(MATCH(_cInput," \r\n\t"))
	{
	}
	else if(MATCH(_cInput,(KCSTR)m_token))
	{
		m_result.PRINT("key(%s) duplicate token '%c'",(KCSTR)KEY(),_cInput);
		return false;
	}
	else
	{
		m_val<<_cInput;
		CHANGE(E_PARSE_VAL);
	}
	return true;
}
bool BasicParser::m_fnE_PARSE_VAL(const char _cInput)
{
	m_val<<_cInput;
	return true;
}
}
