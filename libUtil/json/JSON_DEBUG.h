#ifndef _DEBUG_H_k
#define _DEBUG_H_k

#ifdef __cplusplus
extern "C" {
#endif
namespace nsUtil
{
extern void mc_set_debug(int debug);
extern int mc_get_debug(void);

extern void mc_set_syslog(int syslog);
extern void mc_abort(const char *msg, ...);
extern void mc_debug(const char *msg, ...);
extern void mc_error(const char *msg, ...);
extern void mc_info(const char *msg, ...);

#ifdef MC_MAINTAINER_MODE
#define MC_SET_DEBUG(x) mc_set_debug(x)
#define MC_GET_DEBUG() mc_get_debug()
#define MC_SET_SYSLOG(x) mc_set_syslog(x)
#define MC_ABORT(x, ...) mc_abort(x, ##__VA_ARGS__)
#define MC_DEBUG(x, ...) mc_debug(x, ##__VA_ARGS__)
#define MC_ERROR(x, ...) mc_error(x, ##__VA_ARGS__)
#define MC_INFO(x, ...) mc_info(x, ##__VA_ARGS__)
#else
#define MC_SET_DEBUG(x) if (0) mc_set_debug(x)
#define MC_GET_DEBUG() (0)
#define MC_SET_SYSLOG(x) if (0) mc_set_syslog(x)
#define MC_ABORT(x, ...) if (0) mc_abort(x, ##__VA_ARGS__)
#define MC_DEBUG(x, ...) if (0) mc_debug(x, ##__VA_ARGS__)
#define MC_ERROR(x, ...) if (0) mc_error(x, ##__VA_ARGS__)
#define MC_INFO(x, ...) if (0) mc_info(x, ##__VA_ARGS__)
#endif
}
#ifdef __cplusplus
}
#endif

#endif

