
#include <stdio.h>
#include <string.h>
#include <bitset>
#include "KSTRING.h"

namespace nsUtil
{
static char s_digit0 = '0';
static char s_digit9 = '9';
char * strncat(char * _pszSrc, unsigned int _unMaxSrcLen, const char * _pszFormat, ...)
{
	if( !_pszSrc || _unMaxSrcLen == 0 )
		return _pszSrc;
	unsigned int unCurrentLen = strlen(_pszSrc);
	if( unCurrentLen >= _unMaxSrcLen -1 )
	{
		_pszSrc[_unMaxSrcLen-1] = 0x00;
	}
	else
	{
		char * pszTemp = &(_pszSrc[unCurrentLen]);
		va_list ap;
		va_start(ap, _pszFormat);
		vsnprintf(pszTemp, _unMaxSrcLen-unCurrentLen-1,_pszFormat, ap);
		va_end(ap);
	}
	return _pszSrc;
}
bool isDisitStr(const char * _pszSrc, unsigned int _unSrcLen)
{
	if( _unSrcLen == 0 || !_pszSrc )
		return false;

	unsigned int unStartIdx = 0;
	if( _pszSrc[0] == '+' || _pszSrc[0] == '-' )
		unStartIdx = 1;
	
	for( unsigned int i = unStartIdx; i < _unSrcLen; ++i )
	{
		if( _pszSrc[i] < s_digit0 || _pszSrc[i] > s_digit9 )
			return false;
	}

	return true;
}
void createRandomStr(char * _pszOutStr, unsigned int _unMaxLen)
{
	static const char arrChar[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	static const int nCharLen = sizeof(arrChar) - 1;
	static bool bInit = false;
	if( !bInit )
	{
		bInit = true;
		srand(time(NULL));
	}
	for( unsigned int i = 0; i < _unMaxLen; ++i )
	{
		_pszOutStr[i] = arrChar[rand() % nCharLen];
	}
}
void replaceAll(std::string & _rclsStr, const char * _pszOrigStr, const char * _pszChangeStr)
{
	if( !_pszOrigStr || strlen(_pszOrigStr) == 0 || !_pszChangeStr || strlen(_pszChangeStr) == 0 )
		return;

    size_t start_pos = 0;
    while( (start_pos = _rclsStr.find(_pszOrigStr, start_pos)) != std::string::npos )
    {
        _rclsStr.replace(start_pos, strlen(_pszOrigStr), _pszChangeStr);
        start_pos += strlen(_pszChangeStr);
    }
}
static int s_fnStrStr2(char *_pszSrc,char *_pszKey)
{
	if(_pszSrc==NULL || _pszKey==NULL) return -1;
	if(_pszSrc[0]==0 || _pszKey[0]==0) return -1;
	char *cp = (char *) _pszSrc;char *s1, *s2;
	int idx = 0;
	while (*cp)
	{
		s1 = cp;s2 = (char *) _pszKey;
		while (*s1 && *s2 && !(*s1 - *s2)) s1++, s2++;
		if (!*s2) return idx;
		cp++;idx++;
	}
	return -1;
}
/*********************************** String Class ****************************************/
KSTRING::KSTRING()
{
	m_pszString = NULL;m_unLen=0;m_unRealLen=0;
	m_fnCreateString(E_SZ_STR_CAT); 
}
KSTRING::KSTRING(bool _bBool)
{
	m_pszString = NULL;m_unLen=0;m_unRealLen=0;
	if(_bBool) m_fnCreateString("true");
	else m_fnCreateString("false");
}
KSTRING::KSTRING(char _cVal)
{
	m_pszString = NULL;m_unLen=0;m_unRealLen=0;m_fnCat(_cVal);
}
KSTRING::KSTRING(unsigned char _ucVal)
{
	m_pszString = NULL;m_unLen=0;m_unRealLen=0;m_fnCat(_ucVal);
}
KSTRING::KSTRING(short _sVal)
{
	m_pszString = NULL;m_unLen=0;m_unRealLen=0;
	char * pszTemp = new char [E_SZ_NUM];memset(pszTemp,0x00,E_SZ_NUM);
	snprintf(pszTemp,E_SZ_NUM-1,"%d",_sVal);
	m_fnCat(pszTemp);delete [] pszTemp;
}
KSTRING::KSTRING(unsigned short _usVal)
{
	m_pszString = NULL;m_unLen=0;m_unRealLen=0;
	char * pszTemp = new char [E_SZ_NUM];memset(pszTemp,0x00,E_SZ_NUM);
	snprintf(pszTemp,E_SZ_NUM-1,"%u",_usVal);
	m_fnCat(pszTemp);delete [] pszTemp;
}
KSTRING::KSTRING(int _nVal)
{
	m_pszString = NULL;m_unLen=0;m_unRealLen=0;
	char * pszTemp = new char [E_SZ_NUM];memset(pszTemp,0x00,E_SZ_NUM);
	snprintf(pszTemp,E_SZ_NUM-1,"%d",_nVal);
	m_fnCat(pszTemp);delete [] pszTemp;
}
KSTRING::KSTRING(unsigned int _unVal)
{
	m_pszString = NULL;m_unLen=0;m_unRealLen=0;
	char * pszTemp = new char [E_SZ_NUM];memset(pszTemp,0x00,E_SZ_NUM);
	snprintf(pszTemp,E_SZ_NUM-1,"%u",_unVal);
	m_fnCat(pszTemp);delete [] pszTemp;
}
KSTRING::KSTRING(long _lVal)
{
	m_pszString = NULL;m_unLen=0;m_unRealLen=0;
	char * pszTemp = new char [E_SZ_NUM];memset(pszTemp,0x00,E_SZ_NUM);
	snprintf(pszTemp,E_SZ_NUM-1,"%ld",_lVal);
	m_fnCat(pszTemp);delete [] pszTemp;
}
KSTRING::KSTRING(unsigned long _ulVal)
{
	m_pszString = NULL;m_unLen=0;m_unRealLen=0;
	char * pszTemp = new char [E_SZ_NUM];memset(pszTemp,0x00,E_SZ_NUM);
	snprintf(pszTemp,E_SZ_NUM-1,"%lu",_ulVal);
	m_fnCat(pszTemp);delete [] pszTemp;
}
KSTRING::KSTRING(char * _pszVal)
{
	m_pszString = NULL;m_unLen=0;m_unRealLen=0;m_fnCat((const char *)_pszVal);
}
KSTRING::KSTRING(const char * _pszVal)
{
	m_pszString = NULL;m_unLen=0;m_unRealLen=0;m_fnCat(_pszVal);
}
KSTRING::KSTRING(const KSTRING & _rclsVal)
{
	KSTRING & _rclsTmp = (KSTRING &)_rclsVal;
	m_pszString = NULL;m_unLen=0;m_unRealLen=0;m_fnCat((const char *)_rclsTmp);
}
KSTRING::KSTRING(double _dVal)
{
	m_pszString = NULL; m_unLen = 0; m_unRealLen = 0;
	char * pszTemp = new char[E_SZ_NUM]; memset(pszTemp, 0x00, E_SZ_NUM);
	snprintf(pszTemp, E_SZ_NUM - 1, "%lf", _dVal);
	m_fnCat(pszTemp); delete[] pszTemp;
}
KSTRING::~KSTRING(){clear();}
void KSTRING::m_fnReSize(unsigned int _unSize)
{
	m_fnCreateString(_unSize);
}
//==========> = Operator (Store)
KSTRING & KSTRING::operator=(bool _bBool)
{
	clear();if(_bBool){ m_fnCat("true");}else {m_fnCat("false");}return *this;
}
KSTRING & KSTRING::operator=(char _cVal)
{
	clear();m_fnCat(_cVal);return *this;
}
KSTRING & KSTRING::operator=(unsigned char _ucVal)
{
	clear();m_fnCat((unsigned char)_ucVal);return *this;
}
KSTRING & KSTRING::operator=(short _sVal)
{
	clear(); char * pszTmp = new char [E_SZ_NUM]; memset(pszTmp,0x00,E_SZ_NUM);
	snprintf(pszTmp,E_SZ_NUM-1,"%d",_sVal);
	m_fnCat(pszTmp); delete [] pszTmp;return *this;
}
KSTRING & KSTRING::operator=(unsigned short _usVal)
{
	clear(); char * pszTmp = new char [E_SZ_NUM]; memset(pszTmp,0x00,E_SZ_NUM);
	snprintf(pszTmp,E_SZ_NUM-1,"%u",_usVal);m_fnCat(pszTmp); delete [] pszTmp;return *this;
}
KSTRING & KSTRING::operator=(int _nVal)
{
	clear(); char * pszTmp = new char [E_SZ_NUM]; memset(pszTmp,0x00,E_SZ_NUM);
	snprintf(pszTmp,E_SZ_NUM-1,"%d",_nVal);	
	m_fnCat(pszTmp); delete [] pszTmp;return *this;
}
KSTRING & KSTRING::operator=(unsigned int _unVal)
{
	clear(); char * pszTmp = new char[E_SZ_NUM]; memset(pszTmp,0x00,E_SZ_NUM);
	snprintf(pszTmp,E_SZ_NUM-1,"%u",_unVal);
	m_fnCat(pszTmp); delete [] pszTmp;return *this;
}
KSTRING & KSTRING::operator=(long _lVal)
{
	clear(); char * pszTmp = new char [E_SZ_NUM]; memset(pszTmp,0x00,E_SZ_NUM);
	snprintf(pszTmp,E_SZ_NUM-1,"%ld",_lVal);	
	m_fnCat(pszTmp); delete [] pszTmp;return *this;
}
KSTRING & KSTRING::operator=(unsigned long _unVal)
{
	clear(); char * pszTmp = new char [E_SZ_NUM]; memset(pszTmp,0x00,E_SZ_NUM);
	snprintf(pszTmp,E_SZ_NUM-1,"%lu",_unVal);
	m_fnCat(pszTmp); delete [] pszTmp;return *this;
}
KSTRING & KSTRING::operator=(long long _llVal)
{
	clear(); char * pszTmp = new char [E_SZ_NUM]; memset(pszTmp,0x00,E_SZ_NUM);
	snprintf(pszTmp,E_SZ_NUM-1,"%lld",_llVal);
	m_fnCat(pszTmp); delete [] pszTmp;return *this;
}
KSTRING & KSTRING::operator=(unsigned long long _ullVal)
{
	clear(); char * pszTmp = new char [E_SZ_NUM]; memset(pszTmp,0x00,E_SZ_NUM);
	snprintf(pszTmp,E_SZ_NUM-1,"%llu",_ullVal);
	m_fnCat(pszTmp); delete [] pszTmp;return *this;
}
KSTRING & KSTRING::operator=(char * _pszVal)
{
	clear(); m_fnCat((const char*)_pszVal);return *this;
}
KSTRING & KSTRING::operator=(const char * _pszVal)
{
	clear(); m_fnCat((const char*)_pszVal);return *this;
}
KSTRING & KSTRING::operator=(KSTRING & _rclsSrc)
{
	clear(); 
	m_fnByteCat(_rclsSrc.m_pszString,_rclsSrc.m_unRealLen);
	return *this;
}
KSTRING & KSTRING::operator=(double _dVal)
{
	clear(); char * pszTmp = new char[E_SZ_NUM]; memset(pszTmp, 0x00, E_SZ_NUM);
	snprintf(pszTmp, E_SZ_NUM - 1, "%lf", _dVal);
	m_fnCat(pszTmp); delete[] pszTmp; return *this;
}
//==========> == Operator (Equal)
bool KSTRING::operator==(bool _bBool)
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
bool KSTRING::operator==(char _cVal)
{
	if(m_pszString == NULL){ return false;}
	if(m_unRealLen != 1) {return false;}
	if(m_pszString[0] == _cVal){ return true;}return false;	
}
bool KSTRING::operator==(unsigned char _ucVal)
{
	if(m_pszString == NULL) {return false;}
	if(m_unRealLen != 1) {return false;}
	if((unsigned char)m_pszString[0] == _ucVal){ return true;}return false;
}
bool KSTRING::operator==(short _sVal)
{
	if(m_pszString == NULL){ return false;}
	short temp = (short)atoi(m_pszString);if(temp == _sVal) {return true;}return false;
}
bool KSTRING::operator==(unsigned short _usVal)
{
	if(m_pszString == NULL) {return false;}
	unsigned short temp = (unsigned short)atoi(m_pszString);
	if(temp == _usVal) {return true;}return false;
}
bool KSTRING::operator==(int _nVal)
{
	if(m_pszString == NULL) {return false;}
	int temp = (int)atoi(m_pszString);if(temp == _nVal) {return true;}return false;
}
bool KSTRING::operator==(unsigned int _unVal)
{
	if(m_pszString == NULL) {return false;}
	unsigned int temp = (unsigned int)atoi(m_pszString);if(temp == _unVal) {return true;}return false;
}
bool KSTRING::operator==(long _lVal)
{
	if(m_pszString == NULL) {return false;}
	long temp = (long)atol(m_pszString);
	if(temp == _lVal) {return true;}return false;
}
bool KSTRING::operator==(unsigned long _ulVal)
{
	if(m_pszString == NULL) {return false;}
	unsigned long temp = (unsigned long)atol(m_pszString);
	if(temp == _ulVal) {return true;}return false;
}
bool KSTRING::operator==(char * _szSrc)
{
	if(m_pszString == NULL || _szSrc == NULL){return false;}
	if(strcmp(m_pszString,(char*)_szSrc)==0){return true;}return false;
}
bool KSTRING::operator==(const char * _szSrc)
{
	if(m_pszString == NULL || _szSrc == NULL) {return false;}
	if(strcmp(m_pszString,(char*)_szSrc)==0) {return true;}return false;
}
bool KSTRING::operator==(KSTRING & _rclsCmp)
{
	if(m_pszString==NULL || _rclsCmp.m_pszString==NULL){ return false;}
	if(m_unRealLen != _rclsCmp.m_unRealLen) {return false;}
	if(strcmp(m_pszString,_rclsCmp.m_pszString)==0) {return true;}return false;
}
bool KSTRING::operator==(double _dVal)
{
	double dMy = m_fnAtof(m_pszString);
	if(dMy == _dVal) return true;
	return false;
}
//==========> == Operator (Mismatch)
bool KSTRING::operator!=(bool _bBool)
{
	if (m_pszString == NULL) { return true; }
	if (strcasecmp(m_pszString, "true") == 0)
	{
		if (!_bBool) { return true; }return false;
	}
	else if (strcasecmp(m_pszString, "false") == 0)
	{
		if (!_bBool) { return false; }return true;
	}
	return true;
}
bool KSTRING::operator!=(char _cVal)
{
	if(m_pszString == NULL){ return true;}
	if(m_unRealLen != 1) {return true;}
	if(m_pszString[0] != _cVal){ return true;}return false;	
}
bool KSTRING::operator!=(unsigned char _ucVal)
{
	if(m_pszString == NULL) {return true;}
	if(m_unRealLen != 1) {return true;}
	if((unsigned char)m_pszString[0] != _ucVal){ return true;}return false;
}
bool KSTRING::operator!=(short _sVal)
{
	if(m_pszString == NULL){ return true;}
	short temp = (short)atoi(m_pszString);if(temp != _sVal) {return true;}return false;
}
bool KSTRING::operator!=(unsigned short _usVal)
{
	if(m_pszString == NULL) {return true;}
	unsigned short temp = (unsigned short)atoi(m_pszString);
	if(temp != _usVal) {return true;}return false;
}
bool KSTRING::operator!=(int _nVal)
{
	if(m_pszString == NULL) {return true;}
	int temp = (int)atoi(m_pszString);if(temp != _nVal) {return true;}return false;
}
bool KSTRING::operator!=(unsigned int _unVal)
{
	if(m_pszString == NULL) {return true;}
	unsigned int temp = (unsigned int)atoi(m_pszString);
	if(temp != _unVal) {return true;}return false;
}
bool KSTRING::operator!=(long _lVal)
{
	if(m_pszString == NULL) {return true;}
	long temp = (long)atol(m_pszString);
	if(temp != _lVal) {return true;}return false;
}
bool KSTRING::operator!=(unsigned long _ulVal)
{
	if(m_pszString == NULL) {return true;}
	unsigned long temp = (unsigned long)atol(m_pszString);
	if(temp != _ulVal) {return true;}return false;
}
bool KSTRING::operator!=(char * _szSrc)
{
	if(m_pszString == NULL && _szSrc != NULL){return true;}
	if(m_pszString != NULL && _szSrc == NULL){return true;}
	if(m_pszString == NULL && _szSrc == NULL){return false;}
	if(strcmp(m_pszString,(char*)_szSrc)!=0){return true;}return false;
}
bool KSTRING::operator!=(const char * _szSrc)
{
	if(m_pszString == NULL && _szSrc != NULL){return true;}
	if(m_pszString != NULL && _szSrc == NULL){return true;}
	if(m_pszString == NULL && _szSrc == NULL){return false;}
	if(strcmp(m_pszString,(char*)_szSrc)!=0) {return true;}return false;
}
bool KSTRING::operator!=(KSTRING & _rclsCmp)
{
	if(m_pszString == NULL && _rclsCmp.m_pszString!= NULL){return true;}
	if(m_pszString != NULL &&  _rclsCmp.m_pszString == NULL){return true;}
	if(m_pszString == NULL &&  _rclsCmp.m_pszString == NULL){return false;}
	if(m_unRealLen != _rclsCmp.m_unRealLen) {return true;}
	if(strcmp(m_pszString,_rclsCmp.m_pszString)!=0) {return true;}return false;
}
bool KSTRING::operator!=(double _dVal)
{
	if(m_pszString == NULL) return true;
	double dMy = m_fnAtof(m_pszString);
	if(dMy != _dVal) return true;
	return false;
}
//==========> << Operator (Add/Cat)
KSTRING & KSTRING::operator<<(bool _bVal){if(_bVal) {m_fnCat("true");}else {m_fnCat("false");}return *this;}
KSTRING & KSTRING::operator<<(char _cVal){m_fnCat((const char)_cVal);return *this;}
KSTRING & KSTRING::operator<<(unsigned char _ucVal){m_fnCat((const char)_ucVal);return *this;}
KSTRING & KSTRING::operator<<(short _sVal)
{
	char *pszTmp = new char [E_SZ_NUM]; memset(pszTmp,0x00,E_SZ_NUM);
	snprintf(pszTmp,E_SZ_NUM-1,"%d",_sVal);
	m_fnCat(pszTmp); delete [] pszTmp;return *this;
}
KSTRING & KSTRING::operator<<(unsigned short _usVal)
{
	char *pszTmp = new char[E_SZ_NUM]; memset(pszTmp,0x00,E_SZ_NUM);
	snprintf(pszTmp,E_SZ_NUM-1,"%u",_usVal);
	m_fnCat(pszTmp); delete [] pszTmp;return *this;
}
KSTRING & KSTRING::operator<<(int _nVal)
{
	char *pszTmp = new char [E_SZ_NUM]; memset(pszTmp,0x00,E_SZ_NUM);
	snprintf(pszTmp,E_SZ_NUM-1,"%d",_nVal);
	m_fnCat(pszTmp); delete [] pszTmp;return *this;
}
KSTRING & KSTRING::operator<<(unsigned int _unVal)
{
	char *pszTmp = new char [E_SZ_NUM]; memset(pszTmp,0x00,E_SZ_NUM);
	snprintf(pszTmp,E_SZ_NUM-1,"%u",_unVal);
	m_fnCat(pszTmp); delete [] pszTmp;return *this;
}
KSTRING & KSTRING::operator<<(long _lVal)
{
	char *pszTmp = new char [E_SZ_NUM]; memset(pszTmp,0x00,E_SZ_NUM);
	snprintf(pszTmp,E_SZ_NUM-1,"%ld",_lVal);
	m_fnCat(pszTmp); delete [] pszTmp;return *this;
}
KSTRING & KSTRING::operator<<(unsigned long _ulVal)
{
	char *pszTmp = new char [E_SZ_NUM]; memset(pszTmp,0x00,E_SZ_NUM);
	snprintf(pszTmp,E_SZ_NUM-1,"%lu",_ulVal);
	m_fnCat(pszTmp); delete [] pszTmp;return *this;
}
KSTRING & KSTRING::operator<<(char * _pvVal){m_fnCat((const char*)_pvVal);return *this;}
KSTRING & KSTRING::operator<<(const char * _pvVal){m_fnCat((const char*)_pvVal);return *this;}
KSTRING & KSTRING::operator<<(KSTRING & _rclsSrc)
{
	m_fnByteCat((const char*)_rclsSrc,_rclsSrc.m_unRealLen);return *this;
}
KSTRING & KSTRING::operator<<(double _dVal)
{
	char *pszTmp = new char[E_SZ_NUM]; memset(pszTmp, 0x00, E_SZ_NUM);
	snprintf(pszTmp, E_SZ_NUM - 1, "%lf", _dVal);
	m_fnCat(pszTmp); delete[] pszTmp; return *this;
}
//==========> type casting operator
KSTRING::operator KBOOL()
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
KSTRING::operator KCHR(){if(m_pszString == NULL) {return 0x00;}return (char)m_pszString[0];}
KSTRING::operator KUCHR(){if(m_pszString == NULL){ return 0x00;}return (unsigned char)m_pszString[0];}
KSTRING::operator KSTR(){return m_pszString;}
KSTRING::operator KCSTR(){return (const char *)m_pszString;}
KSTRING::operator KSHORT(){return (short)m_fnAtoi(m_pszString);}
KSTRING::operator KUSHORT(){return (unsigned short)m_fnAtoi(m_pszString);}
KSTRING::operator KINT(){return m_fnAtoi(m_pszString);}
KSTRING::operator KUINT(){return (unsigned int)m_fnAtoi(m_pszString);}
KSTRING::operator KLONG(){return m_fnAtol(m_pszString);}
KSTRING::operator KULONG(){return (unsigned long)m_fnAtol(m_pszString);}
KSTRING::operator KDOUBLE(){return m_fnAtof(m_pszString);}
//==========> [] Operator (Array Get/Set)
char & KSTRING::operator[](unsigned int _unIndex)
{
	static char cRet = 0;if(m_pszString == NULL){return cRet;}
	if(m_unRealLen <=  _unIndex){return cRet;}return m_pszString[_unIndex];
}
void KSTRING::m_fnCat(const char * _szSrc)
{
	unsigned int unNewLen = m_fnStrLen(_szSrc);
	if(unNewLen==0) return;
	if(m_pszString == NULL)
	{
		m_fnCreateString(_szSrc);
	}
	else if(m_unRealLen + unNewLen >= m_unLen)
	{
		m_unLen = m_unLen + unNewLen +E_SZ_STR_CAT;
		char * pszNew = new char [m_unLen];
		memcpy(pszNew,m_pszString,m_unRealLen);
		memcpy(&pszNew[m_unRealLen], _szSrc, unNewLen);
		m_unRealLen = m_unRealLen + unNewLen; 
		pszNew[m_unRealLen]= 0x00;
		delete [] m_pszString; m_pszString = pszNew;
	}
	else
	{
		memcpy(&m_pszString[m_unRealLen],_szSrc,unNewLen);
		m_unRealLen = m_unRealLen + unNewLen; 
		m_pszString[m_unRealLen]= 0x00;
	}
}
void KSTRING::m_fnCat(const char _ucInput)
{
	if(m_pszString == NULL)
	{
		m_fnCreateString(E_SZ_CHR_CAT);m_pszString[0]=_ucInput;m_unRealLen = 1;
	}
	else if(m_unRealLen +1 >= m_unLen)
	{
		m_unLen = m_unLen +E_SZ_CHR_CAT;
		char * pszNew = new char [m_unLen];
		memcpy(pszNew,m_pszString,m_unRealLen);
		pszNew[m_unRealLen] = _ucInput;
		m_unRealLen++; 
		pszNew[m_unRealLen]= 0x00;
		delete [] m_pszString; m_pszString = pszNew;
	}
	else
	{
		m_pszString[m_unRealLen] = _ucInput;
		m_unRealLen++;
		m_pszString[m_unRealLen]=0x00;
	}
}
void KSTRING::m_fnCat(int _nSpace, const char * _szSrc)
{
	unsigned int unSrcLen = m_fnStrLen(_szSrc);
	if(unSrcLen==0) return;
	unsigned int unSpace = abs(_nSpace);
	unsigned int unNewLen =  DEF_STR_TRUE_SELECT(unSpace>unSrcLen,unSpace,unSrcLen);
	if(m_pszString == NULL)
	{
		m_fnCreateString(unNewLen);
		m_fnFixedStrCat(m_pszString,_nSpace,_szSrc);
		m_unRealLen = unNewLen;
	}
	else if(m_unRealLen + unNewLen >= m_unLen)
	{
		m_unLen = m_unLen + unNewLen +E_SZ_STR_CAT;
		char * pszNew = new char [m_unLen];
		memcpy(pszNew,m_pszString,m_unRealLen);pszNew[m_unRealLen]=0x00;
		m_fnFixedStrCat(&pszNew[m_unRealLen],_nSpace,_szSrc);
		m_unRealLen += unNewLen;
		delete [] m_pszString; m_pszString = pszNew;
	}
	else
	{
		m_fnFixedStrCat(&m_pszString[m_unRealLen],_nSpace,_szSrc);
		m_unRealLen += unNewLen;
	}
}
void KSTRING::m_fnHexCat(unsigned char _ucInput)
{
	static char hexc[] = "0123456789ABCDEF";
	unsigned int hi = 0; unsigned int low = 0;
	hi = (_ucInput & 0xF0)>>4;low = (_ucInput & 0x0F);
	m_fnCat((const char)hexc[hi]);m_fnCat((const char)hexc[low]);
}
char * KSTRING::m_fnCatHexs(void * _pvData, unsigned int _unLen)
{
	m_fnCreateString(_unLen*4);unsigned int i=0;unsigned int j=0;unsigned int k=0;
	char * pszBody =(char*)_pvData;m_fnCat("\r\n\r\n");
	for(j=0,i=0;i<_unLen;i++)
	{
		j = i+1;k = j%E_HEX_LINE;
		if((k == 0) && (j != 1)){m_fnHexCat((unsigned char)(*(&pszBody[i]))); m_fnCat("\r\n");}
		else {m_fnHexCat((unsigned char)(*(&pszBody[i]))); m_fnCat(" ");}
	}
	m_fnCat("\r\n\r\n");return m_pszString;
}
void KSTRING::m_fnByteCat(const char * _szSrc, unsigned int _unLen)
{
	if(_szSrc == NULL) return;
	if(m_pszString == NULL)
	{
		m_fnCreateString(_unLen); memcpy(m_pszString,_szSrc,_unLen);m_unRealLen = _unLen;
	}
	else if(m_unRealLen + _unLen >= m_unLen)
	{
		m_unLen = m_unLen + _unLen +E_SZ_STR_CAT;
		char * pszNew = new char [m_unLen];
		memcpy(pszNew,m_pszString,m_unRealLen);
		memcpy(&pszNew[m_unRealLen], _szSrc, _unLen);
		m_unRealLen = m_unRealLen + _unLen; 
		pszNew[m_unRealLen]= 0x00;
		delete [] m_pszString; m_pszString = pszNew;
	}
	else
	{
		memcpy(&m_pszString[m_unRealLen],_szSrc,_unLen);
		m_unRealLen = m_unRealLen + _unLen; 
		m_pszString[m_unRealLen]= 0x00;
	}
}
void KSTRING::clear()
{
	if(m_pszString) delete [] m_pszString;
	m_pszString = NULL;m_unLen = 0;m_unRealLen = 0;
}
void KSTRING::m_fnCreateString(unsigned int _uiSize)
{
	clear();
	m_unLen = _uiSize+1;m_pszString = new char [m_unLen];
	memset(m_pszString,0x00,m_unLen);
}
void KSTRING::m_fnCreateString(const char * _szString)
{
	clear();
	m_unRealLen = m_fnStrLen(_szString);
	if(m_unRealLen==0) return;
	m_unLen = m_unRealLen+1;
	m_pszString = new char [m_unLen];
	memset(m_pszString,0x00,m_unLen);
	m_fnStrCpy(m_pszString,_szString);
}
//=====> Static Util Functions
int KSTRING::m_fnStrLen(const char * _szString)
{
	if(_szString == NULL) return 0;
	return strlen(_szString);
}
int KSTRING::m_fnStrCpy(char * _szDst, const char * _szSrc)
{
	if(_szDst == NULL || _szSrc == NULL) return -1;
	strncpy(_szDst,_szSrc,strlen(_szSrc)); return 1;
}
void KSTRING::m_fnStrnCatCpy(char * dst, const char * src, int _nMax)
{
	unsigned int len = m_fnStrLen(dst);
	unsigned int remain = _nMax-len-1;
	unsigned int srcLen = m_fnStrLen(src);
	if(remain < srcLen){ strncpy(&dst[len],src,remain);dst[(unsigned int)_nMax-1]=0x00;}
	else
	{
		strncpy(&dst[len],src,srcLen);dst[len+srcLen] = 0x00;
	}
}
void KSTRING::m_fnSprintf(const char *format, ...)
{
	char p=0;char cTmp = 0;int nTmp=0;double dTmp=0;long lTmp=0;char * pszTmp=NULL;
	va_list ap;va_start(ap, format);
	while(*format) 
	{
		p = *format++;
		if (p == '%') 
		{
			p = *format++;
			switch (p) 
			{
				case 'c':
					cTmp=va_arg(ap, int);(*this)<<cTmp;
					break;
				case 'd':
					nTmp=va_arg(ap, int);(*this)<<nTmp;
					break;
				case 'u':
					p=*format++;
					if(p=='l')
					{
						lTmp=va_arg(ap,long);(*this)<<lTmp;
					}
					else
					{
						nTmp=va_arg(ap, int);(*this)<<(KUINT)nTmp;
					}
					break;
				case 'f':
					dTmp=va_arg(ap, double);(*this)<<dTmp;
					break;
				case 'l':
					p=*format++;
					if(p== 'f') 
					{
						dTmp=va_arg(ap, double);(*this)<<dTmp;
					}
					else if(p == 'u')
					{
						lTmp=va_arg(ap,long);(*this)<<lTmp;
					}
					break;
				case 's':
					pszTmp=va_arg(ap, char*);(*this)<<pszTmp;
					break;
				default:
					(*this)<<"%"<<p;
					break;
			}
		} 
		else 
		{
			(*this)<<p;
		}
	}
	va_end(ap);
}
void KSTRING::m_fnTrim(const char * _pszTrim)
{
	if(m_pszString == NULL || m_unRealLen == 0) return;
	KSTRING szTmp = m_pszString;
	char * pszOut = m_fnGetOptimizeString((KSTR)szTmp, _pszTrim);
	KSTRING & rSrc = *this;
	rSrc = pszOut;
}
void KSTRING::m_fnUpperCase()
{
	if(m_pszString == NULL || m_unRealLen == 0) return;
	for(unsigned int i=0;i<m_unRealLen;i++)
	{
		m_pszString[i] = toupper((unsigned char)m_pszString[i]);
	}
}
void KSTRING::m_fnStrCat(char *mstr,const char * pFormat,...)
{
	if(mstr==NULL) return;
	unsigned int len = m_fnStrLen(mstr);char *temp = &mstr[len];
	va_list ap;va_start(ap, pFormat);vsprintf(temp, pFormat, ap);va_end(ap);
}
int KSTRING::m_fnStrnCat(char *_szSrc,unsigned int _unMaxSz,const char * _szVar,...)  // -2
{
	if(_szSrc==NULL) return -1;
	unsigned int unLen = strlen(_szSrc);
	if(unLen >= (_unMaxSz-1))
	{
		_szSrc[_unMaxSz-1]=0;return _unMaxSz;
	}
	char *pszTemp = &_szSrc[unLen];
	va_list ap;va_start(ap, _szVar);vsnprintf(pszTemp, _unMaxSz-unLen-1,_szVar, ap);va_end(ap);
	return 0;
}
int KSTRING::m_fnStrCmp(const char * _sz1, const char * _sz2)
{
	if(_sz1 == NULL || _sz2 == NULL) return -1;
	return strcmp(_sz1,_sz2);
}
int KSTRING::m_fnStrnCmp(const char * _sz1, const char * _sz2,unsigned int _unLen)
{
	if(_sz1 == NULL || _sz2 == NULL) return -1;
	return strncmp(_sz1,_sz2,_unLen);
}
int KSTRING::m_fnStrCaseCmp(const char * _sz1, const char * _sz2)
{
	if(_sz1 == NULL || _sz2 == NULL) return -1;
	return strcasecmp(_sz1,_sz2);
}
int KSTRING::m_fnStrnCaseCmp(const char * _sz1, const char * _sz2, unsigned int _unLen)
{
	if(_sz1 == NULL || _sz2 == NULL) return -1;
	return strncasecmp(_sz1,_sz2,_unLen);
}
int KSTRING::m_fnAtoi(const char * _szString)
{
	if(_szString == NULL) return 0;
	if(KSTRING::m_fnStrnCmp(_szString,(char*)"0x", 2)==0)
	{
		//return std::stoi(_szString, NULL, 16);
		int nValue = 0;
		sscanf(_szString, "%x", &nValue); 
		return nValue;
	}
	else
	{
		unsigned int nLen = KSTRING::m_fnStrLen(_szString);
		if(nLen==0) return 0;
		if(_szString[nLen-1] == 'k' || _szString[nLen-1] == 'K')
		{
			return atoi(_szString)*DEF_STR_1K;
		}
		else if(_szString[nLen-1] == 'm' || _szString[nLen-1] == 'M')
		{
			return atoi(_szString)*DEF_STR_1M;
		}
		else if(_szString[nLen-1] == 'g' || _szString[nLen-1] == 'G')
		{
			return atoi(_szString)*DEF_STR_1G;
		}
		else
			return atoi(_szString);
	}
}
long KSTRING::m_fnAtol(const char * _szString)
{
	if(_szString == NULL) return 0;
	if(KSTRING::m_fnStrnCmp(_szString,(char*)"0x", 2)==0)
	{
		//return std::stol(_szString, NULL, 16);
		long nValue = 0;
		sscanf(_szString, "%lx", &nValue); 
		return nValue;
	}
	else
	{
		unsigned int nLen = KSTRING::m_fnStrLen(_szString);
		if(nLen==0) return 0;
		if(_szString[nLen-1] == 'k' || _szString[nLen-1] == 'K')
		{
			return atol(_szString)*DEF_STR_1K;
		}
		else if(_szString[nLen-1] == 'm' || _szString[nLen-1] == 'M')
		{
			return atol(_szString)*DEF_STR_1M;
		}
		else if(_szString[nLen-1] == 'g' || _szString[nLen-1] == 'G')
		{
			return atol(_szString)*DEF_STR_1G;
		}
		else
			return atol(_szString);
	}
}
double KSTRING::m_fnAtof(const char * _szString)
{
	if (_szString == NULL) return 0;
	return atof(_szString);
}
char * KSTRING::m_fnStrStr(const char * _pszSrc, const char * _pszFind)
{
	if(_pszSrc == NULL || _pszFind == NULL) return NULL;
	return strstr((char*)_pszSrc,(char*)_pszFind);
}
const char *KSTRING::m_fnStr(char *_szStr)
{
	if(_szStr == NULL) return "";
	return DEF_STR_TRUE_SELECT(strlen(_szStr)>0, _szStr, "");
}
char * KSTRING::m_fnTrimTailString(char * _pszSrc, const char * _pszTrim)
{
	int unLen = m_fnStrLen(_pszSrc);
	if(unLen==0) return NULL;
	unsigned int j = 0;bool btrim = false;
	for(unLen--;unLen >= 0;unLen--)
	{
		btrim = false;
		for(j = 0;j<strlen(_pszTrim);j++)
		{
			if(_pszSrc[unLen] == _pszTrim[j])
			{
				_pszSrc[unLen]=0x00;btrim = true;break;
			}
		}
		if(btrim == false) break;
	}
	if(_pszSrc[0]==0) return NULL;
	return _pszSrc;
}
char * KSTRING::m_fnSkipString(const char * _pszSrc, const char * _pszTrim)
{
	if(m_fnStrLen(_pszSrc) <=0 || _pszTrim == NULL) return NULL;
	char *Ret = (char*)_pszSrc;
	unsigned int i=0;unsigned int j=0;unsigned int skp_cnt=0;bool btrim = false;
	while(1)
	{
		if(Ret[i]==0) 
		{
			return NULL;
		}
		btrim = false;
		for(j = 0;j<strlen(_pszTrim);j++)
		{
			if(Ret[i] == _pszTrim[j])
			{
				skp_cnt++;btrim = true;break;
			}
		}
		if(!btrim) return &Ret[skp_cnt];
		i++;
	}
	return NULL;
}
char *KSTRING::m_fnGetOptimizeString(char * _pszSrc, const char * _pszTrim)
{
	if(m_fnStrLen(_pszSrc)==0 || m_fnStrLen(_pszTrim) == 0) return NULL;
	char * pszTmp = m_fnSkipString(_pszSrc,_pszTrim);
	return m_fnTrimTailString(pszTmp,_pszTrim);
}
unsigned int  KSTRING::m_fnFixedStrCat(char * _pszDst, int _nSpace, const char * _pszSrc)
{
	unsigned int unDstLen = m_fnStrLen(_pszDst);unsigned int unSrcLen = m_fnStrLen(_pszSrc);
	int nSpace = abs(_nSpace);
	unsigned int unTotalLen = unDstLen;int unDiff = nSpace - unSrcLen; unsigned int i=0;
	if(_nSpace < 0)
	{
		for(i=0;i<unSrcLen;i++) _pszDst[unTotalLen+i] = _pszSrc[i];
		unTotalLen+=unSrcLen; _pszDst[unTotalLen] = 0;
		if(unDiff > 0)
		{
			for(i=0;i<(unsigned int)unDiff;i++)  _pszDst[unTotalLen+i] = ' ';
			unTotalLen+=unDiff; _pszDst[unTotalLen] = 0;
		}
	}
	else
	{
		if(unDiff > 0)
		{
			for(i=0;i<(unsigned int)unDiff;i++)  _pszDst[unTotalLen+i] = ' ';
			unTotalLen += unDiff;
			for(i=0;i<unSrcLen;i++) _pszDst[unTotalLen+i] = _pszSrc[i];
			unTotalLen+=unSrcLen; _pszDst[unTotalLen] = 0;
		}
		else
		{
			for(i=0;i<unSrcLen;i++) _pszDst[unTotalLen+i] = _pszSrc[i];
			unTotalLen+=unSrcLen; _pszDst[unTotalLen] = 0;
		}
	}
	return unTotalLen;
}
static const unsigned char s_arrHexToByte[256] = 
{
 // 0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
   'k','k','k','k','k','k','k','k','k','k','k','k','k','k','k','k',   // 0
   'k','k','k','k','k','k','k','k','k','k','k','k','k','k','k','k',   // 1
   'k','k','k','k','k','k','k','k','k','k','k','k','k','k','k','k',   // 2
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  'k','k','k','k','k','k',   // 3
   'k',0xA,0xB,0xC,0xD,0xE,0xF,'k','k','k','k','k','k','k','k','k',   // 4
   'k','k','k','k','k','k','k','k','k','k','k','k','k','k','k','k',   // 5
   'k',0xA,0xB,0xC,0xD,0xE,0xF,'k','k','k','k','k','k','k','k','k',   // 6
   'k','k','k','k','k','k','k','k','k','k','k','k','k','k','k','k',   // 8
   'k','k','k','k','k','k','k','k','k','k','k','k','k','k','k','k',   // 9
   'k','k','k','k','k','k','k','k','k','k','k','k','k','k','k','k',   // a
   'k','k','k','k','k','k','k','k','k','k','k','k','k','k','k','k',   // b
   'k','k','k','k','k','k','k','k','k','k','k','k','k','k','k','k',   // c
   'k','k','k','k','k','k','k','k','k','k','k','k','k','k','k','k',   // d
   'k','k','k','k','k','k','k','k','k','k','k','k','k','k','k','k',   // e
   'k','k','k','k','k','k','k','k','k','k','k','k','k','k','k','k'    // f
};
static const bool s_arrIsCharHex[256] =
{
// 0       1       2       3       4       5       6       7       8       9       a       b       c       d       e       f
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  // 0
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  // 1
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  // 2
   true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  true ,  false,  false,  false,  false,  false,  false,  // 3
   false,  true ,  true ,  true ,  true ,  true ,  true ,  false,  false,  false,  false,  false,  false,  false,  false,  false,  // 4
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  // 5
   false,  true ,  true ,  true ,  true ,  true ,  true ,  false,  false,  false,  false,  false,  false,  false,  false,  false,  // 6
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  // 8
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  // 9
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  // a
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  // b
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  // c
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  // d
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  // e
   false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false,  false   // f
};
void KSTRING::m_fnUnEscapeToString(const char * _pszSrc,KSTRING &_rclsResult)
{
	unsigned int unLen = m_fnStrLen(_pszSrc);
	if(unLen==0) return;
	const unsigned char* p = (unsigned char*)_pszSrc;
	unsigned char escaped = 0;
	while (p[0] !=0)
	{
		if(*p == '%' &&  p[2] !=0)
		{
			const char high = s_arrHexToByte[(unsigned char)*(p + 1)];
			const char low = s_arrHexToByte[(unsigned char)*(p + 2)];
			if (high!='k' && low!='k')
			{
				escaped = 0;            
				escaped = high << 4 | low;
				if (escaped > 31 && escaped != 127 && escaped != 58)
				{
					_rclsResult<<(KCHR)escaped;
					p+= 3;
				}
				else
				{
					_rclsResult<<'%';
					escaped = p[1];_rclsResult<<(KCHR)escaped;
					escaped = p[2];_rclsResult<<(KCHR)escaped;
					p+=3;
				}
			}
			else
			{
				_rclsResult<<'%';
				escaped = p[1];_rclsResult<<(KCHR)escaped;
				escaped = p[2];_rclsResult<<(KCHR)escaped;
				p+=3;
			}
		}
		else
		{
			escaped = p[0];
			_rclsResult<<(KCHR)escaped;
			p++;
		}
	}
}
void KSTRING::m_fnEscapeToString(const char * _pszSrc, const char *_pszChangeChr, KSTRING &_rclsResult)
{
	static char hex[] = "0123456789ABCDEF";
	const unsigned char* p = (unsigned char*)_pszSrc;
	unsigned int hi = 0; unsigned int low = 0; unsigned int unCnt = 0;
	std::bitset<256> stdBitSet; stdBitSet.reset();
	while(_pszChangeChr[unCnt] !=0)
	{
		stdBitSet.set((unsigned char)_pszChangeChr[unCnt]);
		unCnt++;
	}
	while (p[0] !=0)
	{
		if (p[0] == '%' && p[2]!=0 && s_arrIsCharHex[p[1]] && s_arrIsCharHex[p[2]])
		{
			_rclsResult<<(KCHR)p[0]<<(KCHR)p[1]<<(KCHR)p[2];
			p+=3;
		}
		else if (stdBitSet[(unsigned char)p[0]])   // Replace Charicter
		{
			hi = (*p & 0xF0)>>4;
			low = (*p & 0x0F);
			_rclsResult<<(KCHR)'%'<<(KCHR)hex[hi]<<(KCHR)hex[low];
			p++;
		}
		else
		{
			_rclsResult<<(KCHR)p[0];
			p++;
		}
	}
}
void KSTRING::m_fnEscapeToJson(const char * _pszSrc, KSTRING &_rclsResult)
{
	const unsigned char* p = (unsigned char*)_pszSrc;
	while (p[0] != 0)
	{
		switch (p[0])
		{
		case '"':  _rclsResult<<(KCHR)'\\'<<(KCHR)'"'; break;
		case '\\': _rclsResult<<(KCHR)'\\'<<(KCHR)'\\'; break;
		case '\n': _rclsResult<<(KCHR)'\\'<<(KCHR)'n'; break;
		case '\r': _rclsResult<<(KCHR)'\\'<<(KCHR)'r'; break;
		case '\t': _rclsResult<<(KCHR)'\\'<<(KCHR)'t'; break;
		case '\b': _rclsResult<<(KCHR)'\\'<<(KCHR)'b'; break;
		case '\f': _rclsResult<<(KCHR)'\\'<<(KCHR)'f'; break;
		default:   _rclsResult<<(KCHR)p[0]; break;
		}
		p++;
	}
}
bool KSTRING::m_fnTokenizeC(char *_pszOrig,const char *_pszTrimChr,TokStrings &_rclsResult)
{
	if(_pszOrig == NULL) return false;
	unsigned int pTrim_Len = KSTRING::m_fnStrLen(_pszTrimChr);
	unsigned int i=0;unsigned int j=0;int prev_st = 0;int next_st = 0;
	bool bmatch = false; unsigned int unOrigLen = KSTRING::m_fnStrLen(_pszOrig);
	for(i = 0;i<unOrigLen;i++)
	{
		for(j = 0;j<pTrim_Len;j++)
		{
			if(_pszOrig[i] == _pszTrimChr[j]){_pszOrig[i] = 0;prev_st = 0;break;}
			else prev_st = 1;
		}
		if(prev_st != next_st)
		{
			if(prev_st == 1)   // Detecting String
			{
				if(i == 0) bmatch = false;
				else bmatch = true;
				_rclsResult+= &_pszOrig[i];
			}
		}
		next_st = prev_st;
	}
	if(bmatch) return true;
	else return false;
}
bool KSTRING::m_fnTokenizeS(char *_pszOrig,const char *_pszTrimStr,TokStrings &_rclsResult)
{
	if(_pszOrig == NULL) return false;
	unsigned int match_idx = 0;int match_temp = -1;int Result_Num = 0;
	unsigned int pTrim_Len = KSTRING::m_fnStrLen(_pszTrimStr);
	unsigned int unOrigLen = KSTRING::m_fnStrLen(_pszOrig);
	if(unOrigLen<= pTrim_Len) return false;
	while(1)
	{
		match_temp = s_fnStrStr2(&_pszOrig[match_idx],(char*)_pszTrimStr);
		if(match_temp  == (-1))
		{
			if((Result_Num) > 0) return true;
			else return false;
		}
		match_idx += match_temp;memset(&_pszOrig[match_idx],0x00,pTrim_Len);
		match_idx+=pTrim_Len;		
		if(match_idx >= unOrigLen) return true;
		if(match_temp == 0)
		{
			if((Result_Num) > 0)
			{
				Result_Num--;_rclsResult.m_listString.deltail();
			}
		}
		else if(match_temp > 0)
		{
			if((Result_Num) == 0)
			{
				Result_Num++;_rclsResult+= &_pszOrig[0];
			}
		}
		Result_Num++;_rclsResult+= &_pszOrig[match_idx];
	}
	return true;
}
bool KSTRING::m_fnTokenizeP(char *_pszOrig, const char *_pszPattern,TokStrings &_rclsResult)
{
	if(_pszOrig == NULL) return false;
	int plen = KSTRING::m_fnStrLen(_pszPattern);unsigned int unOrigLen = KSTRING::m_fnStrLen(_pszOrig);
	int p_cnt = 0;int bPrev_P = 3;int bNext_P = 3;char *pTemp = NULL;
	for(unsigned int i=0;i<unOrigLen;i++)
	{
		if(bPrev_P == 1)
		{
			if(_pszOrig[i] != _pszPattern[p_cnt]) bPrev_P = 2;
			else  return false;
		}
		else 
		{
			if(_pszOrig[i] == _pszPattern[p_cnt]){bPrev_P = 1;p_cnt++;_pszOrig[i]=0;}
			else bPrev_P = 2;
		}
		if(bPrev_P != bNext_P)
		{
			if(bPrev_P == 2)
			{
				pTemp = &_pszOrig[i];
				for(unsigned int k=0;((pTemp[k]!=_pszPattern[p_cnt])||(pTemp[k]==0));k++)
				{
					if(pTemp[k] != ' ')
					{
						if(KSTRING::m_fnStrLen(&(pTemp[k]))>0)
						{
							_rclsResult+=&_pszOrig[i+k];
						}
						break;
					}
				}
			}
		}
		bNext_P = bPrev_P;
	}
	if(plen != p_cnt) return false;
	return true;
}
int KSTRING::m_fnRmChr(char* dest,char *source,const char* ptrim)
{
	int cnt = 0;unsigned int len = strlen(source);
	for(unsigned int i = 0;i< len;i++)
	{
		chr_jump:
		for(unsigned int j = 0;j<strlen(ptrim);j++)
		{
			if(*source == ptrim[j])
			{
				source++;cnt++;i++;
				goto chr_jump;
			}
		}
		*dest++ = *source++;			
	}
	return cnt;
}
void KSTRING::addPrint(char _cInput)
{
	m_fnCat(_cInput);
}
/** do tests */
#define DOTEST(bufsz, result, retval, ...) do { \
	KSTRING obj; \
	printf("now test %s\n", #__VA_ARGS__); \
	int r=obj.PRINT( __VA_ARGS__); \
	if(r != retval || strcmp((KCSTR)obj, result) != 0) { \
		printf("error test(%s) differs with system, \"%s\":%d\n", \
			""#bufsz", "#result", "#retval", "#__VA_ARGS__, \
			(KCSTR)obj, r); \
		exit(1); \
		} \
	printf("test(\"%s\":%d) passed\n", (KCSTR)obj, r); \
	} while(0);
int KSTRING::test_sprint_core()
{
	int x = 0;

	/* bufsize, expectedstring, expectedretval, snprintf arguments */
	DOTEST(1024, "hello", 5, "hello");
	DOTEST(1024, "h", 1, "h");
	/* warning from gcc for format string, but it does work
	 * DOTEST(1024, "", 0, ""); */

	//DOTEST(3, "he", 5, "hello");
	//DOTEST(1, "", 7, "%d", 7823089);

	/* test positive numbers */
	DOTEST(1024, "0", 1, "%d", 0);
	DOTEST(1024, "1", 1, "%d", 1);
	DOTEST(1024, "9", 1, "%d", 9);
	DOTEST(1024, "15", 2, "%d", 15);
	DOTEST(1024, "ab15cd", 6, "ab%dcd", 15);
	DOTEST(1024, "167", 3, "%d", 167);
	DOTEST(1024, "7823089", 7, "%d", 7823089);
	DOTEST(1024, " 12", 3, "%3d", 12);
	DOTEST(1024, "012", 3, "%.3d", 12);
	DOTEST(1024, "012", 3, "%3.3d", 12);
	DOTEST(1024, "012", 3, "%03d", 12);
	DOTEST(1024, " 012", 4, "%4.3d", 12);
	DOTEST(1024, "", 0, "%.0d", 0);

	/* test negative numbers */
	DOTEST(1024, "-1", 2, "%d", -1);
	DOTEST(1024, "-12", 3, "%3d", -12);
	DOTEST(1024, " -2", 3, "%3d", -2);
	DOTEST(1024, "-012", 4, "%.3d", -12);
	DOTEST(1024, "-012", 4, "%3.3d", -12);
	DOTEST(1024, "-012", 4, "%4.3d", -12);
	DOTEST(1024, " -012", 5, "%5.3d", -12);
	DOTEST(1024, "-12", 3, "%03d", -12);
	DOTEST(1024, "-02", 3, "%03d", -2);
	DOTEST(1024, "-15", 3, "%d", -15);
	DOTEST(1024, "-7307", 5, "%d", -7307);
	DOTEST(1024, "-12  ", 5, "%-5d", -12);
	DOTEST(1024, "-00012", 6, "%-.5d", -12);

	/* test + and space flags */
	DOTEST(1024, "+12", 3, "%+d", 12);
	DOTEST(1024, " 12", 3, "% d", 12);

	/* test %u */
	DOTEST(1024, "12", 2, "%u", 12);
	DOTEST(1024, "0", 1, "%u", 0);
	DOTEST(1024, "4294967295", 10, "%u", 0xffffffff);

	/* test %x */
	DOTEST(1024, "0", 1, "%x", 0);
	DOTEST(1024, "c", 1, "%x", 12);
	DOTEST(1024, "12ab34cd", 8, "%x", 0x12ab34cd);

	/* test %llu, %lld */
	DOTEST(1024, "18446744073709551615", 20, "%llu",
		(long long)0xffffffffffffffff);
	DOTEST(1024, "-9223372036854775808", 20, "%lld",
		(long long)0x8000000000000000);
	DOTEST(1024, "9223372036854775808", 19, "%llu",
		(long long)0x8000000000000000);

	/* test %s */
	DOTEST(1024, "hello", 5, "%s", "hello");
	DOTEST(1024, "     hello", 10, "%10s", "hello");
	DOTEST(1024, "hello     ", 10, "%-10s", "hello");
	DOTEST(1024, "he", 2, "%.2s", "hello");
	DOTEST(1024, "  he", 4, "%4.2s", "hello");
	DOTEST(1024, "   h", 4, "%4.2s", "h");

	/* test %c */
	DOTEST(1024, "a", 1, "%c", 'a');
	/* warning from gcc for format string, but it does work
	   DOTEST(1024, "    a", 5, "%5c", 'a');
	   DOTEST(1024, "a", 1, "%.0c", 'a'); */

	/* test %n */
	DOTEST(1024, "hello", 5, "hello%n", &x);
	if(x != 5) { printf("the %%n failed\n"); exit(1); }

	/* test %m */
	errno = 0;
	DOTEST(1024, "Success", 7, "%m");

	/* test %p */
	DOTEST(1024, "0x10", 4, "%p", (void*)0x10);
	DOTEST(1024, "(nil)", 5, "%p", (void*)0x0);

	/* test %% */
	DOTEST(1024, "%", 1, "%%");

	/* test %f */
	DOTEST(1024, "0.000000", 8, "%f", 0.0);
	DOTEST(1024, "0.00", 4, "%.2f", 0.0);
	/* differs, "-0.00" DOTEST(1024, "0.00", 4, "%.2f", -0.0); */
	DOTEST(1024, "234.00", 6, "%.2f", 234.005);
	DOTEST(1024, "8973497.1246", 12, "%.4f", 8973497.12456);
	DOTEST(1024, "-12.000000", 10, "%f", -12.0);
	DOTEST(1024, "6", 1, "%.0f", 6.0);

	DOTEST(1024, "6", 1, "%g", 6.0);
	DOTEST(1024, "6.1", 3, "%g", 6.1);
	DOTEST(1024, "6.15", 4, "%g", 6.15);

	/* These format strings are from the code of NSD, Unbound, ldns */

	DOTEST(1024, "abcdef", 6, "%s", "abcdef");
	DOTEST(1024, "005", 3, "%03u", 5);
	DOTEST(1024, "12345", 5, "%03u", 12345);
	DOTEST(1024, "5", 1, "%d", 5);
	DOTEST(1024, "(nil)", 5, "%p", NULL);
	DOTEST(1024, "12345", 5, "%ld", (long)12345);
	DOTEST(1024, "12345", 5, "%lu", (long)12345);
	DOTEST(1024, "       12345", 12, "%12u", (unsigned)12345);
	DOTEST(1024, "12345", 5, "%u", (unsigned)12345);
	DOTEST(1024, "12345", 5, "%llu", (unsigned long long)12345);
	DOTEST(1024, "12345", 5, "%x", 0x12345);
	DOTEST(1024, "12345", 5, "%llx", (long long)0x12345);
	DOTEST(1024, "012345", 6, "%6.6d", 12345);
	DOTEST(1024, "012345", 6, "%6.6u", 12345);
	DOTEST(1024, "1234.54", 7, "%g", 1234.54);
	DOTEST(1024, "123456789.54", 12, "%.12g", 123456789.54);
	DOTEST(1024, "3456789123456.54", 16, "%.16g", 3456789123456.54);
	/* %24g does not work with 24 digits, not enough accuracy,
	 * the first 16 digits are correct */
	DOTEST(1024, "12345", 5, "%3.3d", 12345);
	DOTEST(1024, "000", 3, "%3.3d", 0);
	DOTEST(1024, "001", 3, "%3.3d", 1);
	DOTEST(1024, "012", 3, "%3.3d", 12);
	DOTEST(1024, "-012", 4, "%3.3d", -12);
	DOTEST(1024, "he", 2, "%.2s", "hello");
	DOTEST(1024, "helloworld", 10, "%s%s", "hello", "world");
	DOTEST(1024, "he", 2, "%.*s", 2, "hello");
	DOTEST(1024, "  hello", 7, "%*s", 7, "hello");
	DOTEST(1024, "hello  ", 7, "%*s", -7, "hello");
	DOTEST(1024, "0", 1, "%c", '0'); 
	DOTEST(1024, "A", 1, "%c", 'A'); 
	DOTEST(1024, "", 1, "%c", 0); 
	DOTEST(1024, "\010", 1, "%c", 8); 
	DOTEST(1024, "%", 1, "%%"); 
	DOTEST(1024, "0a", 2, "%02x", 0x0a); 
	DOTEST(1024, "bd", 2, "%02x", 0xbd); 
	DOTEST(1024, "12", 2, "%02ld", (long)12); 
	DOTEST(1024, "02", 2, "%02ld", (long)2); 
	DOTEST(1024, "02", 2, "%02u", (unsigned)2); 
	DOTEST(1024, "765432", 6, "%05u", (unsigned)765432); 
	DOTEST(1024, "10.234", 6, "%0.3f", 10.23421); 
	DOTEST(1024, "123456.234", 10, "%0.3f", 123456.23421); 
	DOTEST(1024, "123456789.234", 13, "%0.3f", 123456789.23421); 
	DOTEST(1024, "123456.23", 9, "%.2f", 123456.23421); 
	DOTEST(1024, "123456", 6, "%.0f", 123456.23421); 
	DOTEST(1024, "0123", 4, "%.4x", 0x0123); 
	DOTEST(1024, "00000123", 8, "%.8x", 0x0123); 
	DOTEST(1024, "ffeb0cde", 8, "%.8x", 0xffeb0cde); 
	DOTEST(1024, " 987654321", 10, "%10lu", (unsigned long)987654321); 
	DOTEST(1024, "   987654321", 12, "%12lu", (unsigned long)987654321); 
	DOTEST(1024, "987654321", 9, "%i", 987654321); 
	DOTEST(1024, "-87654321", 9, "%i", -87654321); 
	DOTEST(1024, "hello           ", 16, "%-16s", "hello"); 
	DOTEST(1024, "                ", 16, "%-16s", ""); 
	DOTEST(1024, "a               ", 16, "%-16s", "a"); 
	DOTEST(1024, "foobarfoobar    ", 16, "%-16s", "foobarfoobar"); 
	DOTEST(1024, "foobarfoobarfoobar", 18, "%-16s", "foobarfoobarfoobar"); 

	/* combined expressions */
	DOTEST(1024, "foo 1.0 size 512 edns", 21,
		"foo %s size %d %s%s", "1.0", 512, "", "edns");
	//DOTEST(15, "foo 1.0 size 5", 21,"foo %s size %d %s%s", "1.0", 512, "", "edns");
	DOTEST(1024, "packet 1203ceff id", 18,
		"packet %2.2x%2.2x%2.2x%2.2x id", 0x12, 0x03, 0xce, 0xff);
	DOTEST(1024, "/tmp/testbound_123abcd.tmp", 26, "/tmp/testbound_%u%s%s.tmp", 123, "ab", "cd");
 
	return 0;
}


/****************************** sprintf core ***********************************************/
StrPrint::StrPrint(){}
StrPrint::~StrPrint(){}
unsigned int StrPrint::strLen(const char * _pszSrc)
{
	if(_pszSrc == NULL) return 0;
	return (unsigned int)strlen(_pszSrc);
}
void StrPrint::print_pad(int* ret, char p, int num)
{
	while(num--) 
	{
		addPrint(p);
		(*ret)++;
	}
}
char StrPrint::get_negsign(int negative, int plus, int space)
{
	if(negative)
		return '-';
	if(plus)
		return '+';
	if(space)
		return ' ';
	return 0;
}
#define PRINT_DEC_BUFSZ 32
 int StrPrint::print_dec(char* buf, int max, unsigned int value)
{
	int i = 0;
	if(value == 0) {
		if(max > 0) {
			buf[0] = '0';
			i = 1;
		}
	} else while(value && i < max) {
		buf[i++] = '0' + value % 10;
		value /= 10;
	}
	return i;
}
 int StrPrint::print_dec_l(char* buf, int max, unsigned long value)
{
	int i = 0;
	if(value == 0) {
		if(max > 0) {
			buf[0] = '0';
			i = 1;
		}
	} else while(value && i < max) {
		buf[i++] = '0' + value % 10;
		value /= 10;
	}
	return i;
}
 int StrPrint::print_dec_ll(char* buf, int max, unsigned long long value)
{
	int i = 0;
	if(value == 0) {
		if(max > 0) {
			buf[0] = '0';
			i = 1;
		}
	} else while(value && i < max) {
		buf[i++] = '0' + value % 10;
		value /= 10;
	}
	return i;
}
 int StrPrint::print_hex(char* buf, int max, unsigned int value)
{
	const char* h = "0123456789abcdef";
	int i = 0;
	if(value == 0) {
		if(max > 0) {
			buf[0] = '0';
			i = 1;
		}
	} else while(value && i < max) {
		buf[i++] = h[value & 0x0f];
		value >>= 4;
	}
	return i;
}
 int StrPrint::print_hex_l(char* buf, int max, unsigned long value)
{
	const char* h = "0123456789abcdef";
	int i = 0;
	if(value == 0) {
		if(max > 0) {
			buf[0] = '0';
			i = 1;
		}
	} else while(value && i < max) {
		buf[i++] = h[value & 0x0f];
		value >>= 4;
	}
	return i;
}
 int StrPrint::print_hex_ll(char* buf, int max, unsigned long long value)
{
	const char* h = "0123456789abcdef";
	int i = 0;
	if(value == 0) {
		if(max > 0) {
			buf[0] = '0';
			i = 1;
		}
	} else while(value && i < max) {
		buf[i++] = h[value & 0x0f];
		value >>= 4;
	}
	return i;
}
 void StrPrint::spool_str_rev(int* ret, const char* buf, int len)
{
	int i = len;
	char cIn = 0;
	while(i) 
	{
		cIn=buf[--i];
		addPrint(cIn);
		(*ret)++;
	}
}
 void StrPrint::spool_str(int* ret, const char* buf, int len)
{
	int i; char cIn = 0;
	for(i=0; i<len; i++) 
	{
		cIn = buf[i];
		addPrint(cIn);
		(*ret)++;
	}
}
 void StrPrint::print_num(int* ret, int minw, int precision,
	int prgiven, int zeropad, int minus, int plus, int space,
	int zero, int negative, char* buf, int len)
{
	int w = len; 
	char s = get_negsign(negative, plus, space);
	if(minus) {
		int numw = w;
		if(precision == 0 && zero) numw = 0;
		if(numw < precision) numw = precision;
		if(s) numw++;
		if(s) print_pad(ret, s, 1);
		if(precision == 0 && zero) {

		} else {
			if(w < precision)
				print_pad(ret, '0', precision - w);
			spool_str_rev(ret, buf, len);
		}
		if(numw < minw)
			print_pad(ret, ' ', minw - numw);
	} else {
		int numw = w;
		if(precision == 0 && zero) numw = 0;
		if(numw < precision) numw = precision;
		if(!prgiven && zeropad && numw < minw) numw = minw;
		else if(s) numw++;
		if(numw < minw)
			print_pad(ret, ' ', minw - numw);

		if(s) {
			print_pad(ret, s, 1);
			numw--;
		}
		if(w < numw)
			print_pad(ret, '0', numw - w);
		if(precision == 0 && zero)
			return;
		spool_str_rev(ret, buf, len);
	}
}
 void StrPrint::print_num_d(int* ret, int value,
	int minw, int precision, int prgiven, int zeropad, int minus,
	int plus, int space)
{
	char buf[PRINT_DEC_BUFSZ];
	int negative = (value < 0);
	int zero = (value == 0);
	int len = print_dec(buf, (int)sizeof(buf),
		(unsigned int)(negative?-value:value));
	print_num(ret, minw, precision, prgiven, zeropad, minus,
		plus, space, zero, negative, buf, len);
}
 void StrPrint::print_num_ld(int* ret, long value,
	int minw, int precision, int prgiven, int zeropad, int minus,
	int plus, int space)
{
	char buf[PRINT_DEC_BUFSZ];
	int negative = (value < 0);
	int zero = (value == 0);
	int len = print_dec_l(buf, (int)sizeof(buf),
		(unsigned long)(negative?-value:value));
	print_num(ret, minw, precision, prgiven, zeropad, minus,
		plus, space, zero, negative, buf, len);
}
 void StrPrint::print_num_lld(int* ret, long long value,
	int minw, int precision, int prgiven, int zeropad, int minus,
	int plus, int space)
{
	char buf[PRINT_DEC_BUFSZ];
	int negative = (value < 0);
	int zero = (value == 0);
	int len = print_dec_ll(buf, (int)sizeof(buf),
		(unsigned long long)(negative?-value:value));
	print_num(ret, minw, precision, prgiven, zeropad, minus,
		plus, space, zero, negative, buf, len);
}
 void StrPrint::print_num_u(int* ret, unsigned int value,
	int minw, int precision, int prgiven, int zeropad, int minus,
	int plus, int space)
{
	char buf[PRINT_DEC_BUFSZ];
	int negative = 0;
	int zero = (value == 0);
	int len = print_dec(buf, (int)sizeof(buf), value);
	print_num(ret, minw, precision, prgiven, zeropad, minus,
		plus, space, zero, negative, buf, len);
}
 void StrPrint::print_num_lu(int* ret, unsigned long value,
	int minw, int precision, int prgiven, int zeropad, int minus,
	int plus, int space)
{
	char buf[PRINT_DEC_BUFSZ];
	int negative = 0;
	int zero = (value == 0);
	int len = print_dec_l(buf, (int)sizeof(buf), value);
	print_num(ret, minw, precision, prgiven, zeropad, minus,
		plus, space, zero, negative, buf, len);
}
 void StrPrint::print_num_llu(int* ret, unsigned long long value,
	int minw, int precision, int prgiven, int zeropad, int minus,
	int plus, int space)
{
	char buf[PRINT_DEC_BUFSZ];
	int negative = 0;
	int zero = (value == 0);
	int len = print_dec_ll(buf, (int)sizeof(buf), value);
	print_num(ret, minw, precision, prgiven, zeropad, minus,
		plus, space, zero, negative, buf, len);
}
 void StrPrint::print_num_x(int* ret, unsigned int value,
	int minw, int precision, int prgiven, int zeropad, int minus,
	int plus, int space)
{
	char buf[PRINT_DEC_BUFSZ];
	int negative = 0;
	int zero = (value == 0);
	int len = print_hex(buf, (int)sizeof(buf), value);
	print_num(ret, minw, precision, prgiven, zeropad, minus,
		plus, space, zero, negative, buf, len);
}
 void StrPrint::print_num_lx(int* ret, unsigned long value,
	int minw, int precision, int prgiven, int zeropad, int minus,
	int plus, int space)
{
	char buf[PRINT_DEC_BUFSZ];
	int negative = 0;
	int zero = (value == 0);
	int len = print_hex_l(buf, (int)sizeof(buf), value);
	print_num(ret, minw, precision, prgiven, zeropad, minus,
		plus, space, zero, negative, buf, len);
}
 void StrPrint::print_num_llx(int* ret, unsigned long long value,
	int minw, int precision, int prgiven, int zeropad, int minus,
	int plus, int space)
{
	char buf[PRINT_DEC_BUFSZ];
	int negative = 0;
	int zero = (value == 0);
	int len = print_hex_ll(buf, (int)sizeof(buf), value);
	print_num(ret, minw, precision, prgiven, zeropad, minus,
		plus, space, zero, negative, buf, len);
}
 void StrPrint::print_num_llp(int* ret, void* value,
	int minw, int precision, int prgiven, int zeropad, int minus,
	int plus, int space)
{
	char buf[PRINT_DEC_BUFSZ];
	int negative = 0;
	int zero = (value == 0);
#if defined(UINTPTR_MAX) && defined(UINT32_MAX) && (UINTPTR_MAX == UINT32_MAX)
	unsigned long long llvalue = (unsigned long)value;
#else
	unsigned long long llvalue = (unsigned long long)value;
#endif
	int len = print_hex_ll(buf, (int)sizeof(buf), llvalue);
	if(zero) {
		buf[0]=')';
		buf[1]='l';
		buf[2]='i';
		buf[3]='n';
		buf[4]='(';
		len = 5;
	} else {
		
		if(len < PRINT_DEC_BUFSZ)
			buf[len++] = 'x';
		if(len < PRINT_DEC_BUFSZ)
			buf[len++] = '0';
	}
	print_num(ret, minw, precision, prgiven, zeropad, minus,
		plus, space, zero, negative, buf, len);
}
#define PRINT_FLOAT_BUFSZ 64 
 int StrPrint::print_remainder(char* buf, int max, double r, int prec)
{
	unsigned long long cap = 1;
	unsigned long long value;
	int len, i;
	if(prec > 19) prec = 19; 
	if(max < prec) return 0;
	for(i=0; i<prec; i++) {
		cap *= 10;
	}
	r *= (double)cap;
	value = (unsigned long long)r;
	if(((unsigned long long)((r - (double)value)*10.0)) >= 5) {
		value++;
		
		if(value >= cap)
			value = cap-1;
	}
	len = print_dec_ll(buf, max, value);
	while(len < prec) { 
		buf[len++] = '0';
	}
	if(len < max)
		buf[len++] = '.';
	return len;
}
 int StrPrint::print_float(char* buf, int max, double value, int prec)
{
	unsigned long long whole = (unsigned long long)value;
	double remain = value - (double)whole;
	int len = 0;
	if(prec != 0)
		len = print_remainder(buf, max, remain, prec);
	len += print_dec_ll(buf+len, max-len, whole);
	return len;
}
 void StrPrint::print_num_f(int* ret, double value,
	int minw, int precision, int prgiven, int zeropad, int minus,
	int plus, int space)
{
	char buf[PRINT_FLOAT_BUFSZ];
	int negative = (value < 0);
	int zero = 0;
	int len;
	if(!prgiven) precision = 6;
	len = print_float(buf, (int)sizeof(buf), negative?-value:value,
		precision);
	print_num(ret, minw, 1, 0, zeropad, minus,
		plus, space, zero, negative, buf, len);
}
 int StrPrint::print_float_g(char* buf, int max, double value, int prec)
{
	unsigned long long whole = (unsigned long long)value;
	double remain = value - (double)whole;
	int before = 0;
	int len = 0;
	while(whole > 0) {
		before++;
		whole /= 10;
	}
	whole = (unsigned long long)value;
	if(prec > before && remain != 0.0) {
		len = print_remainder(buf, max, remain, prec-before);
		while(len > 0 && buf[0]=='0') {
			memmove(buf, buf+1, --len);
		}
	}
	len += print_dec_ll(buf+len, max-len, whole);
	return len;
}
 void StrPrint::print_num_g(int* ret, double value,
	int minw, int precision, int prgiven, int zeropad, int minus,
	int plus, int space)
{
	char buf[PRINT_FLOAT_BUFSZ];
	int negative = (value < 0);
	int zero = 0;
	int len;
	if(!prgiven) precision = 6;
	if(precision == 0) precision = 1;
	len = print_float_g(buf, (int)sizeof(buf), negative?-value:value,
		precision);
	print_num(ret, minw, 1, 0, zeropad, minus,
		plus, space, zero, negative, buf, len);
}
 int StrPrint::my_strnlen(const char* s, int max)
{
	int i;
	for(i=0; i<max; i++)
		if(s[i]==0)
			return i;
	return max;
}
 void StrPrint::print_str(int* ret, char* s,
	int minw, int precision, int prgiven, int zeropad, int minus)
{
	int w;
	if(prgiven)
		w = my_strnlen(s, precision);
	else
	{
		if(s == NULL)
			w = 0;
		else
			w = (int)strlen(s); 
	}
	if(w < minw && !minus)
		print_pad(ret, zeropad ? '0' : ' ', minw - w);
	spool_str(ret, s, w);
	if(w < minw && minus)
		print_pad(ret, ' ', minw - w);
}
 void StrPrint::print_char(int* ret, int c,
	int minw, int minus)
{
	if(1 < minw && !minus)
		print_pad(ret, ' ', minw - 1);
	print_pad(ret, c, 1);
	if(1 < minw && minus)
		print_pad(ret, ' ', minw - 1);
}
int  StrPrint::VSPRINTF(const char* format, va_list arg)
{
	char cIn = 0;
	int ret = 0;
	const char* fmt = format;
	int conv, minw, precision, prgiven, zeropad, minus, plus, space, length;
	while(*fmt) 
	{
		while(*fmt && *fmt!='%') 
		{
			cIn = *fmt++;
			addPrint(cIn);
			ret++;
		}
		if(!*fmt) break;
		fmt++; 
		minw = 0;
		precision = 1;
		prgiven = 0;
		zeropad = 0;
		minus = 0;
		plus = 0;
		space = 0;
		length = 0;
		for(;;) {
			if(*fmt == '0')
				zeropad = 1;
			else if(*fmt == '-')
				minus = 1;
			else if(*fmt == '+')
				plus = 1;
			else if(*fmt == ' ')
				space = 1;
			else break;
			fmt++;
		}
		if(*fmt == '*') {
			fmt++; 
			minw = va_arg(arg, int);
			if(minw < 0) {
				minus = 1;
				minw = -minw;
			}
		} else while(*fmt >= '0' && *fmt <= '9') {
			minw = minw*10 + (*fmt++)-'0';
		}
		if(*fmt == '.') {
			fmt++; 
			prgiven = 1;
			precision = 0;
			if(*fmt == '*') {
				fmt++; 
				precision = va_arg(arg, int);
				if(precision < 0)
					precision = 0;
			} else while(*fmt >= '0' && *fmt <= '9') {
				precision = precision*10 + (*fmt++)-'0';
			}
		}

		if(*fmt == 'l') {
			fmt++; 
			length = 1;
			if(*fmt == 'l') {
				fmt++; 
				length = 2;
			}
		}
		if(!*fmt) conv = 0;
		else	conv = *fmt++;
		switch(conv) {
		case 'i':
		case 'd':
			if(length == 0)
			    print_num_d(&ret, va_arg(arg, int),
				minw, precision, prgiven, zeropad, minus, plus, space);
			else if(length == 1)
			    print_num_ld(&ret, va_arg(arg, long),
				minw, precision, prgiven, zeropad, minus, plus, space);
			else if(length == 2)
			    print_num_lld(&ret,
				va_arg(arg, long long),
				minw, precision, prgiven, zeropad, minus, plus, space);
			break;
		case 'u':
			if(length == 0)
			    print_num_u(&ret,
				va_arg(arg, unsigned int),
				minw, precision, prgiven, zeropad, minus, plus, space);
			else if(length == 1)
			    print_num_lu(&ret,
				va_arg(arg, unsigned long),
				minw, precision, prgiven, zeropad, minus, plus, space);
			else if(length == 2)
			    print_num_llu(&ret,
				va_arg(arg, unsigned long long),
				minw, precision, prgiven, zeropad, minus, plus, space);
			break;
		case 'x':
			if(length == 0)
			    print_num_x(&ret,
				va_arg(arg, unsigned int),
				minw, precision, prgiven, zeropad, minus, plus, space);
			else if(length == 1)
			    print_num_lx(&ret,
				va_arg(arg, unsigned long),
				minw, precision, prgiven, zeropad, minus, plus, space);
			else if(length == 2)
			    print_num_llx(&ret,
				va_arg(arg, unsigned long long),
				minw, precision, prgiven, zeropad, minus, plus, space);
			break;
		case 's':
			print_str(&ret, va_arg(arg, char*),
				minw, precision, prgiven, zeropad, minus);
			break;
		case 'c':
			print_char(&ret, va_arg(arg, int),
				minw, minus);
			break;
		case 'n':
			*va_arg(arg, int*) = ret;
			break;
		case 'm':
			print_str(&ret, strerror(errno),
				minw, precision, prgiven, zeropad, minus);
			break;
		case 'p':
			print_num_llp(&ret, va_arg(arg, void*),
				minw, precision, prgiven, zeropad, minus, plus, space);
			break;
		case '%':
			print_pad(&ret, '%', 1);
			break;
		case 'f':
			print_num_f(&ret, va_arg(arg, double),
				minw, precision, prgiven, zeropad, minus, plus, space);
			break;
		case 'g':
			print_num_g(&ret, va_arg(arg, double),
				minw, precision, prgiven, zeropad, minus, plus, space);
			break;
		default:
		case 0: break;
		}
	}
	return ret;
}
int  StrPrint::PRINT(const char* format, ...)
{
	int r;
	va_list args;
	va_start(args, format);
	r = VSPRINTF(format, args);
	va_end(args);
	return r;
}
/****************************** String Array Class ******************************************/
StringObj::StringObj(){}
StringObj::~StringObj(){}
KStrings::KStrings(){}
KStrings::~KStrings(){}
KStrings & KStrings::operator+=(const char * _pszV)
{
	if(KSTRING::m_fnStrLen(_pszV)==0) return *this;
	StringObj *pNew = new StringObj;
	pNew->m_clsStringData = _pszV;
	m_listString.pushback(pNew);
	return *this;
}
KSTRING & KStrings::operator[](unsigned int _unIndex)
{
	StringObj * pclsStr = (StringObj*)m_listString.index(_unIndex);
	if(pclsStr==NULL) return m_clsExept.m_clsStringData;
	return pclsStr->m_clsStringData;
}
unsigned int KStrings::num(){return m_listString.size();}
/****************************** Token Parse String Array Class ********************************/
TokString::TokString(){m_pszLine = NULL;}
TokString::~TokString(){}
TokStrings::TokStrings(){}
TokStrings::~TokStrings(){}
TokStrings & TokStrings::operator+=(const char * _pszV)
{
	if(KSTRING::m_fnStrLen(_pszV)==0) return *this;
	TokString *pNew = new TokString;
	pNew->m_pszLine = (char*)_pszV;
	m_listString.pushback(pNew);
	return *this;
}
char * TokStrings::operator[](unsigned int _unIndex)
{
	TokString * pclsStr = (TokString*)m_listString.index(_unIndex);
	if(pclsStr==NULL) return NULL;
	return pclsStr->m_pszLine;
}
unsigned int TokStrings::num(){return m_listString.size();}

ARRSTR::ARRSTR(){m_szSep=',';}
ARRSTR::~ARRSTR(){}
ARRSTR & ARRSTR::operator=(ARRSTR & _src)
{
	CLEAR();
	for(KUINT i=0;i<_src.NUMS();i++)
	{
		(*this)+= (KCSTR)_src[i];
	}
	return *this;
}
ARRSTR & ARRSTR::operator+=(KCSTR _pszV)
{
	m_arr+= _pszV;
	return *this;
}
ARRSTR & ARRSTR::operator+=(KSTRING _szV)
{
	m_arr+= (KCSTR)_szV;
	return *this;
}
KSTRING & ARRSTR::operator[](KUINT _unIdx)
{
	return m_arr[_unIdx];
}
KUINT ARRSTR::NUMS(){return m_arr.num();}
void ARRSTR::CLEAR(){m_arr.m_listString.clear();}
KCSTR ARRSTR::STR(KSTRING & _rSrc)
{
	for(KUINT i=0;i<NUMS();i++)
	{
		if(i ==0)
		{
			_rSrc<<m_arr[i];
		}
		else
		{
			_rSrc<<m_szSep; _rSrc<<m_arr[i];
		}
	}
	return (KCSTR)_rSrc;
}
TOKSTR::TOKSTR(){}
TOKSTR::TOKSTR(KCSTR _pszSrc){m_szSrc = _pszSrc;}
TOKSTR::TOKSTR(KSTRING _szSrc){m_szSrc = _szSrc;}
TOKSTR::~TOKSTR(){}
void TOKSTR::operator=(KCSTR _pszSrc)
{
	m_szSrc = _pszSrc;
}
void TOKSTR::operator=(KSTRING _szSrc)
{
	m_szSrc = _szSrc;
}
void TOKSTR::TOK(KCSTR _pszTrim)
{
	m_tok.CLEAR();
	TokStrings tok;
	KSTRING::m_fnTokenizeC((KSTR)m_szSrc,_pszTrim,tok);
	for(KUINT i=0; i<tok.num();i++)
		m_tok+= tok[i];
}
KSTRING & TOKSTR::operator[](KUINT _unIdx)
{
	return m_tok[_unIdx];
}
KUINT TOKSTR::NUMS(){return m_tok.NUMS();}
void TOKSTR::CLEAR(){m_tok.CLEAR();}

/****************************** Compare String Class ***************************************/
CmpString::CmpString(){m_pszString = NULL;}
CmpString::CmpString(const char * _pszSrc){m_pszString = (char*)_pszSrc;}
CmpString::~CmpString(){}
CmpString & CmpString::operator=(char * _pszSrc)
{
	m_pszString = _pszSrc;
	return *this;
}
CmpString & CmpString::operator=(const char * _pszSrc)
{
	m_pszString = (KSTR)_pszSrc;
	return *this;
}
bool CmpString::operator==(bool _bBool)
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
bool CmpString::operator==(char _cVal)
{
	if (m_pszString == NULL) { return false; }
	if (m_pszString[0] == _cVal) { return true; }return false;
}
bool CmpString::operator==(unsigned char _ucVal)
{
	if (m_pszString == NULL) { return false; }
	if ((unsigned char)m_pszString[0] == _ucVal) { return true; }return false;
}
bool CmpString::operator==(short _sVal)
{
	if (m_pszString == NULL) { return false; }
	short temp = (short)atoi(m_pszString); if (temp == _sVal) { return true; }return false;
}
bool CmpString::operator==(unsigned short _usVal)
{
	if (m_pszString == NULL) { return false; }
	unsigned short temp = (unsigned short)atoi(m_pszString);
	if (temp == _usVal) { return true; }return false;
}
bool CmpString::operator==(int _nVal)
{
	if (m_pszString == NULL) { return false; }
	int temp = (int)atoi(m_pszString); if (temp == _nVal) { return true; }return false;
}
bool CmpString::operator==(unsigned int _unVal)
{
	if (m_pszString == NULL) { return false; }
	unsigned int temp = (unsigned int)atoi(m_pszString); if (temp == _unVal) { return true; }return false;
}
bool CmpString::operator==(long _lVal)
{
	if (m_pszString == NULL) { return false; }
	long temp = (long)atol(m_pszString);
	if (temp == _lVal) { return true; }return false;
}
bool CmpString::operator==(unsigned long _ulVal)
{
	if (m_pszString == NULL) { return false; }
	unsigned long temp = (unsigned long)atol(m_pszString);
	if (temp == _ulVal) { return true; }return false;
}
bool CmpString::operator==(char * _szSrc)
{
	if (m_pszString == NULL || _szSrc == NULL) { return false; }
	if (strcmp(m_pszString, (char*)_szSrc) == 0) { return true; }return false;
}
bool CmpString::operator==(const char * _szSrc)
{
	if (m_pszString == NULL || _szSrc == NULL) { return false; }
	if (strcmp(m_pszString, (char*)_szSrc) == 0) { return true; }return false;
}
bool CmpString::operator==(double _dVal)
{
	if (m_pszString == NULL) { return false; }
	double temp = (double)atof(m_pszString);
	if (temp == _dVal) { return true; }return false;
}
bool CmpString::operator!=(bool _bBool)
{
	if (m_pszString == NULL) { return true; }
	if (strcasecmp(m_pszString, "true") == 0)
	{
		if (!_bBool) { return true; }return false;
	}
	else if (strcasecmp(m_pszString, "false") == 0)
	{
		if (!_bBool) { return false; }return true;
	}
	return true;
}
bool CmpString::operator!=(char _cVal)
{
	if (m_pszString == NULL) { return true; }
	if (m_pszString[0] != _cVal) { return true; }return false;
}
bool CmpString::operator!=(unsigned char _ucVal)
{
	if (m_pszString == NULL) { return true; }
	if ((unsigned char)m_pszString[0] != _ucVal) { return true; }return false;
}
bool CmpString::operator!=(short _sVal)
{
	if (m_pszString == NULL) { return true; }
	short temp = (short)atoi(m_pszString); if (temp != _sVal) { return true; }return false;
}
bool CmpString::operator!=(unsigned short _usVal)
{
	if (m_pszString == NULL) { return true; }
	unsigned short temp = (unsigned short)atoi(m_pszString);
	if (temp != _usVal) { return true; }return false;
}
bool CmpString::operator!=(int _nVal)
{
	if (m_pszString == NULL) { return true; }
	int temp = (int)atoi(m_pszString); if (temp != _nVal) { return true; }return false;
}
bool CmpString::operator!=(unsigned int _unVal)
{
	if (m_pszString == NULL) { return true; }
	unsigned int temp = (unsigned int)atoi(m_pszString); if (temp != _unVal) { return true; }return false;
}
bool CmpString::operator!=(long _lVal)
{
	if (m_pszString == NULL) { return true; }
	long temp = (long)atol(m_pszString);
	if (temp != _lVal) { return true; }return false;
}
bool CmpString::operator!=(unsigned long _ulVal)
{
	if (m_pszString == NULL) { return true; }
	unsigned long temp = (unsigned long)atol(m_pszString);
	if (temp != _ulVal) { return true; }return false;
}
bool CmpString::operator!=(char * _szSrc)
{
	if (m_pszString != NULL && _szSrc == NULL) { return true; }
	if (m_pszString == NULL && _szSrc != NULL) { return true; }
	if (m_pszString == NULL && _szSrc == NULL) { return false; }
	if (strcmp(m_pszString, (char*)_szSrc) != 0) { return true; }return false;
}
bool CmpString::operator!=(const char * _szSrc)
{
	if (m_pszString != NULL && _szSrc == NULL) { return true; }
	if (m_pszString == NULL && _szSrc != NULL) { return true; }
	if (m_pszString == NULL && _szSrc == NULL) { return false; }
	if (strcmp(m_pszString, (char*)_szSrc) != 0) { return true; }return false;
}
bool CmpString::operator!=(double _dVal)
{
	if (m_pszString == NULL) { return true; }
	double temp = (double)atof(m_pszString);
	if (temp != _dVal) { return true; }return false;
}
CmpString::operator KBOOL()
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
CmpString::operator KCHR() { if (m_pszString == NULL) { return 0x00; }return (char)m_pszString[0]; }
CmpString::operator KUCHR() { if (m_pszString == NULL) { return 0x00; }return (unsigned char)m_pszString[0]; }
CmpString::operator KSTR() { return m_pszString; }
CmpString::operator KCSTR() { return (const char *)m_pszString; }
CmpString::operator KSHORT() { return (short)KSTRING::m_fnAtoi(m_pszString); }
CmpString::operator KUSHORT() { return (unsigned short)KSTRING::m_fnAtoi(m_pszString); }
CmpString::operator KINT() { return KSTRING::m_fnAtoi(m_pszString); }
CmpString::operator KUINT() { return (unsigned int)KSTRING::m_fnAtoi(m_pszString); }
CmpString::operator KLONG() { return KSTRING::m_fnAtol(m_pszString); }
CmpString::operator KULONG() { return (unsigned long)KSTRING::m_fnAtol(m_pszString); }
CmpString::operator KDOUBLE() { return (double)KSTRING::m_fnAtof(m_pszString); }
}


