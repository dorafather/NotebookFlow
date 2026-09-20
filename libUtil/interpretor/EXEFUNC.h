#ifndef EXE_FUNC_H
#define EXE_FUNC_H
#include "FUNCPARSER.h"

namespace nsUtil
{
typedef bool (*EXE_FP)(QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
struct ExeEntry
{
    eExeType    type;       
    EXE_FP      func;   
};
class ExeFunc
{
	public:
		ExeFunc(FuncParser & _dsl);
		~ExeFunc();
		bool EXE(QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _rcvMsg);
		static bool EXE_SET( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_SUM( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_MINUS( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_DIV( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_MOD( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_CAT( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_STRSTR( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_STRNCMP( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_INSERT( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_DELETE( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_EXTRACT( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_TIME( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_CLOCK( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_PRINT( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_DATE( QTHREAD & _wk, 
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_CPH( QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_LOOP( QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_OBJ( QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_SPLIT( QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_WORDEX( QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static bool EXE_WORDSUM( QTHREAD & _wk,
						POOL::POOLDATA & _rPool, RestMsg & _req,
						ALIST & _params);
		static void copyParam(ALIST & _dst, RestParam & _src);
		static KCSTR paramone(KSTRING & _value,
						POOL::POOLDATA & _rPool,
						RestMsg & _req,
						KSTRING & _buf);
		const ExeEntry* findfuncbyname(KCSTR _name);
		const ExeEntry* findfuncbytype(eExeType _type);
		FuncParser * m_dsl;
};
}
#endif
