#include <string>
#include "SCEPARSER.h"

namespace nsUtil
{
void removeBlockComments(KCSTR _src, KSTRING & _result) 
{
	std::string result = _src;
	size_t startPos = 0;
	while (true) 
	{
		startPos = result.find("/*", startPos);
		if (startPos == std::string::npos)
		break;
		size_t endPos = result.find("*/", startPos + 2);
		if (endPos == std::string::npos) 
		{
			result.erase(startPos);
			break;
		}
		result.erase(startPos, endPos - startPos + 2);
	}
	_result = result.c_str();
}
SceParser::SceParser(){m_curr = NULL;m_langType="kor";}
SceParser::~SceParser(){}
KCSTR SceParser::NAME()
{
	return "NotebookFlow";
}
bool SceParser::PARSE(KSTRING & _buf)
{
	if(_buf.LENGTH()==0)
	{
		m_result = "sce(empty) scenario text empty";
		return false;
	}
	KSTRING tmp;
	removeBlockComments((KCSTR)_buf, tmp);
	m_curr = NULL;
	m_listSceParser.clear();
	for(KUINT i=0;i<tmp.LENGTH();i++)
	{
		char cInput = tmp[i];
		if(m_curr == NULL)
		{
			m_curr = new ClassParser;
			m_listSceParser.pushback(m_curr);
		}
		if(!m_curr->parsestep(cInput))
		{
			m_result.PRINT("sce(%s) %s",(KCSTR)NAME(),(KCSTR)m_curr->m_result);
			return false;
		}
		if(m_curr->m_eSt == ClassParser::E_PARSE_END)
		{
			m_curr = new ClassParser;
			m_listSceParser.pushback(m_curr);
		}
	}
	if(NAME() == NULL)
	{
		m_result.PRINT("sce(empty) not found scenario name");
		return false;
	}
	Iterator itr2;
	ClassParser * pOuter = (ClassParser*)m_listSceParser.next(itr2);
	while(pOuter)
	{
		if(pOuter->m_oName.LENGTH() == 0)
		{
			pOuter = (ClassParser*)m_listSceParser.next(itr2);
			continue;
		}
		Iterator itr3;
		ClassParser * pInner = (ClassParser*)m_listSceParser.next(itr3);
		while(pInner)
		{
			if(pInner != pOuter && pInner->m_oName == pOuter->m_oName)
			{
				m_result.PRINT("sce(%s) class(%s) obj(%s) duplicate object name",
								(KCSTR)NAME(),
								(KCSTR)pOuter->m_oType,
								(KCSTR)pOuter->m_oName);
				return false;
			}
			pInner = (ClassParser*)m_listSceParser.next(itr3);
		}
		pOuter = (ClassParser*)m_listSceParser.next(itr2);
	}
	m_result = "success";
	return true;
}
ClassParser & SceParser::operator[](KUINT _idx)
{
	ClassParser * pFind = (ClassParser*)m_listSceParser.index(_idx);
	if(pFind==NULL) return m_def;
	return *pFind;
}
KUINT SceParser::NUMS()
{
	return m_listSceParser.size();
}
void SceParser::DEBUGGING(KSTRING & _debug)
{
	Iterator itr;
	ClassParser * pFind = (ClassParser*)m_listSceParser.next(itr);
	while(pFind)
	{
		pFind->DEBUGGING(_debug);
		pFind = (ClassParser*)m_listSceParser.next(itr);
	}
}
void SceParser::m_fnTest()
{
	SceParser dls;
	const char * s_szSceParserTest = "SCE:test_sce\r\n{\r\n}\r\n";
	KSTRING buf = s_szSceParserTest;
	dls.PARSE(buf);
	KSTRING debug;
	dls.DEBUGGING(debug);
	printf("%s",(KCSTR)debug);
}
}

