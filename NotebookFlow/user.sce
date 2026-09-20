상태::FLOW.초기
{
  FLOW.수신메시지.이벤트명 == event_0    처리.procInit
  FLOW.수신메시지.이벤트명 == ping    처리.procPing
  FLOW.수신메시지.이벤트명 == sse_connect    처리.procSseAck
  FLOW.수신메시지.이벤트명 == call_ra    처리.procCallRa
  FLOW.수신메시지.이벤트명 == ra_response    처리.procRaDone
  FLOW.수신메시지.이벤트명 == regr    처리.procRegr
  FLOW.수신메시지.이벤트명 == 소멸    처리.procTerm
  FLOW.수신메시지.이벤트명 == 규칙목록    처리.procRuleListInit
  FLOW.수신메시지.이벤트명 == 규칙추가요청    처리.procRuleAddRequest
  FLOW.수신메시지.이벤트명 == 대기중요청조회    처리.procRuleAddRequestQuery
  FLOW.수신메시지.app_event == service_req    처리.procNonProto
}
상태::FLOW.SERVICE
{
  FLOW.수신메시지.이벤트명 == 규칙목록갱신    처리.procRuleListUpdate
  FLOW.수신메시지.이벤트명 == 규칙추가요청    처리.procRuleAddRequest
  FLOW.수신메시지.이벤트명 == 대기중요청조회    처리.procRuleAddRequestQuery
  FLOW.수신메시지.이벤트명 == 중지    처리.procSuspend
  FLOW.수신메시지.이벤트명 == ra_response    처리.procRuleRaAck
  FLOW.수신메시지.이벤트명 == 소멸    처리.procTerm
}
상태::FLOW.SUSPEND
{
  FLOW.수신메시지.이벤트명 == 규칙목록갱신    처리.procRuleListUpdate
  FLOW.수신메시지.이벤트명 == 규칙추가요청    처리.procRuleAddRequest
  FLOW.수신메시지.이벤트명 == 대기중요청조회    처리.procRuleAddRequestQuery
  FLOW.수신메시지.이벤트명 == 재개    처리.procResume
  FLOW.수신메시지.이벤트명 == ra_response    처리.procRuleRaAck
  FLOW.수신메시지.이벤트명 == 소멸    처리.procTerm
}
상태::FLOW.regr_done
{
  FLOW.수신메시지.이벤트명 == ping    처리.procPing
  FLOW.수신메시지.이벤트명 == 소멸    처리.procTerm
}
처리::FLOW.procNonProto
{
  만약에(참)
    로그.출력(Wow None Proto Message 수신메시지.app_event)
    로그.출력(Number 0 = 수신메시지.body.number_list[0])
    로그.출력(Number 1 = 수신메시지.body.number_list[1])
    로그.출력(Number 2 = 수신메시지.body.number_list[2])
    함수.객체저장(echo_body,수신메시지.body)
    전송.sndNonproto
}
처리::FLOW.procInit
{
  만약에(참)
    함수.저장(hits,0)
    로그.출력(세션'hits 세션.hits)
}
처리::FLOW.procPing
{
  만약에(참)
    전송.sndPong
}
처리::FLOW.procSseAck
{
  만약에(참)
    함수.주소저장(sse_addr,수신메시지.주소)
    전송.sndSubAck
}
처리::FLOW.procCallRa
{
  만약에(참)
    전송.sndRa
}
처리::FLOW.procRaDone
{
  만약에(참)
    전송.sndRaDone
}
처리::FLOW.procTerm
{
  만약에(참)
}
처리::FLOW.procRegr
{
  만약에(참)
    함수.저장(regr_val,before)
    처리.procRegrHelper
    상태변경.regr_done
}
처리::FLOW.procRegrHelper
{
  만약에(참)
    함수.더하기(regr_num,1,2)
    함수.저장(regr_val,after)
}
처리::FLOW.procRuleListInit
{
  만약에(참)
    처리.procRuleListUpdate
    상태변경.SERVICE
}
처리::FLOW.procRuleListUpdate
{
  만약에(참)
    함수.객체저장(규칙목록,수신메시지.file_rules)
    전송.sndRuleListToFileRa
}
처리::FLOW.procSuspend
{
  만약에(참)
    전송.sndFileRaPause
    상태변경.SUSPEND
}
처리::FLOW.procResume
{
  만약에(참)
    전송.sndFileRaResume
    상태변경.SERVICE
}
처리::FLOW.procRuleRaAck
{
  만약에(참)
}
처리::FLOW.procRuleAddRequest
{
  만약에(참)
    함수.객체저장(대기중요청목록,수신메시지.PARAMS)
    전송.sndRuleAddRequestAck
}
처리::FLOW.procRuleAddRequestQuery
{
  만약에(참)
    전송.sndPendingProposal
    함수.객체저장(대기중요청목록,수신메시지.PARAMS.제안)
}
전송::FLOW.sndNonproto
{
  전송메시지.app_event = service_rsp
  전송메시지.body.id = 1234
  전송메시지.body.name = hoho
  전송메시지.body = 세션.객체.echo_body
}
전송::FLOW.sndPong
{
  전송메시지.이벤트명 = pong
  전송메시지.주소 = 수신메시지.주소
  전송메시지.PARAMS.echo = 수신메시지.PARAMS.msg
}
전송::FLOW.sndSubAck
{
  전송메시지.이벤트명 = sub_ack
  전송메시지.주소 = 세션.주소.sse_addr
  전송메시지.PARAMS.status = subscribed
}
전송::FLOW.sndRa
{
  전송메시지.이벤트명 = ra_call
  전송메시지.주소.도메인 = 설정.RA_ADDRESS.rag-ra-domain
  전송메시지.주소.경로 = /rag/search
  전송메시지.메소드 = POST
  전송메시지.query = 수신메시지.PARAMS.q
}
전송::FLOW.sndRaDone
{
  전송메시지.이벤트명 = ra_done
  전송메시지.주소 = 세션.주소.sse_addr
  전송메시지.PARAMS.result = 수신메시지.PARAMS.result
  전송메시지.PARAMS.echo = 수신메시지.PARAMS.echo
}
전송::FLOW.sndFileRaPause
{
  전송메시지.이벤트명 = file_ra_pause
  전송메시지.주소.도메인 = 설정.RA_ADDRESS.file-ra-domain
  전송메시지.주소.경로 = /command
  전송메시지.메소드 = POST
  전송메시지.PARAMS.명령 = 일시정지
}
전송::FLOW.sndFileRaResume
{
  전송메시지.이벤트명 = file_ra_resume
  전송메시지.주소.도메인 = 설정.RA_ADDRESS.file-ra-domain
  전송메시지.주소.경로 = /command
  전송메시지.메소드 = POST
  전송메시지.PARAMS.명령 = 재개
}
전송::FLOW.sndRuleListToFileRa
{
  전송메시지.이벤트명 = 규칙목록
  전송메시지.주소.도메인 = 설정.RA_ADDRESS.file-ra-domain
  전송메시지.주소.경로 = /command
  전송메시지.메소드 = POST
  전송메시지.PARAMS.명령 = 규칙목록
  전송메시지.PARAMS.exe_rules = 세션.객체.규칙목록
}
전송::FLOW.sndRuleAddRequestAck
{
  전송메시지.이벤트명 = 규칙추가요청_ack
  전송메시지.주소 = 수신메시지.주소
  전송메시지.PARAMS.status = 접수됨
}
전송::FLOW.sndPendingProposal
{
  전송메시지.이벤트명 = 대기중요청응답
  전송메시지.주소 = 수신메시지.주소
  전송메시지.PARAMS.제안 = 세션.객체.대기중요청목록
}
