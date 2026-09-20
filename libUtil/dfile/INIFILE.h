#ifndef INI_FILE_H
#define INI_FILE_H
#include "DFILE.h"
#include "INIPARSER.h"
#include "LOCK.h"
#include "GPOLLING.h"

namespace nsUtil
{
class IniFileReader
{
	public:
		IniFileReader();
		~IniFileReader();
		void Read(KCSTR _path);
		void IMPORT(RestMsg & _msg);
		void JSON(RestMsg & _msg);
		void STR(KSTRING & _buf);
		KCSTR GET(KCSTR _cate, KCSTR _key, KSTRING & _buf);
		static void inifiletimeout(Gpolling::info * _info);
		FileReader m_file;
		IniParser m_parser;
		MUTEX m_lock;
};
}
#endif
