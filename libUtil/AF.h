#ifndef AF_DEF_H
#define AF_DEF_H
#include "PosixCompat.h"
/***************************** Default I/O Defined ***************************************/
#define API_I_SCEIF "SCEIF"
/***************************** Event Defined *******************************************/
#define API_E_SOCKET_STATUS                "SOCKET_STATUS"
#define API_E_SOCKET_RCV                   "SOCKET_RCV"
#define API_E_SOCKET_SND                   "SOCKET_SND"
#define API_E_SOCKET_ALIVE                 "SOCKET_ALIVE"
#define API_E_TIMER                        "TIMER"
#define API_E_USER                         "USER"
#define API_E_SCENARIO                     "SCENARIO"
#define API_E_EXT_IO                          "EXT_EVENT"
#define API_E_NULL                        "NULL_EVENT"
/****************************** Param ************************************************/
#define API_P_RCV (unsigned int)0
#define API_P_SND 1
#define API_P_EVENT          "EVENT"
#define API_P_KEY                 "API_KEY"
#define API_P_IF_NAME         "IF_NAME"
#define API_P_WORKER_IDX "WORKER_IDX"
#define API_P_POOL_ID        "POOL-ID"
#define API_P_POOL_KEY        "POOL-KEY"
#define API_P_POOL_UNIQ    "POOL-UNIQ"
#define API_P_PACKET          "PACKET"
#define API_P_ADDR            "ADDR"
#define API_P_S_ADDR          "S_ADDR"
#define API_P_LOC_NAME        "LOC_NAME"
#define API_P_RTE_NAME        "RTE_NAME"
#define API_P_STATUS          "STATUS"
#define API_P_CONNECTED       "CONNECTED"
#define API_P_DISCONNECTED    "DISCONNECTED"
#define API_P_TCP_ROLE        "TCP_ROLE"
#define API_P_SERVER          "SERVER"
#define API_P_CLIENT          "CLIENT"
#define API_P_TIME            "TIME_VAL"       // time val --> ex 1000 ms 
#define API_P_TIMER_TYPE      "TIMER_TYPE"     // MAP, CHECK, GARBAGE, ALIVE 
#define API_P_TIMER_USER      "TIMER_USER"
#define API_P_TIMER_CHECK     "TIMER_CHECK"
#define API_P_TIMER_GARBAGE   "TIMER_GARBAGE"
#define API_P_TIMER_ALIVE     "TIMER_ALIVE"
#define API_P_TIMER_STAT_CREATE       "TIMER_STAT_CREATED"
#define API_P_TIMER_OPTION    "TIMER_OPTION"   // ONCE, PERIODIC
#define API_P_TIMER_ONCE      "TIMER_ONCE"
#define API_P_TIMER_PERIODIC  "TIMER_PERIODIC"

typedef char * KSTR;
typedef const char * KCSTR;
typedef char KCHR;
typedef unsigned char KUCHR;
typedef unsigned char KHEX;
typedef short KSHORT;
typedef unsigned short KUSHORT;
typedef int KINT;
typedef unsigned int KUINT;
typedef long KLONG;
typedef unsigned long KULONG;
typedef bool KBOOL;
typedef void * KVOID;
typedef const void * KCVOID;
typedef double KDOUBLE;
#define KNULL ""
#endif
