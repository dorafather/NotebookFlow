#ifndef COMPSTR_H
#define COMPSTR_H
#include "KSTRING.h"
namespace nsUtil
{
class COMPSTR
{
	public: 
		COMPSTR();
		COMPSTR(KCSTR _pszSrc);
		~COMPSTR();
		COMPSTR & operator=(KSTR _pszSrc);
		COMPSTR & operator=(KCSTR _pszSrc);
		KBOOL operator==(KBOOL _bBool);
		KBOOL operator==(KCHR _cVal);
		KBOOL operator==(KUCHR _ucVal);
		KBOOL operator==(KSHORT _sVal);
		KBOOL operator==(KUSHORT _usVal);
		KBOOL operator==(KINT _nVal);
		KBOOL operator==(KUINT _unVal);
		KBOOL operator==(KLONG _lVal);
		KBOOL operator==(KULONG _ulVal);
		KBOOL operator==(KSTR _szSrc);
		KBOOL operator==(KCSTR _szSrc);
		KBOOL operator==(KDOUBLE _dVal);
		KBOOL operator!=(KBOOL _bBool);
		KBOOL operator!=(KCHR _cVal);
		KBOOL operator!=(KUCHR _ucVal);
		KBOOL operator!=(KSHORT _sVal);
		KBOOL operator!=(KUSHORT _usVal);
		KBOOL operator!=(KINT _nVal);
		KBOOL operator!=(KUINT _unVal);
		KBOOL operator!=(KLONG _lVal);
		KBOOL operator!=(KULONG _ulVal);
		KBOOL operator!=(KSTR _szSrc);
		KBOOL operator!=(KCSTR _szSrc);
		KBOOL operator!=(KDOUBLE _dVal);
		operator KBOOL();
		operator KCHR() ;
		operator KUCHR() ;
		operator KSTR() ;
		operator KCSTR() ;
		operator KSHORT() ;
		operator KUSHORT() ;
		operator KINT() ;
		operator KUINT() ;
		operator KLONG() ;
		operator KULONG() ;
		operator KDOUBLE() ;
		KUINT LENGTH();
	private:
		char * m_pszString;
};
}
#endif
