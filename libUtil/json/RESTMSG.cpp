#include "RESTMSG.h"

namespace nsUtil
{
JsonObject g_clsExpParam;
unsigned int g_unInternalCnt = 0;
void g_TestJsonMsg()
{
	RestMsg orgMsg;
	orgMsg.SET("event").VAL() = "start_req";

	orgMsg.SET("session").SET("id").VAL() = 5001;
	orgMsg.SET("session").SET("caller").VAL() = "01012345678";
	orgMsg.SET("session").SET("called").VAL() = "01098765432";

	orgMsg.SET("body").SET("content-type").VAL() = "sdp";
	orgMsg.SET("body").SET("sdp").VAL() = "o~v-s-c-t-....~~~~";

	orgMsg.SET("ip_list").ARR();
	orgMsg.GET("ip_list")[0].SET("ip").VAL() = "1.1.1.1";
	orgMsg.GET("ip_list")[0].SET("port").VAL() = "10000";
	
	orgMsg.SET("ip_list").ARR();
	orgMsg.GET("ip_list")[1].SET("ip").VAL() = "2.2.2.2";
	orgMsg.SET("ip_list")[1].SET("port").VAL() = "20000";

	orgMsg.SET("black_list").ARR().VAL() = "0000";
	orgMsg.SET("black_list").ARR().VAL() = "9999";

	RestParam cpParam; KSTRING szTmp;
	cpParam = orgMsg.GET("ip_list");
	printf("Copy Param Test\n%s\r\n",(KCSTR)cpParam.STR(szTmp));

	printf("*** Build Msg Get Api Test\r\n");
	printf("session.caller = %s\r\n",(KCSTR)orgMsg.GET("session").GET("caller").VAL());  
	printf("ip_list[1].ip = %s, ip_list[1].port=%d\r\n",(KCSTR)orgMsg.GET("ip_list")[1].GET("ip").VAL(),
											(KUINT)orgMsg.GET("ip_list")[1].GET("port").VAL());
	printf("black_list[0] = %s\r\n",(KSTR)orgMsg.GET("black_list")[0].VAL());

	char * szEncoding = orgMsg.STR();
	printf("Encoding\n%s",szEncoding);

	RestMsg rcvMsg; rcvMsg.PARSE(szEncoding);
	char * szDecoding = rcvMsg.STR();
	printf("Decoding\n%s",szDecoding);

	printf("*** Parsing Msg Get Api Test");
	printf("session.caller = %s\r\n",(KCSTR)rcvMsg.GET("session").GET("caller").VAL());  
	printf("ip_list[1].ip = %s, ip_list[1].port=%d\r\n",(KCSTR)rcvMsg.GET("ip_list")[1].GET("ip").VAL(),
											(KUINT)rcvMsg.GET("ip_list")[1].GET("port").VAL());
	printf("black_list[0] = %s\r\n",(KSTR)rcvMsg.GET("black_list")[0].VAL());

	RestMsg cpMsg; cpMsg = rcvMsg;
	printf("Copy Msg Test\n%s\r\n",cpMsg.STR());
}
// >>>>>>>>>>>>>>>> Recursive Null Check
static bool isnullobj(RestParam & _obj)
{
	if ((_obj.NUMS() == 0 && _obj.m_clsJData.LENGTH() == 0) ||
		(_obj.m_eType == json_type_null))
		return true;
	return false;
}
static void delobj(RestMsg & _msg, RestParam & _obj)
{
	if (_obj.m_pclsOwner)
	{
		RestParam * pOw = (RestParam*)_obj.m_pclsOwner;
		pOw->DEL((KCSTR)_obj.KEY());
	}
	else
	{
		_msg.DEL((KCSTR)_obj.KEY());
	}
}
static void delobjR(RestMsg & _msg, RestParam & _obj, bool & _bdel)
{
	if (isnullobj(_obj))
	{
		delobj(_msg, _obj);
		_bdel = true;
		return;
	}
	for (KUINT i = 0; i < _obj.NUMS(); ++i)
	{
		RestParam & child = _obj[i];
		delobjR(_msg, child, _bdel);
		if (_bdel)
			return;
	}
}
static void checkJson(RestMsg & _msg)
{
	bool bdel = false;
	jumpRetry:
	for (KUINT i = 0; i < _msg.NUMS(); ++i)
	{
		RestParam & item = _msg[i];
		delobjR(_msg, item, bdel);
		if (bdel)
		{
			bdel = false;
			goto jumpRetry;
		}
	}
}
void g_testNullJson()
{
	RestMsg msg;
	msg.SET("a").VAL() = 1234;
	msg.SET("b").VAL() = KNULL;
	msg.SET("obj");

	msg.SET("obj5").SET("obj6").SET("obj7").SET("e").VAL() = KNULL;
	msg.SET("obj5").SET("obj6").SET("obj7").SET("f").VAL() = KNULL;
	msg.SET("obj5").SET("obj6").SET("obj7").SET("g").VAL() = KNULL;
	msg.SET("obj5").SET("obj6").SET("obj7").SET("h").VAL() = KNULL;
	msg.SET("obj5").SET("obj6").SET("obj7").SET("j").VAL() = KNULL;
	
	msg.SET("obj2").SET("val1").VAL() = 2222;
	msg.SET("obj2").SET("val2").VAL() = 3333;
	msg.SET("obj2").SET("obj3").SET("c").VAL() = 555;
	msg.SET("obj2").SET("obj3").SET("d").VAL() = 5556;
	msg.SET("obj2").SET("obj3").SET("obj4").SET("e").VAL() = 5756;

	printf("before:\n%s\r\n", msg.STR());
	checkJson(msg);
	printf("after:\n%s\r\n", msg.STR());
}
/********************************** Restfull Param ****************************************/
RestParam::RestParam()
{
	m_eType = json_type_object;
}
RestParam::~RestParam()
{
}
RestParam & RestParam::operator=(RestParam & _rclsSrc)
{
	JsonObject & dst = (JsonObject&)(*this); JsonObject & src = (JsonObject&)_rclsSrc;
	dst = src;
	return (*this);
}
bool RestParam::EMPTY()
{
	if(m_clsJKey.m_unRealLen==0 && m_clsJData.m_unRealLen==0) return true;
	if(m_eType == json_type_object || m_eType == json_type_array)
	{
		if(NUMS() ==0) return true;
	}
	else
	{
		if(m_clsJData.m_unRealLen==0) return true;
	}
	return false;
}
KSTRING & RestParam::KEY()
{
	return m_clsJKey;
}
KSTRING & RestParam::VAL()
{
	if(m_eType != json_type_array_item)
		m_eType = json_type_string;
	return m_clsJData;
}
RestParam & RestParam::GET(const char * _pszParam)
{
	JsonObject * pclsFind = find(_pszParam);
	if(pclsFind==NULL)
	{
		return (RestParam&)g_clsExpParam;
	}
	return (RestParam&)*pclsFind;
}
RestParam & RestParam::SET(const char * _pszParam)
{
	if(m_eType == json_type_array_item)
	{
		m_eType = json_type_object;
	}
	JsonObject * pclsFind = find(_pszParam);
	if(pclsFind==NULL)
	{
		pclsFind = add(json_type_object, _pszParam,"");
	}
	return (RestParam&)*pclsFind;
}
RestParam & RestParam::SETR(TokStrings & _tok, KUINT &_cnt)
{
	if(_tok[_cnt]==NULL) return *this;
	m_eType = json_type_object;
	TokStrings bracketTok;
	KSTRING curTok(_tok[_cnt]);
	KSTRING::m_fnTokenizeC((KSTR)curTok, "[]", bracketTok);
	RestParam * pItem = NULL;
	if (bracketTok.num() == 2)
	{
		KUINT nIdx = KSTRING::m_fnAtoi(bracketTok[1]);
		RestParam * pArr = (RestParam*)find(bracketTok[0]);
		if (pArr == NULL)
		{
			pArr = &SET(bracketTok[0]);
			pArr->m_eType = json_type_array;
		}
		while (pArr->NUMS() <= nIdx)
		{
			pArr->ARR();
		}
		pItem = &(*pArr)[nIdx];
	}
	else
	{
		pItem = &SET(_tok[_cnt]);
	}
	_cnt++;
	return pItem->SETR(_tok,_cnt);
}
void RestParam::DEL(const char * _pszParam)
{
	del(_pszParam);
}
void RestParam::DEL(unsigned int _idx)
{
	if(m_eType == json_type_array)
	{
		m_listJson.delidx(_idx);
	}
}
RestParam & RestParam::ARR()
{
	m_eType = json_type_array;
	KSTRING clsTmp = (++g_unInternalCnt);
	JsonObject * pclsFind = add(json_type_array_item,(KCSTR)clsTmp,"");
	return (RestParam&)*pclsFind;
}
RestParam & RestParam::operator[](unsigned int _unIndex)
{
	JsonObject * pclsFind = (JsonObject*)m_listJson.index(_unIndex);
	if(pclsFind==NULL) return (RestParam&)g_clsExpParam;
	return (RestParam&)*pclsFind;
}
RestParam & RestParam::INDEX(unsigned int _unIndex)
{
	JsonObject * pclsFind = (JsonObject*)m_listJson.index(_unIndex);
	if(pclsFind==NULL) return (RestParam&)g_clsExpParam;
	return (RestParam&)*pclsFind;
}
unsigned int RestParam::NUMS()
{
	return m_listJson.size();
}	
char * RestParam::STR(KSTRING & _build)
{
	build(_build,0);
	return (KSTR)_build;
}
char * RestParam::BODY(KSTRING & _rclsBuild)
{
	unsigned int _unDepth = 0;
	if(m_eType == json_type_boolean || m_eType == json_type_double
		|| m_eType == json_type_int || m_eType == json_type_string)
	{
		if(m_clsJKey.m_unRealLen > 0 && m_clsJData.m_unRealLen > 0)
		{
			_rclsBuild<<"\""<<m_clsJData; _rclsBuild<<"\"";
		}
	}
	else if(m_eType == json_type_object)
	{
		if(m_listJson.size() == 0) return (KSTR)_rclsBuild;
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
		_rclsBuild<<"}";
	}
	else if(m_eType == json_type_array)
	{
		if(m_listJson.size() == 0) return (KSTR)_rclsBuild;
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
		_rclsBuild<<"]";
	}
	else if(m_eType == json_type_array_item)
	{
		_rclsBuild<<"\""<<m_clsJData; _rclsBuild<<"\"";
	}
	return (KSTR)_rclsBuild;
}
/********************************** Restfull Message **************************************/
static RestParam s_defJson;
static RestParam & s_fnGetNextJ(TokStrings & _rTok, RestMsg & _rMsg,
									RestParam * _pParam, unsigned int &_rCur);
static RestParam & s_fnGetNextArrJ(TokStrings & _rTok, RestMsg & _rMsg,
									RestParam * _pParam, unsigned int &_rCur)
{
	RestParam * pFind = NULL;
	TokStrings tmpTok; 
	KSTRING tmp((KSTR)_rTok[_rCur]);
	KSTRING::m_fnTokenizeC((KSTR)tmp,"[]", tmpTok);
	if(tmpTok.num()==2)
	{
		TokStrings tmpTok2;
		KSTRING::m_fnTokenizeC(tmpTok[1], " =", tmpTok2);
		if(tmpTok2.num()==2)  
		{
			for(KUINT i=0;i<_pParam->NUMS();i++)
			{
				RestParam & item = (*_pParam)[i];
				if(item.GET(tmpTok2[0]).VAL() == tmpTok2[1])
				{
					_rCur = _rCur +1;
					TokStrings tmpTok2; 
					KSTRING tmp2((KSTR)_rTok[_rCur]);
					KSTRING::m_fnTokenizeC((KSTR)tmp2,"[]", tmpTok2);
					pFind = (RestParam*)item.find(tmpTok2[0]);
					return s_fnGetNextJ(_rTok, _rMsg,pFind,_rCur);
				}
			}
			return s_defJson;
		}
		else  
		{
			KUINT nIdx = KSTRING::m_fnAtoi(tmpTok2[0]);
			if(_pParam->NUMS() <= nIdx) return s_defJson;
			RestParam & rParam2 = (*_pParam)[nIdx];
			if(_rCur + 1 >= _rTok.num())
			{
				return rParam2;
			}
			_rCur = _rCur +1;
			TokStrings tmpTok3;
			KSTRING tmp3((KSTR)_rTok[_rCur]);
			KSTRING::m_fnTokenizeC((KSTR)tmp3,"[]", tmpTok3);
			pFind = (RestParam*)rParam2.find(tmpTok3[0]);
			return s_fnGetNextJ(_rTok, _rMsg,pFind,_rCur);
		}
	}
	else  
	{
		_rCur = _rCur +1;
		if(_rCur == _rTok.num())
		{
			return (*_pParam);
		}
		else
		{
			TokStrings tmpTok4; 
			KSTRING tmp4((KSTR)_rTok[_rCur]);
			KSTRING::m_fnTokenizeC((KSTR)tmp4,"[]", tmpTok4);
			pFind = (RestParam*)_pParam->find(tmpTok4[0]);
			return s_fnGetNextJ(_rTok, _rMsg,pFind ,_rCur);
		}
	}
}
RestParam & s_fnGetNextJ(TokStrings & _rTok, RestMsg & _rMsg,
									RestParam * _pParam, unsigned int &_rCur)
{
	if(_pParam==NULL) return s_defJson;
	RestParam * pFind = NULL;
	TokStrings tmpTok; 
	KSTRING tmp(_rTok[_rCur]);
	KSTRING::m_fnTokenizeC((KSTR)tmp,"[]", tmpTok);
	if(_rCur==0)
	{
		if(tmpTok.num() ==2)
		{
			if(_rTok.num()==1)  
			{
				pFind = (RestParam*)_rMsg.find(tmpTok[0]);
				if(pFind==NULL) return s_defJson;
				KUINT nIdx = KSTRING::m_fnAtoi(tmpTok[1]);
				if(pFind->num() <= nIdx) return s_defJson;
				return (*pFind)[nIdx];
			}
			else
			{
				pFind = (RestParam*)_rMsg.find(tmpTok[0]);
				if(pFind==NULL) return s_defJson;
				return s_fnGetNextArrJ(_rTok, _rMsg,pFind ,_rCur);
			}
		}
		else
		{
			if(_rTok.num()==1)
			{
				return _rMsg.GET(tmpTok[0]);
			}
			else
			{
				pFind = (RestParam*)_rMsg.find(tmpTok[0]);
				if(pFind==NULL) return s_defJson;
				return s_fnGetNextArrJ(_rTok, _rMsg,pFind ,_rCur);
			}
		}
	}
	else if(_rCur == _rTok.num())
	{
		return (*_pParam);
	}
	else
	{
		return s_fnGetNextArrJ(_rTok,_rMsg,_pParam,_rCur);
	}
	return s_defJson;
}
static RestParam & s_fnGetJ(KSTRING _szKey, RestMsg & _rJmsg)
{
	unsigned int nCnt = 0;RestParam tmpParam;
	TokStrings tok;
	KSTRING::m_fnTokenizeC((KSTR)_szKey, " .", tok);
	return s_fnGetNextJ(tok,_rJmsg,&tmpParam, nCnt);
}

RestMsg::RestMsg(){}
RestMsg::~RestMsg(){}
RestMsg & RestMsg::operator=(RestMsg & _rclsSrc)
{
	JsonK & dst = (JsonK&)(*this); JsonK & src = (JsonK&)_rclsSrc;
	dst = src;
	return (*this);
}
bool RestMsg::PARSE(const char * _pszJson){return parse(_pszJson);}
RestParam & RestMsg::GET(const char * _pszParam)
{
	JsonObject * pclsFind = find(_pszParam);
	if(pclsFind==NULL)
	{
		return (RestParam&)g_clsExpParam;
	}
	return (RestParam&)*pclsFind;
}
RestParam & RestMsg::GETR(const char * _pszParam)
{
	return s_fnGetJ(_pszParam, *this);
}
RestParam & RestMsg::SET(const char * _pszParam)
{
	JsonObject * pclsFind = find(_pszParam);
	if(pclsFind==NULL)
	{
		pclsFind = add(json_type_object, _pszParam,"");
	}
	return (RestParam&)*pclsFind;
}
RestParam & RestMsg::SETR(const char * _pszParam)
{
	KSTRING tmp; tmp = _pszParam;
	TokStrings tok;KSTRING::m_fnTokenizeC((KSTR)tmp," .", tok);
	unsigned int nCnt=0;
	TokStrings bracketTok;
	KSTRING::m_fnTokenizeC((KSTR)tok[nCnt], "[]", bracketTok);
	RestParam * pItem = NULL;
	if(bracketTok.num()==2)
	{
		KUINT nIdx = KSTRING::m_fnAtoi(bracketTok[1]);
		RestParam * pArr = (RestParam*)find(bracketTok[0]);
		if (pArr == NULL)
		{
			pArr = &SET(bracketTok[0]);
			pArr->m_eType = json_type_array;
		}
		while (pArr->NUMS() <= nIdx)
		{
			pArr->ARR();
		}
		pItem = &(*pArr)[nIdx];
	}
	else
	{
		pItem = &SET(tok[nCnt]);
	}
	nCnt++;
	return pItem->SETR(tok,nCnt);
}
void RestMsg::DEL(const char * _pszParam)
{
	del(_pszParam);
}
RestParam & RestMsg::operator[](unsigned int _unIndex)
{
	JsonObject * pclsFind = (JsonObject*)m_listJson.index(_unIndex);
	if(pclsFind==NULL) return (RestParam&)g_clsExpParam;
	return (RestParam&)*pclsFind;
}
unsigned int RestMsg::NUMS()
{
	return num();
}
char * RestMsg::STR()
{
	checkJson(*this);
	m_szMsg = KNULL;
	m_szMsg.m_fnReSize(1024);
	return build(m_szMsg);
}
void RestMsg::TEST()
{
	g_TestJsonMsg();
}
void RestMsg::TESTR()
{
	// test
	/*
	{
	        "first": "hoho",
	        "aa":
	        {
	                "bb":
	                {
	                        "cc": "hoho"
	                }
	        },
	        "body":
	        {
	                "list":
	                [
	                        {
	                                "type": "a100",
	                                "content": "haha"
	                        },
	                        {
	                                "type": "b100",
	                                "content": "hoho"
	                        }
	                ]
	        },
	        "root-list":
	        [
	                {
	                        "type": "c100",
	                        "content": "haha"
	                },
	                {
	                        "type": "d100",
	                        "content": "hoho"
	                }
	        ],
	        "root-list2":
	        [
	                "haha",
	                "hoho"
	        ]

	}
	first                            : hoho
	aa.bb.cc                         : hoho
	body.list[type==b100].content    : hoho
	root-list[1].content             : hoho
	root-list2[1]                    : hoho
	*/
	RestMsg jMsg;
	jMsg.SET("first").VAL() = "hoho1";
	jMsg.SET("aa").SET("bb").SET("cc").VAL() = "hoho2";
	RestParam & body = jMsg.SET("body");
		RestParam & list = body.SET("list");
			RestParam & list0 = list.ARR();
				list0.SET("type").VAL() = "a100";
				list0.SET("content").VAL() = "haha";
			RestParam & list1 = list.ARR();
				list1.SET("type").VAL() = "b100";
				list1.SET("content").VAL() = "hoho3";
	RestParam & rootList = jMsg.SET("root-list");
		RestParam & rootList0 = rootList.ARR();
			rootList0.SET("type").VAL() = "c100";
			rootList0.SET("content").VAL() = "haha";
		RestParam & rootList1 = rootList.ARR();
			rootList1.SET("type").VAL() = "d100";
			rootList1.SET("content").VAL() = "hoho4";
	RestParam & rootList2 = jMsg.SET("root-list2");
		rootList2.ARR().VAL() = "haha";
		rootList2.ARR().VAL() = "hoho5";
	printf("%s\n",(KCSTR)jMsg.STR());
	printf("%-32s : %s\n","first",(KCSTR)jMsg.GETR("first").VAL());
	printf("%-32s : %s\n","aa.bb.cc",(KCSTR)jMsg.GETR("aa.bb.cc").VAL());
	printf("%-32s : %s\n","body.list[type==b100].content",
						(KCSTR)jMsg.GETR("body.list[type==b100].content").VAL());
	printf("%-32s : %s\n","root-list[1].content",
						(KCSTR)jMsg.GETR("root-list[1].content").VAL());
	printf("%-32s : %s\n","root-list2[1]",
						(KCSTR)jMsg.GETR("root-list2[1]").VAL());
}
}