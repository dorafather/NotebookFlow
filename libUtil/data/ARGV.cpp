#include <string.h>
#include "ARGV.h"

namespace nsUtil
{
/******************************** Bytes Data ***************************************/
void g_fnMoveString(KSTRING & _rclsDst, KSTRING & _rclsSrc)
{
	_rclsDst = KNULL;
	_rclsDst.m_unLen = _rclsSrc.m_unLen; _rclsSrc.m_unLen=0;
	_rclsDst.m_unRealLen = _rclsSrc.m_unRealLen; _rclsSrc.m_unRealLen = 0;
	_rclsDst.m_pszString = _rclsSrc.m_pszString; _rclsSrc.m_pszString = NULL; 
}
Bytes::Bytes() :m_szData(KNULL)
{
	m_bReference = false;
}
Bytes::Bytes(Bytes & _rclsSrc)
{
	setkey(_rclsSrc.m_pszKey);
	CONSTRUCT((void*)&_rclsSrc);
}
Bytes::~Bytes(){}
void Bytes::SETKEY(KCSTR _key)
{
	setkey(_key);
	m_key = _key;
}
Bytes & Bytes::operator=(Bytes & _rclsSrc) 
{
	setkey(_rclsSrc.m_pszKey);
	CONSTRUCT((void*)&_rclsSrc);
	return *this;
}
Bytes & Bytes::operator<<(Bytes & _rclsSrc)
{
	if(_rclsSrc.m_szData.m_unRealLen >  0)
	{
		m_szData = KNULL;
		g_fnMoveString(m_szData,_rclsSrc.m_szData);
		m_bReference = false;
	}
	return *this;
}
Bytes::operator KVOID()
{
	if(m_bReference)
	{
		void * pvResult = NULL;
		sscanf((KCSTR)m_szData,"%p", &pvResult);
		return pvResult;
	}
	else
		return (void*)m_szData.m_pszString;
}
KSTRING & Bytes::KEY()
{
	return m_key;
}
KSTRING & Bytes::VAL()
{
	return m_szData;
}
unsigned int Bytes::LEN()
{
	return m_szData.m_unRealLen;
}
void Bytes::MOVE(void * _pvData, unsigned int _unLen)
{
	m_szData=KNULL; m_szData.m_unRealLen = _unLen; m_szData.m_unLen = _unLen;
	m_szData.m_pszString = (char*)_pvData; 
}
void Bytes::COPY(void * _pvData,unsigned int _unLen)
{
	if(_pvData==NULL || _unLen==0) return;
	if((_unLen+1) < m_szData.m_unLen)
	{
		m_szData=KNULL;
		m_szData.m_fnByteCat((const char *)_pvData,_unLen);
	}
	else
	{
		m_szData=KNULL;
		m_szData.m_fnByteCat((const char *)_pvData,_unLen);
	}
}
void Bytes::REFER(void * _pvObject)
{
	m_bReference = true;
	m_szData.PRINT("%p",_pvObject);
}
void Bytes::RELEASE()
{
	m_bReference = false;
	m_szData.m_unLen = 0;
	m_szData.m_unRealLen = 0;
	m_szData.m_pszString = NULL;
}
void Bytes::CONSTRUCT(void * _src)
{
	Bytes * pSrc = (Bytes*)_src;
	m_bReference = pSrc->m_bReference;
	m_szData = pSrc->m_szData;
}
/***************************** Function Args *************************************/
Bytes ARG::m_DefBytes; 
ARG::ARG()
{
	m_plistArgs = new StlList;
}
ARG::~ARG()
{
	if(m_plistArgs) delete m_plistArgs;
}
ARG & ARG::operator=(ARG & _rclsSrc)
{
	Iterator clsItor; Bytes * pclsNew = NULL;
	if(_rclsSrc.m_plistArgs)
	{
		if(m_plistArgs) delete m_plistArgs;
		m_plistArgs = new StlList;
		Bytes * pclsInfo = (Bytes *)_rclsSrc.m_plistArgs->next(clsItor);
		while(pclsInfo)
		{
			pclsNew = new Bytes;
			*pclsNew = *pclsInfo;
			m_plistArgs->pushback(pclsNew);
			pclsInfo = (Bytes*)_rclsSrc.m_plistArgs->next(clsItor);
		}
	}
	return *this;
}
ARG & ARG::operator<<(ARG & _rclsSrc)
{
	if(_rclsSrc.m_plistArgs)
	{
		if(m_plistArgs) delete m_plistArgs;
		m_plistArgs = _rclsSrc.m_plistArgs;
		_rclsSrc.m_plistArgs = NULL;
	}
	else
	{
		m_plistArgs = _rclsSrc.m_plistArgs;
		_rclsSrc.m_plistArgs = NULL;
	}
	return *this;
}
Bytes & ARG::operator[](unsigned int _nIndex)
{
	if(m_plistArgs==NULL)
	{
		m_plistArgs = new StlList;
		return m_DefBytes;
	}
	Bytes * pFind = (Bytes*)m_plistArgs->index(_nIndex);
	if(pFind==NULL) return m_DefBytes;
	return *pFind;
}
Bytes & ARG::SET(KSTRING  _clsKey)
{
	if(m_plistArgs==NULL) m_plistArgs = new StlList;
	Bytes * pclsNew = (Bytes*)m_plistArgs->find((KCSTR)_clsKey);
	if(pclsNew == NULL)
	{
		pclsNew = new Bytes;
		pclsNew->SETKEY((KCSTR)_clsKey);
		m_plistArgs->pushback(pclsNew);
	}
	return *pclsNew;
}
Bytes & ARG::GET(KSTRING  _clsKey) 
{
	if(m_plistArgs==NULL) return m_DefBytes;
	Bytes * pclsFind = (Bytes*)m_plistArgs->find((KCSTR)_clsKey);
	if(pclsFind==NULL) {return m_DefBytes;}
	return *pclsFind;
}
void ARG::CLEAR()
{
	if(m_plistArgs)
	{
		delete m_plistArgs;
		m_plistArgs = new StlList;
	}
}
unsigned int ARG::genhash(char * _pszKey, unsigned int _unMax)
{
	if(_pszKey==NULL || _unMax==1 || KSTRING::m_fnStrLen(_pszKey)==0) return 0;
	unsigned int unResult = 0;
	unsigned long key = 0;unsigned long ch = 0;
	key = 5381;
	for(unsigned int i = 0;(_pszKey[i] != 0x00);i++)
	{
		ch = (unsigned long)_pszKey[i];
		key = ((key<< 5) + key) + ch;
	}
	unResult = (unsigned int)(key%_unMax);
	return unResult;
}
}
