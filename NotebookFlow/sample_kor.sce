/*

한글 DSL 
NotebookFlow 시나리오

*/
상태::초기
{
    event_0    처리.procInit 
    event_1    처리.proc1_init
    event_2    처리.proc2
    event_7    처리.proc7_init 
    소멸       처리.procTerm
}
상태::st_ringing
{
    event_2       처리.proc2
    event_3       처리.proc3_ringing 
    init_timeout    처리.procInitTimeout
    ring_timeout    처리.procRingTimeout
    소멸       처리.procTerm
}
상태::st_connected
{
    longcall_timeout    처리.procLongTimeout
    event_9    처리.proc99_recursive
    event_2    처리.proc2
    event_4    처리.proc4_connected
    소멸       처리.procTerm
}
상태::종료
{
	소멸    처리.procTerm 
}
처리::procInit
{
    만약에(참)	
		함수.저장(caller   ,수신메시지.PARAMS.caller  )
		함수.저장(called   ,수신메시지.PARAMS.called  )
		함수.저장(cnt      ,수신메시지.PARAMS.caller  )
		함수.저장(func_cnt ,0                     )
        함수.클럭(start_t)
        함수.이력(StartTime,세션.start_t)
        함수.이력(caller,세션.caller)
        함수.이력(called,세션.called)
        함수.이력(sce-name,세션.sce_name)
        함수.통계(MCSATT)
        함수.블럭통계(ALGATT)
}
처리::procTerm
{
    만약에(참)
        LOG.info(Term Senario!)
        함수.클럭(end_t)
        함수.이력(EndTime,세션.end_t)
        함수.빼기(dur_t,세션.end_t,세션.start_t)
        함수.이력(DurTime,세션.dur_t)
        함수.통계(recursive_cnt)
        함수.통계(recursive_cnt)
}
처리::test0
{
    만약에(수신메시지.PARAMS.caller !!! aaa/bbb/678/999)
        처리.test1
    그외
        처리.test1
}
처리::test1
{
    만약에(수신메시지.PARAMS.caller === aaa) OR (수신메시지.PARAMS.called === 12) OR (수신메시지.PARAMS.ua_no !!! 567)
        처리.test2
    그외
        처리.test2
}
처리::test2
{
    만약에(수신메시지.PARAMS.caller === 010) AND (수신메시지.PARAMS.caller !!! 678) AND (수신메시지.PARAMS.caller !!! 456)
        처리.test3
    그외
        처리.test3
}
처리::test3
{
    만약에(수신메시지.PARAMS.caller === 0101234)
        처리.test4
    그외
        처리.test4
}
처리::test4
{
    만약에(수신메시지.PARAMS.caller.길이 == 10)
        처리.test5
    그외
        처리.test5
}
처리::test5
{
    만약에(세션.caller.길이 <= 11)
        처리.test6
    그외
        처리.test6
}
처리::test6
{
    만약에(수신메시지.PARAMS.id == 1)
        처리.test99
    그외
        처리.test99
}
처리::test99
{
    만약에(참)
        함수.저장(bcp_reason,1234)
        함수.저장(bcp_reason2, 수신메시지.PARAMS.caller)
        함수.출력(print_test,%010s,123)
        함수.출력(print_test2,%-15s,수신메시지.PARAMS.caller)
        함수.출력(print_test3,%15s,수신메시지.PARAMS.caller)
        함수.날짜(curDate0,%Y-%m-%d)
        함수.날짜(curDate1,문장.sntDate1)
        함수.날짜(curDate2,문장.sntDate2)
        함수.날짜(curDate3,문장.sntDate3)
        함수.날짜(curDate4,문장.sntDate4)
        함수.날짜(curDate5,문장.sntDate5)
        함수.날짜(curDate6,문장.sntDate6)
        함수.날짜(curDate7,문장.sntDate7)
        함수.저장(cnt,10)
        함수.더하기(sum_result,세션.cnt,1)
        함수.빼기(cnt,세션.cnt,2)
        함수.나누기(avg, 수신메시지.PARAMS.called,3)
        함수.나머지(idx, 수신메시지.PARAMS.called,5)
        함수.붙이기(name,a,b,->, 수신메시지.PARAMS.caller)
        함수.부분비교(strstr,수신메시지.PARAMS.caller,456)
        함수.앞자리비교(strncmp, 수신메시지.PARAMS.caller,01012)
        함수.삽입(strInsert, 세션.caller,aaa,0)
        함수.삭제(strDelete, 세션.caller,0,3)
        함수.추출(strExtract, 세션.caller,0,3)
        함수.저장(rec_id, 수신메시지.PARAMS.id)
        함수.시간(unix_time)
        함수.클럭(clock_time)
        함수.저장(test,0)
}
처리::proc0_init
{
    만약에(참)
        처리.test0
}
처리::proc1_init
{
    만약에(참)
        상태변경.st_ringing
}
처리::proc2
{
    만약에(세션.STATE != st_ringing)
        상태변경.중지
    그외
        타이머.longcall
        전송.sndRsp
        상태변경.st_connected
}
처리::proc3_ringing
{
    만약에(참)
        타이머.ring 
}
처리::proc4_connected
{
    만약에(참)
        타이머.longcall
}
처리::proc99_recursive
{
    만약에(세션.func_cnt <= 4)
        유틸.func_counting
        처리.proc99_recursive
        함수.통계(recursive_cnt)
        함수.통계(recursive_cnt)
    그외
        유틸.func_counting
        상태변경.종료
}
처리::proc7_init
{
    만약에(참)
        타이머.init_timer
}
처리::proc9_connected
{
    만약에(참)
        상태변경.중지
}
처리::procInitTimeout
{
    만약에(참)
        상태변경.종료
}
처리::procRingTimeout
{
    만약에(참)
        상태변경.st_connected
}
처리::procLongTimeout
{
    만약에(참)
        전송.sndBye
        상태변경.중지
}
/* claude-code 2026-08-30 - 타이머 3개 전부 "전송메시지.PARAMS.시간"(ring은
   "PARAM.시간" 오타까지)을 쓰고 있었는데, EXEACTION.cpp ExeAction::EXE_TIMER()의
   IS_DSL_K_TIME() 분기는 LHS가 정확히 "시간"/"TIME"일 때만 타므로(재귀
   dot 분리 없음) "PARAMS.시간"은 이 분기를 못 타고 일반 PARAMS 필드로만
   채워져 실제 타이머 만료시각(API_P_TIME)이 끝내 미설정(0ms)으로 남는다 -
   fs_tick 자기재무장 타이머가 초당 168회로 폭주한 것과 같은 결함(File RA
   동시성재설계 작업 중 실측 확인, notebookflow_engine_status 메모리 참고).
   "전송메시지.시간"(PARAMS 접두어 제거)이 올바른 문법이다. tmp_cnd는
   시간/타입이 아닌 일반 PARAMS 필드라 원래도 문제없어 그대로 뒀다. */
타이머::init_timer
{
    전송메시지.타입 = init_timeout
    전송메시지.시간 = 3000
}
타이머::ring
{
    전송메시지.타입 = ring_timeout
    전송메시지.시간 = 5000
}
타이머::longcall
{
    전송메시지.타입 = longcall_timeout
    전송메시지.시간 = 20000
    전송메시지.PARAMS.tmp_cnd = haha
}
전송::sndRsp
{
    전송메시지.타입 = rsp_info
    전송메시지.SERVICE_TYPE = MH
    전송메시지.주소    = 수신메시지.주소
    전송메시지.PARAMS.caller = 세션.caller
    전송메시지.PARAMS.called = 세션.called
    전송메시지.PARAMS.reason = long_call_time_out
    전송메시지.PARAMS.result = ok
    전송메시지.detailed.dev = 세션.caller
    전송메시지.detailed.type = rsp
}
전송::sndBye
{
    전송메시지.타입 = bye
    전송메시지.주소    = INI.RA_ADDRESS.rag-ra
    전송메시지.PARAMS.caller = 세션.caller
    전송메시지.PARAMS.called = 세션.called
    전송메시지.PARAMS.reason = long_call_time_out
    전송메시지.PARAMS.result = ok
}
문장::sntDate1
{%Y-%m-%d}
문장::sntDate2
{%F}
문장::sntDate3
{%Y/%m/%d %H:%M:%S}
문장::sntDate4
{%T}
문장::sntDate5
{%Y %m %d }
문장::sntDate6
{%A %B %d %Y}
문장::sntDate7
{Y%m%d %H%M%S}
