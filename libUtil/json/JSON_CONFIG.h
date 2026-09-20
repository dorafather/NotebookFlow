
/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */
#ifndef json_config_h_k
#define json_config_h_k
/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */
/* #undef HAVE_DOPRNT */

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if your system has a GNU libc compatible `malloc' function, and
   to 0 otherwise. */
#define HAVE_MALLOC 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `open' function. */
#define HAVE_OPEN 1

/* Define to 1 if your system has a GNU libc compatible `realloc' function,
   and to 0 otherwise. */
#define HAVE_REALLOC 1

/* Define to 1 if you have the <stdarg.h> header file. */
#define HAVE_STDARG_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Define to 1 if you have the `strndup' function. */
#define HAVE_STRNDUP 1

/* Define to 1 if you have the <syslog.h> header file. */
#define HAVE_SYSLOG_H 1

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vasprintf' function. */
#define HAVE_VASPRINTF 1

/* Define to 1 if you have the `vprintf' function. */
#define HAVE_VPRINTF 1

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Define to 1 if you have the `vsyslog' function. */
#define HAVE_VSYSLOG 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "json-c"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "michael@metaparadigm.com"

/* Define to the full name of this package. */
#define PACKAGE_NAME "json-c"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "json-c 0.9"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "json-c"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.9"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "0.9"

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to rpl_malloc if the replacement function should be used. */
/* #undef malloc */

/* Define to rpl_realloc if the replacement function should be used. */
/* #undef realloc */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* This file was generated once by `configure` on a Linux build machine and
 * checked in as-is - it is NOT regenerated per platform. The HAVE_* flags
 * above are therefore Linux-only facts, incorrectly assumed true for any
 * build. Each json_*.cxx file already has proper `#if HAVE_X` / `#if
 * !HAVE_X` fallback guards for exactly this situation; correct the flags
 * that are false under MSVC so those existing guards do what they were
 * clearly written to do. */
#ifdef _MSC_VER
#undef HAVE_STRNCASECMP	/* json_tokener.cxx: falls back to _strnicmp */
#undef HAVE_STRNDUP		/* json_object.cxx/json_tokener.cxx: falls back to a local strndup() */
#undef HAVE_VASPRINTF		/* json_printbuf.cxx: no MSVC vasprintf (see PosixCompat.hxx instead) */
#undef HAVE_STRINGS_H		/* json_arraylist.cxx: strings.h doesn't exist on Windows */
#undef HAVE_SYSLOG_H		/* json_debug.cxx: syslog.h doesn't exist on Windows */
#undef HAVE_VSYSLOG		/* json_debug.cxx: no vsyslog - falls back to vprintf/vfprintf */
#undef HAVE_UNISTD_H		/* json_debug.cxx: unistd.h doesn't exist on Windows */
#undef HAVE_SYS_PARAM_H	/* json_debug.cxx: sys/param.h doesn't exist on Windows */
#endif

#endif
