#ifndef _STLMAP_H
#define _STLMAP_H
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string>
#include <string.h>
#ifdef _MSC_VER
#include "PosixCompat.h"
#else
#include <strings.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <iterator>
#include <list>
#include <map>
namespace nsUtil
{
typedef uintptr_t STLPTR_t;
typedef std::list<STLPTR_t> StlListLong_t;
typedef StlListLong_t::iterator ListItr_t;
typedef std::map<unsigned int, STLPTR_t> StlMapInt_t;
typedef std::map<std::string, STLPTR_t> StlMapStr_t;
typedef StlMapInt_t::iterator MapIntItr_t;
typedef StlMapStr_t::iterator MapStrItr_t;
typedef bool (*PFuncSortList_t)(STLPTR_t, STLPTR_t);
typedef unsigned int STLKEYNUM;
typedef char * STLKEYSTR;
class Iterator
{
	public:
		Iterator();
		~Iterator();
		ListItr_t & nextlist();
		MapIntItr_t & nextmapint();
		MapStrItr_t & nextmapstr();
		ListItr_t m_stItorList;
		MapIntItr_t m_stMapItorInit;
		MapStrItr_t m_stMapItorStr;
		unsigned int m_unCnt;
};
/******************* Base Object of (StlList or StlMap) ***************************************/ 
class StlObject
{
	public:
		StlObject();
		StlObject(StlObject & _src);
		virtual ~StlObject();
		StlObject & operator=(StlObject & _src);
		bool operator==(unsigned int _uiKey);
		bool operator==(const char * _pszKey);
		operator STLKEYNUM();
		operator STLKEYSTR();
		void setkey(unsigned int _uiKey);
		void setkey(const char * _pszKey);
		virtual void CONSTRUCT(void * _pvSrc);
		unsigned int m_uiKey;
		char *m_pszKey;
	private:
		char * createcopy(const char * _pszSrc);
};
/******************************* RU stl list ********************************************/
class StlList
{
	public:
		StlList();
		~StlList();
		bool  pushback(StlObject *_obj,bool _bChk=false);
		bool  pushfront(StlObject *_obj,bool _bChk=false);
		bool insertidx(StlObject * _obj,unsigned int _uiIndex,bool _bChk);
		void * find(unsigned int _uiKey);
		void * find(const char * _pszKey);
		void * find(StlObject *_obj);
		void * index(unsigned int _uiIndex);
		bool delhead();
		bool del(unsigned int _uiKey);
		bool del(const char * _pszKey);
		bool deltail();
		bool delidx(unsigned int _unIndex);
		bool clear();
		void * begin(ListItr_t & _item);
		void * next(ListItr_t & _item);
		bool isend(ListItr_t & _item);
		void * next(Iterator & _itor);
		void sort(PFuncSortList_t _pfn);  
		void reverse();
		unsigned int size();
		StlListLong_t m_listR;
	private:
		ListItr_t m_listItor;
};
class StlMap
{
	public:
		StlMap();
		~StlMap();
		void * find(unsigned int _nKey);
		void * find(const char * _pszKey);
		bool set(unsigned int _nKey, StlObject * _obj);  
		bool set(const char * _pszKey,StlObject * _obj);
		bool del(unsigned int _nKey);
		bool del(const char * _pszKey);
		void clear();
		void * begini(MapIntItr_t & _item);
		void * nexti(MapIntItr_t & _item);
		bool isend(MapIntItr_t & _item);
		void * begins(MapStrItr_t & _item);
		void * nexts(MapStrItr_t & _item);
		bool isends(MapStrItr_t & _item);
		void * getnexti(Iterator & _rclsItor);
		void * getnexts(Iterator & _rclsItor);
		unsigned int sizeint();
		unsigned int sizestr();
		StlMapInt_t m_mapR;
		StlMapStr_t m_mapS;
	private:
		MapIntItr_t m_mapItorInt;
		MapStrItr_t m_mapItorStr;
};

}
#endif

