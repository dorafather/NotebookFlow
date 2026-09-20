/*
  Test message for 함수.반복 validation.
  Send via: nats pub PUB.DSL '<json>'

{
  "SCE_EVENT": "START_REQ",
  "ACTION_EVENT": "START",
  "AS_ID": 99,
  "PARAMS":
  {
    "id": 1,
    "caller": "01012345678",
    "called": "01099998888",
    "SCE_NAME": "반복_test",
    "input_rows":
    [
      {
        "SUB_ID": "2",
        "NAME": "김철수",
        "MSISDN": "01087654321",
        "COLORING_YN": "1"
      },
      {
        "SUB_ID": "3",
        "NAME": "이영희",
        "MSISDN": "01011112222",
        "COLORING_YN": "0"
      },
      {
        "SUB_ID": "4",
        "NAME": "박지성",
        "MSISDN": "01033334444",
        "COLORING_YN": "1"
      },
      {
        "SUB_ID": "5",
        "NAME": "손흥민",
        "MSISDN": "01055556666",
        "COLORING_YN": "1"
      },
      {
        "SUB_ID": "6",
        "NAME": "김연아",
        "MSISDN": "01077778888",
        "COLORING_YN": "1"
      }
    ]
  }
}
*/

SCE::NAME
{
  반복_test
}
상태::초기
{
  START   처리.pLoopInit
  소멸    처리.pTerm
}
상태::종료
{
  소멸    처리.pTerm
}
처리::pLoopInit
{
  만약에(참)
	함수.저장(caller, 수신메시지.PARAMS.caller)
	함수.저장(called, 수신메시지.PARAMS.called)
    처리.pBuildList
}
처리::pBuildList
{
  만약에(참)
    함수.컴프리헨션(name_list,수신메시지.PARAMS.input_rows,1)
    함수.컴프리헨션(msisdn_list,수신메시지.PARAMS.input_rows,2)
    처리.pLoopStart
}
처리::pLoopStart
{
  만약에(참)
    함수.반복(name_list, pIterProc)
    상태변경.종료
}
처리::pIterProc
{
  만약에(반복횟수 > 0) 그리고 (반복횟수 < 3)
    함수.저장(loop_test,1)   
  그외그외(반복횟수 == 3)
    함수.저장(loop_test,2)
    상태변경.중지
  그외
    함수.저장(loop_test,0)
}
처리::pTerm
{
  만약에(참)
    함수.저장(loop_test,끝)
}
