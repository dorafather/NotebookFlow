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
}
처리::FLOW.클루드코드완료처리
{
  만약에(참)
    함수.저장(agent_chat_id,수신메시지.chat_id)
    함수.저장(agent_result,수신메시지.result)
    전송.클루드코드완료텔레그램전송
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
    함수.앞자리비교(cmd_텔넷,수신메시지.result[0].message.text,텔넷)
    함수.앞자리비교(cmd_파일,수신메시지.result[0].message.text,파일)
    함수.앞자리비교(cmd_사진,수신메시지.result[0].message.text,사진)
    함수.앞자리비교(cmd_고객문의,수신메시지.result[0].message.text,고객문의)
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
    함수.저장(download_file_name,photo.jpg)
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
  그외그외(세션.cmd_텔넷 == 1)
    처리.텔레그램텔넷명령처리
  그외그외(세션.cmd_파일 == 1)
    처리.텔레그램파일명령처리
  그외그외(세션.cmd_사진 == 1)
    처리.텔레그램사진명령처리
  그외그외(세션.cmd_고객문의 == 1)
    처리.텔레그램고객문의명령처리
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
    함수.붙이기(telnet_rsp,수신메시지.stderr,수신메시지.stdout)
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

