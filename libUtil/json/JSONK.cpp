#include "JSONK.h"

namespace nsUtil
{
/********************************** Json Text Sample **************************************/
const char g_pszJTest[]=
"{																			     \r\n"
"	\"Version\": \"Rel 1.35_linux\",                                                                                                                     \r\n"
"	\"History\": [\"1\",\"2\",\"3\"],														     \r\n"
"	\"History2\": 																	     \r\n"
"	[																		     \r\n"
"		{                                                                                                                                            \r\n"
"			\"aa\": \"a_a_\",\"bb\": \"b_b_\"		                                                                                     \r\n"
"		},                                                                                                                                           \r\n"
"		{                                                                                                                                            \r\n"
"			\"cc\": \"c_c_\",\"dd\": \"d_d_\"		                                                                                     \r\n"
"		}                                                                                                                                            \r\n"
"	],																	             \r\n"
"	\"LOC_KT\":                                                                                                                                          \r\n"
"	{                                                                                                                                                    \r\n"
"		\"Addr\":{\"IP\": \"0.0.0.0\",\"Port\": \"5060\",\"Type\": \"UDP\",\"Domain\": \"iok.com\"},                                                 \r\n"
"		\"RMT\":                                                                                                                                     \r\n"
"		{                                                                                                                                            \r\n"
"			\"RMT_KT\" :                                                                                                                         \r\n"
"			{                                                                                                                                    \r\n"
"				\"RTE\":                                                                                                                     \r\n"
"				{                                                                                                                            \r\n"
"					\"RTE_KT_01\":                                                                                                       \r\n"
"					{                                                                                                                    \r\n"
"						\"Addr\":{\"IP\": \"0.0.0.0\",\"Port\": \"5060\",	\"Type\": \"UDP\",	\"Domain\": \"ibc1.kt.com\"} \r\n"
"					},                                                                                                                   \r\n"
"					\"RTE_KT_02\":                                                                                                       \r\n"
"					{                                                                                                                    \r\n"
"						\"Addr\":{\"IP\": \"0.0.0.0\",\"Port\": \"5060\",	\"Type\": \"UDP\",	\"Domain\": \"ibc2.kt.com\"} \r\n"
"					}                                                                                                                    \r\n"
"				},                                                                                                                           \r\n"
"				\"RSEQ\":                                                                                                                    \r\n"
"				{                                                                                                                            \r\n"
"					\"RSEQ_KT_01\": [ \"RTE_KT_01\", \"RTE_KT_02\" ],                                                                    \r\n"
"					\"RSEQ_KT_02\": [ \"RTE_KT_02\", \"RTE_KT_01\" ]                                                                     \r\n"
"				},                                                                                                                           \r\n"
"				\"SSEQ\":                                                                                                                    \r\n"
"				{                                                                                                                            \r\n"
"					\"SSEQ_KT_1_1\":{\"RTE_KT_01\": \"1\",\"RTE_KT_02\": \"1\"},                                                         \r\n"
"					\"SSEQ_KT_1_5\":{\"RTE_KT_01\": \"1\",	\"RTE_KT_02\": \"5\"}                                                        \r\n"
"				},                                                                                                                           \r\n"
"				\"TEST\": [{\"123\": \"abc\"},\"a\",\"b\",\"c\",\"d\",{\"123\": \"abc\"}]   						     \r\n"
"			}                                                                                                                                    \r\n"
"		},                                                                                                                                           \r\n"
"		\"SRV\":	   															     \r\n"
"		{																	     \r\n"
"			\"key1\": \"val1\",\"key2\": \"val2\" 												     \r\n"
"		}																	     \r\n"
"	},                                                                                                                                                   \r\n"
"	\"GRP_1\":	   																     \r\n"
"	{																		     \r\n"
"		\"key1\": \"val1\",\"key2\": \"val2\" 													     \r\n"
"	},																		     \r\n"
"	\"End\": [{\"a\": \"aaa\"},{\"b\": \"bbb\"},{\"c\": \"ccc\"}],											     \r\n"
"	\"subdupArr\":																	     \r\n"
"	[                                                                                                                                                    \r\n"
"		{                                                                                                                                            \r\n"
"			\"subsubarr\": [\"a\",\"b\",\"c\"],                                                                                                  \r\n"
"			\"subsubarr2\": [\"d\",\"e\",\"f\"]                                                                                                  \r\n"
"		},                                                                                                                                           \r\n"
"		{                                                                                                                                            \r\n"
"			\"subsubarr\": [\"a\",\"b\",\"c\"],                                                                                                  \r\n"
"			\"subsubarr2\": [\"d\",\"e\",\"f\"]                                                                                                  \r\n"
"		},                                                                                                                                           \r\n"
"	],                                                                                                                                                   \r\n"
"	\"service\":                                                                                                                                         \r\n"
"	{                                                                                                                                                    \r\n"
"		\"trace\": \"true\",                                                                                                                         \r\n"
"		\"charging\": \"false\"                                                                                                                      \r\n"
"	},                                                                                                                                                   \r\n"
"	\"timer\":                     	                                                                                                                     \r\n"
"	{                                                                                                                                                    \r\n"
"		\"garbage\": \"60000\",                                                                                                                      \r\n"
"		\"refresh\": \"1000\"                                                                                                                        \r\n"
"	}                                                                                                                                                    \r\n"
"}                                                                                                                                                           \r\n";
/******************************** Test Functions *****************************************/
void g_fnJsonTest()
{
	//==========> Json Parse Test
	JsonK clsParseTest;
	if(clsParseTest.parse(g_pszJTest))
	{
		
		JsonK clsParseTest2; clsParseTest2 = clsParseTest;
		
		JsonObject * pclsFind = clsParseTest2.find("History");
		if(pclsFind)
		{
			//ListItr_t stItor;
			//JsonObject * pclsBegin = pclsFind->begin(stItor);
		}
		JsonObject * pclsFindLoc = clsParseTest2.find("LOC_KT");
		if(pclsFindLoc)
		{
			JsonObject * pclsAddr = pclsFindLoc->find("Addr");
			if(pclsAddr)
			{
				JsonObject * pclsDomain = pclsAddr->find("Domain");
				if(pclsDomain)
				{
					
				}
			}
		}
		JsonObject * pclsService = clsParseTest2.find("service");
		if(pclsService)
		{
			if(pclsService->findval("trace") == true)
			{
				
			}
		}
		JsonObject * pclsTimer = clsParseTest2.find("timer");
		if(pclsTimer)
		{
			if(pclsTimer->findval("garbage") == 60000)
			{
				
			}
			
		}
	}
	else
	{
		
	}
	//==========> Json Build Test
	JsonK clsBuildTest;
	JsonObject *  pclsBuild = clsBuildTest.add(json_type_object,"key1",KNULL);
	if(pclsBuild)
	{
		JsonObject * pclsArr = pclsBuild->add(json_type_array,"arr", KNULL);
		if(pclsArr)
		{
			pclsArr->add(json_type_array_item, KNULL,"arr_0");
			pclsArr->add(json_type_array_item, KNULL,"arr_1");
			pclsArr->add(json_type_array_item, KNULL,"arr_2");
		}
		pclsBuild->add(json_type_string,"sub_name1","name_val1");
		pclsBuild->add(json_type_string,"sub_name2","name_val2");
	}
	JsonObject * pclsOther = clsBuildTest.add(json_type_array,"subArr",KNULL);
	if(pclsOther)
	{
		JsonObject * pclsSubItem = pclsOther->add(json_type_object,"subArr_key1",KNULL);
		if(pclsSubItem)
		{
			pclsSubItem->add(json_type_string,"subkey1","sub_val1");
			pclsSubItem->add(json_type_string,"subkey2","sub_val2");
		}
		pclsOther->add(json_type_array_item,KNULL, "EleVal1");
		pclsOther->add(json_type_array_item,KNULL, "EleVal2");
		JsonObject * pclsDupDup = pclsOther->add(json_type_object,"dupdup",KNULL);
		if(pclsDupDup)
		{
			JsonObject * pclsDupArr = pclsDupDup->add(json_type_array,"duparr",KNULL);
			if(pclsDupArr)
			{
				pclsDupArr->add(json_type_array_item,KNULL,"dup_val1");
				pclsDupArr->add(json_type_array_item,KNULL,"dup_val2");
				pclsDupArr->add(json_type_array_item,KNULL,"dup_val3");
			}
		}
	}
	
}
/*********************************** Json Object ****************************************/
JsonObject::JsonObject()
{
	m_eType = json_type_null;m_pclsOwner=NULL;
}
JsonObject::~JsonObject()
{
	m_pclsOwner=NULL;
}
JsonObject & JsonObject::operator=(JsonObject & _rclsSrc)
{
	JsonObject * pOwner = m_pclsOwner;
	clear();
	setkey(_rclsSrc.m_uiKey);
	setkey(_rclsSrc.m_pszKey);
	m_eType = _rclsSrc.m_eType;
	m_clsJKey = _rclsSrc.m_clsJKey;
	m_clsJData = _rclsSrc.m_clsJData;
	m_pclsOwner = pOwner;
	ListItr_t stItor;
	JsonObject * pclsFind = _rclsSrc.begin(stItor);
	JsonObject * pclsNew = NULL;
	while(pclsFind)
	{
		pclsNew = new JsonObject;
		*pclsNew = *pclsFind;
		pclsNew->m_pclsOwner = this;
		m_listJson.pushback(pclsNew);
		pclsFind = _rclsSrc.next(stItor);
	}
	return *this;
}
JsonObject * JsonObject::find(const char * _pszKey)
{
	return (JsonObject*)m_listJson.find(_pszKey);
}
bool JsonObject::del(const char * _pszKey)
{
	return m_listJson.del(_pszKey);
}
JsonObject * JsonObject::begin(ListItr_t & _rstItor)
{
	return (JsonObject*)m_listJson.begin(_rstItor);
}
JsonObject * JsonObject::next(ListItr_t & _rstItor)
{
	return (JsonObject*)m_listJson.next(_rstItor);
}
unsigned int JsonObject::num()
{
	return m_listJson.size();
}
void JsonObject::clear()
{
	if(m_pszKey) delete [] m_pszKey;
	m_pszKey = NULL;
	m_uiKey = 0;
	m_eType  = json_type_null;
	m_clsJKey = KNULL;
	m_clsJData = KNULL;
	m_pclsOwner = NULL;
	m_listJson.clear();
}
#define def_json_escape
void JsonObject::build(KSTRING & _rclsBuild, unsigned int _unDepth)
{
	if(m_eType == json_type_boolean || m_eType == json_type_double
		|| m_eType == json_type_int || m_eType == json_type_string)
	{
		if(m_clsJKey.m_unRealLen > 0 && m_clsJData.m_unRealLen > 0)
		{
			catdepth(_rclsBuild,_unDepth);
			#ifdef def_json_escape
			KSTRING escaped;
			KSTRING::m_fnEscapeToJson((const char *)m_clsJData,escaped);
			_rclsBuild<<"\""<<m_clsJKey<<"\": \""<<escaped<<"\"";
			#else
			_rclsBuild<<"\""<<m_clsJKey<<"\": \""<<m_clsJData<<"\"";
			#endif
		}
	}
	else if(m_eType == json_type_object)
	{
		if(!(m_pclsOwner && m_pclsOwner->m_eType == json_type_array))
		{
			catdepth(_rclsBuild,_unDepth);
			_rclsBuild<<"\""<<m_clsJKey<<"\":\r\n";
		}
		catdepth(_rclsBuild,_unDepth);
		_rclsBuild<<"{\r\n";
		ListItr_t stItor; JsonObject * pclsFind = (JsonObject*)m_listJson.begin(stItor);
		while(pclsFind)
		{
			pclsFind->build(_rclsBuild,_unDepth+1);
			pclsFind = (JsonObject*)m_listJson.next(stItor);
			if(pclsFind)
				_rclsBuild<<",\r\n";
			else
				_rclsBuild<<"\r\n";
		}
		catdepth(_rclsBuild,_unDepth);
		_rclsBuild<<"}";
	}
	else if(m_eType == json_type_array)
	{
		if(!(m_pclsOwner && m_pclsOwner->m_eType == json_type_array))
		{
			catdepth(_rclsBuild,_unDepth);
			_rclsBuild<<"\""<<m_clsJKey<<"\":\r\n";
		}
		catdepth(_rclsBuild,_unDepth);
		_rclsBuild<<"[\r\n";
		ListItr_t stItor; JsonObject * pclsFind = (JsonObject*)m_listJson.begin(stItor);
		while(pclsFind)
		{
			pclsFind->build(_rclsBuild,_unDepth+1);
			pclsFind = (JsonObject*)m_listJson.next(stItor);
			if(pclsFind)
				_rclsBuild<<",\r\n";
			else
				_rclsBuild<<"\r\n";
		}
		catdepth(_rclsBuild,_unDepth);
		_rclsBuild<<"]";
	}
	else if(m_eType == json_type_array_item)
	{
		catdepth(_rclsBuild,_unDepth);
		#ifdef def_json_escape
		KSTRING escaped;
		KSTRING::m_fnEscapeToJson((const char *)m_clsJData,escaped);
		_rclsBuild<<"\""<<escaped<<"\"";
		#else
		_rclsBuild<<"\""<<m_clsJData<<"\"";
		#endif
	}
}
KSTRING & JsonObject::findval(const char * _pszKey)
{
	JsonObject * pclsFind = find(_pszKey);
	if(pclsFind == NULL) return m_clsNull;
	return pclsFind->m_clsJData;
}
JsonObject *JsonObject::add(json_type _eT,const char *_pszKey, KSTRING _clsVal)
{
	JsonObject * pclsAdd = addinternal(_eT,_pszKey);
	if(pclsAdd) pclsAdd->m_clsJData = _clsVal;
	return pclsAdd;
}
JsonObject *JsonObject::addinternal(json_type _eT,const char *_pszKey)
{
	if(!(m_eType == json_type_object || m_eType == json_type_array))
	{
		return NULL;
	}
	if(m_eType == json_type_array)
	{
		if(!(_eT == json_type_object ||	_eT == json_type_array_item))
		{
			return NULL;
		}
	}
	JsonObject *pclsObj = new JsonObject;pclsObj->setkey(_pszKey);
	pclsObj->m_eType = _eT;pclsObj->m_clsJKey = _pszKey;
	pclsObj->m_pclsOwner = this;
	if(m_listJson.pushback(pclsObj) == false)
	{
		delete pclsObj;return NULL;
	}
	return pclsObj;
}
void JsonObject::catdepth(KSTRING &_rclsBuff,unsigned int _unDepth)
{
	for(unsigned int i=0;i<_unDepth;i++)  _rclsBuff<<'\t';
}
/******************************** JSON ************************************************/
JsonK::JsonK()
{
	m_pclsCurObj = NULL;
}
JsonK::~JsonK()
{
}
JsonK & JsonK::operator=(JsonK & _rclsSrc)
{
	m_clsRaw = _rclsSrc.m_clsRaw;
	m_listJson.clear();
	ListItr_t stItor;
	JsonObject * pclsFind = _rclsSrc.begin(stItor);
	JsonObject * pclsNew = NULL;
	while(pclsFind)
	{
		pclsNew = new JsonObject;
		*pclsNew = *pclsFind;
		m_listJson.pushback(pclsNew);
		pclsFind = _rclsSrc.next(stItor);
	}
	return *this;
}
bool JsonK::parse(const char *_pszRaw)
{
	m_clsRaw = _pszRaw;
	json_object *new_obj = json_tokener_parse(_pszRaw);
	if (is_error(new_obj))
	{
		return false;
	}
	m_pclsCurObj = NULL; m_listJson.clear();
	bool bErr = false;
	extract(new_obj, 0,&bErr);
	if(bErr)
	{
		printf("error parsing json: eSipUtil Detected.\n");
		json_object_put(new_obj);
		return false;
	}
	json_object_put(new_obj);
	return true;
}
KSTRING & JsonK::findval(const char * _pszKey)
{
	JsonObject * pclsFind = find(_pszKey);
	if(pclsFind == NULL) return m_clsNull;
	return pclsFind->m_clsJData;
}
void JsonK::clear()
{
	m_listJson.clear();
}
JsonObject *JsonK::add(json_type _eT,const char * _pszKey,KSTRING _clsVal)
{
	JsonObject * pclsAdd = addinternal(_eT,_pszKey);
	if(pclsAdd) pclsAdd->m_clsJData = _clsVal;
	return pclsAdd;
}
JsonObject *JsonK::addinternal(json_type _eT,const char *_pszKey)
{
	if(_eT == json_type_array_item) return NULL;
	JsonObject *pclsObj = new JsonObject;pclsObj->setkey(_pszKey);
	pclsObj->m_eType = _eT; pclsObj->m_clsJKey = _pszKey;
	if(m_listJson.pushback(pclsObj) == false)
	{
		delete pclsObj;return NULL;
	}
	return pclsObj;
}
JsonObject * JsonK::find(const char * _pszKey)
{
	return (JsonObject*)m_listJson.find(_pszKey);
}
bool JsonK::del(const char * _pszKey)
{
	return m_listJson.del(_pszKey);
}
JsonObject * JsonK::begin(ListItr_t & _rstItor)
{
	return (JsonObject*)m_listJson.begin(_rstItor);
}
JsonObject * JsonK::next(ListItr_t & _rstItor)
{
	return (JsonObject*)m_listJson.next(_rstItor);
}
unsigned int JsonK::num()
{
	return m_listJson.size();
}
char * JsonK::build()
{
	m_clsRaw = KNULL; m_clsRaw.m_fnReSize(DEF_CFG_1M);
	ListItr_t stItor;JsonObject * pclsFind = (JsonObject*)m_listJson.begin(stItor);
	m_clsRaw<<"{\r\n";
	while(pclsFind)
	{
		pclsFind->build(m_clsRaw,1);
		pclsFind = (JsonObject*)m_listJson.next(stItor);
		if(pclsFind)
				m_clsRaw<<",\r\n";
			else
				m_clsRaw<<"\r\n";
	}
	m_clsRaw<<"\r\n}\r\n";
	return (KSTR)m_clsRaw;
}
char * JsonK::build(KSTRING & _szBuild)
{
	ListItr_t stItor;JsonObject * pclsFind = (JsonObject*)m_listJson.begin(stItor);
	_szBuild<<"{\r\n";
	while(pclsFind)
	{
		pclsFind->build(_szBuild,1);
		pclsFind = (JsonObject*)m_listJson.next(stItor);
		if(pclsFind)
			_szBuild<<",\r\n";
		else
			_szBuild<<"\r\n";
	}
	_szBuild<<"}\r\n";
	return (KSTR)_szBuild;
}
void JsonK::extract(json_object *obj, unsigned int _unDepth,bool * _pbError)
{
	int arr_cnt = 0; json_type mtype; json_object *temp_obj = NULL;
	JsonObject *pTmp_Json = m_pclsCurObj;
	JsonObject *pTmp_Json2 = NULL;
	JsonObject *pTmp_Json3 = NULL;
	if(_unDepth == 0 && json_object_get_type(obj) == json_type_array)
	{
		int rootArrCnt = json_object_array_length(obj);
		JsonObject * pRootArr = add(json_type_array, "결과", "Json_Arr");
		m_pclsCurObj = pRootArr;
		for(int ri = 0; ri < rootArrCnt; ri++)
		{
			json_object * rootElem = json_object_array_get_idx(obj, ri);
			json_type rootElemType = json_object_get_type(rootElem);
			if(rootElemType == json_type_array)
			{
				if(_pbError) *_pbError = true;
				return;
			}
			else if(rootElemType == json_type_object)
			{
				JsonObject * pRootItem = pRootArr->add(json_type_object, "결과", "Json_Obj");
				m_pclsCurObj = pRootItem;
				extract(rootElem, _unDepth + 1, _pbError);
				m_pclsCurObj = pRootArr;
			}
			else
			{
				KSTRING clsRootVal;
				switch(rootElemType)
				{
				case json_type_boolean: clsRootVal = json_object_get_boolean(rootElem); break;
				case json_type_double:  clsRootVal = json_object_get_double(rootElem); break;
				case json_type_int:     clsRootVal = json_object_get_int(rootElem); break;
				case json_type_string:  clsRootVal = json_object_get_string(rootElem); break;
				default: clsRootVal = "null"; break;
				}
				pRootArr->add(json_type_array_item, "Json_Arr_Item", (KCSTR)clsRootVal);
			}
		}
		m_pclsCurObj = NULL;
		return;
	}
	if(_unDepth == 0)
	{
		lh_table * ptable = json_object_get_object(obj);
		if(ptable==NULL)
		{
			if(_pbError) * _pbError = true;
			return;
		}
	}
	json_object_object_foreach(obj, key, val)
	{
		KSTRING clsVal;
		if(val == NULL)
		{
			mtype = json_type_null;
			clsVal = "null";
		}
		else
		{
			mtype = json_object_get_type(val);
		}
		switch (mtype)
		{
		case json_type_null:
			if (_unDepth == 0) add(mtype, key, (KCSTR)clsVal);
			else m_pclsCurObj->add(mtype, key, (KCSTR)clsVal);
			break;
		case json_type_boolean:
			clsVal = json_object_get_boolean(val);
			if (_unDepth == 0) add(mtype, key, (KCSTR)clsVal);
			else m_pclsCurObj->add(mtype, key, (KCSTR)clsVal);
			break;
		case json_type_double:
			clsVal = json_object_get_double(val);
			if (_unDepth == 0) add(mtype, key, (KCSTR)clsVal);
			else m_pclsCurObj->add(mtype, key, (KCSTR)clsVal);
			break;
		case json_type_int:
			clsVal = json_object_get_int(val);
			if (_unDepth == 0) add(mtype, key, (KCSTR)clsVal);
			else m_pclsCurObj->add(mtype, key, (KCSTR)clsVal);
			break;
		case json_type_string:
			clsVal = json_object_get_string(val);
			if (_unDepth == 0) add(mtype, key, (KCSTR)clsVal);
			else m_pclsCurObj->add(mtype, key, (KCSTR)clsVal);
			break;
		case json_type_object:
			if (m_pclsCurObj == NULL)
			{
				m_pclsCurObj = add(json_type_object, key, "Json_Obj");
				extract(val, _unDepth + 1,_pbError);
				m_pclsCurObj = NULL;
			}
			else
			{
				if (_unDepth == 0)
				{
					m_pclsCurObj = add(json_type_object, key, "Json_Obj");
					pTmp_Json = m_pclsCurObj;
				}
				else
					pTmp_Json = m_pclsCurObj->add(json_type_object, key, "Json_Obj");
				pTmp_Json2 = m_pclsCurObj; m_pclsCurObj = pTmp_Json;
				extract(val, _unDepth + 1,_pbError);	m_pclsCurObj = pTmp_Json2;
			}
			break;
		case json_type_array:
			arr_cnt = json_object_array_length(val);
			if (_unDepth == 0)
			{
				m_pclsCurObj = add(json_type_array, key, "Json_Arr");
				pTmp_Json = m_pclsCurObj;
			}
			else
				pTmp_Json = m_pclsCurObj->add(json_type_array, key, "Json_Arr");
			pTmp_Json2 = m_pclsCurObj;
			for (int i = 0; i<arr_cnt; i++)
			{
				temp_obj = json_object_array_get_idx(val, i);
				mtype = json_object_get_type(temp_obj);
				if(mtype == json_type_array)
				{
					if(_pbError) *_pbError = true;
					return;
				}
				else if (mtype == json_type_object)
				{
					pTmp_Json3 = pTmp_Json->add(json_type_object, key, "Json_Obj");
					m_pclsCurObj = pTmp_Json3;
					extract(temp_obj, _unDepth + 1,_pbError);
					m_pclsCurObj = pTmp_Json;
				}
				else
				{
					KSTRING clsArrVal;
					switch (mtype)
					{
					case json_type_boolean:
						clsArrVal = json_object_get_boolean(temp_obj);
						pTmp_Json->add(json_type_array_item, "Json_Arr_Item", (KCSTR)clsArrVal);
						break;
					case json_type_double:
						clsArrVal = json_object_get_double(temp_obj);
						pTmp_Json->add(json_type_array_item, "Json_Arr_Item", (KCSTR)clsArrVal);
						break;
					case json_type_int:
						clsArrVal = json_object_get_int(temp_obj);
						pTmp_Json->add(json_type_array_item, "Json_Arr_Item", (KCSTR)clsArrVal);
						break;
					case json_type_string:
						pTmp_Json->add(json_type_array_item, "Json_Arr_Item",
							json_object_get_string(temp_obj));
					default:
						break;
					};
				}
			}
			m_pclsCurObj = pTmp_Json2;
			break;
		default:
			break;
		};
	}
}
 const char * JsonK::stringtype(json_type _eT)
{
	switch(_eT)
	{
		case json_type_null: return "json_type_null";
		case json_type_boolean: return "json_type_boolean";
		case json_type_double: return "json_type_double";
		case json_type_int: return "json_type_int";
		case json_type_object: return "json_type_object";
		case json_type_array: return "json_type_array";
		case json_type_string: return "json_type_string";
		case json_type_array_item: return "json_type_array_item";
		
	}
	return "UnKnown";
}
 void JsonK::builditem(KSTRING &_rclsBuild,const char * _pszKey,KSTRING _clsVal)
{
	if(KSTRING::m_fnStrLen(_pszKey) > 0 && _clsVal.m_unRealLen > 0)
	{
		_rclsBuild<<"\""<<_pszKey<<"\":\""<<_clsVal<<"\"";
	}
	else if(KSTRING::m_fnStrLen(_pszKey) ==0 && _clsVal.m_unRealLen > 0)
	{
		_rclsBuild<<"\""<<_clsVal<<"\"";
	}
}
 void JsonK::catdepth(KSTRING &_rclsBuff,unsigned int _unDepth)
{
	for(unsigned int i=0;i<_unDepth;i++)  _rclsBuff<<'\t';
}
}
