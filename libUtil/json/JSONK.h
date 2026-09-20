#ifndef JSONK_HXX
#define JSONK_HXX
#include "STL.h"
#include "KSTRING.h"
#include "JSON.h"
namespace nsUtil
{
/******************************** Test Functions *****************************************/
void g_fnJsonTest();
/*********************************** Json Object ****************************************/
class JsonObject : public StlObject
{
	public:
		JsonObject();
		virtual ~JsonObject();
		JsonObject & operator=(JsonObject & _rclsSrc);
		void build(KSTRING & _rclsBuild, unsigned int _unDepth);
		KSTRING & findval(const char * _pszKey);
		JsonObject *add(json_type _eT,const char * _pszKey, KSTRING _clsVal);
		JsonObject * find(const char * _pszKey);
		bool del(const char * _pszKey);
		JsonObject * begin(ListItr_t & _rstItor);
		JsonObject * next(ListItr_t & _rstItor);
		unsigned int num();
		void clear();
		json_type m_eType;
		KSTRING m_clsJKey;
		KSTRING m_clsJData;
		JsonObject *m_pclsOwner;
	protected:
		JsonObject * addinternal(json_type _eT,const char *_pszKey);
		StlList m_listJson;
	private:
		static void catdepth(KSTRING &_rclsBuff,unsigned int _unDepth);
		KSTRING m_clsNull;
};
/******************************** JSON ************************************************/
class JsonK
{
	public:
		JsonK();
		virtual ~JsonK();
		JsonK & operator=(JsonK & _rclsSrc);
		bool parse(const char *_pszRaw);
		JsonObject *add(json_type _eT,const char * _pszKey,KSTRING _clsVal);
		JsonObject * find(const char * _pszKey);
		bool del(const char * _pszKey);
		JsonObject * begin(ListItr_t & _rstItor);
		JsonObject * next(ListItr_t & _rstItor);
		unsigned int num();
		char * build();
		char * build(KSTRING & _szBuild);
		KSTRING & findval(const char * _pszKey);
		void clear();
		static const char * stringtype(json_type _eT);
		static void builditem(KSTRING &_rclsBuild,const char * _pszKey,KSTRING _clsVal);
		static void catdepth(KSTRING &_rclsBuff,unsigned int _unDepth);
	protected:
		JsonObject * addinternal(json_type _eT,const char *_pszKey);
		StlList m_listJson;
	private:
		void extract(json_object *obj,unsigned int _unDepth,bool * _pbError);
		KSTRING m_clsRaw;
		KSTRING m_clsNull;
		JsonObject *m_pclsCurObj;
};
}
#endif
