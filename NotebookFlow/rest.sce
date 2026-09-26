상태::FLOW.초기
{
  FLOW.수신메시지.이벤트명 == 시작    처리.procRestInit
  FLOW.수신메시지.이벤트명 == 슬랙알람전송    처리.슬랙알람직접트리거
  FLOW.수신메시지.이벤트명 == 디스코드알람전송    처리.디스코드알람직접트리거
  FLOW.수신메시지.이벤트명 == Ntfy알람전송    처리.Ntfy알람직접트리거
  FLOW.수신메시지.이벤트명 == 텔레그램알람전송    처리.텔레그램알람직접트리거
  FLOW.수신메시지.이벤트명 == sndUpText    처리.게시글전송트리거
  FLOW.수신메시지.이벤트명 == 클루드코드완료    처리.클루드코드완료처리
  FLOW.수신메시지.이벤트명 == 깃허브이슈등록    처리.깃허브이슈등록트리거
  FLOW.수신메시지.이벤트명 == 깃허브이슈폴링    처리.깃허브이슈폴링처리
  FLOW.수신메시지.이벤트명 == NF봇타이머    처리.NF봇핑전송
  FLOW.수신메시지.이벤트명 == NF봇그룹메시지전송    처리.NF봇그룹메시지직접트리거
  FLOW.수신메시지.이벤트명 == 업데이트확인틱    처리.업데이트확인처리
  UPDATE.수신메시지.응답코드 == 200    처리.업데이트버전확인응답처리
  UPDATE.수신메시지.응답코드 == 403    처리.업데이트확인실패처리
  UPDATE.수신메시지.응답코드 == 404    처리.업데이트확인실패처리
  UPDATE_DL.수신메시지.저장모드 == 1    처리.업데이트파일다운로드완료처리
  FLOW.수신메시지.이벤트명 == 주식감시틱    처리.주식관심종목감시처리
  KRX.수신메시지.응답코드 == 200    처리.KRX응답분기처리
  FLOW.수신메시지.이벤트명 == 기상알림틱    처리.기상알림확인처리
  KMA.수신메시지.응답코드 == 200    처리.KMA응답분기처리
  TELEGRAM.수신메시지.이벤트명 == 텔레그램타이머    처리.텔레그램핑전송
  TELEGRAM.수신메시지.이벤트명 == 텔레그램환영타이머    처리.텔레그램환영처리
  TELEGRAM.수신메시지.ok == 1    처리.텔레그램수신처리
  TELEGRAM.수신메시지.저장모드 == 1    처리.텔레그램파일저장완료처리
  TELEGRAM.수신메시지.폴더모드 == 1    처리.텔레그램폴더응답처리
  TELEGRAM.수신메시지.이미지모드 == 1    처리.텔레그램help이미지응답처리
  DISCORD.수신메시지.응답코드 == 204    처리.디스코드알람응답처리
  NTFY.수신메시지.event == message    처리.Ntfy응답처리
  SLACK.수신메시지.응답코드 == 200    처리.슬랙응답처리
  GITHUB.수신메시지.응답코드 == 200    처리.깃허브테스트응답처리
  GITHUB.수신메시지.응답코드 == 201    처리.깃허브이슈등록응답처리
  GITHUB.수신메시지.응답코드 == 403    처리.깃허브이슈등록실패처리
  GITHUB.수신메시지.응답코드 == 404    처리.깃허브이슈등록실패처리
  GITHUB.수신메시지.응답코드 == 422    처리.깃허브이슈등록실패처리
  BSKY.수신메시지.handle == dorafather.bsky.social    처리.procBskyTokRcv
  LINEAR.수신메시지.주소.경로 == /graphql    처리.procLinearRsp
  SSH.수신메시지.응답코드 == 200    처리.sshRsp
}
처리::FLOW.procRestInit
{
  만약에(참)
    처리.텔레그램초기화
    처리.깃허브이슈기준선설정
    처리.NF봇초기화
    타이머.업데이트확인타이머
    처리.주식관심종목초기화
    타이머.주식감시타이머
    처리.기상청관심지역초기화
    타이머.기상알림타이머
}
처리::FLOW.클루드코드완료처리
{
  만약에(참)
    함수.저장(agent_chat_id,수신메시지.chat_id)
    함수.저장(agent_result,수신메시지.result)
    전송.클루드코드완료텔레그램전송
}
처리::FLOW.업데이트확인처리
{
  만약에(참)
    함수.날짜(update_check_hour,%H)
    타이머.업데이트확인타이머
    처리.업데이트확인시각비교
}
처리::FLOW.업데이트확인시각비교
{
  만약에(세션.update_check_hour == 설정.INFO.install_hour)
    함수.저장(update_mode,확인)
    함수.저장(gh_token,설정.GITHUB.token)
    전송.업데이트버전확인전송
  그외
    로그.출력(업데이트 확인 스킵 - 시각 불일치)
}
처리::UPDATE.업데이트버전확인응답처리
{
  만약에(세션.update_mode == 확인) 그리고(수신메시지.sha != 설정.INFO.last_applied_version) 그리고(설정.INFO.last_applied_version != NULL)
    함수.저장(update_new_sha,수신메시지.sha)
    전송.업데이트알림텔레그램전송
  그외그외(세션.update_mode == 실행)
    함수.저장(update_new_sha,수신메시지.sha)
    함수.저장(update_dl_step,1)
    전송.업데이트파일1다운로드
  그외
    로그.출력(업데이트 확인 - 새 버전 없음 또는 기준선 없음)
}
처리::UPDATE.업데이트확인실패처리
{
  만약에(참)
    로그.출력(업데이트 버전 확인 실패 수신메시지.message)
}
처리::UPDATE.업데이트파일다운로드완료처리
{
  만약에(세션.update_dl_step == 1)
    함수.저장(update_dl_step,2)
    전송.업데이트파일2다운로드
  그외
    전송.업데이트에이전트명령전송
}
처리::TELEGRAM.텔레그램업데이트명령처리
{
  만약에(참)
    함수.저장(update_mode,실행)
    함수.저장(gh_token,설정.GITHUB.token)
    전송.델레그램응답전송
    전송.업데이트버전확인전송
}
처리::TELEGRAM.텔레그램초기화
{
  만약에(참)
    함수.저장(tg_offset,0)
    함수.저장(idx,0)
    처리.텔레그램핑전송
    타이머.텔레그램환영타이머
}
처리::TELEGRAM.텔레그램핑전송
{
  만약에(참)
    함수.저장(tg_token,설정.TELEGRAM.bot_token)
    전송.텔레그램폴
    타이머.텔레그램타이머
}
처리::TELEGRAM.텔레그램환영처리
{
  만약에(참)
    전송.텔레그램환영메시지전송
    전송.텔레그램help이미지생성
}
처리::TELEGRAM.텔레그램수신처리
{
  만약에(수신메시지.주소.경로 !!! 세션.nfbot_token)
    처리.NF봇수신처리
  그외그외(수신메시지.result[0].update_id != NULL)
    처리.텔레그램에코응답처리
  그외그외(수신메시지.result.file_path != NULL)
    처리.텔레그램파일다운로드
  그외
    로그.출력(telegram 새 메시지 없음)
}
처리::TELEGRAM.텔레그램에코응답처리
{
  만약에(수신메시지.result[0].message.from.id == 설정.TELEGRAM.my_chat_id)
    함수.더하기(tg_offset,수신메시지.result[0].update_id,1)
    함수.앞자리비교(cmd_help,수신메시지.result[0].message.text,help)
    함수.앞자리비교(cmd_깃허브,수신메시지.result[0].message.text,깃허브)
    함수.앞자리비교(cmd_슬랙,수신메시지.result[0].message.text,슬랙)
    함수.앞자리비교(cmd_디스코드,수신메시지.result[0].message.text,디스코드)
    함수.앞자리비교(cmd_노티파이,수신메시지.result[0].message.text,노티파이)
    함수.앞자리비교(cmd_블루스카이,수신메시지.result[0].message.text,블루스카이)
    함수.앞자리비교(cmd_클루드코드,수신메시지.result[0].message.text,클루드코드)
    함수.앞자리비교(cmd_클루드초기화,수신메시지.result[0].message.text,클루드초기화)
    함수.앞자리비교(cmd_텔넷,수신메시지.result[0].message.text,텔넷)
    함수.앞자리비교(cmd_파일,수신메시지.result[0].message.text,파일)
    함수.앞자리비교(cmd_사진,수신메시지.result[0].message.text,사진)
    함수.앞자리비교(cmd_고객문의,수신메시지.result[0].message.text,고객문의)
    함수.앞자리비교(cmd_업데이트,수신메시지.result[0].message.text,업데이트)
    함수.앞자리비교(cmd_주식,수신메시지.result[0].message.text,주식)
    함수.앞자리비교(cmd_기상청,수신메시지.result[0].message.text,기상청)
    처리.텔레그램수신메시지분기
  그외
    함수.더하기(tg_offset,수신메시지.result[0].update_id,1)
    로그.출력(알 수 없는 발신자로부터의 메시지 무시)
}
처리::TELEGRAM.텔레그램수신메시지분기
{
  만약에(수신메시지.result[0].message.document.file_id != NULL)
    처리.텔레그램파일업로드감지
  그외그외(수신메시지.result[0].message.photo[0].file_id != NULL)
    처리.텔레그램사진업로드감지
  그외
    처리.텔레그램명령분기
}
처리::TELEGRAM.텔레그램파일업로드감지
{
  만약에(참)
    함수.저장(download_file_id,수신메시지.result[0].message.document.file_id)
    함수.저장(download_file_name,수신메시지.result[0].message.document.file_name)
    로그.출력(파일 업로드 감지 세션.download_file_name)
    전송.텔레그램파일메타조회
}
처리::TELEGRAM.텔레그램사진업로드감지
{
  만약에(수신메시지.result[0].message.photo[3].file_id != NULL)
    함수.저장(download_file_id,수신메시지.result[0].message.photo[3].file_id)
    처리.텔레그램사진업로드감지2
  그외그외(수신메시지.result[0].message.photo[2].file_id != NULL)
    함수.저장(download_file_id,수신메시지.result[0].message.photo[2].file_id)
    처리.텔레그램사진업로드감지2
  그외그외(수신메시지.result[0].message.photo[1].file_id != NULL)
    함수.저장(download_file_id,수신메시지.result[0].message.photo[1].file_id)
    처리.텔레그램사진업로드감지2
  그외
    함수.저장(download_file_id,수신메시지.result[0].message.photo[0].file_id)
    처리.텔레그램사진업로드감지2
}
처리::TELEGRAM.텔레그램사진업로드감지2
{
  만약에(참)
    함수.저장(download_file_name,notebookflow_)
    함수.날짜(photo_ts,%Y%m%d%H%M%S)
    함수.붙이기(download_file_name,세션.photo_ts,_,세션.download_file_id,.jpg)
    로그.출력(사진 업로드 감지 세션.download_file_id)
    전송.텔레그램파일메타조회
}
처리::TELEGRAM.텔레그램파일다운로드
{
  만약에(참)
    함수.저장(download_file_path,수신메시지.result.file_path)
    전송.텔레그램파일실제다운로드
}
처리::TELEGRAM.텔레그램파일저장완료처리
{
  만약에(수신메시지.저장성공 == 1)
    로그.출력(파일 저장 완료 세션.download_file_name)
    전송.텔레그램파일저장알림전송
  그외
    로그.출력(파일 저장 실패)
}
처리::TELEGRAM.텔레그램폴더응답처리
{
  만약에(수신메시지.성공 == 1)
    전송.텔레그램폴더회신전송
  그외
    로그.출력(폴더 조회 실패)
}
처리::TELEGRAM.텔레그램help이미지응답처리
{
  만약에(수신메시지.성공 == 1)
    함수.저장(photo_path,수신메시지.이미지경로)
    전송.텔레그램사진전송
  그외
    로그.출력(help 이미지 생성 실패)
}
처리::TELEGRAM.텔레그램명령분기
{
  만약에(세션.cmd_help == 1)
    전송.텔레그램help이미지생성
  그외그외(세션.cmd_깃허브 == 1)
    처리.텔레그램깃허브명령처리
  그외그외(세션.cmd_슬랙 == 1)
    처리.텔레그램슬랙명령처리
  그외그외(세션.cmd_디스코드 == 1)
    처리.텔레그램디스코드명령처리
  그외그외(세션.cmd_노티파이 == 1)
    처리.텔레그램노티파이명령처리
  그외그외(세션.cmd_블루스카이 == 1)
    처리.텔레그램블루스카이명령처리
  그외그외(세션.cmd_클루드코드 == 1)
    처리.텔레그램클루드코드명령처리
  그외그외(세션.cmd_클루드초기화 == 1)
    처리.텔레그램클루드초기화명령처리
  그외그외(세션.cmd_텔넷 == 1)
    처리.텔레그램텔넷명령처리
  그외그외(세션.cmd_파일 == 1)
    처리.텔레그램파일명령처리
  그외그외(세션.cmd_사진 == 1)
    처리.텔레그램사진명령처리
  그외그외(세션.cmd_고객문의 == 1)
    처리.텔레그램고객문의명령처리
  그외그외(세션.cmd_업데이트 == 1)
    처리.텔레그램업데이트명령처리
  그외그외(세션.cmd_주식 == 1)
    처리.텔레그램주식명령처리
  그외그외(세션.cmd_기상청 == 1)
    처리.텔레그램기상청명령처리
  그외
    전송.델레그램응답전송
}
처리::TELEGRAM.텔레그램사진명령처리
{
  만약에(참)
    함수.단어분리(photo_cmd_word_list,수신메시지.result[0].message.text)
    함수.단어합치기(cmd_rest,세션.리스트.photo_cmd_word_list,1)
    함수.앞자리비교(cmd_타입,세션.cmd_rest,얻기)
    처리.텔레그램사진명령분기
}
처리::TELEGRAM.텔레그램사진명령분기
{
  만약에(세션.cmd_타입 == 1)
    함수.단어분리(photo_type_word_list,세션.cmd_rest)
    함수.단어합치기(photo_path,세션.리스트.photo_type_word_list,1)
    로그.출력(사진 요청 세션.photo_path)
    전송.텔레그램사진전송
  그외
    전송.델레그램응답전송
}
처리::TELEGRAM.텔레그램파일명령처리
{
  만약에(참)
    함수.단어분리(cmd_word_list,수신메시지.result[0].message.text)
    함수.단어합치기(cmd_rest,세션.리스트.cmd_word_list,1)
    함수.앞자리비교(cmd_타입,세션.cmd_rest,얻기)
    함수.앞자리비교(cmd_폴더,세션.cmd_rest,폴더)
    처리.텔레그램파일명령분기
}
처리::TELEGRAM.텔레그램파일명령분기
{
  만약에(세션.cmd_타입 == 1)
    함수.단어분리(type_word_list,세션.cmd_rest)
    함수.단어합치기(file_path,세션.리스트.type_word_list,1)
    로그.출력(파일 요청 세션.file_path)
    전송.텔레그램파일전송
  그외그외(세션.cmd_폴더 == 1)
    함수.단어분리(dir_word_list,세션.cmd_rest)
    함수.단어합치기(dir_path,세션.리스트.dir_word_list,1)
    로그.출력(폴더 조회 요청 세션.dir_path)
    전송.텔레그램폴더조회
  그외
    전송.델레그램응답전송
}
처리::TELEGRAM.텔레그램깃허브명령처리
{
  만약에(참)
    함수.단어분리(cmd_word_list,수신메시지.result[0].message.text)
    함수.단어합치기(cmd_rest,세션.리스트.cmd_word_list,1)
    함수.앞자리비교(cmd_이슈등록,세션.cmd_rest,이슈등록)
    함수.앞자리비교(cmd_타입,세션.cmd_rest,이슈)
    처리.텔레그램깃허브명령분기
}
처리::TELEGRAM.텔레그램깃허브명령분기
{
  만약에(세션.cmd_이슈등록 == 1)
    함수.저장(pending_reply_chat_id,수신메시지.result[0].message.chat.id)
    처리.텔레그램깃허브이슈등록명령처리
  그외그외(세션.cmd_타입 == 1)
    함수.저장(pending_reply_chat_id,수신메시지.result[0].message.chat.id)
    처리.깃허브테스트처리
  그외
    전송.델레그램응답전송
}
처리::TELEGRAM.텔레그램깃허브이슈등록명령처리
{
  만약에(참)
    함수.단어분리(gh_reg_word_list,세션.cmd_rest)
    함수.단어합치기(issue_reg_title,세션.리스트.gh_reg_word_list,1)
    함수.저장(issue_reg_body,문장.깃허브텔레그램이슈등록본문문장)
    함수.저장(gh_token,설정.GITHUB.token)
    로그.출력(텔레그램 발 이슈등록 세션.issue_reg_title)
    전송.깃허브이슈등록전송
}
처리::TELEGRAM.텔레그램고객문의명령처리
{
  만약에(참)
    함수.단어분리(cmd_word_list,수신메시지.result[0].message.text)
    함수.단어합치기(cmd_rest,세션.리스트.cmd_word_list,1)
    함수.저장(pending_reply_chat_id,수신메시지.result[0].message.chat.id)
    함수.저장(nfbot_group_message,문장.NF봇고객문의본문문장)
    로그.출력(고객문의 커뮤니티 전달 세션.cmd_rest)
    전송.NF봇그룹메시지전송
    전송.델레그램응답전송
}
처리::TELEGRAM.텔레그램슬랙명령처리
{
  만약에(참)
    함수.단어분리(cmd_word_list,수신메시지.result[0].message.text)
    함수.단어합치기(cmd_rest,세션.리스트.cmd_word_list,1)
    함수.앞자리비교(cmd_타입,세션.cmd_rest,알람)
    처리.텔레그램슬랙명령분기
}
처리::TELEGRAM.텔레그램슬랙명령분기
{
  만약에(세션.cmd_타입 == 1)
    함수.단어분리(type_word_list,세션.cmd_rest)
    함수.단어합치기(alert_message,세션.리스트.type_word_list,1)
    전송.슬랙알람직접전송
  그외
    전송.델레그램응답전송
}
처리::TELEGRAM.텔레그램디스코드명령처리
{
  만약에(참)
    함수.단어분리(cmd_word_list,수신메시지.result[0].message.text)
    함수.단어합치기(cmd_rest,세션.리스트.cmd_word_list,1)
    함수.앞자리비교(cmd_타입,세션.cmd_rest,알람)
    처리.텔레그램디스코드명령분기
}
처리::TELEGRAM.텔레그램디스코드명령분기
{
  만약에(세션.cmd_타입 == 1)
    함수.단어분리(type_word_list,세션.cmd_rest)
    함수.단어합치기(alert_message,세션.리스트.type_word_list,1)
    전송.디스코드알람직접전송
  그외
    전송.델레그램응답전송
}
처리::TELEGRAM.텔레그램노티파이명령처리
{
  만약에(참)
    함수.단어분리(cmd_word_list,수신메시지.result[0].message.text)
    함수.단어합치기(cmd_rest,세션.리스트.cmd_word_list,1)
    함수.앞자리비교(cmd_타입,세션.cmd_rest,알람)
    처리.텔레그램노티파이명령분기
}
처리::TELEGRAM.텔레그램노티파이명령분기
{
  만약에(세션.cmd_타입 == 1)
    함수.단어분리(type_word_list,세션.cmd_rest)
    함수.단어합치기(alert_message,세션.리스트.type_word_list,1)
    전송.Ntfy알람직접전송
  그외
    전송.델레그램응답전송
}
처리::TELEGRAM.텔레그램블루스카이명령처리
{
  만약에(참)
    함수.단어분리(cmd_word_list,수신메시지.result[0].message.text)
    함수.단어합치기(cmd_rest,세션.리스트.cmd_word_list,1)
    함수.앞자리비교(cmd_타입,세션.cmd_rest,알람)
    처리.텔레그램블루스카이명령분기
}
처리::TELEGRAM.텔레그램블루스카이명령분기
{
  만약에(세션.cmd_타입 == 1)
    함수.단어분리(type_word_list,세션.cmd_rest)
    함수.단어합치기(post_message,세션.리스트.type_word_list,1)
    전송.sndBskyAccToken
  그외
    전송.델레그램응답전송
}
처리::TELEGRAM.텔레그램클루드코드명령처리
{
  만약에(참)
    함수.단어분리(cmd_word_list,수신메시지.result[0].message.text)
    함수.단어합치기(cmd_rest,세션.리스트.cmd_word_list,1)
    로그.출력(클루드 명령 실행 세션.cmd_rest)
    전송.클루드명령에코
    전송.클루드코드명령전송
}
처리::TELEGRAM.텔레그램클루드초기화명령처리
{
  만약에(참)
    전송.클루드초기화명령전송
}
처리::TELEGRAM.텔레그램텔넷명령처리
{
  만약에(참)
    함수.단어분리(cmd_word_list,수신메시지.result[0].message.text)
    함수.단어합치기(cmd_rest,세션.리스트.cmd_word_list,1)
    함수.앞자리비교(cmd_ssh_회사서버,세션.cmd_rest,회사서버)
    함수.앞자리비교(cmd_ssh_홈서버,세션.cmd_rest,홈서버)
    처리.텔레그램텔넷명령분기
}
처리::TELEGRAM.텔레그램텔넷명령분기
{
  만약에(세션.cmd_ssh_회사서버 == 1)
    함수.저장(ssh_target,회사서버)
    함수.단어분리(ssh_cmd_word_list,세션.cmd_rest)
    함수.단어합치기(cmd_rest,세션.리스트.ssh_cmd_word_list,1)
    처리.텔레그램텔넷명령실행
  그외그외(세션.cmd_ssh_홈서버 == 1)
    함수.저장(ssh_target,홈서버)
    함수.단어분리(ssh_cmd_word_list,세션.cmd_rest)
    함수.단어합치기(cmd_rest,세션.리스트.ssh_cmd_word_list,1)
    처리.텔레그램텔넷명령실행
  그외
    함수.저장(ssh_target,기본)
    처리.텔레그램텔넷명령실행
}
처리::TELEGRAM.텔레그램텔넷명령실행
{
  만약에(세션.ssh_target == 회사서버)
    로그.출력(텔넷 회사서버 대상 명령 실행 세션.cmd_rest)
    전송.텔넷명령어전송_회사서버
  그외그외(세션.ssh_target == 홈서버)
    로그.출력(텔넷 홈서버 대상 명령 실행 세션.cmd_rest)
    전송.텔넷명령어전송_홈서버
  그외
    로그.출력(텔넷 기본서버 대상 명령 실행 세션.cmd_rest)
    전송.텔넷명령어전송
}
처리::TELEGRAM.텔레그램알람직접트리거
{
  만약에(참)
    함수.저장(alert_message,수신메시지.message)
    전송.텔레그램알람직접응답
    전송.텔레그램알람전송
}
처리::NOTEBOOKFLOW_BOT.NF봇초기화
{
  만약에(참)
    함수.저장(nfbot_offset,0)
    처리.NF봇핑전송
}
처리::NOTEBOOKFLOW_BOT.NF봇핑전송
{
  만약에(참)
    함수.저장(nfbot_token,설정.NOTEBOOKFLOW_BOT.bot_token)
    전송.NF봇폴
    타이머.NF봇타이머
}
처리::NOTEBOOKFLOW_BOT.NF봇수신처리
{
  만약에(수신메시지.result[0].update_id != NULL)
    처리.NF봇수신메시지처리
  그외
    로그.출력(notebookflow_bot 새 메시지 없음)
}
처리::NOTEBOOKFLOW_BOT.NF봇수신메시지처리
{
  만약에(참)
    함수.더하기(nfbot_offset,수신메시지.result[0].update_id,1)
    로그.출력(NotebookFLOW 커뮤니티 채널 수신 세션.nfbot_offset)
    전송.NF봇접수확인전송
}
처리::NOTEBOOKFLOW_BOT.NF봇그룹메시지직접트리거
{
  만약에(참)
    함수.저장(nfbot_group_message,수신메시지.message)
    전송.NF봇그룹메시지직접응답
    전송.NF봇그룹메시지전송
}
처리::DISCORD.디스코드알람응답처리
{
  만약에(참)
    로그.출력(디스코드 알람 응답.)
}
처리::DISCORD.디스코드알람직접트리거
{
  만약에(참)
    함수.저장(alert_message,수신메시지.message)
    전송.디스코드알람직접응답
    전송.디스코드알람직접전송
}
처리::NTFY.Ntfy응답처리
{
  만약에(참)
    로그.출력(ntfy 알림 전송 완료)
}
처리::NTFY.Ntfy알람직접트리거
{
  만약에(참)
    함수.저장(alert_message,수신메시지.message)
    전송.Ntfy알람직접응답
    전송.Ntfy알람직접전송
}
처리::SLACK.슬랙응답처리
{
  만약에(참)
    로그.출력(slack 알림 전송 완료)
}
처리::SLACK.슬랙알람직접트리거
{
  만약에(참)
    함수.저장(alert_message,수신메시지.message)
    전송.슬랙알람직접응답
    전송.슬랙알람직접전송
}
처리::GITHUB.깃허브테스트응답처리
{
  만약에(세션.gh_poll_mode == 기준선)
    함수.저장(gh_poll_mode,0)
    함수.객체저장(gh_poll_issues,수신메시지.결과)
    함수.저장(gh_last_seen_issue,세션.객체.gh_poll_issues[0].number)
    타이머.깃허브이슈폴링타이머
    로그.출력(github 이슈 기준선 설정 완료)
  그외그외(세션.gh_poll_mode == 폴링)
    함수.저장(gh_poll_mode,0)
    함수.객체저장(gh_poll_issues,수신메시지.결과)
    함수.저장(gh_poll_idx,0)
    함수.저장(gh_new_issue_count,0)
    타이머.깃허브이슈폴링타이머
    처리.깃허브새이슈순회
  그외그외(세션.pending_reply_chat_id != NULL)
    함수.객체저장(gh_issues,수신메시지.결과)
    함수.저장(issue_titles,이슈,목록:)
    함수.저장(issue_count,0)
    함수.저장(idx,0)
    처리.깃허브이슈순회
  그외
    로그.출력(github 이슈 조회 완료)
}
처리::GITHUB.깃허브이슈순회
{
  만약에(세션.객체.gh_issues[세션.idx].title != NULL)
    함수.더하기(issue_count,세션.issue_count,1)
    함수.붙이기(issue_titles,세션.객체.gh_issues[세션.idx].title,|)
    함수.더하기(idx,세션.idx,1)
    처리.깃허브이슈순회
  그외
    처리.깃허브이슈텔레그램회신
}
처리::GITHUB.깃허브이슈텔레그램회신
{
  만약에(세션.issue_count == 0)
    함수.저장(reply_text,GitHub,이슈가,없습니다)
    전송.깃허브이슈텔레그램회신전송
  그외
    함수.저장(reply_text,세션.issue_titles)
    전송.깃허브이슈텔레그램회신전송
}
처리::GITHUB.깃허브테스트처리
{
  만약에(참)
    함수.저장(gh_token,설정.GITHUB.token)
    전송.깃허브이슈조회
}
처리::GITHUB.깃허브이슈기준선설정
{
  만약에(참)
    함수.저장(gh_token,설정.GITHUB.token)
    함수.저장(gh_poll_mode,기준선)
    전송.깃허브이슈조회
}
처리::GITHUB.깃허브이슈폴링처리
{
  만약에(참)
    함수.저장(gh_token,설정.GITHUB.token)
    함수.저장(gh_poll_mode,폴링)
    전송.깃허브이슈조회
}
처리::GITHUB.깃허브새이슈순회
{
  만약에(세션.객체.gh_poll_issues[세션.gh_poll_idx].number != NULL) 그리고(세션.객체.gh_poll_issues[세션.gh_poll_idx].number > 세션.gh_last_seen_issue) 그리고(세션.gh_new_issue_count == 0)
    함수.저장(gh_new_issue_titles,문장.깃허브새이슈항목문장)
    함수.저장(gh_new_issue_bodies,문장.깃허브새이슈본문문장)
    함수.더하기(gh_new_issue_count,세션.gh_new_issue_count,1)
    함수.더하기(gh_poll_idx,세션.gh_poll_idx,1)
    처리.깃허브새이슈순회
  그외그외(세션.객체.gh_poll_issues[세션.gh_poll_idx].number != NULL) 그리고(세션.객체.gh_poll_issues[세션.gh_poll_idx].number > 세션.gh_last_seen_issue)
    함수.붙이기(gh_new_issue_titles,세션.gh_new_issue_titles,|,문장.깃허브새이슈항목문장)
    함수.붙이기(gh_new_issue_bodies,세션.gh_new_issue_bodies,|,문장.깃허브새이슈본문문장)
    함수.더하기(gh_new_issue_count,세션.gh_new_issue_count,1)
    함수.더하기(gh_poll_idx,세션.gh_poll_idx,1)
    처리.깃허브새이슈순회
  그외
    처리.깃허브새이슈완료처리
}
처리::GITHUB.깃허브새이슈완료처리
{
  만약에(세션.gh_new_issue_count > 0)
    함수.저장(gh_last_seen_issue,세션.객체.gh_poll_issues[0].number)
    전송.깃허브새이슈텔레그램알림전송
    전송.깃허브이슈검토명령전송
  그외
    로그.출력(github 새 이슈 없음)
}
처리::GITHUB.깃허브이슈등록트리거
{
  만약에(참)
    함수.저장(gh_token,설정.GITHUB.token)
    함수.저장(issue_reg_title,수신메시지.title)
    함수.저장(issue_reg_body,수신메시지.body)
    전송.깃허브이슈등록트리거응답
    전송.깃허브이슈등록전송
}
처리::GITHUB.깃허브이슈등록응답처리
{
  만약에(참)
    함수.저장(issue_reg_url,수신메시지.html_url)
    전송.깃허브이슈등록완료알림전송
}
처리::GITHUB.깃허브이슈등록실패처리
{
  만약에(참)
    함수.저장(issue_reg_error,수신메시지.message)
    전송.깃허브이슈등록실패알림전송
}
처리::LINEAR.procLinearRsp
{
  만약에(참)
    로그.출력(Linear Rsp)
    로그.출력(0 수신메시지.data.issues.nodes[0].title)
    로그.출력(0 수신메시지.data.issues.nodes[1].title)
    로그.출력(0 수신메시지.data.issues.nodes[2].title)
}
처리::BSKY.게시글전송트리거
{
  만약에(참)
    함수.저장(post_message,수신메시지.message)
    전송.게시글전송응답
    전송.sndBskyAccToken
}
처리::BSKY.procBskyTokRcv
{
  만약에(참)
    함수.저장(bsky_access,수신메시지.accessJwt)
    함수.저장(bsky_did,수신메시지.did)
    함수.날짜(bsky_now,%Y-%m-%dT%H:%M:%S)
    전송.sndUpText
}
처리::FLOW.노션테스트
{
  만약에(참)
    로그.출력(notion test start)
    전송.sndNotionMyQuery
}
처리::SSH.sshRsp
{
  만약에(참)
    로그.출력(ssh 응답)
    로그.출력(수신메시지.stderr)
    로그.출력(수신메시지.stdout)
    함수.저장(telnet_rsp,수신메시지.stderr)
    함수.붙이기(telnet_rsp,수신메시지.stdout)
    처리.sshRsp길이분기
}
처리::SSH.sshRsp길이분기
{
  만약에(세션.telnet_rsp.길이 > 4096)
    전송.텔넷회신문서전송
  그외
    전송.텔넷회신
}
전송::TELEGRAM.클루드명령에코
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 수신메시지.result[0].message.chat.id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 세션.cmd_rest
}
전송::TELEGRAM.델레그램응답전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 수신메시지.result[0].message.chat.id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 문장.텔레그램응답문장
}
전송::TELEGRAM.클루드코드명령전송
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.AGENT.domain
  전송메시지.주소.경로 = 설정.AGENT.cmd_path
  전송메시지.instruction = 세션.cmd_rest
  전송메시지.callback_chat_id = 수신메시지.result[0].message.chat.id
  전송메시지.session_key = claude_persistent
  전송메시지.agent_name = 설정.AGENT.agent_name
}
전송::TELEGRAM.클루드초기화명령전송
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.AGENT.domain
  전송메시지.주소.경로 = 설정.AGENT.reset_path
  전송메시지.session_key = claude_persistent
  전송메시지.callback_chat_id = 수신메시지.result[0].message.chat.id
}
전송::TELEGRAM.클루드코드완료텔레그램전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 세션.agent_chat_id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 세션.agent_result
}
전송::TELEGRAM.텔레그램help요청전송
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 수신메시지.result[0].message.chat.id
  전송메시지.text = 문장.텔레그램help안내문구
}
타이머::TELEGRAM.텔레그램타이머
{
  전송메시지.이벤트명 = 텔레그램타이머
  전송메시지.시간 = 5000
}
타이머::TELEGRAM.텔레그램환영타이머
{
  전송메시지.이벤트명 = 텔레그램환영타이머
  전송메시지.시간 = 3000
}
타이머::GITHUB.깃허브이슈폴링타이머
{
  전송메시지.이벤트명 = 깃허브이슈폴링
  전송메시지.시간 = 86400000
}
타이머::NOTEBOOKFLOW_BOT.NF봇타이머
{
  전송메시지.이벤트명 = NF봇타이머
  전송메시지.시간 = 60000
}
타이머::UPDATE.업데이트확인타이머
{
  전송메시지.이벤트명 = 업데이트확인틱
  전송메시지.시간 = 3600000
}
전송::TELEGRAM.텔레그램폴
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램폴경로
  전송메시지.주소.파라미터[0].key = offset
  전송메시지.주소.파라미터[0].val = 세션.tg_offset
}
전송::TELEGRAM.텔레그램환영메시지전송
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 설정.TELEGRAM.my_chat_id
  전송메시지.text = 문장.텔레그램환영문구
}
전송::TELEGRAM.텔레그램알람직접응답
{
  전송메시지.주소 = 수신메시지.주소
  전송메시지.RESULT = 0
}
전송::TELEGRAM.텔레그램알람전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 설정.TELEGRAM.my_chat_id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 세션.alert_message
}
전송::NOTEBOOKFLOW_BOT.NF봇폴
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.NOTEBOOKFLOW_BOT.domain
  전송메시지.주소.경로 = 문장.NF봇폴경로
  전송메시지.주소.파라미터[0].key = offset
  전송메시지.주소.파라미터[0].val = 세션.nfbot_offset
}
전송::NOTEBOOKFLOW_BOT.NF봇접수확인전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.NOTEBOOKFLOW_BOT.domain
  전송메시지.주소.경로 = 문장.NF봇응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 수신메시지.result[0].message.chat.id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 문장.NF봇접수확인문장
}
전송::NOTEBOOKFLOW_BOT.NF봇그룹메시지직접응답
{
  전송메시지.주소 = 수신메시지.주소
  전송메시지.RESULT = 0
}
전송::NOTEBOOKFLOW_BOT.NF봇그룹메시지전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.NOTEBOOKFLOW_BOT.domain
  전송메시지.주소.경로 = 문장.NF봇응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 설정.NOTEBOOKFLOW_BOT.community_chat_id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 세션.nfbot_group_message
}
전송::DISCORD.디스코드알람직접응답
{
  전송메시지.주소 = 수신메시지.주소
  전송메시지.RESULT = 0
}
전송::DISCORD.디스코드알람직접전송
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.DISCORD.webhook_url
  전송메시지.content = 세션.alert_message
}
전송::NTFY.Ntfy알람직접응답
{
  전송메시지.주소 = 수신메시지.주소
  전송메시지.RESULT = 0
}
전송::NTFY.Ntfy알람직접전송
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.NTFY.domain
  전송메시지.주소.경로 = /
  전송메시지.topic = 설정.NTFY.topic
  전송메시지.message = 세션.alert_message
}
전송::SLACK.슬랙알람직접응답
{
  전송메시지.주소 = 수신메시지.주소
  전송메시지.RESULT = 0
}
전송::SLACK.슬랙알람직접전송
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.SLACK.webhook_url
  전송메시지.text = 세션.alert_message
}
전송::GITHUB.깃허브이슈텔레그램회신전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 세션.pending_reply_chat_id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 세션.reply_text
}
전송::GITHUB.깃허브이슈조회
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.GITHUB.domain
  전송메시지.주소.경로 = /repos/dorafather/NotebookFlow/issues
  전송메시지.헤더[0].key = Authorization
  전송메시지.헤더[0].val = 문장.깃허브인증값
  전송메시지.헤더[1].key = User-Agent
  전송메시지.헤더[1].val = NotebookFlow
}
전송::GITHUB.깃허브새이슈텔레그램알림전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 설정.TELEGRAM.my_chat_id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 문장.깃허브새이슈알림문장
}
전송::GITHUB.깃허브이슈검토명령전송
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.AGENT.domain
  전송메시지.주소.경로 = 설정.AGENT.cmd_path
  전송메시지.instruction = 문장.깃허브새이슈검토지시문장
  전송메시지.callback_chat_id = 설정.TELEGRAM.my_chat_id
}
전송::GITHUB.깃허브이슈등록트리거응답
{
  전송메시지.주소 = 수신메시지.주소
  전송메시지.RESULT = 0
}
전송::GITHUB.깃허브이슈등록전송
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.GITHUB.domain
  전송메시지.주소.경로 = /repos/dorafather/NotebookFlow/issues
  전송메시지.헤더[0].key = Authorization
  전송메시지.헤더[0].val = 문장.깃허브인증값
  전송메시지.헤더[1].key = User-Agent
  전송메시지.헤더[1].val = NotebookFlow
  전송메시지.title = 세션.issue_reg_title
  전송메시지.body = 세션.issue_reg_body
}
전송::GITHUB.깃허브이슈등록완료알림전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 설정.TELEGRAM.my_chat_id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 문장.깃허브이슈등록완료문장
}
전송::GITHUB.깃허브이슈등록실패알림전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 설정.TELEGRAM.my_chat_id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 문장.깃허브이슈등록실패문장
}
전송::UPDATE.업데이트버전확인전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.UPDATE.domain
  전송메시지.주소.경로 = /repos/dorafather/NotebookFlow/commits/main
  전송메시지.헤더[0].key = Authorization
  전송메시지.헤더[0].val = 문장.업데이트깃허브인증값
  전송메시지.헤더[1].key = User-Agent
  전송메시지.헤더[1].val = NotebookFlow
}
전송::UPDATE.업데이트알림텔레그램전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 설정.TELEGRAM.my_chat_id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 문장.업데이트알림문장
}
전송::UPDATE.업데이트파일1다운로드
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.UPDATE_DL.domain
  전송메시지.주소.경로 = /dorafather/NotebookFlow/main/NotebookFlow/rest.sce
  전송메시지.저장.파일명 = official_rest.sce
}
전송::UPDATE.업데이트파일2다운로드
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.UPDATE_DL.domain
  전송메시지.주소.경로 = /dorafather/NotebookFlow/main/CLAUDE.md
  전송메시지.저장.파일명 = official_CLAUDE.md
}
전송::UPDATE.업데이트에이전트명령전송
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.AGENT.domain
  전송메시지.주소.경로 = 설정.AGENT.cmd_path
  전송메시지.instruction = 문장.업데이트지시문장
  전송메시지.callback_chat_id = 설정.TELEGRAM.my_chat_id
  전송메시지.agent_name = 설정.AGENT.agent_name
}
전송::LINEAR.sndLinearIssue
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.LINEAR.domain
  전송메시지.주소.경로 = /graphql
  전송메시지.헤더[0].key = Authorization
  전송메시지.헤더[0].val = 설정.LINEAR.api_key
  전송메시지.query = 문장.stmtLinearQuery
}
전송::BSKY.게시글전송응답
{
  전송메시지.주소 = 수신메시지.주소
  전송메시지.RESULT = 0
}
전송::BSKY.sndBskyAccToken
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.BSKY.domain
  전송메시지.주소.경로 = 문장.actk
  전송메시지.identifier = 설정.BSKY.my_id
  전송메시지.password = 설정.BSKY.app_pass
}
전송::BSKY.sndUpText
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.BSKY.domain
  전송메시지.주소.경로 = 문장.bskyPostPath
  전송메시지.헤더[0].key = Authorization
  전송메시지.헤더[0].val = 문장.bskyAuthHeader
  전송메시지.repo = 세션.bsky_did
  전송메시지.collection = 문장.bskyCollection
  전송메시지.record.$type = 문장.bskyCollection
  전송메시지.record.text = 세션.post_message
  전송메시지.record.createdAt = 문장.bskyCreatedAt
}
전송::NOTION.sndNotionMyQuery
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.NOTION.domain
  전송메시지.주소.경로 = /v1/users/me
  전송메시지.헤더[0].key = Authorization
  전송메시지.헤더[0].val = 문장.buildNotionAuth
  전송메시지.헤더[1].key = Notion-Version
  전송메시지.헤더[1].val = 2026-03-11
}
전송::SSH.텔넷명령어전송
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.SSH.domain
  전송메시지.주소.경로 = 설정.SSH.cmd_path
  전송메시지.host = 설정.SSH_기본.host
  전송메시지.port = 설정.SSH_기본.port
  전송메시지.username = 설정.SSH_기본.username
  전송메시지.password = 설정.SSH_기본.password
  전송메시지.command = 세션.cmd_rest
}
전송::SSH.텔넷명령어전송_회사서버
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.SSH.domain
  전송메시지.주소.경로 = 설정.SSH.cmd_path
  전송메시지.host = 설정.SSH_회사서버.host
  전송메시지.port = 설정.SSH_회사서버.port
  전송메시지.username = 설정.SSH_회사서버.username
  전송메시지.password = 설정.SSH_회사서버.password
  전송메시지.command = 세션.cmd_rest
}
전송::SSH.텔넷명령어전송_홈서버
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.SSH.domain
  전송메시지.주소.경로 = 설정.SSH.cmd_path
  전송메시지.host = 설정.SSH_홈서버.host
  전송메시지.port = 설정.SSH_홈서버.port
  전송메시지.username = 설정.SSH_홈서버.username
  전송메시지.password = 설정.SSH_홈서버.password
  전송메시지.command = 세션.cmd_rest
}
전송::TELEGRAM.텔넷회신
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 설정.TELEGRAM.my_chat_id
  전송메시지.text = 세션.telnet_rsp
}
전송::TELEGRAM.텔레그램파일전송
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램문서전송경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 설정.TELEGRAM.my_chat_id
  전송메시지.문서.경로 = 세션.file_path
  전송메시지.문서.파일명 = 세션.file_path
  전송메시지.문서.필드명 = document
  전송메시지.문서.타입 = application/octet-stream
  전송메시지.문서.캡션 = 요청하신 파일입니다
}
전송::TELEGRAM.텔레그램사진전송
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램사진전송경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 설정.TELEGRAM.my_chat_id
  전송메시지.문서.경로 = 세션.photo_path
  전송메시지.문서.파일명 = 세션.photo_path
  전송메시지.문서.필드명 = photo
  전송메시지.문서.타입 = image/jpeg
}
전송::TELEGRAM.텔레그램파일메타조회
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램파일메타조회경로
  전송메시지.주소.파라미터[0].key = file_id
  전송메시지.주소.파라미터[0].val = 세션.download_file_id
}
전송::TELEGRAM.텔레그램파일실제다운로드
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램파일다운로드경로
  전송메시지.저장.파일명 = 세션.download_file_name
}
전송::TELEGRAM.텔레그램폴더조회
{
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.폴더.경로 = 세션.dir_path
}
전송::TELEGRAM.텔레그램help이미지생성
{
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.헬프이미지 = 1
}
전송::TELEGRAM.텔레그램폴더회신전송
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 설정.TELEGRAM.my_chat_id
  전송메시지.text = 수신메시지.요약
}
전송::TELEGRAM.텔레그램파일저장알림전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 설정.TELEGRAM.my_chat_id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 문장.텔레그램파일저장알림문장
}
전송::TELEGRAM.텔넷회신문서전송
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램문서전송경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 설정.TELEGRAM.my_chat_id
  전송메시지.문서.내용 = 세션.telnet_rsp
  전송메시지.문서.파일명 = ssh_output.txt
  전송메시지.문서.필드명 = document
  전송메시지.문서.타입 = text/plain
  전송메시지.문서.캡션 = 명령 결과가 길어 파일로 첨부합니다
}
문장::TELEGRAM.텔레그램응답문장
{받았습니다: $$$수신메시지.result[0].message.text$$$}
문장::TELEGRAM.텔레그램환영문구
{NotebookFlow가 연결되었습니다.

이 API 브릿지는 무한히 확장됩니다. Slack, Discord, GitHub처럼 이미 연결된 서비스뿐 아니라, 세상의 어떤 REST API든 - Claude Code에게 "이 서비스도 연동해줘"라고 말만 하면, Claude가 NotebookFlow의 스크립팅 언어(Flow)로 직접 시나리오를 작성해 즉시 서비스를 만들어냅니다. 당신만의 자동화가 지금 이 순간부터 시작됩니다.

이 API 브릿지는 그냥 자동화 도구가 아니라, 사람과 AI 에이전트가 함께 만들어가는 작은 다리입니다.

쓰다가 불편한 점, 버그, 개선 아이디어가 떠오르면 언제든 Claude Code에게 "GitHub에 이슈로 등록해줘"라고 말씀해주세요. (GitHub 계정과 개인 액세스 토큰이 필요합니다 - 처음 한 번만 설정하시면 됩니다)

dorafather와 샛별이가 여러분의 이슈에 적극적으로 소통할 것을 약속합니다. 여러분의 목소리가 NotebookFlow를 함께 만들어갑니다.}
문장::TELEGRAM.텔레그램help안내문구
{
[사용 가능한 명령어]
 깃허브    이슈 - GitHub 이슈 목록 조회
 슬랙     알람 [문구] - Slack 알림 전송
 디스코드  알람 [문구] - Discord 알림 전송
 노티파이  알람 [문구] - ntfy 알림 전송
 블루스카이 알람 [문구] - Bluesky 포스팅
 클루드코드 [지시]    - Claude Code에게 작업 지시
 텔넷     [명령어]   - 원격 서버에서 셸 명령 실행
 파일 얻기 [경로]     - 로컬 파일을 전송받기
 파일 폴더 [경로]     - 로컬 폴더 목록 조회(최대 20개)
 사진 얻기 [경로]     - 로컬 파일을 전송받기
 주식 관심종목 추가/삭제/조회 [종목명] - 관심종목 등락 감시(전일 기준, ±5% 기본)

불편한 점이나 버그, 아이디어가 있으면 Claude Code에게 "GitHub에 이슈로 등록해줘"라고 말씀해주세요.
}
문장::TELEGRAM.텔레그램폴경로
{/bot$$$세션.tg_token$$$/getUpdates}
문장::TELEGRAM.텔레그램응답경로
{/bot$$$세션.tg_token$$$/sendMessage}
문장::TELEGRAM.텔레그램문서전송경로
{/bot$$$세션.tg_token$$$/sendDocument}
문장::TELEGRAM.텔레그램사진전송경로
{/bot$$$세션.tg_token$$$/sendPhoto}
문장::TELEGRAM.텔레그램파일메타조회경로
{/bot$$$세션.tg_token$$$/getFile}
문장::TELEGRAM.텔레그램파일다운로드경로
{/file/bot$$$세션.tg_token$$$/$$$세션.download_file_path$$$}
문장::TELEGRAM.텔레그램파일저장알림문장
{파일 저장 완료: $$$수신메시지.저장경로$$$}
문장::NOTEBOOKFLOW_BOT.NF봇폴경로
{/bot$$$세션.nfbot_token$$$/getUpdates}
문장::NOTEBOOKFLOW_BOT.NF봇응답경로
{/bot$$$세션.nfbot_token$$$/sendMessage}
문장::NOTEBOOKFLOW_BOT.NF봇접수확인문장
{접수되었습니다. NotebookFLOW 커뮤니티 채널에서 확인 후 검토하겠습니다.}
문장::NOTEBOOKFLOW_BOT.NF봇고객문의본문문장
{[고객문의] $$$세션.cmd_rest$$$ (Telegram chat_id: $$$세션.pending_reply_chat_id$$$)}
문장::GITHUB.깃허브인증값
{token $$$세션.gh_token$$$}
문장::GITHUB.깃허브새이슈항목문장
{#$$$세션.객체.gh_poll_issues[세션.gh_poll_idx].number$$$ $$$세션.객체.gh_poll_issues[세션.gh_poll_idx].title$$$}
문장::GITHUB.깃허브새이슈알림문장
{새 이슈 등록됨: $$$세션.gh_new_issue_titles$$$}
문장::GITHUB.깃허브새이슈본문문장
{$$$세션.객체.gh_poll_issues[세션.gh_poll_idx].body$$$}
문장::GITHUB.깃허브새이슈검토지시문장
{아래는 GitHub 저장소 dorafather/NotebookFlow에 새로 등록된 이슈($$$세션.gh_new_issue_titles$$$)의 본문 전문이다. GitHub API를 호출하거나 이슈를 직접 조회하지 말고, 아래 제공된 내용만 보고 핵심을 요약해서 텔레그램으로 보고해줘. 코드 수정이나 실행은 하지 말고 요약 보고만 해줘. --- 이슈 본문 --- $$$세션.gh_new_issue_bodies$$$}
문장::GITHUB.깃허브이슈등록완료문장
{GitHub 이슈가 등록되었습니다: $$$세션.issue_reg_url$$$}
문장::GITHUB.깃허브이슈등록실패문장
{GitHub 이슈 등록 실패: $$$세션.issue_reg_error$$$ (addr.ini의 GITHUB.token 권한을 확인해주세요)}
문장::GITHUB.깃허브텔레그램이슈등록본문문장
{Telegram "깃허브 이슈등록" 명령으로 등록된 이슈입니다. (chat_id: $$$세션.pending_reply_chat_id$$$)}
문장::UPDATE.업데이트깃허브인증값
{token $$$세션.gh_token$$$}
문장::UPDATE.업데이트알림문장
{NotebookFlow 공식 저장소에 새로운 업데이트가 있습니다. '업데이트'라고 답하면 반영을 시작합니다. (최신 커밋: $$$세션.update_new_sha$$$)}
문장::UPDATE.업데이트지시문장
{NotebookFlow 공식 저장소(dorafather/NotebookFlow)의 최신 커밋($$$세션.update_new_sha$$$)
기준 공식 배포 파일이 이미 이 작업 디렉터리의 downloads 폴더에
다운로드되어 있다:
- downloads/official_rest.sce (공식 최신 rest.sce)
- downloads/official_CLAUDE.md (공식 최신 CLAUDE.md, 저장소 루트 파일)

작업 순서:
1) 이 작업 디렉터리의 rest.sce와 downloads/official_rest.sce를 비교해서,
   공식 Default 카탈로그(서비스별 처리::/전송::/타이머::/문장:: 블록)
   변경분만 반영해라. addr.ini의 실제 토큰/시크릿 값과, 이 로컬 rest.sce
   안에서 공식 저장소에는 없는 이름으로 사용자가 직접 추가한 것으로
   보이는 커스텀 블록(개인 SSH 서버 대상, 개인 커스텀 명령 등)은 절대
   건드리지 마라.
2) 상위 폴더의 CLAUDE.md를 downloads/official_CLAUDE.md와 비교해서
   갱신해라. 이 작업 디렉터리 밖이라 접근이 안 되면(샌드박스 제한으로
   상위 폴더가 안 보일 수 있다), 억지로 시도하지 말고 그 사실을 그대로
   요약에 남겨라.
3) 새 Telegram 명령이 추가됐다면 help.json도 함께 갱신해라.
4) GitHub API를 직접 호출하거나 git 명령을 실행하지 마라 - 이미 받아둔
   downloads/official_* 파일만 보고 비교하면 충분하다(Read/Edit/Write
   도구만 사용, Bash나 네트워크 조회는 필요 없다).
5) 작업을 마친 뒤 addr.ini의 [INFO] 섹션 last_applied_version 값을
   $$$세션.update_new_sha$$$ 로 갱신해라(토큰/시크릿이 아닌 버전 기록용
   필드라 안전하게 덮어써도 된다. 값이 이미 있어도 그대로 새 값으로
   교체해라).
6) 무엇을 반영했고 무엇은 보존했는지, CLAUDE.md는 실제로 갱신했는지
   여부를 포함해서 한국어로 간단히 요약해서 답해라.}
문장::LINEAR.stmtLinearQuery
{$$$<$$$ issues(first: 10) $$$<$$$ nodes $$$<$$$ id title identifier state $$$<$$$ name $$$>$$$ $$$>$$$ $$$>$$$ $$$>$$$}
문장::BSKY.bskyPostPath
{/xrpc/com.atproto.repo.createRecord}
문장::BSKY.bskyCollection
{app.bsky.feed.post}
문장::BSKY.actk
{/xrpc/com.atproto.server.createSession}
문장::BSKY.bskyAuthHeader
{Bearer $$$세션.bsky_access$$$}
문장::BSKY.bskyCreatedAt
{$$$세션.bsky_now$$$+09:00}
문장::NOTION.buildNotionAuth
{Bearer $$$설정.NOTION.token$$$}
처리::FLOW.주식관심종목초기화
{
  만약에(참)
    함수.저장(krx_watch_csv,없음)
    함수.저장(krx_seed_found,0)
    처리.주식시드확인1
}
처리::FLOW.주식시드확인1
{
  만약에(설정.KRX_WATCHLIST.종목1 != NULL) 그리고(세션.krx_seed_found != 1)
    함수.저장(krx_watch_csv,설정.KRX_WATCHLIST.종목1)
    함수.저장(krx_seed_found,1)
    처리.주식시드확인2
  그외그외(설정.KRX_WATCHLIST.종목1 != NULL)
    함수.붙이기(krx_watch_csv,|,설정.KRX_WATCHLIST.종목1)
    처리.주식시드확인2
  그외
    처리.주식시드확인2
}
처리::FLOW.주식시드확인2
{
  만약에(설정.KRX_WATCHLIST.종목2 != NULL) 그리고(세션.krx_seed_found != 1)
    함수.저장(krx_watch_csv,설정.KRX_WATCHLIST.종목2)
    함수.저장(krx_seed_found,1)
    처리.주식시드확인3
  그외그외(설정.KRX_WATCHLIST.종목2 != NULL)
    함수.붙이기(krx_watch_csv,|,설정.KRX_WATCHLIST.종목2)
    처리.주식시드확인3
  그외
    처리.주식시드확인3
}
처리::FLOW.주식시드확인3
{
  만약에(설정.KRX_WATCHLIST.종목3 != NULL) 그리고(세션.krx_seed_found != 1)
    함수.저장(krx_watch_csv,설정.KRX_WATCHLIST.종목3)
    함수.저장(krx_seed_found,1)
    처리.주식시드확인4
  그외그외(설정.KRX_WATCHLIST.종목3 != NULL)
    함수.붙이기(krx_watch_csv,|,설정.KRX_WATCHLIST.종목3)
    처리.주식시드확인4
  그외
    처리.주식시드확인4
}
처리::FLOW.주식시드확인4
{
  만약에(설정.KRX_WATCHLIST.종목4 != NULL) 그리고(세션.krx_seed_found != 1)
    함수.저장(krx_watch_csv,설정.KRX_WATCHLIST.종목4)
    함수.저장(krx_seed_found,1)
    처리.주식시드확인5
  그외그외(설정.KRX_WATCHLIST.종목4 != NULL)
    함수.붙이기(krx_watch_csv,|,설정.KRX_WATCHLIST.종목4)
    처리.주식시드확인5
  그외
    처리.주식시드확인5
}
처리::FLOW.주식시드확인5
{
  만약에(설정.KRX_WATCHLIST.종목5 != NULL) 그리고(세션.krx_seed_found != 1)
    함수.저장(krx_watch_csv,설정.KRX_WATCHLIST.종목5)
    함수.저장(krx_seed_found,1)
    로그.출력(주식 관심종목 시드 로딩 완료)
  그외그외(설정.KRX_WATCHLIST.종목5 != NULL)
    함수.붙이기(krx_watch_csv,|,설정.KRX_WATCHLIST.종목5)
    로그.출력(주식 관심종목 시드 로딩 완료)
  그외
    로그.출력(주식 관심종목 시드 로딩 완료)
}
처리::FLOW.주식관심종목감시처리
{
  만약에(참)
    함수.저장(krx_mode,폴링단건)
    타이머.주식감시타이머
    처리.주식관심종목순회시작
}
처리::TELEGRAM.텔레그램주식명령처리
{
  만약에(참)
    함수.단어분리(cmd_word_list,수신메시지.result[0].message.text)
    함수.단어합치기(cmd_rest,세션.리스트.cmd_word_list,1)
    함수.앞자리비교(cmd_관심종목,세션.cmd_rest,관심종목)
    처리.텔레그램주식명령분기
}
처리::TELEGRAM.텔레그램주식명령분기
{
  만약에(세션.cmd_관심종목 == 1)
    처리.텔레그램주식관심종목명령처리
  그외
    전송.델레그램응답전송
}
처리::TELEGRAM.텔레그램주식관심종목명령처리
{
  만약에(참)
    함수.단어분리(cmd_word_list2,세션.cmd_rest)
    함수.단어합치기(cmd_rest2,세션.리스트.cmd_word_list2,1)
    함수.앞자리비교(cmd_추가,세션.cmd_rest2,추가)
    함수.앞자리비교(cmd_삭제,세션.cmd_rest2,삭제)
    함수.앞자리비교(cmd_조회,세션.cmd_rest2,조회)
    처리.텔레그램주식관심종목명령분기
}
처리::TELEGRAM.텔레그램주식관심종목명령분기
{
  만약에(세션.cmd_추가 == 1)
    함수.저장(pending_reply_chat_id,수신메시지.result[0].message.chat.id)
    처리.텔레그램주식관심종목추가명령처리
  그외그외(세션.cmd_삭제 == 1)
    함수.저장(pending_reply_chat_id,수신메시지.result[0].message.chat.id)
    처리.텔레그램주식관심종목삭제명령처리
  그외그외(세션.cmd_조회 == 1)
    함수.저장(pending_reply_chat_id,수신메시지.result[0].message.chat.id)
    처리.텔레그램주식관심종목조회명령처리
  그외
    전송.델레그램응답전송
}
처리::TELEGRAM.텔레그램주식관심종목추가명령처리
{
  만약에(참)
    함수.단어분리(cmd_word_list3,세션.cmd_rest2)
    함수.단어합치기(krx_target_name,세션.리스트.cmd_word_list3,1)
    함수.저장(krx_mode,검색)
    전송.KRX종목검색전송
}
처리::TELEGRAM.텔레그램주식관심종목삭제명령처리
{
  만약에(참)
    함수.단어분리(cmd_word_list4,세션.cmd_rest2)
    함수.단어합치기(krx_target_name,세션.리스트.cmd_word_list4,1)
    함수.저장(krx_del_match_prefix,세션.krx_target_name)
    함수.붙이기(krx_del_match_prefix,:)
    함수.쪼개기(krx_watch_list,세션.krx_watch_csv,|)
    함수.저장(krx_del_idx,0)
    함수.저장(krx_del_found,0)
    함수.저장(krx_new_csv,없음)
    함수.저장(krx_new_found,0)
    처리.주식관심종목삭제순회
}
처리::KRX.주식관심종목삭제순회
{
  만약에(세션.krx_del_idx >= 세션.리스트.krx_watch_list.SIZE)
    처리.주식관심종목삭제완료
  그외
    처리.주식관심종목삭제항목검사
}
처리::KRX.주식관심종목삭제항목검사
{
  만약에(세션.리스트.krx_watch_list[세션.krx_del_idx] === 세션.krx_del_match_prefix)
    함수.더하기(krx_del_found,세션.krx_del_found,1)
    함수.더하기(krx_del_idx,세션.krx_del_idx,1)
    처리.주식관심종목삭제순회
  그외
    처리.주식관심종목삭제보존
}
처리::KRX.주식관심종목삭제보존
{
  만약에(세션.krx_new_found == 0)
    함수.저장(krx_new_csv,세션.리스트.krx_watch_list[세션.krx_del_idx])
    함수.저장(krx_new_found,1)
    함수.더하기(krx_del_idx,세션.krx_del_idx,1)
    처리.주식관심종목삭제순회
  그외
    함수.붙이기(krx_new_csv,|,세션.리스트.krx_watch_list[세션.krx_del_idx])
    함수.더하기(krx_del_idx,세션.krx_del_idx,1)
    처리.주식관심종목삭제순회
}
처리::KRX.주식관심종목삭제완료
{
  만약에(세션.krx_del_found > 0)
    함수.저장(krx_watch_csv,세션.krx_new_csv)
    함수.저장(krx_reply_text,문장.주식관심종목삭제완료문장)
    전송.주식관심종목응답전송
  그외
    함수.저장(krx_reply_text,문장.주식관심종목삭제실패문장)
    전송.주식관심종목응답전송
}
처리::TELEGRAM.텔레그램주식관심종목조회명령처리
{
  만약에(참)
    함수.저장(krx_mode,조회단건)
    처리.주식관심종목순회시작
}
처리::KRX.주식관심종목순회시작
{
  만약에(세션.krx_watch_csv == 없음)
    함수.저장(krx_walk_lines,없음)
    처리.주식관심종목순회완료
  그외
    함수.쪼개기(krx_watch_list,세션.krx_watch_csv,|)
    함수.저장(krx_walk_idx,0)
    함수.저장(krx_walk_found,0)
    함수.저장(krx_walk_lines,없음)
    처리.주식관심종목순회다음
}
처리::KRX.주식관심종목순회다음
{
  만약에(세션.krx_walk_idx >= 세션.리스트.krx_watch_list.SIZE)
    처리.주식관심종목순회완료
  그외
    함수.쪼개기(krx_item_parts,세션.리스트.krx_watch_list[세션.krx_walk_idx],:)
    함수.저장(krx_walk_name,세션.리스트.krx_item_parts[0])
    함수.저장(krx_walk_code,세션.리스트.krx_item_parts[1])
    전송.KRX시세조회전송
}
처리::KRX.주식관심종목순회완료
{
  만약에(세션.krx_mode == 조회단건) 그리고(세션.krx_walk_lines != 없음)
    함수.저장(krx_reply_text,세션.krx_walk_lines)
    전송.주식관심종목응답전송
  그외그외(세션.krx_mode == 조회단건)
    함수.저장(krx_reply_text,문장.주식관심종목조회빈목록문장)
    전송.주식관심종목응답전송
  그외그외(세션.krx_mode == 폴링단건) 그리고(세션.krx_walk_lines != 없음)
    함수.저장(krx_watch_alert_text,세션.krx_walk_lines)
    전송.주식감시알림텔레그램전송
  그외
    로그.출력(주식 관심종목 감시 - 임계값 초과 종목 없음, 알림 생략)
}
처리::KRX.KRX응답분기처리
{
  만약에(세션.krx_mode == 검색)
    함수.객체저장(krx_search_items,수신메시지.response.body.items.item)
    함수.저장(krx_search_idx,0)
    처리.주식종목검색순회
  그외그외(세션.krx_mode == 조회단건)
    함수.객체저장(krx_price_items,수신메시지.response.body.items.item)
    처리.주식관심종목조회단건처리
  그외그외(세션.krx_mode == 폴링단건)
    함수.객체저장(krx_price_items,수신메시지.response.body.items.item)
    처리.주식감시단건처리
  그외
    로그.출력(KRX 알 수 없는 응답 모드)
}
처리::KRX.주식종목검색순회
{
  만약에(세션.객체.krx_search_items[세션.krx_search_idx].itmsNm == NULL)
    처리.주식종목검색실패
  그외그외(세션.객체.krx_search_items[세션.krx_search_idx].itmsNm == 세션.krx_target_name)
    함수.저장(krx_resolved_code,세션.객체.krx_search_items[세션.krx_search_idx].srtnCd)
    처리.주식종목검색성공
  그외
    함수.더하기(krx_search_idx,세션.krx_search_idx,1)
    처리.주식종목검색순회
}
처리::KRX.주식종목검색성공
{
  만약에(세션.krx_watch_csv == 없음)
    함수.저장(krx_watch_csv,세션.krx_target_name)
    함수.붙이기(krx_watch_csv,:,세션.krx_resolved_code)
    함수.저장(krx_reply_text,문장.주식관심종목추가완료문장)
    전송.주식관심종목응답전송
  그외
    함수.붙이기(krx_watch_csv,|,세션.krx_target_name,:,세션.krx_resolved_code)
    함수.저장(krx_reply_text,문장.주식관심종목추가완료문장)
    전송.주식관심종목응답전송
}
처리::KRX.주식종목검색실패
{
  만약에(참)
    함수.저장(krx_reply_text,문장.주식관심종목검색실패문장)
    전송.주식관심종목응답전송
}
처리::KRX.주식관심종목조회단건처리
{
  만약에(세션.객체.krx_price_items[0].clpr == NULL)
    함수.더하기(krx_walk_idx,세션.krx_walk_idx,1)
    처리.주식관심종목순회다음
  그외
    함수.저장(krx_line_name,세션.객체.krx_price_items[0].itmsNm)
    함수.저장(krx_line_price,세션.객체.krx_price_items[0].clpr)
    함수.저장(krx_line_rate,세션.객체.krx_price_items[0].fltRt)
    처리.주식관심종목조회라인추가
}
처리::KRX.주식관심종목조회라인추가
{
  만약에(세션.krx_walk_found == 0)
    함수.저장(krx_walk_lines,문장.KRX주식시세라인문장)
    함수.저장(krx_walk_found,1)
    함수.더하기(krx_walk_idx,세션.krx_walk_idx,1)
    처리.주식관심종목순회다음
  그외
    함수.붙이기(krx_walk_lines,|,문장.KRX주식시세라인문장)
    함수.더하기(krx_walk_idx,세션.krx_walk_idx,1)
    처리.주식관심종목순회다음
}
처리::KRX.주식감시단건처리
{
  만약에(세션.객체.krx_price_items[0].fltRt == NULL)
    함수.더하기(krx_walk_idx,세션.krx_walk_idx,1)
    처리.주식관심종목순회다음
  그외
    함수.저장(krx_line_name,세션.객체.krx_price_items[0].itmsNm)
    함수.저장(krx_line_price,세션.객체.krx_price_items[0].clpr)
    함수.저장(krx_line_rate,세션.객체.krx_price_items[0].fltRt)
    함수.부분비교(krx_is_neg,세션.krx_line_rate,-)
    처리.주식감시부호분기
}
처리::KRX.주식감시부호분기
{
  만약에(세션.krx_is_neg == 1)
    함수.추출(krx_rate_abs,세션.krx_line_rate,1,10)
    처리.주식감시임계값비교
  그외
    함수.저장(krx_rate_abs,세션.krx_line_rate)
    처리.주식감시임계값비교
}
처리::KRX.주식감시임계값비교
{
  만약에(세션.krx_rate_abs >= 설정.KRX.threshold)
    처리.주식감시라인추가
  그외
    함수.더하기(krx_walk_idx,세션.krx_walk_idx,1)
    처리.주식관심종목순회다음
}
처리::KRX.주식감시라인추가
{
  만약에(세션.krx_walk_found == 0)
    함수.저장(krx_walk_lines,문장.KRX주식감시알림라인문장)
    함수.저장(krx_walk_found,1)
    함수.더하기(krx_walk_idx,세션.krx_walk_idx,1)
    처리.주식관심종목순회다음
  그외
    함수.붙이기(krx_walk_lines,|,문장.KRX주식감시알림라인문장)
    함수.더하기(krx_walk_idx,세션.krx_walk_idx,1)
    처리.주식관심종목순회다음
}
전송::KRX.KRX종목검색전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.KRX.domain
  전송메시지.주소.경로 = /getStockPriceInfo_V2
  전송메시지.주소.파라미터[0].key = serviceKey
  전송메시지.주소.파라미터[0].val = 설정.KRX.service_key
  전송메시지.주소.파라미터[1].key = resultType
  전송메시지.주소.파라미터[1].val = json
  전송메시지.주소.파라미터[2].key = numOfRows
  전송메시지.주소.파라미터[2].val = 20
  전송메시지.주소.파라미터[3].key = pageNo
  전송메시지.주소.파라미터[3].val = 1
  전송메시지.주소.파라미터[4].key = likeItmsNm
  전송메시지.주소.파라미터[4].val = 세션.krx_target_name
}
전송::KRX.KRX시세조회전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.KRX.domain
  전송메시지.주소.경로 = /getStockPriceInfo_V2
  전송메시지.주소.파라미터[0].key = serviceKey
  전송메시지.주소.파라미터[0].val = 설정.KRX.service_key
  전송메시지.주소.파라미터[1].key = resultType
  전송메시지.주소.파라미터[1].val = json
  전송메시지.주소.파라미터[2].key = numOfRows
  전송메시지.주소.파라미터[2].val = 1
  전송메시지.주소.파라미터[3].key = pageNo
  전송메시지.주소.파라미터[3].val = 1
  전송메시지.주소.파라미터[4].key = likeSrtnCd
  전송메시지.주소.파라미터[4].val = 세션.krx_walk_code
}
전송::TELEGRAM.주식관심종목응답전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 세션.pending_reply_chat_id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 세션.krx_reply_text
}
전송::TELEGRAM.주식감시알림텔레그램전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 설정.TELEGRAM.my_chat_id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 문장.주식감시알림문장
}
타이머::KRX.주식감시타이머
{
  전송메시지.이벤트명 = 주식감시틱
  전송메시지.시간 = 86400000
}
문장::KRX.KRX주식시세라인문장
{$$$세션.krx_line_name$$$ $$$세션.krx_line_price$$$원 $$$세션.krx_line_rate$$$% (전일 기준)}
문장::KRX.KRX주식감시알림라인문장
{$$$세션.krx_line_name$$$ $$$세션.krx_line_rate$$$% $$$세션.krx_line_price$$$원 (전일 기준)}
문장::TELEGRAM.주식감시알림문장
{[주식 관심종목 알림] 전일 대비 등락률 $$$설정.KRX.threshold$$$% 이상 변동된 종목이 있습니다 (전일 기준).
$$$세션.krx_watch_alert_text$$$}
문장::TELEGRAM.주식관심종목추가완료문장
{$$$세션.krx_target_name$$$($$$세션.krx_resolved_code$$$)를 관심종목에 추가했습니다. 매일 전일 기준 등락률을 확인해 $$$설정.KRX.threshold$$$% 이상 변동 시 알려드립니다.}
문장::TELEGRAM.주식관심종목검색실패문장
{"$$$세션.krx_target_name$$$" 종목을 찾을 수 없습니다. 정확한 종목명으로 다시 시도해주세요.}
문장::TELEGRAM.주식관심종목삭제완료문장
{$$$세션.krx_target_name$$$을(를) 관심종목에서 삭제했습니다.}
문장::TELEGRAM.주식관심종목삭제실패문장
{$$$세션.krx_target_name$$$은(는) 등록된 관심종목이 아닙니다.}
문장::TELEGRAM.주식관심종목조회빈목록문장
{등록된 관심종목이 없습니다. "주식 관심종목 추가 삼성전자"처럼 말씀해주세요.}
처리::FLOW.기상청관심지역초기화
{
  만약에(참)
    함수.저장(kma_watch_csv,없음)
    함수.저장(kma_seed_found,0)
    처리.기상청지역시드확인1
}
처리::FLOW.기상청지역시드확인1
{
  만약에(설정.KMA_WATCHLIST.지역1 != NULL) 그리고(세션.kma_seed_found != 1)
    함수.저장(kma_watch_csv,설정.KMA_WATCHLIST.지역1)
    함수.저장(kma_seed_found,1)
    처리.기상청지역시드확인2
  그외그외(설정.KMA_WATCHLIST.지역1 != NULL)
    함수.붙이기(kma_watch_csv,|,설정.KMA_WATCHLIST.지역1)
    처리.기상청지역시드확인2
  그외
    처리.기상청지역시드확인2
}
처리::FLOW.기상청지역시드확인2
{
  만약에(설정.KMA_WATCHLIST.지역2 != NULL) 그리고(세션.kma_seed_found != 1)
    함수.저장(kma_watch_csv,설정.KMA_WATCHLIST.지역2)
    함수.저장(kma_seed_found,1)
    처리.기상청지역시드확인3
  그외그외(설정.KMA_WATCHLIST.지역2 != NULL)
    함수.붙이기(kma_watch_csv,|,설정.KMA_WATCHLIST.지역2)
    처리.기상청지역시드확인3
  그외
    처리.기상청지역시드확인3
}
처리::FLOW.기상청지역시드확인3
{
  만약에(설정.KMA_WATCHLIST.지역3 != NULL) 그리고(세션.kma_seed_found != 1)
    함수.저장(kma_watch_csv,설정.KMA_WATCHLIST.지역3)
    함수.저장(kma_seed_found,1)
    처리.기상청지역시드확인4
  그외그외(설정.KMA_WATCHLIST.지역3 != NULL)
    함수.붙이기(kma_watch_csv,|,설정.KMA_WATCHLIST.지역3)
    처리.기상청지역시드확인4
  그외
    처리.기상청지역시드확인4
}
처리::FLOW.기상청지역시드확인4
{
  만약에(설정.KMA_WATCHLIST.지역4 != NULL) 그리고(세션.kma_seed_found != 1)
    함수.저장(kma_watch_csv,설정.KMA_WATCHLIST.지역4)
    함수.저장(kma_seed_found,1)
    처리.기상청지역시드확인5
  그외그외(설정.KMA_WATCHLIST.지역4 != NULL)
    함수.붙이기(kma_watch_csv,|,설정.KMA_WATCHLIST.지역4)
    처리.기상청지역시드확인5
  그외
    처리.기상청지역시드확인5
}
처리::FLOW.기상청지역시드확인5
{
  만약에(설정.KMA_WATCHLIST.지역5 != NULL) 그리고(세션.kma_seed_found != 1)
    함수.저장(kma_watch_csv,설정.KMA_WATCHLIST.지역5)
    함수.저장(kma_seed_found,1)
    로그.출력(기상청 관심지역 시드 로딩 완료)
  그외그외(설정.KMA_WATCHLIST.지역5 != NULL)
    함수.붙이기(kma_watch_csv,|,설정.KMA_WATCHLIST.지역5)
    로그.출력(기상청 관심지역 시드 로딩 완료)
  그외
    로그.출력(기상청 관심지역 시드 로딩 완료)
}
처리::FLOW.기상알림확인처리
{
  만약에(참)
    함수.날짜(kma_alert_hour_now,%H)
    타이머.기상알림타이머
    처리.기상알림시각비교
}
처리::FLOW.기상알림시각비교
{
  만약에(세션.kma_alert_hour_now == 설정.KMA.alert_hour)
    함수.저장(kma_mode,폴링)
    처리.KMA관심지역순회시작
  그외
    로그.출력(기상 알림 스킵 - 시각 불일치)
}
처리::TELEGRAM.텔레그램기상청명령처리
{
  만약에(참)
    함수.단어분리(kma_cmd_word_list,수신메시지.result[0].message.text)
    함수.단어합치기(cmd_rest,세션.리스트.kma_cmd_word_list,1)
    함수.앞자리비교(cmd_날씨,세션.cmd_rest,날씨)
    함수.앞자리비교(cmd_지역,세션.cmd_rest,지역)
    처리.텔레그램기상청명령분기
}
처리::TELEGRAM.텔레그램기상청명령분기
{
  만약에(세션.cmd_날씨 == 1)
    함수.저장(pending_reply_chat_id,수신메시지.result[0].message.chat.id)
    처리.텔레그램기상청날씨명령처리
  그외그외(세션.cmd_지역 == 1)
    함수.저장(pending_reply_chat_id,수신메시지.result[0].message.chat.id)
    처리.텔레그램기상청지역명령처리
  그외
    전송.델레그램응답전송
}
처리::TELEGRAM.텔레그램기상청날씨명령처리
{
  만약에(참)
    함수.단어분리(kma_weather_word_list,세션.cmd_rest)
    함수.단어합치기(kma_target_name,세션.리스트.kma_weather_word_list,1)
    함수.저장(kma_mode,조회단건)
    처리.KMA지역명확정
}
처리::KMA.KMA지역명확정
{
  만약에(세션.kma_target_name != NULL)
    처리.KMA지역좌표확인
  그외
    함수.저장(kma_target_name,설정.KMA.default_region)
    처리.KMA지역좌표확인
}
처리::TELEGRAM.텔레그램기상청지역명령처리
{
  만약에(참)
    함수.단어분리(kma_region_word_list,세션.cmd_rest)
    함수.단어합치기(kma_region_rest,세션.리스트.kma_region_word_list,1)
    함수.앞자리비교(cmd_지역추가,세션.kma_region_rest,추가)
    함수.앞자리비교(cmd_지역삭제,세션.kma_region_rest,삭제)
    함수.앞자리비교(cmd_지역조회,세션.kma_region_rest,조회)
    처리.텔레그램기상청지역명령분기
}
처리::TELEGRAM.텔레그램기상청지역명령분기
{
  만약에(세션.cmd_지역추가 == 1)
    처리.텔레그램기상청지역추가명령처리
  그외그외(세션.cmd_지역삭제 == 1)
    처리.텔레그램기상청지역삭제명령처리
  그외그외(세션.cmd_지역조회 == 1)
    처리.텔레그램기상청지역조회명령처리
  그외
    전송.델레그램응답전송
}
처리::TELEGRAM.텔레그램기상청지역조회명령처리
{
  만약에(참)
    함수.저장(kma_mode,지역조회)
    처리.KMA관심지역순회시작
}
처리::TELEGRAM.텔레그램기상청지역추가명령처리
{
  만약에(참)
    함수.단어분리(kma_add_word_list,세션.kma_region_rest)
    함수.단어합치기(kma_target_name,세션.리스트.kma_add_word_list,1)
    처리.KMA지역명검증
}
처리::KMA.KMA지역명검증
{
  만약에(세션.kma_target_name == 서울)
    함수.저장(kma_region_found,1)
    처리.KMA지역추가확정
  그외그외(세션.kma_target_name == 부산)
    함수.저장(kma_region_found,1)
    처리.KMA지역추가확정
  그외그외(세션.kma_target_name == 대구)
    함수.저장(kma_region_found,1)
    처리.KMA지역추가확정
  그외그외(세션.kma_target_name == 인천)
    함수.저장(kma_region_found,1)
    처리.KMA지역추가확정
  그외그외(세션.kma_target_name == 광주)
    함수.저장(kma_region_found,1)
    처리.KMA지역추가확정
  그외그외(세션.kma_target_name == 대전)
    함수.저장(kma_region_found,1)
    처리.KMA지역추가확정
  그외그외(세션.kma_target_name == 울산)
    함수.저장(kma_region_found,1)
    처리.KMA지역추가확정
  그외그외(세션.kma_target_name == 세종)
    함수.저장(kma_region_found,1)
    처리.KMA지역추가확정
  그외그외(세션.kma_target_name == 수원)
    함수.저장(kma_region_found,1)
    처리.KMA지역추가확정
  그외그외(세션.kma_target_name == 제주)
    함수.저장(kma_region_found,1)
    처리.KMA지역추가확정
  그외
    함수.저장(kma_region_found,0)
    처리.KMA지역추가확정
}
처리::KMA.KMA지역추가확정
{
  만약에(세션.kma_region_found == 0)
    함수.저장(kma_reply_text,문장.KMA지역미지원문장)
    전송.기상청응답전송
  그외그외(세션.kma_watch_csv == 없음)
    함수.저장(kma_watch_csv,세션.kma_target_name)
    함수.저장(kma_reply_text,문장.KMA지역추가완료문장)
    전송.기상청응답전송
  그외
    함수.붙이기(kma_watch_csv,|,세션.kma_target_name)
    함수.저장(kma_reply_text,문장.KMA지역추가완료문장)
    전송.기상청응답전송
}
처리::TELEGRAM.텔레그램기상청지역삭제명령처리
{
  만약에(참)
    함수.단어분리(kma_del_word_list,세션.kma_region_rest)
    함수.단어합치기(kma_target_name,세션.리스트.kma_del_word_list,1)
    함수.쪼개기(kma_watch_list,세션.kma_watch_csv,|)
    함수.저장(kma_del_idx,0)
    함수.저장(kma_del_found,0)
    함수.저장(kma_new_csv,없음)
    함수.저장(kma_new_found,0)
    처리.KMA지역삭제순회
}
처리::KMA.KMA지역삭제순회
{
  만약에(세션.kma_del_idx >= 세션.리스트.kma_watch_list.SIZE)
    처리.KMA지역삭제완료
  그외
    처리.KMA지역삭제항목검사
}
처리::KMA.KMA지역삭제항목검사
{
  만약에(세션.리스트.kma_watch_list[세션.kma_del_idx] == 세션.kma_target_name)
    함수.더하기(kma_del_found,세션.kma_del_found,1)
    함수.더하기(kma_del_idx,세션.kma_del_idx,1)
    처리.KMA지역삭제순회
  그외
    처리.KMA지역삭제보존
}
처리::KMA.KMA지역삭제보존
{
  만약에(세션.kma_new_found == 0)
    함수.저장(kma_new_csv,세션.리스트.kma_watch_list[세션.kma_del_idx])
    함수.저장(kma_new_found,1)
    함수.더하기(kma_del_idx,세션.kma_del_idx,1)
    처리.KMA지역삭제순회
  그외
    함수.붙이기(kma_new_csv,|,세션.리스트.kma_watch_list[세션.kma_del_idx])
    함수.더하기(kma_del_idx,세션.kma_del_idx,1)
    처리.KMA지역삭제순회
}
처리::KMA.KMA지역삭제완료
{
  만약에(세션.kma_del_found > 0)
    함수.저장(kma_watch_csv,세션.kma_new_csv)
    함수.저장(kma_reply_text,문장.KMA지역삭제완료문장)
    전송.기상청응답전송
  그외
    함수.저장(kma_reply_text,문장.KMA지역삭제실패문장)
    전송.기상청응답전송
}
처리::KMA.KMA지역좌표확인
{
  만약에(세션.kma_target_name == 서울)
    함수.저장(kma_nx,60)
    함수.저장(kma_ny,127)
    함수.저장(kma_region_found,1)
    처리.KMA지역확인완료
  그외그외(세션.kma_target_name == 부산)
    함수.저장(kma_nx,98)
    함수.저장(kma_ny,76)
    함수.저장(kma_region_found,1)
    처리.KMA지역확인완료
  그외그외(세션.kma_target_name == 대구)
    함수.저장(kma_nx,89)
    함수.저장(kma_ny,90)
    함수.저장(kma_region_found,1)
    처리.KMA지역확인완료
  그외그외(세션.kma_target_name == 인천)
    함수.저장(kma_nx,55)
    함수.저장(kma_ny,124)
    함수.저장(kma_region_found,1)
    처리.KMA지역확인완료
  그외그외(세션.kma_target_name == 광주)
    함수.저장(kma_nx,58)
    함수.저장(kma_ny,74)
    함수.저장(kma_region_found,1)
    처리.KMA지역확인완료
  그외그외(세션.kma_target_name == 대전)
    함수.저장(kma_nx,67)
    함수.저장(kma_ny,100)
    함수.저장(kma_region_found,1)
    처리.KMA지역확인완료
  그외그외(세션.kma_target_name == 울산)
    함수.저장(kma_nx,102)
    함수.저장(kma_ny,84)
    함수.저장(kma_region_found,1)
    처리.KMA지역확인완료
  그외그외(세션.kma_target_name == 세종)
    함수.저장(kma_nx,66)
    함수.저장(kma_ny,103)
    함수.저장(kma_region_found,1)
    처리.KMA지역확인완료
  그외그외(세션.kma_target_name == 수원)
    함수.저장(kma_nx,60)
    함수.저장(kma_ny,121)
    함수.저장(kma_region_found,1)
    처리.KMA지역확인완료
  그외그외(세션.kma_target_name == 제주)
    함수.저장(kma_nx,52)
    함수.저장(kma_ny,38)
    함수.저장(kma_region_found,1)
    처리.KMA지역확인완료
  그외
    함수.저장(kma_region_found,0)
    처리.KMA지역확인완료
}
처리::KMA.KMA지역확인완료
{
  만약에(세션.kma_region_found == 0) 그리고(세션.kma_mode == 조회단건)
    함수.저장(kma_reply_text,문장.KMA지역미지원문장)
    전송.기상청응답전송
  그외그외(세션.kma_region_found == 0)
    함수.더하기(kma_walk_idx,세션.kma_walk_idx,1)
    처리.KMA관심지역순회다음
  그외
    처리.KMA시각계산
}
처리::KMA.KMA시각계산
{
  만약에(참)
    함수.날짜(kma_base_date,%Y%m%d)
    함수.날짜(kma_hour,%H)
    처리.KMA발표시각분기
}
처리::KMA.KMA발표시각분기
{
  만약에(세션.kma_hour >= 23)
    함수.저장(kma_base_time,2300)
    함수.저장(kma_base_hour,23)
    전송.KMA단기예보조회전송
  그외그외(세션.kma_hour >= 20)
    함수.저장(kma_base_time,2000)
    함수.저장(kma_base_hour,20)
    전송.KMA단기예보조회전송
  그외그외(세션.kma_hour >= 17)
    함수.저장(kma_base_time,1700)
    함수.저장(kma_base_hour,17)
    전송.KMA단기예보조회전송
  그외그외(세션.kma_hour >= 14)
    함수.저장(kma_base_time,1400)
    함수.저장(kma_base_hour,14)
    전송.KMA단기예보조회전송
  그외그외(세션.kma_hour >= 11)
    함수.저장(kma_base_time,1100)
    함수.저장(kma_base_hour,11)
    전송.KMA단기예보조회전송
  그외그외(세션.kma_hour >= 8)
    함수.저장(kma_base_time,0800)
    함수.저장(kma_base_hour,8)
    전송.KMA단기예보조회전송
  그외그외(세션.kma_hour >= 5)
    함수.저장(kma_base_time,0500)
    함수.저장(kma_base_hour,5)
    전송.KMA단기예보조회전송
  그외
    함수.저장(kma_base_time,0200)
    함수.저장(kma_base_hour,2)
    전송.KMA단기예보조회전송
}
처리::KMA.KMA응답분기처리
{
  만약에(세션.kma_mode == 조회단건)
    함수.객체저장(kma_items,수신메시지.response.body.items.item)
    함수.저장(kma_item_idx,0)
    함수.저장(kma_tmp,없음)
    함수.저장(kma_pop,없음)
    함수.저장(kma_sky_code,없음)
    함수.저장(kma_pty_code,없음)
    처리.KMA항목순회
  그외그외(세션.kma_mode == 폴링)
    함수.객체저장(kma_items,수신메시지.response.body.items.item)
    함수.저장(kma_item_idx,0)
    함수.저장(kma_tmp,없음)
    함수.저장(kma_pop,없음)
    함수.저장(kma_sky_code,없음)
    함수.저장(kma_pty_code,없음)
    처리.KMA항목순회
  그외그외(세션.kma_mode == 지역조회)
    함수.객체저장(kma_items,수신메시지.response.body.items.item)
    함수.저장(kma_item_idx,0)
    함수.저장(kma_tmp,없음)
    함수.저장(kma_pop,없음)
    함수.저장(kma_sky_code,없음)
    함수.저장(kma_pty_code,없음)
    처리.KMA항목순회
  그외
    로그.출력(KMA 알 수 없는 응답 모드)
}
처리::KMA.KMA항목순회
{
  만약에(세션.객체.kma_items[세션.kma_item_idx].category == NULL)
    처리.KMA요약생성
  그외
    처리.KMA항목검사
}
처리::KMA.KMA항목검사
{
  만약에(세션.객체.kma_items[세션.kma_item_idx].category == TMP)
    함수.저장(kma_tmp,세션.객체.kma_items[세션.kma_item_idx].fcstValue)
    함수.더하기(kma_item_idx,세션.kma_item_idx,1)
    처리.KMA항목순회
  그외그외(세션.객체.kma_items[세션.kma_item_idx].category == POP)
    함수.저장(kma_pop,세션.객체.kma_items[세션.kma_item_idx].fcstValue)
    함수.더하기(kma_item_idx,세션.kma_item_idx,1)
    처리.KMA항목순회
  그외그외(세션.객체.kma_items[세션.kma_item_idx].category == SKY)
    함수.저장(kma_sky_code,세션.객체.kma_items[세션.kma_item_idx].fcstValue)
    함수.더하기(kma_item_idx,세션.kma_item_idx,1)
    처리.KMA항목순회
  그외그외(세션.객체.kma_items[세션.kma_item_idx].category == PTY)
    함수.저장(kma_pty_code,세션.객체.kma_items[세션.kma_item_idx].fcstValue)
    함수.더하기(kma_item_idx,세션.kma_item_idx,1)
    처리.KMA항목순회
  그외
    함수.더하기(kma_item_idx,세션.kma_item_idx,1)
    처리.KMA항목순회
}
처리::KMA.KMA요약생성
{
  만약에(세션.kma_pty_code == 1)
    함수.저장(kma_weather_text,비)
    처리.KMA임계값판정
  그외그외(세션.kma_pty_code == 2)
    함수.저장(kma_weather_text,비또는눈)
    처리.KMA임계값판정
  그외그외(세션.kma_pty_code == 3)
    함수.저장(kma_weather_text,눈)
    처리.KMA임계값판정
  그외그외(세션.kma_pty_code == 4)
    함수.저장(kma_weather_text,소나기)
    처리.KMA임계값판정
  그외그외(세션.kma_sky_code == 1)
    함수.저장(kma_weather_text,맑음)
    처리.KMA임계값판정
  그외그외(세션.kma_sky_code == 3)
    함수.저장(kma_weather_text,구름많음)
    처리.KMA임계값판정
  그외그외(세션.kma_sky_code == 4)
    함수.저장(kma_weather_text,흐림)
    처리.KMA임계값판정
  그외
    함수.저장(kma_weather_text,정보없음)
    처리.KMA임계값판정
}
처리::KMA.KMA임계값판정
{
  만약에(세션.kma_mode == 조회단건)
    처리.KMA단건응답조립
  그외그외(세션.kma_mode == 지역조회)
    처리.KMA지역조회라인추가
  그외그외(세션.kma_pty_code != 0)
    처리.KMA감시라인추가
  그외그외(세션.kma_pop >= 설정.KMA.threshold)
    처리.KMA감시라인추가
  그외
    함수.더하기(kma_walk_idx,세션.kma_walk_idx,1)
    처리.KMA관심지역순회다음
}
처리::KMA.KMA단건응답조립
{
  만약에(참)
    함수.저장(kma_reply_text,문장.KMA날씨요약문장)
    전송.기상청응답전송
}
처리::KMA.KMA감시라인추가
{
  만약에(세션.kma_walk_found == 0)
    함수.저장(kma_walk_lines,문장.KMA감시알림라인문장)
    함수.저장(kma_walk_found,1)
    함수.더하기(kma_walk_idx,세션.kma_walk_idx,1)
    처리.KMA관심지역순회다음
  그외
    함수.붙이기(kma_walk_lines,|,문장.KMA감시알림라인문장)
    함수.더하기(kma_walk_idx,세션.kma_walk_idx,1)
    처리.KMA관심지역순회다음
}
처리::KMA.KMA지역조회라인추가
{
  만약에(세션.kma_walk_found == 0)
    함수.저장(kma_walk_lines,문장.KMA날씨요약문장)
    함수.저장(kma_walk_found,1)
    함수.더하기(kma_walk_idx,세션.kma_walk_idx,1)
    처리.KMA관심지역순회다음
  그외
    함수.붙이기(kma_walk_lines,|,문장.KMA날씨요약문장)
    함수.더하기(kma_walk_idx,세션.kma_walk_idx,1)
    처리.KMA관심지역순회다음
}
처리::KMA.KMA관심지역순회시작
{
  만약에(세션.kma_watch_csv == 없음)
    함수.저장(kma_walk_lines,없음)
    처리.KMA관심지역순회완료
  그외
    함수.쪼개기(kma_watch_list,세션.kma_watch_csv,|)
    함수.저장(kma_walk_idx,0)
    함수.저장(kma_walk_found,0)
    함수.저장(kma_walk_lines,없음)
    처리.KMA관심지역순회다음
}
처리::KMA.KMA관심지역순회다음
{
  만약에(세션.kma_walk_idx >= 세션.리스트.kma_watch_list.SIZE)
    처리.KMA관심지역순회완료
  그외
    함수.저장(kma_target_name,세션.리스트.kma_watch_list[세션.kma_walk_idx])
    처리.KMA지역좌표확인
}
처리::KMA.KMA관심지역순회완료
{
  만약에(세션.kma_mode == 지역조회) 그리고(세션.kma_walk_lines != 없음)
    함수.저장(kma_reply_text,세션.kma_walk_lines)
    전송.기상청응답전송
  그외그외(세션.kma_mode == 지역조회)
    함수.저장(kma_reply_text,문장.KMA지역조회빈목록문장)
    전송.기상청응답전송
  그외그외(세션.kma_walk_lines != 없음)
    함수.저장(kma_watch_alert_text,세션.kma_walk_lines)
    전송.기상알림텔레그램전송
  그외
    로그.출력(기상청 관심지역 감시 - 임계값 초과 지역 없음, 알림 생략)
}
전송::KMA.KMA단기예보조회전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.KMA.domain
  전송메시지.주소.경로 = /getVilageFcst
  전송메시지.주소.파라미터[0].key = serviceKey
  전송메시지.주소.파라미터[0].val = 설정.KMA.service_key
  전송메시지.주소.파라미터[1].key = pageNo
  전송메시지.주소.파라미터[1].val = 1
  전송메시지.주소.파라미터[2].key = numOfRows
  전송메시지.주소.파라미터[2].val = 12
  전송메시지.주소.파라미터[3].key = dataType
  전송메시지.주소.파라미터[3].val = JSON
  전송메시지.주소.파라미터[4].key = base_date
  전송메시지.주소.파라미터[4].val = 세션.kma_base_date
  전송메시지.주소.파라미터[5].key = base_time
  전송메시지.주소.파라미터[5].val = 세션.kma_base_time
  전송메시지.주소.파라미터[6].key = nx
  전송메시지.주소.파라미터[6].val = 세션.kma_nx
  전송메시지.주소.파라미터[7].key = ny
  전송메시지.주소.파라미터[7].val = 세션.kma_ny
}
전송::TELEGRAM.기상청응답전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 세션.pending_reply_chat_id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 세션.kma_reply_text
}
전송::TELEGRAM.기상알림텔레그램전송
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.텔레그램응답경로
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 설정.TELEGRAM.my_chat_id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 문장.기상알림문장
}
타이머::KMA.기상알림타이머
{
  전송메시지.이벤트명 = 기상알림틱
  전송메시지.시간 = 3600000
}
문장::KMA.KMA날씨요약문장
{$$$세션.kma_target_name$$$ 날씨: $$$세션.kma_weather_text$$$, 기온 $$$세션.kma_tmp$$$도, 강수확률 $$$세션.kma_pop$$$% (오늘 $$$세션.kma_base_hour$$$시 발표 기준)}
문장::KMA.KMA감시알림라인문장
{$$$세션.kma_target_name$$$ $$$세션.kma_weather_text$$$ 강수확률 $$$세션.kma_pop$$$%}
문장::TELEGRAM.기상알림문장
{[기상청 날씨 알림] 강수확률 $$$설정.KMA.threshold$$$% 이상 또는 비/눈 예보가 있는 관심지역이 있습니다.
$$$세션.kma_watch_alert_text$$$}
문장::TELEGRAM.KMA지역미지원문장
{"$$$세션.kma_target_name$$$"은(는) 아직 지원하지 않는 지역입니다. 지원 지역: 서울, 부산, 대구, 인천, 광주, 대전, 울산, 세종, 수원, 제주}
문장::TELEGRAM.KMA지역추가완료문장
{$$$세션.kma_target_name$$$을(를) 관심지역에 추가했습니다. 매일 $$$설정.KMA.alert_hour$$$시경 강수확률 $$$설정.KMA.threshold$$$% 이상 또는 비/눈 예보 시 알려드립니다.}
문장::TELEGRAM.KMA지역삭제완료문장
{$$$세션.kma_target_name$$$을(를) 관심지역에서 삭제했습니다.}
문장::TELEGRAM.KMA지역삭제실패문장
{$$$세션.kma_target_name$$$은(는) 등록된 관심지역이 아닙니다.}
문장::TELEGRAM.KMA지역조회빈목록문장
{등록된 관심지역이 없습니다. "기상청 지역 추가 서울"처럼 말씀해주세요.}

