#include "INIFILE.h"

namespace nsUtil
{
IniFileReader::IniFileReader()
{
}
IniFileReader::~IniFileReader()
{
}
void IniFileReader::Read(KCSTR  _path)
{
	m_lock.WLOCK();
	m_file.init(_path);
	m_parser.PARSE(m_file.m_pszRawData);
	m_file.enablecheckchanged();
	m_lock.UNLOCK();
	Gpolling::setTimer(this, "ini", 1000, inifiletimeout);
}
void IniFileReader::IMPORT(RestMsg & _msg)
{
	m_lock.WLOCK();
	m_parser.IMPORT(_msg);
	KSTRING buf;
	m_parser.STR(buf);
	m_file.writefile((KCSTR)buf, buf.LENGTH());
	m_lock.UNLOCK();
}
void IniFileReader::JSON(RestMsg & _msg)
{
	m_lock.RLOCK();
	m_parser.JSON(_msg);
	m_lock.UNLOCK();
}
void IniFileReader::STR(KSTRING & _buf)
{
	m_lock.RLOCK();
	m_parser.STR(_buf);
	m_lock.UNLOCK();
}
KCSTR IniFileReader::GET(KCSTR _cate, KCSTR _key, KSTRING & _buf)
{
	m_lock.RLOCK();
	IniObject * pFind = m_parser.findLine(_cate, _key);
	if(pFind==NULL)
	{
		m_lock.UNLOCK();
		return NULL;
	}
	_buf = pFind->VAL();
	m_lock.UNLOCK();
	return (KCSTR)_buf;
}
void IniFileReader::inifiletimeout(Gpolling::info * _info)
{
	IniFileReader * pFile = (IniFileReader*)(_info->m_pOwner);
	pFile->m_lock.WLOCK();
	if(pFile->m_file.checkchanged())
	{
		pFile->m_parser.PARSE(pFile->m_file.m_pszRawData);
	}
	pFile->m_lock.UNLOCK();
}
}
