#include "COMPSTR.h"

namespace nsUtil
{
COMPSTR::COMPSTR(){m_pszString = NULL;}
COMPSTR::COMPSTR(KCSTR _pszSrc){m_pszString = (KSTR)_pszSrc;}
COMPSTR::~COMPSTR(){}
KUINT COMPSTR::LENGTH()
{
	if(m_pszString == NULL) return 0;
	return (KUINT)strlen(m_pszString);
}
COMPSTR & COMPSTR::operator=(KSTR _pszSrc)
{
	m_pszString = _pszSrc;
	return *this;
}
COMPSTR & COMPSTR::operator=(KCSTR _pszSrc)
{
	m_pszString = (KSTR)_pszSrc;
	return *this;
}
KBOOL COMPSTR::operator==(KBOOL _bBool)
{
	if (m_pszString == NULL) { return false; }
	if (strcasecmp(m_pszString, "true") == 0)
	{
		if (_bBool) { return true; }return false;
	}
	else if (strcasecmp(m_pszString, "false") == 0)
	{
		if (_bBool) { return false; }return true;
	}return false;
}
KBOOL COMPSTR::operator==(KCHR _cVal)
{
	if (m_pszString == NULL) { return false; }
	if (m_pszString[0] == _cVal) { return true; }return false;
}
KBOOL COMPSTR::operator==(KUCHR _ucVal)
{
	if (m_pszString == NULL) { return false; }
	if ((unsigned char)m_pszString[0] == _ucVal) { return true; }return false;
}
KBOOL COMPSTR::operator==(KSHORT _sVal)
{
	if (m_pszString == NULL) { return false; }
	short temp = (short)atoi(m_pszString); if (temp == _sVal) { return true; }return false;
}
KBOOL COMPSTR::operator==(KUSHORT _usVal)
{
	if (m_pszString == NULL) { return false; }
	unsigned short temp = (unsigned short)atoi(m_pszString);
	if (temp == _usVal) { return true; }return false;
}
KBOOL COMPSTR::operator==(KINT _nVal)
{
	if (m_pszString == NULL) { return false; }
	int temp = (int)atoi(m_pszString); if (temp == _nVal) { return true; }return false;
}
KBOOL COMPSTR::operator==(KUINT _unVal)
{
	if (m_pszString == NULL) { return false; }
	unsigned int temp = (unsigned int)atoi(m_pszString); if (temp == _unVal) { return true; }return false;
}
KBOOL COMPSTR::operator==(KLONG _lVal)
{
	if (m_pszString == NULL) { return false; }
	long temp = (long)atol(m_pszString);
	if (temp == _lVal) { return true; }return false;
}
KBOOL COMPSTR::operator==(KULONG _ulVal)
{
	if (m_pszString == NULL) { return false; }
	unsigned long temp = (unsigned long)atol(m_pszString);
	if (temp == _ulVal) { return true; }return false;
}
KBOOL COMPSTR::operator==(KSTR _szSrc)
{
	if (m_pszString == NULL || _szSrc == NULL) { return false; }
	if (strcmp(m_pszString, (char*)_szSrc) == 0) { return true; }return false;
}
KBOOL COMPSTR::operator==(KCSTR _szSrc)
{
	if (m_pszString == NULL || _szSrc == NULL) { return false; }
	if (strcmp(m_pszString, (char*)_szSrc) == 0) { return true; }return false;
}
KBOOL COMPSTR::operator==(KDOUBLE _dVal)
{
	if (m_pszString == NULL) { return false; }
	double temp = (double)atof(m_pszString);
	if (temp == _dVal) { return true; }return false;
}
KBOOL COMPSTR::operator!=(KBOOL _bBool)
{
	if (m_pszString == NULL) { return true; }
	if (strcasecmp(m_pszString, "true") == 0)
	{
		if (!_bBool) { return true; }return false;
	}
	else if (strcasecmp(m_pszString, "false") == 0)
	{
		if (!_bBool) { return false; }return true;
	}return false;
}
KBOOL COMPSTR::operator!=(KCHR _cVal)
{
	if (m_pszString == NULL) { return true; }
	if(strlen(m_pszString) != 1) return true;
	if (m_pszString[0] != _cVal) { return true; }return false;
}
KBOOL COMPSTR::operator!=(KUCHR _ucVal)
{
	if (m_pszString == NULL) { return true; }
	if(strlen(m_pszString) != 1) return true;
	if ((unsigned char)m_pszString[0] != _ucVal) { return true; }return false;
}
KBOOL COMPSTR::operator!=(KSHORT _sVal)
{
	if (m_pszString == NULL) { return true; }
	short temp = (short)atoi(m_pszString); if (temp != _sVal) { return true; }return false;
}
KBOOL COMPSTR::operator!=(KUSHORT _usVal)
{
	if (m_pszString == NULL) { return true; }
	unsigned short temp = (unsigned short)atoi(m_pszString);
	if (temp != _usVal) { return true; }return false;
}
KBOOL COMPSTR::operator!=(KINT _nVal)
{
	if (m_pszString == NULL) { return true; }
	int temp = (int)atoi(m_pszString); if (temp != _nVal) { return true; }return false;
}
KBOOL COMPSTR::operator!=(KUINT _unVal)
{
	if (m_pszString == NULL) { return true; }
	unsigned int temp = (unsigned int)atoi(m_pszString); if (temp != _unVal) { return true; }return false;
}
KBOOL COMPSTR::operator!=(KLONG _lVal)
{
	if (m_pszString == NULL) { return true; }
	long temp = (long)atol(m_pszString);
	if (temp != _lVal) { return true; }return false;
}
KBOOL COMPSTR::operator!=(KULONG _ulVal)
{
	if (m_pszString == NULL) { return true; }
	unsigned long temp = (unsigned long)atol(m_pszString);
	if (temp != _ulVal) { return true; }return false;
}
KBOOL COMPSTR::operator!=(KSTR _szSrc)
{
	if (m_pszString != NULL && _szSrc == NULL) { return true; }
	if (m_pszString == NULL && _szSrc != NULL) { return true; }
	if (m_pszString == NULL && _szSrc == NULL) { return false; }
	if (strcmp(m_pszString, (char*)_szSrc) != 0) { return true; }return false;
}
KBOOL COMPSTR::operator!=(KCSTR _szSrc)
{
	if (m_pszString != NULL && _szSrc == NULL) { return true; }
	if (m_pszString == NULL && _szSrc != NULL) { return true; }
	if (m_pszString == NULL && _szSrc == NULL) { return false; }
	if (strcmp(m_pszString, (char*)_szSrc) != 0) { return true; }return false;
}
KBOOL COMPSTR::operator!=(KDOUBLE _dVal)
{
	if (m_pszString == NULL) { return true; }
	double temp = (double)atof(m_pszString);
	if (temp != _dVal) { return true; }return false;
}
COMPSTR::operator KBOOL()
{
	if (m_pszString == NULL) { return false; }
	if (strcasecmp(m_pszString, "true") == 0)
	{
		return true;
	}
	else if (strcasecmp(m_pszString, "false") == 0)
	{
		return false;
	}return false;
}
COMPSTR::operator KCHR() { if (m_pszString == NULL) { return 0x00; }return (char)m_pszString[0]; }
COMPSTR::operator KUCHR() { if (m_pszString == NULL) { return 0x00; }return (unsigned char)m_pszString[0]; }
COMPSTR::operator KSTR() { return m_pszString; }
COMPSTR::operator KCSTR() { return (const char *)m_pszString; }
COMPSTR::operator KSHORT() { return (short)KSTRING::m_fnAtoi(m_pszString); }
COMPSTR::operator KUSHORT() { return (unsigned short)KSTRING::m_fnAtoi(m_pszString); }
COMPSTR::operator KINT() { return KSTRING::m_fnAtoi(m_pszString); }
COMPSTR::operator KUINT() { return (unsigned int)KSTRING::m_fnAtoi(m_pszString); }
COMPSTR::operator KLONG() { return KSTRING::m_fnAtol(m_pszString); }
COMPSTR::operator KULONG() { return (unsigned long)KSTRING::m_fnAtol(m_pszString); }
COMPSTR::operator KDOUBLE() { return (double)KSTRING::m_fnAtof(m_pszString); }
}