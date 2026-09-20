#include "STL.h"
namespace nsUtil
{
Iterator::Iterator()
{
	m_unCnt = 0;
}
Iterator::~Iterator()
{
}
ListItr_t & Iterator::nextlist()
{
	m_unCnt++; 
	return m_stItorList;
}
MapIntItr_t & Iterator::nextmapint()
{
	m_unCnt++; 
	return m_stMapItorInit;
}
MapStrItr_t & Iterator::nextmapstr()
{
	m_unCnt++; 
	return m_stMapItorStr;
}
StlObject::StlObject()
{
	m_uiKey=0;
	m_pszKey = NULL;
}
StlObject::StlObject(StlObject & _src)
{
	setkey(_src.m_pszKey);
	CONSTRUCT((void*)&_src);
}
StlObject::~StlObject()
{
	if(m_pszKey) 
		delete [] m_pszKey;
}
StlObject & StlObject::operator=(StlObject & _src)
{
	setkey(_src.m_pszKey);
	CONSTRUCT((void*)&_src);
	return *this;
}
bool StlObject::operator==(unsigned int _uiKey)
{
	if(m_uiKey == _uiKey) return true;
	return false;
}
bool StlObject::operator==(const char * _pszKey)
{
	if(_pszKey==NULL) return false;
	if(m_pszKey==NULL) return false;
	if(strcmp(m_pszKey,_pszKey)==0) return true;
	return false;
}
StlObject::operator STLKEYNUM()
{
	return m_uiKey;
}
StlObject::operator STLKEYSTR()
{
	return m_pszKey;
}
void StlObject::setkey(unsigned int _uiKey)
{
	m_uiKey = _uiKey;
}
void StlObject::setkey(const char * _pszKey)
{
	if(m_pszKey) delete [] m_pszKey;
	m_pszKey = createcopy(_pszKey);
}
void StlObject::CONSTRUCT(void * _pvSrc)
{
}
char * StlObject::createcopy(const char * _pszSrc)
{
	if(_pszSrc == NULL) return NULL;
	unsigned int unLen = strlen(_pszSrc);
	char *pszTmp = new char [unLen+1];
	strncpy(pszTmp,_pszSrc,unLen);pszTmp[unLen] = 0x00;
	return pszTmp;
}
StlList::StlList()
{
}
StlList::~StlList()
{
	clear();
}
bool StlList::pushback(StlObject *_obj,bool _bChk)
{
	if(_bChk == true && find(_obj))
	{
		delete _obj;	return false;
	}
	m_listR.push_back((STLPTR_t)_obj);
	return true;
}
bool StlList::pushfront(StlObject *_obj,bool _bChk)
{
	if(_bChk == true && find(_obj))
	{
		delete _obj;return false;
	}
	m_listR.push_front((STLPTR_t)_obj);
	return true;
}
bool StlList::insertidx(StlObject * _obj,unsigned int _uiIndex,bool _bChk)
{
	ListItr_t itrFind;unsigned int uiCnt = 0;
	for(itrFind = m_listR.begin();itrFind != m_listR.end();)
	{
		if(uiCnt == _uiIndex)
		{
			m_listR.insert(itrFind,(STLPTR_t)_obj);
			return true;
		}
		itrFind++;uiCnt++;
	}
	return pushback(_obj,_bChk);
}
void * StlList::find(unsigned int _uiKey)
{
	if(m_listR.size() ==0) return NULL;
	ListItr_t itrFind;StlObject *pclsObj = NULL;
	for(itrFind = m_listR.begin();itrFind != m_listR.end();)
	{
		pclsObj = (StlObject *)*itrFind;
		if(*pclsObj == _uiKey) 
		{
			return (void*)pclsObj;
		}
		itrFind++;
	}
	return NULL;
}
void * StlList::find(const char * _pszKey)
{
	if(m_listR.size() ==0) return NULL;
	ListItr_t itrFind;StlObject *pclsObj = NULL;
	for(itrFind = m_listR.begin();itrFind != m_listR.end();)
	{
		pclsObj = (StlObject *)*itrFind;
		if(*pclsObj == _pszKey) 
		{
			return (void*)pclsObj;
		}
		itrFind++;
	}
	return NULL;
}
void * StlList::find(StlObject *_obj)
{
	if(_obj->m_uiKey) return find(_obj->m_uiKey);
	else if(_obj->m_pszKey) return find(_obj->m_pszKey);
	return NULL;		
}
void * StlList::index(unsigned int _uiIndex)
{
	if(m_listR.size() ==0) return NULL;
	ListItr_t itrFind;
	unsigned int uiCnt = 0;
	for(itrFind = m_listR.begin();itrFind != m_listR.end();)
	{
		if(uiCnt == _uiIndex) return (void*)(*itrFind);
		itrFind++;uiCnt++;
	}
	return NULL;
}
bool StlList::delhead()
{
	if(m_listR.size() ==0) return false;
	ListItr_t ITOR;
	ITOR = m_listR.begin();
	StlObject *pclsObj = (StlObject *)*ITOR;
	delete pclsObj;
	m_listR.erase(ITOR);
	return true;
}
bool StlList::del(unsigned int  _uiKey)
{
	if(m_listR.size() ==0) return false;
	ListItr_t itrFind;StlObject *pclsObj = NULL;
	for(itrFind = m_listR.begin();itrFind != m_listR.end();)
	{
		pclsObj = (StlObject *)*itrFind;
		if(*pclsObj == _uiKey) 
		{
			delete pclsObj;
			m_listR.erase(itrFind);return true;
		}
		itrFind++;
	}
	return false;
}
bool StlList::del(const char * _pszKey)
{
	if(m_listR.size() ==0) return false;
	ListItr_t itrFind;StlObject *pclsObj = NULL;
	for(itrFind = m_listR.begin();itrFind != m_listR.end();)
	{
		pclsObj = (StlObject *)*itrFind;
		if(*pclsObj == _pszKey) 
		{
			delete pclsObj;
			m_listR.erase(itrFind);return true;
		}
		itrFind++;
	}
	return false;
}
bool StlList::deltail()
{
	if(m_listR.size() ==0) return false;
	ListItr_t itrFind;
	itrFind = m_listR.end();itrFind--;
	StlObject *pclsObj = (StlObject *)*itrFind;
	delete pclsObj;
	m_listR.erase(itrFind);
	return true;
}
bool StlList::delidx(unsigned int _unIndex)
{
	if(m_listR.size() ==0) return false;
	ListItr_t itrFind;StlObject *pclsObj = NULL; unsigned int unCnt=0;
	for(itrFind = m_listR.begin();itrFind != m_listR.end();)
	{
		pclsObj = (StlObject *)*itrFind;
		if( unCnt == _unIndex) 
		{
			delete pclsObj;
			m_listR.erase(itrFind);return true;
		}
		itrFind++; unCnt++;
	}
	return false;
}
bool StlList::clear()
{
	if(m_listR.size() ==0) return false;
	ListItr_t itrFind;StlObject *pclsObj = NULL; 
	for(itrFind = m_listR.begin();itrFind != m_listR.end();)
	{
		pclsObj = (StlObject *)*itrFind;
		delete pclsObj;
		m_listR.erase(itrFind++);
	}
	return true;
}
void * StlList::begin(ListItr_t & _item)
{
	_item = m_listR.begin();
	if(m_listR.size() ==0) return NULL;
	return (void*)*_item;
}
void * StlList::next(ListItr_t & _item)
{
	if(_item != m_listR.end())
	{
		_item++;
		if(_item == m_listR.end()) return NULL;
		return (void*)*_item;
	}
	return NULL;
}

bool StlList::isend(ListItr_t & _item)
{
	if(_item == m_listR.end()) return true;
	return false;
}
void * StlList::next(Iterator & _rclsItor)
{
	if(_rclsItor.m_unCnt==0)
	{
		return begin(_rclsItor.nextlist());
	}
	else
	{
		return next(_rclsItor.nextlist());
	}
}
void StlList::sort(PFuncSortList_t _pfn)
{
	if(_pfn) m_listR.sort(_pfn);
}
void StlList::reverse()
{
	m_listR.reverse();
}
unsigned int StlList::size()
{
	return m_listR.size();
}
/**************************** STL Map **************************************************/
StlMap::StlMap()
{
}
StlMap::~StlMap()
{
	clear();
}
void * StlMap::find(unsigned int _nKey)
{
	MapIntItr_t itrFind = m_mapR.find(_nKey);
	if (itrFind == m_mapR.end()) return NULL;
	STLPTR_t ulResult = itrFind->second;
	return (void*)ulResult;
}
void * StlMap::find(const char * _pszKey)
{
	if(_pszKey==NULL) return NULL;
	MapStrItr_t itrFind = m_mapS.find(StlMapStr_t::key_type(_pszKey));
	if (itrFind == m_mapS.end()) return NULL;
	STLPTR_t ulData = itrFind->second;
	return (void*)ulData;
}		
bool StlMap::set(unsigned int _nKey, StlObject * _obj)
{
	MapIntItr_t itrFind = m_mapR.find(_nKey);
	if (itrFind != m_mapR.end())
	{
		delete _obj;	
		return false;
	}
	m_mapR.insert(std::pair<unsigned int,STLPTR_t>(_nKey,(STLPTR_t)_obj));
	return true;
}
bool StlMap::set(const char *_pszKey,StlObject * _obj)
{
	if(_pszKey==NULL)
	{
		delete _obj;	
		return false;
	}
	MapStrItr_t itrFind = m_mapS.find(StlMapStr_t::key_type(_pszKey));
	if (itrFind != m_mapS.end())
	{
		delete _obj;	
		return false;
	}
	
	m_mapS.insert(std::pair<StlMapStr_t::key_type,STLPTR_t>(StlMapStr_t::key_type(_pszKey),(STLPTR_t)_obj));
	return true;
}
bool StlMap::del(unsigned int _nKey)
{
	MapIntItr_t itrFind = m_mapR.find(_nKey);
	if (itrFind == m_mapR.end()) return false;
	delete ((StlObject*)(itrFind->second));m_mapR.erase(itrFind);
	return true;
}
bool StlMap::del(const char *_pszKey)
{
	if(_pszKey==NULL) return false;
	MapStrItr_t itrFind = m_mapS.find(StlMapStr_t::key_type(_pszKey));
	if (itrFind == m_mapS.end()) return false;
	delete ((StlObject*)(itrFind->second));m_mapS.erase(itrFind);
	return true;
}
void StlMap::clear()
{
	MapIntItr_t itrInt;MapStrItr_t itrStr;
	for(itrInt = m_mapR.begin();itrInt != m_mapR.end();)
	{
		delete ((StlObject*)itrInt->second);m_mapR.erase(itrInt++);
	}
	for(itrStr = m_mapS.begin();itrStr != m_mapS.end();)
	{
		delete ((StlObject*)itrStr->second);m_mapS.erase(itrStr++);
	}
}
void * StlMap::begini(MapIntItr_t & _item)
{
	_item = m_mapR.begin();
	if(m_mapR.size() ==0) return NULL;
	return (void*)(_item->second);
}
void * StlMap::nexti(MapIntItr_t & _item)
{
	if(_item != m_mapR.end())
	{
		_item++;
		if(_item == m_mapR.end()) return NULL;
		return (void*)(_item->second);
	}
	return NULL;
}
bool StlMap::isend(MapIntItr_t & _item)
{
	if(_item != m_mapR.end()) return true;
	return false;
}
void * StlMap::begins(MapStrItr_t & _item)
{
	_item = m_mapS.begin();
	if(m_mapS.size() ==0) return NULL;
	return (void*)(_item->second);
}
void * StlMap::nexts(MapStrItr_t & _item)
{
	if(_item != m_mapS.end())
	{
		_item++;
		if(_item == m_mapS.end()) return NULL;
		return (void*)(_item->second);
	}
	return NULL;
}
bool StlMap::isends(MapStrItr_t & _item)
{
	if(_item != m_mapS.end()) return true;
	return false;
}
void * StlMap::getnexti(Iterator & _rclsItor)
{
	if(_rclsItor.m_unCnt==0)
	{
		return begini(_rclsItor.nextmapint());
	}
	else
	{
		return nexti(_rclsItor.nextmapint());
	}
}
void * StlMap::getnexts(Iterator & _rclsItor)
{
	if(_rclsItor.m_unCnt==0)
	{
		return begins(_rclsItor.nextmapstr());
	}
	else
	{
		return nexts(_rclsItor.nextmapstr());
	}
}
unsigned int StlMap::sizeint()
{
	return m_mapR.size();
}
unsigned int StlMap::sizestr()
{
	return m_mapS.size();
}

}
