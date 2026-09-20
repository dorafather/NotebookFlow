#include "JSON_CONFIG.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

#ifdef _MSC_VER
#include "PosixCompat.h"
#include <io.h>        // _open, _read, _write, _close
#include <fcntl.h>     // _O_RDONLY, _O_WRONLY, _O_TRUNC, _O_CREAT
#define open  _open
#define read  _read
#define write _write
#define close _close
#define O_RDONLY _O_RDONLY
#define O_WRONLY _O_WRONLY
#define O_TRUNC  _O_TRUNC
#define O_CREAT  _O_CREAT
#else
#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */

#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif /* HAVE_SYS_STAT_H */

#if HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */

#if HAVE_UNISTD_H
# include <unistd.h>
#endif /* HAVE_UNISTD_H */

#ifdef WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <io.h>
#endif /* defined(WIN32) */

#if !HAVE_OPEN && defined(WIN32)
# define open _open
#endif
#endif


#include "JSON_BITS.h"
#include "JSON_DEBUG.h"
#include "JSON_PRINTBUF.h"
#include "JSON_OBJECT.h"
#include "JSON_TOKENER.h"
#include "JSON_UTIL.h"
namespace nsUtil
{
struct json_object* json_object_from_file(char *filename)
{
  struct printbuf *pb;
  struct json_object *obj;
  char buf[JSON_FILE_BUF_SIZE];
  int fd, ret;

  if((fd = open(filename, O_RDONLY)) < 0) {
    MC_ERROR("json_object_from_file: error reading file %s: %s\n",
        filename, strerror(errno));
    return (struct json_object*)error_ptr(-1);
  }
  if(!(pb = printbuf_new())) {
    MC_ERROR("json_object_from_file: printbuf_new failed\n");
    return (struct json_object*)error_ptr(-1);
  }
  while((ret = read(fd, buf, JSON_FILE_BUF_SIZE)) > 0) {
    printbuf_memappend(pb, buf, ret);
  }
  close(fd);
  if(ret < 0) {
    MC_ABORT("json_object_from_file: error reading file %s: %s\n",
        filename, strerror(errno));
    printbuf_free(pb);
    return (struct json_object*)error_ptr(-1);
  }
  obj = json_tokener_parse(pb->buf);
  printbuf_free(pb);
  return obj;
}

int json_object_to_file(char *filename, struct json_object *obj)
{
  const char *json_str;
  int fd, ret;
  unsigned int wpos, wsize;

  if(!obj) {
    MC_ERROR("json_object_to_file: object is null\n");
    return -1;
  }

  if((fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, 0644)) < 0) {
    MC_ERROR("json_object_to_file: error opening file %s: %s\n",
        filename, strerror(errno));
    return -1;
  }

  if(!(json_str = json_object_to_json_string(obj))) { return -1; }


  wsize = (unsigned int)(strlen(json_str) & UINT_MAX); /* CAW: probably unnecessary, but the most 64bit safe */
  wpos = 0;
  while(wpos < wsize) {
    if((ret = write(fd, json_str + wpos, wsize-wpos)) < 0) {
      close(fd);
      MC_ERROR("json_object_to_file: error writing file %s: %s\n",
        filename, strerror(errno));
      return -1;
    }

   /* because of the above check for ret < 0, we can safely cast and add */
    wpos += (unsigned int)ret;
  }

  close(fd);
  return 0;
}
}

