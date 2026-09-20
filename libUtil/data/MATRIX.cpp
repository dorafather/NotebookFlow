#include "MATRIX.h"

namespace nsUtil
{
static void s_fnCatDepth(KSTRING & _rDebug, KUINT _unDepth)
{
	for(KUINT i=0;i<_unDepth;i++) _rDebug<<'\t';
}
KMatrixColums::PFuncParseState KMatrixColums::m_pfnParseHandle[KMatrixColums::MAX]=
{
	KMatrixColums::m_fnIDLE,
	KMatrixColums::m_fnCOLUM,
};
KMatrixColums::KMatrixColums(KMatrix * _pclsOwner)
{
	m_pclsCur=NULL;m_eSt = IDLE;m_pclsOwner=_pclsOwner;
}
KMatrixColums::~KMatrixColums(){}
void KMatrixColums::m_fnClear()
{
	m_pclsCur = NULL;m_eSt = IDLE;
	m_listColum.clear();
}
bool KMatrixColums::m_fnParse(char * _cInput)
{
	return m_pfnParseHandle[m_eSt](this,_cInput);
}
void KMatrixColums::m_fnDebug(KSTRING & _rDebug,KUINT _unDepth)
{
	s_fnCatDepth(_rDebug,_unDepth+1);
	for(KUINT i=0;i<NUMS();i++)
	{
		_rDebug<<'(';
		_rDebug<<INDEX(i);
		_rDebug<<") ";
	}
	_rDebug<<"\r\n";
}
KSTRING & KMatrixColums::operator[](KUINT _unIdx)
{
	KMatrixColum * pclsFind = (KMatrixColum*)m_listColum.index(_unIdx);
	if(pclsFind==NULL) return m_def.m_szVal;
	if(pclsFind->m_szVal.m_unRealLen==0)
	{
		if(pclsFind->m_pszVal==NULL) return m_def.m_szVal;
		pclsFind->m_szVal = pclsFind->m_pszVal;
	}
	return pclsFind->m_szVal;
}
KSTRING & KMatrixColums::INDEX(KUINT _unIdx)
{
	KMatrixColum * pclsFind = (KMatrixColum*)m_listColum.index(_unIdx);
	if(pclsFind==NULL) return m_def.m_szVal;
	if(pclsFind->m_szVal.m_unRealLen==0)
	{
		if(pclsFind->m_pszVal==NULL) return m_def.m_szVal;
		pclsFind->m_szVal = pclsFind->m_pszVal;
	}
	return pclsFind->m_szVal;
}
KSTRING & KMatrixColums::ADD()
{
	KMatrixColum * pclsNew = new KMatrixColum;
	m_listColum.pushback(pclsNew);
	return pclsNew->m_szVal;
}
KUINT KMatrixColums::NUMS(){return m_listColum.size();}
bool KMatrixColums::m_fnIDLE(KMatrixColums *_pclsObj,char * _cInput)
{
	if(_pclsObj->m_pclsOwner->m_fnIsDep2(_cInput))
	{
		_cInput[0] = 0x00;
	}
	else
	{
		_pclsObj->m_eSt = COLUM;
		_pclsObj->m_pclsCur = new KMatrixColum;
		_pclsObj->m_listColum.pushback(_pclsObj->m_pclsCur);
		_pclsObj->m_pclsCur->m_pszVal = _cInput;
	}
	return true;
}
bool KMatrixColums::m_fnCOLUM(KMatrixColums *_pclsObj,char * _cInput)
{
	if(_pclsObj->m_pclsOwner->m_fnIsDep2(_cInput))
	{
		_pclsObj->m_eSt = IDLE;
		_cInput[0] = 0x00;
	}
	else
	{

	}
	return true;
}

KMatrix::PFuncParseState KMatrix::m_pfnParseHandle[KMatrix::MAX]=
{
	KMatrix::m_fnIDLE,
	KMatrix::m_fnLINE,
};
KMatrix::KMatrix() : m_def(this)
{
	m_eSt = IDLE;m_pclsCur = NULL;m_pszRaw = NULL;m_szDep1="\r\n";	m_szDep2=" \t";
}
KMatrix::~KMatrix(){if(m_pszRaw) delete [] m_pszRaw;}
void KMatrix::m_fnClear()
{
	if(m_pszRaw) delete [] m_pszRaw;
	m_pszRaw = NULL;
	m_eSt = IDLE;m_pclsCur = NULL;m_pszRaw = NULL;
	m_listLine.clear();
}
bool KMatrix::m_fnParse(const char * _pszSrc, const char * _pszDep1, const char * _pszDep2)
{
	m_fnClear();
	KUINT unLen = KSTRING::m_fnStrLen(_pszSrc);
	if(unLen==0) return false;
	m_pszRaw = new char [unLen+1]; memset(m_pszRaw,0x00,unLen+1);
	memcpy(m_pszRaw,_pszSrc,unLen+1);
	m_szDep1 = _pszDep1; m_szDep2 = _pszDep2;
	char * pszPos = m_pszRaw;
	for(KUINT i=0;i<unLen;i++)
	{
		if(m_pfnParseHandle[m_eSt](this,pszPos)==false)
		{
			return false;
		}
		pszPos++;
	}
	for(KUINT i=0;i<NUMS();i++)
	{
		for(KUINT j=0;j<INDEX(i).NUMS();j++)
		{
			KSTRING szTmp = KSTRING::m_fnGetOptimizeString((KSTR)INDEX(i)[j]," \t");
			INDEX(i)[j] = szTmp;
		}
	}
	return true;
}
bool KMatrix::m_fnParse(char * _pszSrc)
{
	if(m_pfnParseHandle[m_eSt](this,_pszSrc)==false)
	{
		return false;
	}
	for(KUINT i=0;i<NUMS();i++)
	{
		for(KUINT j=0;j<INDEX(i).NUMS();j++)
		{
			KSTRING szTmp = KSTRING::m_fnGetOptimizeString((KSTR)INDEX(i)[j]," \t");
			INDEX(i)[j] = szTmp;
		}
	}
	return true;
}
KMatrixColums & KMatrix::ADD()
{
	KMatrixColums * pclsNew = new KMatrixColums(this);
	m_listLine.pushback(pclsNew);
	return *pclsNew;
}
void KMatrix::DEL(KUINT _unLine)
{
	m_listLine.delidx(_unLine);
}
void KMatrix::m_fnDebug(KSTRING & _rDebug,KUINT _unDepth)
{
	for(KUINT i=0;i<NUMS();i++)
	{
		INDEX(i).m_fnDebug(_rDebug,_unDepth);
	}
}
KMatrixColums & KMatrix::operator[](KUINT _unIdx)
{
	KMatrixColums * pclsFind = (KMatrixColums*)m_listLine.index(_unIdx);
	if(pclsFind==NULL) return m_def;
	return (*pclsFind);
}
KMatrixColums & KMatrix::INDEX(KUINT _unIdx)
{
	KMatrixColums * pclsFind = (KMatrixColums*)m_listLine.index(_unIdx);
	if(pclsFind==NULL) return m_def;
	return (*pclsFind);
}
KUINT KMatrix::NUMS(){return m_listLine.size();}
bool KMatrix::m_fnIDLE(KMatrix *_pclsObj,char * _cInput)
{
	if(_pclsObj->m_fnIsDep2(_cInput) || _pclsObj->m_fnIsDep1(_cInput))
	{
		_cInput[0] = 0x00;
	}
	else
	{
		_pclsObj->m_eSt = LINE;
		_pclsObj->m_pclsCur = new KMatrixColums(_pclsObj);
		_pclsObj->m_listLine.pushback(_pclsObj->m_pclsCur);
		return _pclsObj->m_pclsCur->m_fnParse(_cInput);
	}
	return true;
}
bool KMatrix::m_fnLINE(KMatrix *_pclsObj, char * _cInput)
{
	if(_pclsObj->m_fnIsDep1(_cInput))
	{
		_pclsObj->m_eSt = IDLE;	
		_cInput[0]=0x00;
	}
	else
	{
		return _pclsObj->m_pclsCur->m_fnParse(_cInput);
	}
	return true;
}
bool KMatrix::m_fnIsDep1(char * _cInput)
{
	for(KUINT i=0;i<m_szDep1.m_unRealLen;i++)
	{
		if(m_szDep1[i] == _cInput[0]) return true;
	}
	return false;
}
bool KMatrix::m_fnIsDep2(char * _cInput)
{
	for(KUINT i=0;i<m_szDep2.m_unRealLen;i++)
	{
		if(m_szDep2[i] == _cInput[0]) return true;
	}
	return false;
}
static char s_szMatrix[]=
"  a1 a2 a3 a4\r\n"
"  b1 b2 b3 b5\r\n"
"  c1 c2      \r\n"
"  d1 d2 d3   \r\n";
void KMatrix::m_fnTest()
{
	printf("%s",s_szMatrix);
	KMatrix Arr; Arr.m_fnParse(s_szMatrix,"\r\n"," \t");
	printf("[0][2] %s\r\n",(KSTR)Arr[0][2]);
	printf("[1][0] %s\r\n",(KSTR)Arr[1][0]);
	printf("[2][1] %s\r\n",(KSTR)Arr[2][1]);
	printf("[3][2] %s\r\n",(KSTR)Arr[3][2]);
}
void KMatrix::m_fnTest2(KSTRING & _szOrg, KSTRING & _szResult)
{
	printf("%s",s_szMatrix);
	_szOrg<<s_szMatrix;
	KMatrix Arr; Arr.m_fnParse(s_szMatrix,"\r\n"," \t");
	_szResult<<"[0][2] "<<(KSTR)Arr[0][2]; _szResult<<"\r\n";
	_szResult<<"[1][0] "<<(KSTR)Arr[1][0]; _szResult<<"\r\n";
	_szResult<<"[2][1] "<<(KSTR)Arr[2][1]; _szResult<<"\r\n";
	_szResult<<"[3][2] "<<(KSTR)Arr[3][2]; _szResult<<"\r\n";
}
}
