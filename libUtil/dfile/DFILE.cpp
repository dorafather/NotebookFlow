#include "DFILE.h"
#include "KSTRING.h"
#ifdef _MSC_VER
#include <sys/utime.h>
#include <fcntl.h>
#else
#include <utime.h>
#include <fcntl.h>      
#include <unistd.h>     
#include <sys/stat.h>   
#endif

namespace nsUtil
{
/*************************** File Time Info Class *******************************************/
static unsigned int s_arrCrc32[] = 
{
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};
FileReader::FileReader()
{
	m_pszRawData= NULL;
	clear();
	m_bEnhanced=false;
}
FileReader::~FileReader()
{
	if(m_pszRawData)
	{
		delete [] m_pszRawData;
	}
}
void FileReader::init(const char * _pszPath)
{
	m_clsPath = _pszPath;
	if(KSTRING::m_fnStrLen(_pszPath)==0) return;   
	#ifdef _MSC_VER
	_chmod((KCSTR)m_clsPath, _S_IREAD | _S_IWRITE);
	#else
	chmod((KCSTR)m_clsPath,0755);
	#endif
	updatetime();
	if(S_ISDIR(m_stFileInfo.st_mode)) m_bIsDir = true;
	updatecrc();
}
bool FileReader::writefile(KCSTR _pszData, KUINT _len)
{
	if(_pszData==NULL) return false;
	#ifdef _MSC_VER
	_chmod((KCSTR)m_clsPath, _S_IREAD | _S_IWRITE);
	int nFd = _open((KCSTR)m_clsPath, _O_CREAT|_O_WRONLY|_O_TRUNC, _S_IREAD|_S_IWRITE);
	#else
	chmod((KCSTR)m_clsPath,0755);
	int nFd = open((KCSTR)m_clsPath, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
	#endif
	if( nFd < 0 )
	{
		return false;
	}
	if(_len > 0)
	{
		#ifdef _MSC_VER
		int lWriteLen = _write(nFd, _pszData, _len);
		#else
		ssize_t lWriteLen = write(nFd, _pszData, _len);
		#endif
		if( lWriteLen != _len )
		{
			#ifdef _MSC_VER
			_close(nFd);
			#else
			close(nFd);
			#endif
			return false;
		}
		#ifdef _MSC_VER
		_close(nFd);
		#else
		close(nFd);
		#endif
		return true;
	}
	return true;
}
bool FileReader::updatetime()
{
	if(KSTRING::m_fnStrLen((KCSTR)m_clsPath)==0) return false;  
	memset(&m_stFileInfo,0x00,sizeof(struct stat));
	stat((KCSTR)m_clsPath,&m_stFileInfo);
	m_unPrevModiTime = m_unCurrentModiTime; 
	m_unCurrentModiTime = (unsigned int)m_stFileInfo.st_mtime;
	m_unPrevSize = m_unCurrentSize; 
	m_unCurrentSize = (unsigned int)(m_stFileInfo.st_size);
	if(m_unPrevModiTime != m_unCurrentModiTime)
	{
		memset(&m_stTm,0x00,sizeof(tm));
		localtime_r(&m_stFileInfo.st_mtime,&m_stTm);	
		m_nYr = m_stTm.tm_year + 1900;
		m_nMon = m_stTm.tm_mon +1;
		m_nDay = m_stTm.tm_mday;
		m_nHr = m_stTm.tm_hour;
		m_nMin = m_stTm.tm_min;
		m_nSec = m_stTm.tm_sec;
		memset(m_szTime,0x00,sizeof(m_szTime));
		snprintf(m_szTime,sizeof(m_szTime)-1,"%04d-%02d-%02d-%02d-%02d-%02d",
									m_nYr,m_nMon,m_nDay,m_nHr,m_nMin,m_nSec);
		return true;
	}
	if(m_unPrevSize != m_unCurrentSize) return true;
	return false;
}
bool FileReader::updatecrc()
{
	if(m_bIsDir) return false;
	if(KSTRING::m_fnStrLen((KCSTR)m_clsPath)==0) return false;   
	FILE *fp = NULL;
	if ((fp = fopen((KSTR)m_clsPath, "rb")) == NULL)   
	{
		
		m_unReadSz=0;
		if(m_pszRawData)
		{
			delete [] m_pszRawData; m_pszRawData=NULL;
		}
		return false;
	}
	unsigned int unRealSz = getfilesize();
	if(unRealSz==0)
	{
		
	}
	if(m_pszRawData)
	{
		delete [] m_pszRawData; m_pszRawData=NULL;
	}
	m_pszRawData = new char[unRealSz+10]; memset(m_pszRawData,0x00,unRealSz+10);
	if(unRealSz > 0)
	{
		m_unReadSz=fread(m_pszRawData,1,unRealSz,fp);
	}
	else m_unReadSz = 0;
	fclose(fp);
	if(m_unReadSz == 0)
	{
		
	}
	else if(unRealSz != (unsigned int)m_unReadSz)
	{
		if(m_pszRawData)
		{
			delete [] m_pszRawData; m_pszRawData=NULL;
		}
		m_unReadSz = 0;return false;
	}
	m_unPrevCrc = m_ulCurrentCrc;
	unsigned int unCrc = 0; 
	m_ulCurrentCrc = getcrc(unCrc,m_pszRawData,m_unReadSz);
	if(m_unPrevCrc != m_ulCurrentCrc) return true;
	return false;
}
void FileReader::clear()
{
	m_unReadSz=0;m_bIsDir = false;
	memset(m_szTime,0x00,sizeof(m_szTime));
	memset(&m_stFileInfo,0x00,sizeof(struct stat));
	memset(&m_stTm,0x00,sizeof(tm));
	m_nYr  = 0;m_nMon = 0;m_nDay = 0;m_nHr  = 0;
	m_nMin = 0;	m_nSec = 0;
	m_unCurrentModiTime = 0; m_unPrevModiTime = 0; 
	m_unPrevCrc = 0;m_ulCurrentCrc=0;
	m_bCheckInit = false;m_unPrevSize = 0; 
	m_unCurrentSize = 0;
}
void FileReader::changelasttime(unsigned int _unTime)
{
	changetouchtime((KCSTR)m_clsPath,_unTime);
}
void FileReader::changelasttime(const char * _pszTime)
{
	changetouchtime((KCSTR)m_clsPath,_pszTime);
}
unsigned int FileReader::getfilesize()
{
	if(m_bIsDir) return 0;
	if(KSTRING::m_fnStrLen((KCSTR)m_clsPath)==0) return 0;
	return (unsigned int)(m_stFileInfo.st_size);
}
void FileReader::enablecheckchanged(bool _bEnhanced)
{
	if(m_bCheckInit ==false)
	{
		m_bCheckInit = true;
		updatetime();
		if(_bEnhanced)
		{
			m_bEnhanced = true;
			updatecrc();
		}
	}
}
bool FileReader::checkchanged()
{
	if(m_bCheckInit == false) return false;
	if(updatetime())
	{
		updatecrc();
		return true;
	}
	else if(m_bEnhanced)
	{
		return updatecrc();
	}
	return false;
}
char * FileReader::gettimestring(unsigned int _unTime,char * _pszSource, unsigned int _nBufLen)
{
	time_t stTime = (time_t)_unTime; tm stTm;localtime_r(&stTime,&stTm);	
	snprintf(_pszSource,_nBufLen-1,"%04d-%02d-%02d-%02d-%02d-%02d",
											stTm.tm_year + 1900,
											stTm.tm_mon +1,
											stTm.tm_mday,
											stTm.tm_hour,
											stTm.tm_min,
											stTm.tm_sec);
	return _pszSource;
}
unsigned int FileReader::gettimeint(char * _pszSource)
{
	struct tm stTm;time_t unTime;
	stTm.tm_year = KSTRING::m_fnAtoi(&_pszSource[0]) - 1900;
	stTm.tm_mon =  KSTRING::m_fnAtoi(&_pszSource[5])-1;
	stTm.tm_mday =KSTRING::m_fnAtoi(&_pszSource[8]); 
	stTm.tm_hour = KSTRING::m_fnAtoi(&_pszSource[11]);
	stTm.tm_min = KSTRING::m_fnAtoi(&_pszSource[14]);
	stTm.tm_sec = KSTRING::m_fnAtoi(&_pszSource[17]);
	unTime = mktime(&stTm);
	return (unsigned int)unTime;
}
void FileReader::changetouchtime(const char * _pszPath,unsigned int _unTime)
{
#ifdef _MSC_VER
	struct _utimbuf stUt; stUt.modtime = (time_t)_unTime;
	_utime(_pszPath,&stUt);
#else
	struct utimbuf stUt; stUt.modtime = (time_t)_unTime;
	utime(_pszPath,&stUt);
#endif
}
void FileReader::changetouchtime(const char * _pszPath,const char * _pszTime)
{
	struct tm stTm;time_t unTime;
	stTm.tm_year = KSTRING::m_fnAtoi(&_pszTime[0]) - 1900;
	stTm.tm_mon =  KSTRING::m_fnAtoi(&_pszTime[5])-1;
	stTm.tm_mday =KSTRING::m_fnAtoi(&_pszTime[8]);
	stTm.tm_hour = KSTRING::m_fnAtoi(&_pszTime[11]);
	stTm.tm_min = KSTRING::m_fnAtoi(&_pszTime[14]);
	stTm.tm_sec = KSTRING::m_fnAtoi(&_pszTime[17]);
	unTime = mktime(&stTm);
#ifdef _MSC_VER
	struct _utimbuf stUt; stUt.modtime = unTime;
	_utime(_pszPath,&stUt);
#else
	struct utimbuf stUt; stUt.modtime = unTime;
	utime(_pszPath,&stUt);
#endif
}
unsigned long FileReader::getcrc(unsigned int _unCrc, char * _pszSrc, unsigned int _unSize)
{
	if(_unSize ==0) return 0;
	char *pszTmp = _pszSrc;_unCrc = _unCrc ^ ~0U;
	while (_unSize--)
		_unCrc = s_arrCrc32[(_unCrc ^ *pszTmp++) & 0xFF] ^ (_unCrc >> 8);
	return _unCrc ^ ~0U;
}
}