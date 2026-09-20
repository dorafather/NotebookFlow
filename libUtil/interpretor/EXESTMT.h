#ifndef EXE_STMT_H
#define EXE_STMT_H
#include "STMTPARSER.h"

namespace nsUtil
{
class ExeStmt
{
	public:
		ExeStmt(StmtParser & _dsl);
		~ExeStmt();
		bool EXE(POOL::POOLDATA & _rPool, 
				RestMsg & _rcvMsg,
				KSTRING & _val);
		StmtParser * m_dsl;
};
}
#endif
