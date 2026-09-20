#include "INIPARSER.h"
#include "DFILE.h"

namespace nsUtil
{
IniObject::IniObject(){m_bComment=false;}
IniObject::~IniObject(){}
KSTRING & IniObject::KEY(){return (KSTRING&)m_szKey;}
KSTRING & IniObject::VAL(){return (KSTRING&)m_szVal;}
void IniObject::build(KSTRING & _build)
{
	if(m_bComment)
	{
		_build<<m_szVal;	_build<<"\n";
	}
	else
	{
		_build<<m_szKey;
		if(m_szVal.m_unRealLen > 0)
		{
			_build<<"=";	_build<<m_szVal;
			if(m_szComment.m_unRealLen>0)
			{
				_build<<"  #"; _build<<m_szComment;
			}
		}
		_build<<"\n";
	}
}
void IniObject::JSON(RestParam & _item)
{
	_item.SET("key").VAL() = KEY();
	std::string tmp;
	serialPath((KCSTR)VAL(), tmp);
	_item.SET("val").VAL() = tmp.c_str();
}
IniCategory::IniCategory(){}
IniCategory::~IniCategory(){}
KSTRING & IniCategory::KEY()
{
	return (KSTRING&)m_szKey;
}
IniObject * IniCategory::add(KSTRING _szKey,KSTRING _szVal)
{
	IniObject * pNew = new IniObject;
	pNew->setkey((KCSTR)_szKey);
	pNew->m_szKey = _szKey;
	pNew->m_szVal = _szVal;
	m_list.pushback(pNew);
	return pNew;
}
IniObject * IniCategory::find(KSTRING _szKey)
{
	return (IniObject*)m_list.find((KCSTR)_szKey);
}
void IniCategory::build(KSTRING & _build)
{
	if(m_szKey == "default_qwerty")
	{

	}
	else
	{
		_build<<"["; _build<<m_szKey; _build<<"]"; _build<<"\n";
	}
	Iterator Itr;
	IniObject * pLine = (IniObject*)m_list.next(Itr);
	while(pLine)
	{
		pLine->build(_build);
		pLine = (IniObject*)m_list.next(Itr);
	}
	_build<<"\n";
}
void IniCategory::JSON(RestMsg & _msg)
{
	RestParam & myList = _msg.SET((KCSTR)KEY());
	Iterator Itr;
	IniObject * pLine = (IniObject*)m_list.next(Itr);
	while(pLine)
	{
		RestParam & item = myList.ARR();
		pLine->JSON(item);
		pLine = (IniObject*)m_list.next(Itr);
	}
}
void IniCategory::IMPORT(RestParam & _item)
{
	m_list.clear();
	for(KUINT i=0;i<_item.NUMS();i++)
	{
		RestParam & item = _item[i];
		std::string tmp;
		deserialPath((KCSTR)item.GET("val").VAL(),tmp);
		add(item.GET("key").VAL(), tmp.c_str());
	}
}
IniParser::IniParser(){}
IniParser::~IniParser(){}
IniCategory & IniParser::operator[](KUINT _idx)
{
	IniCategory * pFind = (IniCategory *)m_list.index(_idx);
	if(pFind==NULL) return m_def;
	return *pFind;
}
void IniParser::PARSE(char * _pszRaw)
{
	TokStrings arrLine;
	char * pszRealLine = NULL;
	unsigned int unCategoryCnt = 0;
	KSTRING::m_fnTokenizeC(_pszRaw, "\r\n", arrLine);
	KSTRING szCurrentSub;
	KSTRING clsCmdKeyGen;			
	for(KUINT i=0;i<arrLine.num();i++)
	{
		pszRealLine = KSTRING::m_fnGetOptimizeString(arrLine[i]," \t");
		if(pszRealLine)
		{
			if(pszRealLine[0] == '#')
			{
				clsCmdKeyGen=KNULL;
				clsCmdKeyGen.PRINT("%s.%02u",DEF_CFG_MAGIC_CODE,(KUINT)(unCategoryCnt++));
				setline(szCurrentSub, clsCmdKeyGen, pszRealLine)->m_bComment = true;
			}
			else if(pszRealLine[0] == '[')
			{
				szCurrentSub = KSTRING::m_fnGetOptimizeString(pszRealLine," []\t");
				IniCategory * pFindCate = find(szCurrentSub);
				if(pFindCate == NULL)
				{
					pFindCate = add(szCurrentSub);
				}
			}
			else
			{
				char * pszKey;char *pszVal;
				tok(pszRealLine,&pszKey,&pszVal);
				if(pszKey && pszVal)
				{
					setline(szCurrentSub, pszKey,pszVal);
				}
			}
		}
	}
}
IniCategory * IniParser::add(KSTRING _szCate)
{
	IniCategory * pNew = new IniCategory;
	pNew->setkey((KCSTR)_szCate);
	pNew->m_szKey = _szCate;
	m_list.pushback(pNew);
	return pNew;
}
IniCategory * IniParser::find(KSTRING _szCate)
{
	return (IniCategory*)m_list.find((KCSTR)_szCate);
}
IniObject * IniParser::findLine(KSTRING _szCate, KSTRING _szKey)
{
	KSTRING szTmp = _szCate;
	if(_szCate.m_unRealLen == 0)
		szTmp = "default_qwerty";
	IniCategory* pFind = find(szTmp);
	if(pFind==NULL) return NULL;
	return pFind->find(_szKey);
}
IniObject * IniParser::setline(KSTRING _szCate, KSTRING _szKey, KSTRING _szVal)
{
	if(_szCate.m_unRealLen == 0) _szCate = "default_qwerty";
	IniCategory * pFindCate = find(_szCate);
	if(pFindCate == NULL)
	{
		pFindCate = add(_szCate);
	}
	IniObject * pFindLine = pFindCate->find(_szKey);
	if(pFindLine==NULL)
	{
		pFindLine = pFindCate->add(_szKey,_szVal);
	}
	TokStrings tok;
	KSTRING tmp(_szVal);
	KSTRING::m_fnTokenizeC((KSTR)tmp,"#",tok);
	if(tok.num()>= 2)
	{
		pFindLine->m_szVal = KNULL;
		KSTRING::m_fnTrimTailString(tok[0]," \t\r\n");
		pFindLine->m_szVal = tok[0];
		pFindLine->m_szComment = tok[1];
	}
	else
	{
		pFindLine->m_szVal = _szVal;
	}
	return pFindLine;
}
char * IniParser::STR(KSTRING & _build)
{
	Iterator Itr;
	IniCategory * pCate = (IniCategory*)m_list.next(Itr);
	while(pCate)
	{
		pCate->build(_build);
		pCate = (IniCategory*)m_list.next(Itr);
	}
	return (KSTR)_build;
}
void IniParser::JSON(RestMsg & _msg)
{
	Iterator Itr;
	IniCategory * pCate = (IniCategory*)m_list.next(Itr);
	while(pCate)
	{
		pCate->JSON(_msg);
		pCate = (IniCategory*)m_list.next(Itr);
	}
}
void IniParser::IMPORT(RestMsg & _msg)
{
	m_list.clear();
	for(KUINT i = 0;i<_msg.NUMS();i++)
	{
		RestParam & cate = _msg[i];
		IniCategory * pCate = add(cate.KEY());
		if(pCate)
		{
			pCate->IMPORT(cate);
		}
	}
}
void IniParser::tok(char * _pszOrig, char **_ppszKey, char ** _ppszVal)
{
	*_ppszKey = NULL; *_ppszVal = NULL;
	char * pszStart = KSTRING::m_fnGetOptimizeString(_pszOrig," \t");
	unsigned int nLen = KSTRING::m_fnStrLen(pszStart);
	if(pszStart == NULL) return;
	bool bVal = false;
	for(unsigned int i=0;i<nLen;i++)
	{
		if(bVal)
		{
			if(pszStart[i] == '=' || pszStart[i] == ' ')
			{
				pszStart[i] = 0x00;
			}
			else
			{
				*_ppszVal = KSTRING::m_fnSkipString(&pszStart[i]," \t");
				return;
			}
		}
		else
		{
			if(pszStart[i] == '=')
			{
				pszStart[i]=0x00;
				*_ppszKey = KSTRING::m_fnTrimTailString(pszStart," \t");
				bVal = true;
			}
		}
	}
}
KUINT IniParser::NUMS()
{
	return m_list.size();
}
}
