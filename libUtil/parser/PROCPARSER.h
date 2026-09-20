#ifndef PROC_PARSER_H
#define PROC_PARSER_H
#include "BASICPARSER.h"
#include "CLASSPARSER.h"
#include "CXIFPARSER.h"
#include "STRCONV.h"
#include "FUNCPARSER.h"
#include "LOGPARSER.h"

namespace nsUtil
{
class ProcParser : public StlObject
{
	public:
		class Procedure : public StlObject
		{
			public:
				class ProcExe : public StlObject
				{
					public:
						ProcExe();
						~ProcExe();
						bool PARSE(KCSTR _data);
						void JSON(RestParam & _item);
						void STR(KSTRING & _buf); 
						void IMPORT(RestParam & _item);
						KSTRING m_type;
						BasicParser m_exe;
						FuncParser m_func;
						LogParser m_log;
						KSTRING m_result;
				};
				Procedure();
				~Procedure();
				bool PARSE(KCSTR _data);
				void JSON(RestParam & _item);
				void STR(KSTRING & _buf); 
				void IMPORT(RestParam & _item);
				ProcExe & operator[](KUINT _idx);
				KUINT NUMS();
				CxIfParser m_cond;
				StlList m_list; 
				KSTRING m_result;
				ProcExe m_defExe;
		};
		ProcParser();
		~ProcParser();
		ProcParser & operator=(ProcParser & _src);		
		Procedure & operator[](KUINT _idx);
		void CONSTRUCT(void * _pvSrc);
		bool PARSE(ClassParser & _class);
		void JSON(RestParam & _item);
		void STR(KSTRING & _buf);
		void IMPORT(RestParam & _item);
		KUINT NUMS();
		KSTRING m_oType;
		KSTRING m_oName;
		KSTRING m_oSpace;
		KSTRING m_result;
	private:
		STRCONV m_conv;
		StlList m_list;  
		Procedure m_def;
};
}
#endif

