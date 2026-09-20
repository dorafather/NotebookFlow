#include "EXEPARAM.h"
#include "EXECORE.h"
#include "EXELOG.h"

namespace nsUtil
{
static void printCol(const char * _input, KUINT _nMax, KSTRING & _szPrint)
{
	if(_input == NULL)
	{
		for (KUINT i = 0; i < (_nMax+1); i++)
			_szPrint<<' ';
	}
	else
	{
		KUINT len = strlen(_input) + 1;
		if(len <= _nMax)
		{
			KUINT cnt = _nMax - len;
			_szPrint.PRINT("%s ", _input);
			for (KUINT i = 0; i < cnt; i++)
				_szPrint<<' ';
		}
		else
		{
			_szPrint.PRINT("%s ", _input);
		}
	}
}
static void printLine(KUINT _nMax, KSTRING & _szPrint)
{
	for (KUINT i = 0; i < (_nMax); i++)
		_szPrint<<'-';
}
void printMat(KMatrix & _mat,KSTRING & _szPrint)
{
	ALIST arrMaxColLen;
	KUINT nMaxColLen = 0;
	KUINT nMaxColNum = 0;
	for(KUINT i=0;i<_mat.NUMS();i++)
	{
		if(_mat[i].NUMS()> nMaxColNum)
			nMaxColNum = _mat[i].NUMS();
	}
	for(KUINT j=0;j<nMaxColNum;j++)
	{
		nMaxColLen=0;
		for(KUINT i=0;i<_mat.NUMS();i++)
		{
			if(nMaxColLen < _mat[i][j].LENGTH())
				nMaxColLen = _mat[i][j].LENGTH();
		}
		nMaxColLen+=2;
		arrMaxColLen.PUSH().VAL() = nMaxColLen;
	}
	KUINT nCol0 = 0;
	for(KUINT i=0;i<_mat.NUMS();i++)
	{
		KMatrixColums & col = _mat[i];
		if(i != 0 && _mat[i][0] == "line")
		{
			for(KUINT j=0;j<nCol0;j++)
			{
				printLine((KUINT)arrMaxColLen[j].VAL(), _szPrint);
			}
			 _szPrint.PRINT("\n");
		}
		else
		{
			for(KUINT j=0;j<col.NUMS();j++)
			{
				printCol((KCSTR)_mat[i][j], (KUINT)arrMaxColLen[j].VAL(), _szPrint);
			}
			_szPrint.PRINT("\n");
		}
		if(i==0)
		{
			nCol0 = col.NUMS();
			for(KUINT j=0;j<col.NUMS();j++)
			{
				printLine((KUINT)arrMaxColLen[j].VAL(), _szPrint);
			}
			 _szPrint.PRINT("\n");
		}
	}
}
KCSTR printSessionParams(POOL::POOLDATA & _rPool, KUINT _colNum, KSTRING & _buf)
{
	KMatrix mat; 
	KMatrixColums * pCurLine = NULL;
	KMatrixColums & Line0 = mat.ADD();
	for(KUINT i=0;i<_colNum;i++)
	{
		Line0.ADD() = "KEY";
		Line0.ADD() = "VALUE";
	}
	for(KUINT i=0;i<_rPool.m_list.NUMS();i++)
	{
		ALIST::Kobj & item = _rPool.m_list[i];
		KUINT colIdx = i%_colNum;
		if(colIdx==0)
		{
			KMatrixColums & newLine = mat.ADD();
			pCurLine = &newLine;
			pCurLine->ADD() = (KCSTR)item.KEY();
			pCurLine->ADD() = (KCSTR)item.VAL();
		}
		else
		{
			pCurLine->ADD() = (KCSTR)item.KEY();
			pCurLine->ADD() = (KCSTR)item.VAL();
		}
	}
	printMat(mat, _buf);
	return (KCSTR)_buf;
}
ExeLog::ExeLog(LogParser & _dsl)
{
	m_dsl = &_dsl;
}
ExeLog::~ExeLog()
{
}
bool ExeLog::EXE(QTHREAD & _wk, 
					POOL::POOLDATA & _rPool, 
					RestMsg & _req)
{
	LogParser & LogP = (LogParser&)*m_dsl;
	KSTRING log;
	for(KUINT i=0;i<LogP.m_listParam.NUMS();i++)
	{
		KSTRING & szLog = LogP.m_listParam[i].VAL();
		BasicParser param; 
		param.TOKEN() = ".";
		param.PARSE((KCSTR)szLog);
		KSTRING buf;
		KCSTR pVal = ExeParam::PARAM(param.KEY(),param.VAL(), _rPool,_req,buf);
		log.PRINT("%s ",pVal);
	}
	TIME dTime;
	printf("SCEN %04u.%02u.%02u:%02u.%02u.%02u-%03u> %s\n",
									dTime.Year(),
									dTime.Mon(),
									dTime.Day(),
									dTime.Hour(),
									dTime.Min(),
									dTime.Sec(),
									dTime.Msec(),
									(KCSTR)log);
	return true;
}
}
