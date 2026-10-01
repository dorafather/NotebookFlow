#ifndef AF_SHA_256_H
#define AF_SHA_256_H
#include "KSTRING.h"
namespace nsUtil
{
class Sha256
{
	public:
		static KCSTR encrypt(KSTRING _key, KSTRING & _plain, KSTRING & _result);
		static KCSTR decrypt(KSTRING _key, KSTRING & _plain, KSTRING & _result);
		static void test();
};
}
#endif
