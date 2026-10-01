#include "EXEPARAM.h"
#include "EXECORE.h"
#include "EXEIF.h"

namespace nsUtil
{
ExeIf::ExeIf(IfParser & _dsl)
{
	m_dsl = &_dsl;
}
ExeIf::~ExeIf()
{
}
bool ExeIf::EXE_IFProc(KSTRING & ifA,
			 			KSTRING & ifB,
			 			KSTRING & ifCond,
			 			KSTRING & ifC,
			 			KSTRING & ifD,
						QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, 
						RestMsg & _req,
						RestMsg * _snd)
{	
	KSTRING A; KSTRING B;
	KSTRING cmpLog;
	KSTRING lparam; KSTRING rparam;
	if(ifB.LENGTH()>0)
	{
		lparam.PRINT("%s.%s",(KCSTR)ifA,(KCSTR)ifB);
	}
	else
	{
		lparam.PRINT("%s",(KCSTR)ifA);
	}
	if(ifD.LENGTH()>0)
	{
		rparam.PRINT("%s.%s",(KCSTR)ifC,(KCSTR)ifD);
	}
	else
	{
		rparam.PRINT("%s",(KCSTR)ifC);
	}
	if(endsWithLength((KCSTR)ifB)==1)
	{
		KSTRING buf;
		A = ExeParam::PARAMLength(ifA,ifB, _rPool,_req,buf);
	}
	else
	{
		KSTRING buf;
		A = ExeParam::PARAM(ifA,ifB,_rPool,_req,buf);
	}
	if(endsWithLength((KCSTR)ifD)==1)
	{
		KSTRING buf;
		B = ExeParam::PARAMLength(ifC,ifD,_rPool,_req,buf);
	}
	else
	{
		KSTRING buf;
		B = ExeParam::PARAM(ifC,ifD,_rPool,_req,buf);
	}
	if(IS_DSL_K_TRUE((KCSTR)ifCond))
	{
		return true;
	}
	else if(IS_DSL_K_FALSE((KCSTR)ifCond)) 
	{
		return false;
	}
	else if(ifCond == "==")
	{
		if(ifC == "NULL" || ifC == "null")
		{
			if(A.LENGTH() == 0 )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		// claude-code 2026-10-01 (dorafather 승인): 다중값 구분자를 "/"에서
		// "^"로 교체. PROCPARSER/ACTIONPARSER/STATEPARSER가 파싱 시점에
		// 리터럴 "/"를 전부 "^^^"로 치환하는데(2부 2.11절, URL 경로 보존용)
		// 처리::의 만약에 조건(이 EXEIF.cpp 경로)은 그 치환을 실행 전에
		// 복원하지 않아, "/"로 다중값을 나누려 해도 이미 "^^^"로 바뀐
		// 뒤라 하나도 안 쪼개져 항상 매치에 실패했다(실측 확인, CLAUDE.md
		// 1.20절). "^"는 이 치환 대상이 아니라서 처리:: 조건에서도 안전하게
		// 구분자로 쓸 수 있다.
#if 1
		TOKSTR tok; tok = (KCSTR)B;tok.TOK("^");
#else
		TOKSTR tok; tok = (KCSTR)B;tok.TOK("/");
#endif
		for(KUINT i=0;i<tok.NUMS();i++)
		{
			if(A == tok[i])
			{
				return true;
			}
		}
		return false;
	}
	else if(ifCond == ">=")
	{
		if(A.LENGTH()==0 || B.LENGTH()==0)
		{
			return false;
		}
		if((KUINT)A >= (KUINT)B)
		{
			return true;
		}
		return false;
	}
	else if(ifCond == "<=")
	{
		if(A.LENGTH()==0 || B.LENGTH()==0)
		{
			return false;
		}
		if((KUINT)A <= (KUINT)B)
		{
			return true;
		}
		return false;
	}
	else if(ifCond == "<")
	{
		if(A.LENGTH()==0 || B.LENGTH()==0)
		{
			return false;
		}
		if((KUINT)A < (KUINT)B)
		{
			return true;
		}
		return false;
	}
	else if(ifCond == ">")
	{
		if(A.LENGTH()==0 || B.LENGTH()==0)
		{
			return false;
		}
		if((KUINT)A > (KUINT)B)
		{
			return true;
		}
		return false;
	}
	else if(ifCond == "!=")
	{
		if(ifC == "NULL" || ifC == "null")
		{
			if(A.LENGTH() != 0 )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		if(A 	!= B)
		{
			return true;
		}
		return false;
	}
	else if(ifCond == "!!!")
	{
		if(A.LENGTH()==0 || B.LENGTH()==0)
		{
			return false;
		}
#if 1
		TOKSTR tok; tok = (KCSTR)B; tok.TOK("^");  // claude-code 2026-10-01: 위 == 분기와 동일한 이유로 "/"→"^" 교체
#else
		TOKSTR tok; tok = (KCSTR)B; tok.TOK("/");
#endif
		for(KUINT i=0;i<tok.NUMS();i++)
		{
			if(STRSTR((KSTR)A,(KSTR)tok[i]))
			{
				return true;
			}
		}
		return false;
	}
	else if(ifCond == "===")
	{
		if(A.LENGTH()==0 || B.LENGTH()==0) 
		{
			return false;
		}
#if 1
		TOKSTR tok; tok = (KCSTR)B;tok.TOK("^");  // claude-code 2026-10-01: 위 == 분기와 동일한 이유로 "/"→"^" 교체
#else
		TOKSTR tok; tok = (KCSTR)B;tok.TOK("/");
#endif
		for(KUINT i=0;i<tok.NUMS();i++)
		{
			if(strncmp((KSTR)A,(KSTR)tok[i],tok[i].LENGTH())==0)
			{
				return true;
			}
		}
		return false;
	}
	return false;
}
bool ExeIf::EXE(QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _req)
{
	IfParser & ifP = *m_dsl;
	if(IS_DSL_K_TRUE((KCSTR)ifP.m_if_a)) return true;
	if(IS_DSL_K_FALSE((KCSTR)ifP.m_if_a)) return false;
	return EXE_IFProc(ifP.m_if_a, ifP.m_if_b,
							ifP.m_if_cond,
							ifP.m_if_c, ifP.m_if_d,
						      _wk, _rPool,_req, NULL);
}
}
