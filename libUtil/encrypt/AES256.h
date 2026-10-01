#ifndef AF_AES_256_H
#define AF_AES_256_H
#include "KSTRING.h"
namespace nsUtil
{
class Aes256
{
	public:
		static KCSTR encrypt(KSTRING _key, KSTRING & _plain, KSTRING & _result);
		static KCSTR decrypt(KSTRING _key, KSTRING & _enc,   KSTRING & _result);
		static void  test(KSTRING _input);
};
}
#endif
