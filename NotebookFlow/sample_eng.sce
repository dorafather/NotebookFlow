SCE::NAME
{
  sample_eng
}
MAP::lib_test
{
	(a,1) (b,2) (c,3)
	(d,4)
}
STATE::INIT
{
    event_0    PROC.procInit      
    event_1    PROC.proc1_init
    event_2    PROC.proc2
    event_77   PROC.procIfTest
    CBK        PROC.procCbk
    event_7    PROC.proc7_init 
    TERM       PROC.procTerm
}
STATE::st_ringing
{ 
    event_2       PROC.proc2
    event_3       PROC.proc3_ringing 
    init_timeout    PROC.procInitTimeout
    ring_timeout    PROC.procRingTimeout
    CBK        PROC.procCbk
    TERM       PROC.procTerm
}
STATE::st_connected
{
    longcall_timeout PROC.procLongTimeout
    event_9    PROC.proc99_recursive
    event_2    PROC.proc2
    event_4    PROC.proc4_connected
    TERM       PROC.procTerm
}
STATE::STOP
{
    TERM    PROC.procTerm 
}
PROC::procIfTest 
{
    IF(RCV_MSG.PARAMS.value == 1)
        LOG.info( if 1)
        FUNC.SET(condition_test,1)
    ELSEIF(RCV_MSG.PARAMS.value == 2)
        LOG.info( if 2)
        FUNC.SET(condition_test,2)
    ELSEIF(RCV_MSG.PARAMS.value == 3/4)
        LOG.info( if 3 or 4)
        FUNC.SET(condition_test,RCV_MSG.PARAMS.value)
    ELSE
        LOG.info( if other)
        FUNC.SET(condition_test,RCV_MSG.PARAMS.value)
}
PROC::procCbk
{
    IF(TRUE)
        GOTO.st_ringing
}
PROC::procInit
{
    IF(TRUE)
        ACT_SET.setSession
        FUNC.CLOCK(start_t)
        FUNC.SET(cat_test,1234) 
        LOG.info(1. cat_test = SESSION.cat_test)
        FUNC.SET(cat_test,NULL)
        LOG.info(2. cat_test = SESSION.cat_test)
        FUNC.CDR(StartTime,SESSION.start_t)
        FUNC.CDR(caller,SESSION.caller)
        FUNC.CDR(called,SESSION.called)
        FUNC.CDR(sce-name,SESSION.sce_name)
        FUNC.STAT(MCSATT)
        FUNC.FBSTAT(ALGATT)
}
PROC::procTerm
{
    // Session Terminate 
    IF(TRUE)
        LOG.info(Term Senario!)
        FUNC.CLOCK(end_t)
        FUNC.CDR(EndTime,SESSION.end_t)
        FUNC.MINUS(dur_t,SESSION.end_t,SESSION.start_t)
        FUNC.CDR(DurTime,SESSION.dur_t)
        FUNC.STAT(recursive_cnt)
        FUNC.STAT(recursive_cnt)
        PROC.procSessionPrint
}
PROC::procSessionPrint
{
    IF(SESSION.TRACE_ENABLE == 1)
        FUNC.SET(final_trace,1)
        LOG.info(SESSION.*)
}
PROC::test0
{
    IF(RCV_DATA.caller !!! aaa/bbb/678/999)
        PROC.test1
    ELSE
        PROC.test1
}
PROC::test1
{
    IF(RCV_DATA.caller === aaa) OR (RCV_DATA.called === 12) OR (RCV_DATA.ua_no !!! 567)
        PROC.test2
    ELSE
        PROC.test2
}
PROC::test2
{
    IF(RCV_DATA.caller === 010) AND (RCV_DATA.caller !!! 678) AND (RCV_DATA.caller !!! 456)
        PROC.test3
    ELSE
        PROC.test3
}
PROC::test3
{
    IF(RCV_DATA.caller === 0101234)
        PROC.test4
    ELSE
        PROC.test4
}
PROC::test4
{
    IF(RCV_DATA.caller.LENGTH == 10)
        PROC.test5
    ELSE
        PROC.test5
}
PROC::test5
{
    IF(SESSION.caller.LENGTH <= 11)
        PROC.test6
    ELSE
        PROC.test6
}
PROC::test6
{
    IF(RCV_MSG.PARAMS.id == 1)
        PROC.test99
    ELSE
        PROC.test99
}
PROC::test99
{
    IF(TRUE)
        FUNC.SET(bcp_reason,1234)
        FUNC.SET(bcp_reason2, RCV_DATA.caller)
        FUNC.PRINT(print_test,%010s,123)
        FUNC.PRINT(print_test2,%-15s,RCV_DATA.caller)
        FUNC.PRINT(print_test3,%15s,RCV_DATA.caller)
        FUNC.DATE(curDate0,%Y-%m-%d)
        FUNC.DATE(curDate1,STMT.sntDate1)
        FUNC.DATE(curDate2,STMT.sntDate2)
        FUNC.DATE(curDate3,STMT.sntDate3)
        FUNC.DATE(curDate4,STMT.sntDate4)
        FUNC.DATE(curDate5,STMT.sntDate5)
        FUNC.DATE(curDate6,STMT.sntDate6)
        FUNC.DATE(curDate7,STMT.sntDate7)
        FUNC.SET(cnt,10)
        FUNC.SUM(sum_result,SESSION.cnt,1)
        FUNC.MINUS(cnt,SESSION.cnt,2)
        FUNC.DIV(avg, RCV_DATA.called,3)
        FUNC.MOD(idx, RCV_DATA.called,5)
        FUNC.CAT(name,a,b,->, RCV_DATA.caller)
        FUNC.STRSTR(strstr,RCV_DATA.caller,456)
        FUNC.STRNCMP(strncmp, RCV_DATA.caller,01012)
        FUNC.INSERT(strInsert, SESSION.caller,aaa,0)
        FUNC.DELETE(strDelete, SESSION.caller,0,3)
        FUNC.EXTRACT(strExtract, SESSION.caller,0,3)
        FUNC.SET(rec_id, RCV_MSG.PARAMS.id)
        FUNC.TIME(unix_time)
        FUNC.CLOCK(clock_time)
        FUNC.SET(test,0)
}
PROC::proc0_init
{
    IF(TRUE)
        PROC.test0
}
PROC::proc1_init
{
    IF(TRUE)
        GOTO.st_ringing
}
PROC::proc2
{
    IF(SESSION.STATE != st_ringing)
        GOTO.BREAK
    ELSE
        ACT_TIMER.longcall
        ACT_SEND.sndRsp
        GOTO.st_connected
}
PROC::proc3_ringing
{
    IF(TRUE)
        ACT_TIMER.ring
}
PROC::proc4_connected
{
    IF(TRUE)
        ACT_TIMER.longcall
}
PROC::proc99_recursive
{
    IF(SESSION.func_cnt <= 4)
        ACT_UTIL.func_counting
        PROC.proc99_recursive
        FUNC.STAT(recursive_cnt)
        FUNC.STAT(recursive_cnt)
    ELSE
        ACT_UTIL.func_counting
        GOTO.STOP
}
PROC::proc7_init
{
    IF(TRUE)
        ACT_TIMER.init_timer
}
PROC::proc9_connected
{
    IF(TRUE)
        GOTO.BREAK
}
PROC::procInitTimeout
{
    IF(TRUE)
        GOTO.STOP
}
PROC::procRingTimeout
{
    IF(TRUE)
        GOTO.st_connected
}
PROC::procLongTimeout
{
    IF(TRUE)
        ACT_SEND.sndBye
        GOTO.STOP
}
ACT_TIMER::init_timer
{
    SND_HDR.TYPE = init_timeout
    SND_DATA.TIME = 3000
}
ACT_TIMER::ring
{
    SND_HDR.TYPE = ring_timeout
    SND_DATA.TIME = 60000
}
ACT_TIMER::longcall
{
    SND_HDR.TYPE = longcall_timeout
    SND_DATA.TIME = 360000
    SND_DATA.tmp_cnd = haha
}
ACT_SEND::sndRsp
{
    SND_HDR.TYPE = rsp_info
    SND_HDR.SERVICE_TYPE = MH
    SND_HDR.Dest_Type = NPDB
    SND_DATA.caller = SESSION.caller
    SND_DATA.called = SESSION.called
    SND_DATA.reason = long_call_time_out
    SND_DATA.result = ok
    SND_MSG.detailed.dev = SESSION.caller
    SND_MSG.detailed.type = rsp
}
ACT_SEND::sndBye
{
    SND_HDR.TYPE = bye
    SND_HDR.SERVICE_TYPE = MH
    SND_HDR.Dest_Type = PPDB
    SND_DATA.caller = SESSION.caller
    SND_DATA.called = SESSION.called
    SND_DATA.reason = long_call_time_out
    SND_DATA.result = ok
}
ACT_SET::setSession
{
    SESSION.caller = RCV_DATA.caller
    SESSION.called = RCV_DATA.called
    SESSION.cnt = RCV_DATA.caller
    SESSION.sce_name = RCV_MSG.PARAMS.SCE_NAME
    SESSION.func_cnt = 0
}
ACT_UTIL::etc
{
    SESSION.cnt = SUM(SESSION.cnt,1)
    SESSION.cnt = MINUS(SESSION.cnt,3)
    SESSION.AVG = DIV(RCV_DATA.called,3)
    SESSION.IDX = MOD(RCV_DATA.called,5)
    SESSION.name = CAT(a,b,->,RCV_DATA.caller)
    SESSION.strstr = STRSTR(RCV_DATA.caller,456)
    SESSION.strncmp = STRNCMP(RCV_DATA.caller,01012)
    SESSION.strInsert = INSERT(SESSION.caller,aaa,3)
    SESSION.strDelete = DELETE(SESSION.caller,0,3)
    SESSION.strExtract = EXTRACT(SESSION.caller,0,3)
}
ACT_UTIL::func_counting
{
    SESSION.func_cnt = SUM(SESSION.func_cnt,1)
}
ACT_SEND::alarmSce
{
    SND_HDR.TYPE = ALARM
    SND_HDR.SERVICE_TYPE = NATS
    SND_HDR.TOPIC = pam.alarm.publish
    
    SND_MSG.BODY.alarmCode   = SVC_CRASH
    SND_MSG.BODY.host        = INI.systemInfo.SYSTEM.SYSNAME
    SND_MSG.BODY.service     = SYS_CFG.PROC_NAME
    SND_MSG.BODY.title       = Service process crash detected
    SND_MSG.BODY.description = OMP00 서비스 프로세스가 비정상 종료되었습니다
    SND_MSG.BODY.severity    = CRITICAL
    SND_MSG.BODY.status      = firing
}
ACT_SEND::traceSce
{
    SND_HDR.TYPE = TRACE_SCE
    SND_HDR.SERVICE_TYPE = NATS
    SND_HDR.TOPIC = pam.trace.publish
    
    SND_MSG.BODY.mdn           = SESSION.caller
    SND_MSG.BODY.process       = SYS_CFG.PROC_NAME
    SND_MSG.BODY.targetProcess = SYS_CFG.PROC_NAME
    SND_MSG.BODY.message       = test_trace
    SND_MSG.BODY.level         = INFO
    SND_MSG.BODY.rawData       = 
    SND_MSG.BODY.timestamp     = TIME.MSEC
    SND_MSG.BODY.protocol      = SLP
    SND_MSG.BODY.isDisplay     = true
}
ACT_SEND::SND_MCS_SDM_1_REQ
{
        SND_HDR.TYPE = SDM
        SND_HDR.SERVICE_TYPE = MCS_SDM_1_RSP
        SND_HDR.QUERY-TYPE = INSERT
        SND_HDR.QUERY-DATA = QUERY.QY_MCS_SUB_delete
} 
QUERY::QY_MCS_SUB_delete
{
	DELETE ABCD
}
STMT::sntDate1
{%Y-%m-%d}
STMT::sntDate2
{%F}
STMT::sntDate3
{%Y/%m/%d %H:%M:%S}
STMT::sntDate4
{%T}
STMT::sntDate5
{%Y %m %d }
STMT::sntDate6
{%A %B %d %Y}
STMT::sntDate7
{Y%m%d %H%M%S}