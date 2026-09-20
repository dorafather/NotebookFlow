#include "STMTPARSER.h"

namespace nsUtil
{
StmtParser::StmtParser()
{
}
StmtParser::~StmtParser()
{
}
StmtParser & StmtParser::operator=(StmtParser & _src)
{
	CONSTRUCT((void *)&_src);
	return *this;
}
void StmtParser::CONSTRUCT(void * _pvSrc)
{
	StmtParser * pSrc = (StmtParser*)_pvSrc;
	m_oType = pSrc->m_oType;
	m_oName = pSrc->m_oName;
	m_oSpace = pSrc->m_oSpace;
	setkey((KCSTR)pSrc->m_oName);
	m_conv = pSrc->m_conv;	
}
bool StmtParser::PARSE(ClassParser & _class)
{
	m_oType = _class.m_oType;
	m_oName = _class.m_oName;
	m_oSpace = _class.m_oSpace;
	setkey((KCSTR)m_oName);
	m_conv.PARSE((KCSTR)_class.m_oBody);
	return true;
}
void StmtParser::JSON(RestParam & _item)
{
	_item.SET("stmt-name").VAL() = m_oName;
	_item.SET("name-space").VAL() = m_oSpace;
	ALIST nulList;
	KSTRING buf;
	m_conv.STR(nulList, buf);
	_item.SET("stmt-data").VAL() = buf;
}
void StmtParser::STR(KSTRING & _buf)
{
	_buf.PRINT("%s::%s.%s\n{",(KCSTR)m_oType,
								(KCSTR)m_oSpace,
								(KCSTR)m_oName);
	ALIST nulList;
	KSTRING buf;
	m_conv.STR(nulList, buf);
	_buf<<buf;
	_buf.PRINT("}\n");
}
void StmtParser::IMPORT(RestParam & _item)
{
	m_oType = DEF_DSL_CLASS_STMT_kor;
	m_oName = _item.GET("stmt-name").VAL();
	m_oSpace = _item.GET("name-space").VAL();
	setkey((KCSTR)m_oName);
	m_conv.PARSE((KCSTR)_item.GET("stmt-data").VAL());
}
}

