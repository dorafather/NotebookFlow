/*
	filesystem_watch.sce - 자기재무장(self-rearming) 타이머 세션.
	업무지침_FileRA_동시성재설계.md + 설계_2차확정_RA무상태화_세션생애주기.md
	산출물 2 (dsl-process-developer, 2026-08-30).

	⚠️ 이 파일은 "등록"되어 실행되는 게 아니라 문서/리뷰용 사본이다(2026-08-29
	사본과 동일한 사정 - Flow::RUNFLOW()가 "./system.sce"/"./user.sce" 두
	고정 경로만 열고, 이 엔진에는 list.cfg류 "시나리오 이름 -> 파일" 등록
	메커니즘이 없다). 실제로 로드되는 내용은 NotebookFlow-release/NotebookFlow/
	user.sce 끝부분(96번째 줄, "dsl-process-developer 2026-08-30" 주석
	이후)에 그대로 병합돼 있다.

	2026-08-29에 작성했던 구버전(fs_scan_start/fs_process_file/fs_scan_done,
	File RA가 능동 스케줄러를 갖던 가정)은 설계가 완전히 뒤집혀 폐기됐다.

	방향 전환:
	  - 예전: File RA가 watch_list.json으로 스스로 스캔 주기를 갖고, 새
	    파일을 발견하면 스스로 PUT을 쏴서 세션을 시작시켰다.
	  - 지금: File RA는 순수 무상태 명령 실행 서버(POST/PUT /command)다.
	    "언제 부를지"는 이 세션 자신이 ACT_TIMER로 스스로 결정한다.

	흐름:
	  1) START_REQ(ACTION_EVENT=fs_watch_start,
	     PARAMS.원본디렉토리/대상디렉토리/명령=예:이동)
	     -> procFsWatchStart: PARAMS를 세션 변수(fs_src_dir/fs_dst_dir/
	        fs_cmd)로 저장 -> 첫 명령을 File RA에 즉시 전송(sndFsWatchCmd)
	        -> 상태변경.st_watching (GOTO는 이후 statement를 건너뛰므로
	        항상 마지막에 둔다 - EXEPROC.cpp 확인).
	  2) File RA의 즉시 ack가 ACTION_EVENT=ra_response로 돌아옴(Main.cpp
	     RA호출 재투입 규칙, 고정) -> procFsWatchAck(st_watching 전용,
	     상태::초기의 회귀 검증용 ra_response 핸들러와 상태별로 분리돼
	     안 겹침).
	  3) File RA의 asyncio Job이 진행되며 fs_job_progress(진행)/
	     fs_job_done(완료)을 PUT으로 흘려보냄 -> procFsJobProgress/
	     procFsJobDone이 세션.sse_addr(있으면)로 중계.
	  4) procFsJobDone 마지막에 타이머.fs_tick으로 재무장 -> 30초 뒤
	     procFsTick -> 다시 File RA 호출(2로 복귀). STOP_REQ가 올 때까지
	     무한 반복 - "PC가 꺼질 때까지 산다"는 게 이 루프 자체를 뜻한다.

	⚠️ 실측 확인된 엔진 결함/제약 3가지(이번 범위에서 엔진은 안 고치고
	설계로 우회. a는 실제 런타임 폭주로 재현한 뒤 찾은 진짜 버그다):
	  a) ⚠️ 최초 구현 때 sample_kor.sce 관례대로 "전송메시지.PARAMS.시간
	     = 30000"으로 썼다가 자기재무장 루프가 30초가 아니라 초당
	     20~30회로 폭주하는 사고를 실제로 겪었다(File RA 로그에서 1초
	     미만 사이 168회 재무장 확인 - STOP_REQ로 강제 종료해 막음).
	     원인: AssignParser::m_AB가 LHS를 첫 "."에서만 자르므로
	     b="PARAMS.시간"이 되는데, EXEACTION.cpp IS_DSL_K_TIME(b)는 b가
	     "시간"/"TIME"과 완전일치할 때만 타므로 이 경우 안 타고, arg의
	     실제 API_P_TIME이 끝내 세팅 안 돼 TIMER.cpp가 0을 읽어 사실상
	     0ms 타이머(즉시 재무장 폭주)가 된다. **올바른 문법은 "PARAMS."
	     접두사 없이 "전송메시지.시간 = 30000"** - 아래 코드가 이 형태다.
	  b) 타이머 주기(전송메시지.시간)는 리터럴 상수만 지원. a에서 찾은
	     올바른 분기조차 ExeParam::PARAM()을 거치지 않고 RHS 토큰을
	     그대로 대입하기 때문 - "세션.watch_interval" 같은 참조가 안
	     먹힌다. 그래서 30000(ms) 고정.
	  c) POOL 세션 GC 유휴 타임아웃 기본값 60초(SESSION.cpp
	     POOLOPTION::m_nAliveTimeSec, FLOW.cpp의 s_opt가 재정의 안 함) -
	     타이머 주기가 60초보다 짧아야 매 타이머 발동이 GC를 피한다.

	파일 목록을 이 .sce 레이어로 들여오지 않는 이유: 전송메시지.PARAMS.xxx
	= 수신메시지.PARAMS.yyy 대입(EXEPARAM.cpp)이 KSTRING 스칼라 문자열
	복사만 지원해서, RA가 돌려준 파일 배열을 세션에 담았다가 다음 RA
	호출에 재주입하는 경로 자체가 이 엔진에 없다. 대신 "원본경로 폴더
	전체에 명령 적용"이라는 배치 하나만 매 tick 새로 호출한다 - "이동"은
	원본에서 파일을 들어내므로 멱등적이라 File RA 쪽에 지문 비교 상태를
	둘 필요가 없다(RA 무상태 원칙과 일치).
*/
처리::procFsWatchStart
{
    만약에(참)
        함수.저장(fs_src_dir,수신메시지.PARAMS.원본디렉토리)
        함수.저장(fs_dst_dir,수신메시지.PARAMS.대상디렉토리)
        함수.저장(fs_cmd,수신메시지.PARAMS.명령)
        전송.sndFsWatchCmd
        상태변경.st_watching
}
처리::procFsTick
{
    만약에(참)
        전송.sndFsWatchCmd
}
처리::procFsWatchAck
{
    만약에(참)
        LOG.info(file-ra command accepted)
}
처리::procFsJobProgress
{
    만약에(참)
        전송.sndFsProgressPush
}
처리::procFsJobDone
{
    만약에(참)
        전송.sndFsDonePush
        타이머.fs_tick
}
처리::procFsWatchTerm
{
    만약에(참)
        LOG.info(filesystem_watch session terminated)
}
타이머::fs_tick
{
    전송메시지.타입 = fs_tick
    전송메시지.시간 = 30000
}
전송::sndFsWatchCmd
{
    전송메시지.타입 = fs_watch_cmd
    전송메시지.주소    = INI.RA_ADDRESS.file-ra
    전송메시지.PARAMS.명령 = 세션.fs_cmd
    전송메시지.PARAMS.원본경로 = 세션.fs_src_dir
    전송메시지.PARAMS.대상경로 = 세션.fs_dst_dir
    전송메시지.PARAMS.callback_as_id = 세션.AS_ID
}
전송::sndFsProgressPush
{
    전송메시지.타입 = fs_job_progress_push
    전송메시지.주소    = 세션.sse_addr
    전송메시지.PARAMS.processed = 수신메시지.PARAMS.processed
    전송메시지.PARAMS.total = 수신메시지.PARAMS.total
}
전송::sndFsDonePush
{
    전송메시지.타입 = fs_job_done_push
    전송메시지.주소    = 세션.sse_addr
    전송메시지.PARAMS.processed = 수신메시지.PARAMS.processed
    전송메시지.PARAMS.total = 수신메시지.PARAMS.total
    전송메시지.PARAMS.cancelled = 수신메시지.PARAMS.cancelled
}
상태::st_watching
{
    fs_tick           처리.procFsTick
    ra_response       처리.procFsWatchAck
    fs_job_progress   처리.procFsJobProgress
    fs_job_done       처리.procFsJobDone
    소멸              처리.procFsWatchTerm
}
