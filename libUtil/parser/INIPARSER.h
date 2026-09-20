#ifndef INI_PARSER_H
#define INI_PARSER_H
#include "BASICPARSER.h"
#include "RESTMSG.h"

namespace nsUtil
{
class IniObject : public StlObject
{
	public:
		IniObject();
		~IniObject();
		KSTRING & KEY();
		KSTRING & VAL();
		void build(KSTRING & _build);
		void JSON(RestParam & _item);
		bool m_bComment;
		KSTRING m_szKey;
		KSTRING m_szVal;
		KSTRING m_szComment;
};
class IniCategory : public StlObject
{
	public:
		IniCategory();
		~IniCategory();
		KSTRING & KEY();
		IniObject * add(KSTRING _szKey,KSTRING _szVal);
		IniObject * find(KSTRING _szKey);
		void build(KSTRING & _build);
		void JSON(RestMsg & _item);
		void IMPORT(RestParam & _item);
		KSTRING m_szKey;
		StlList m_list;
};
class IniParser : public StlObject
{
	public:
		IniParser();
		~IniParser();
		void PARSE(char * _pszRaw);
		IniCategory & operator[](KUINT _idx);
		IniCategory * add(KSTRING _szCate);
		IniCategory * find(KSTRING _szCate);
		IniObject * findLine(KSTRING _szCate, KSTRING _szKey);
		IniObject * setline(KSTRING _szCate, KSTRING _szKey, KSTRING _szVal);
		char * STR(KSTRING & _build);
		void JSON(RestMsg & _msg);
		void IMPORT(RestMsg & _msg);
		void tok(char * _pszOrig, char **_ppszKey, char ** _ppszVal);
		KUINT NUMS();
		StlList m_list;	
		IniCategory m_def;
};
}
#endif

