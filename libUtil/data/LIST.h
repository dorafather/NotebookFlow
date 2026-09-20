#ifndef __LIST_H
#define __LIST_H
#include "STL.h"
#include "KSTRING.h"

namespace nsUtil
{
/********************** LIST API *************************/
class ALIST : public StlObject
{
	public:
		class Kobj : public StlObject
		{
			public:
				Kobj();
				~Kobj();
				Kobj & operator=(Kobj & _rSrc);
				void SET(KCSTR _key);
				KSTRING & KEY();
				KSTRING & VAL();
				KCSTR DEBUGGING(KSTRING & _debug);
				KSTRING m_key;
				KSTRING m_val;
		};
		ALIST();
		ALIST(ALIST & _src);
		~ALIST();
		ALIST & operator=(ALIST & _rSrc);
		Kobj & operator[](KUINT _unIdx); 
		Kobj & SET(KSTRING _szKey);       
		Kobj & GET(KSTRING _szKey);     
		Kobj & PUSH();                         
		Kobj & FRONT();                      
		void DEL(KSTRING _szKey);
		bool POP(KSTRING & _get);
		KUINT NUMS();
		void CLEAR();
		KSTR DEBUGGING(KSTRING & _rDebug);
	private:
		static Kobj m_clsNull;
		StlList m_list;
};
}
#endif

