#include "EXEPARAM.h"
#include "EXECORE.h"
#include "EXEACTION.h"
#include "BASICPARSER.h"
#include "FLOW.h"

namespace nsUtil
{
static void s_fnMoveTopKey(RestMsg & _msg, KCSTR _kor, KCSTR _eng)
{
	RestParam & src = _msg.GET(_kor);
	if(src.EMPTY()) return;
	if (src.NUMS() > 0)
	{
		RestParam & dst = _msg.SET(_eng);
		dst = src;
		dst.setkey(_eng);
		dst.KEY() = _eng;
	}
	else
	{
		_msg.SET(_eng).VAL() = src.VAL();
	}
	_msg.DEL(_kor);
}
static void s_fnTranslateSndMsgKorToEng(RestMsg & _msg)
{
	s_fnMoveTopKey(_msg, DEF_DSL_K_ADDR_kor,     DEF_DSL_K_ADDR_eng);
	s_fnMoveTopKey(_msg, DEF_DSL_K_TYPE_kor,     DEF_ACTION_EVENT);
	s_fnMoveTopKey(_msg, DEF_DSL_K_SVC_TYPE_kor, DEF_DSL_K_SVC_TYPE_eng);
	s_fnMoveTopKey(_msg, DEF_DSL_K_TIME_kor,     DEF_DSL_K_TIME_eng);
	s_fnMoveTopKey(_msg, DEF_DSL_K_LIST_kor,     DEF_DSL_K_LIST_eng);
	s_fnMoveTopKey(_msg, DEF_DSL_K_METHOD_kor,     DEF_DSL_K_METHOD_eng);
}
void CHECK(RestMsg & _msg)
{
	if(_msg.GET(DEF_SCE_ID).EMPTY()) _msg.DEL(DEF_SCE_ID);
	if(_msg.GET(DEF_AS_ID).EMPTY()) _msg.DEL(DEF_AS_ID);
	if(_msg.GET(DEF_ACTION_EVENT).EMPTY()) _msg.DEL(DEF_ACTION_EVENT);
	if(_msg.GET(DEF_SCE_SVC_TYPE).EMPTY()) _msg.DEL(DEF_SCE_SVC_TYPE);
	RestParam & rParams = _msg.GET(DEF_PARAMS);
	if(!rParams.EMPTY())
	{
		unsigned int i = 0; KSTRING szTmp;
		Jump_ReDelete:
		for(i=0;i<rParams.NUMS();i++)
	{
		if(rParams[i].EMPTY())
		{
			szTmp = rParams[i].KEY();
			rParams.DEL((KCSTR)szTmp);
			goto Jump_ReDelete;
		}
	}

	}
}
ExeAction::ExeAction(ActionParser & _dsl)
{
	m_dsl = &_dsl;
}
ExeAction::~ExeAction()
{
}
void ExeAction::SEND(QTHREAD & _wk,
					POOL::POOLDATA & _rPool,
					RestMsg & _msg)
{
	s_fnTranslateSndMsgKorToEng(_msg);
	Flow::OBJ().NOTIFY(_wk, _rPool, _msg);
}
bool ExeAction::EXE_SEND(QTHREAD & _wk,
					POOL::POOLDATA & _rPool,
					RestMsg & _req)
{
	ActionParser & actP = *m_dsl;
	RestMsg msg;
	if(_rPool.GET(DEF_SCE_ID).VAL() == "NonProto")
	{
	}
	else
	{
		msg.SET(DEF_SCE_EVENT).VAL() = DEF_SCE_ACTION;
		msg.SET(DEF_ACTION_EVENT).VAL() = "user-defined";
		msg.SET(DEF_SCE_ID).VAL() = (KCSTR)_rPool.GET(DEF_SCE_ID).VAL();
		msg.SET(DEF_AS_ID).VAL() = (KCSTR)_rPool.GET(DEF_AS_ID).VAL();
	}
	for(KUINT i=0;i<actP.NUMS();i++)
	{
		AssignParser & line = actP[i];
		KSTRING & a = line.m_AB.KEY();
		KSTRING & b = line.m_AB.VAL();
		KSTRING & c = line.m_CD.KEY();
		KSTRING & d = line.m_CD.VAL();
		if(IS_DSL_K_SND_MSG((KCSTR)a))
		{
			TOKSTR tok(d); tok.TOK(".");
			if(IS_DSL_K_TYPE((KCSTR)b))
			{
				msg.SET(DEF_ACTION_EVENT).VAL() = c;
			}
			else if(IS_DSL_K_ACTION_EVENT((KCSTR)b))
			{
				msg.SET(DEF_ACTION_EVENT).VAL() = c;
			}
			else if((IS_DSL_K_SESSION((KCSTR)c)
				&& (IS_DSL_K_OBJ((KCSTR)tok[0]) || IS_DSL_K_ADDR((KCSTR)tok[0])))
				|| (IS_DSL_K_RCV_MSG((KCSTR)c) && IS_DSL_K_ADDR((KCSTR)d)))
			{
				RestParam & sndParam = msg.SETR((KCSTR)b);
				if (IS_DSL_K_RCV_MSG((KCSTR)c))
				{
					RestParam & srcParam = _req.GETR((KCSTR)d);
					KSTRING sndKey = sndParam.KEY();
					sndParam = srcParam;
					sndParam.setkey((KCSTR)sndKey);
					sndParam.KEY() = sndKey;
				}
				else
				{
					ExeCore::Session *pSes = (ExeCore::Session*)_rPool.GETU();
					if(pSes)
					{
						KSTRING tmp;
						for(KUINT i=1;i<tok.NUMS();i++)
						{
							if(i == (tok.NUMS()-1))
								tmp.PRINT("%s",(KCSTR)tok[i]);
							else
								tmp.PRINT("%s.",(KCSTR)tok[i]);
						}
						RestParam & srcParam = pSes->m_subObj.GETR((KCSTR)tmp);
						KSTRING sndKey = sndParam.KEY();
						sndParam = srcParam;
						sndParam.setkey((KCSTR)sndKey);
						sndParam.KEY() = sndKey;
					}
				}
			}
			else
			{
				KSTRING &val = (KSTRING &)msg.SETR(b).VAL();
				KSTRING lval;
				val = ExeParam::PARAM(c, d, _rPool,_req, lval);
			}
		}
	}
	SEND(_wk,_rPool,msg);
	return true;
}
bool ExeAction::EXE_TIMER(QTHREAD & _wk,
					POOL::POOLDATA & _rPool,
					RestMsg & _req)
{
	ActionParser & actP = *m_dsl;
	ARG arg;
	arg.SET(API_P_TIMER_TYPE).VAL() = "SCE_TIMER";
	arg.SET(DEF_SCE_ID).VAL() = _rPool.GET(DEF_SCE_ID).VAL();
	arg.SET("UNIQ_ID").VAL() = _rPool.GET("UNIQ_ID").VAL();
	if(_rPool.GET(DEF_SCE_ID).VAL() != "NonProto")
	{
		arg.SET(DEF_AS_ID).VAL() = _rPool.GET(DEF_AS_ID).VAL();
		arg.SET(API_P_POOL_UNIQ).VAL() = _rPool.GET(API_P_POOL_UNIQ).VAL();
	}
	RestMsg msg;
	KSTRING domain_name = actP.m_oSpace;
	RestParam & addr = msg.SET(DEF_DSL_K_ADDR_kor);
	KSTRING & domain_addr = addr.SET(DEF_DSL_K_DOMAIN_kor).VAL();
	ExeCore::OBJ().GETINI((KCSTR)domain_name,"domain",domain_addr);
	if(_rPool.GET(DEF_SCE_ID).VAL() == "NonProto")
	{
		msg.SET(DEF_SCE_ID).VAL() = (KCSTR)_rPool.GET(DEF_SCE_ID).VAL();
	}
	else
	{
		msg.SET(DEF_SCE_EVENT).VAL() = DEF_SCE_ACTION;
		msg.SET(DEF_SCE_ID).VAL() = (KCSTR)_rPool.GET(DEF_SCE_ID).VAL();
		msg.SET(DEF_AS_ID).VAL() = (KCSTR)_rPool.GET(DEF_AS_ID).VAL();
	}
	for(KUINT i=0;i<actP.NUMS();i++)
	{
		AssignParser & line = actP[i];
		KSTRING & a = line.m_AB.KEY();
		KSTRING & b = line.m_AB.VAL();
		KSTRING & c = line.m_CD.KEY();
		KSTRING & d = line.m_CD.VAL();
		if(IS_DSL_K_SND_MSG((KCSTR)a))
		{
			if(IS_DSL_K_TYPE((KCSTR)b))
			{
				msg.SET(DEF_ACTION_EVENT).VAL() = c;
			}
			else if(IS_DSL_K_TIME((KCSTR)b))
			{
				arg.SET(API_P_TIME).VAL()= c;
			}
			else
			{
				KSTRING &val = (KSTRING &)msg.SETR(b).VAL();
				KSTRING lval;
				val = ExeParam::PARAM(c, d, _rPool,_req, lval);
			}
		}
	}
	KSTRING & pk = arg.SET(API_P_PACKET).VAL();
	pk = msg.STR();
	_wk.SETTIMER(arg);
	return true;
}
bool ExeAction::EXE(QTHREAD & _wk,
					POOL::POOLDATA & _rPool,
					RestMsg & _req)
{
	if(IS_DSL_CLASS_ACT_SEND((KCSTR)m_dsl->m_oType))
	{
		return EXE_SEND(_wk,_rPool,_req);
	}
	else if(IS_DSL_CLASS_ACT_TIMER((KCSTR)m_dsl->m_oType))
	{
		return EXE_TIMER(_wk,_rPool,_req);
	}
	return false;
}
}
