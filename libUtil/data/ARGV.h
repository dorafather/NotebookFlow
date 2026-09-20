#ifndef _ARGV_H
#define _ARGV_H
#include "STL.h"
#include "AF.h"
#include "KSTRING.h"

namespace nsUtil
{
typedef struct KPTR
{
	KPTR(void * _pvData){m_pvData = _pvData;}
	void * m_pvData;
}KPTR;
void g_fnMoveString(KSTRING & _rclsDst, KSTRING & _rclsSrc);
class Bytes : public StlObject
{
	public:
		Bytes();
		Bytes(Bytes & _rclsSrc);
		~Bytes();
		Bytes & operator=(Bytes & _rclsSrc);                            
		Bytes & operator<<(Bytes & _rclsSrc);                        
		operator KVOID();   
		void SETKEY(KCSTR _key);
		KSTRING & KEY();
		KSTRING & VAL();                                             
		unsigned int LEN();                                            
		void MOVE(void * _pvData, unsigned int _unLen);
		void COPY(void * _pvData,unsigned int _unLen); 
		void REFER(void * _pvObject);                          
		void RELEASE();                                              
		void CONSTRUCT(void * _src);
		KSTRING m_key;
		KSTRING m_szData;                                             
		bool m_bReference;
};
class ARG
{
	public:
		ARG();
		virtual ~ARG();
		ARG & operator=(ARG & _rclsSrc);  
		ARG & operator<<(ARG & _rclsSrc);
		Bytes & operator[](unsigned int _nIndex);              
		Bytes & SET(KSTRING  _clsKey);            
		Bytes & GET(KSTRING  _clsKey);           
		void CLEAR();
		static unsigned int genhash(char * _pszKey, unsigned int _unMax);
	protected:
		StlList * m_plistArgs;
		static Bytes m_DefBytes;
};
}
#endif
