#ifndef _json_util_h_k
#define _json_util_h_k

#include "JSON_OBJECT.h"

#ifdef __cplusplus
extern "C" {
#endif
namespace nsUtil
{
#define JSON_FILE_BUF_SIZE 4096

/* utility functions */
extern struct json_object* json_object_from_file(char *filename);
extern int json_object_to_file(char *filename, struct json_object *obj);
}
#ifdef __cplusplus
}
#endif

#endif

