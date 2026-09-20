#ifndef _bits_h_k
#define _bits_h_k
#include <stdint.h>
namespace nsUtil
{
#ifndef json_min
#define json_min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef json_max
#define json_max(a,b) ((a) > (b) ? (a) : (b))
#endif

#define hexdigit(x) (((x) <= '9') ? (x) - '0' : ((x) & 7) + 9)
#define error_ptr(error) ((void*)error)
#define is_error(ptr) ((uintptr_t)(ptr) > (uintptr_t)(-4000L))
}
#endif

