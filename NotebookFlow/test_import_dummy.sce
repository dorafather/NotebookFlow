상태::FLOW.초기
{
  FLOW.수신메시지.이벤트명 == 시작    처리.procRestInit
  TELEGRAM.수신메시지.이벤트명 == telegram_tick    처리.procTelegramPoll
  TELEGRAM.수신메시지.ok == 1    처리.procTelegramResponse
  FLOW.수신메시지.이벤트명 == ntfy_test    처리.procNtfyTest
  NTFY.수신메시지.event == message    처리.procNtfyResponse
  FLOW.수신메시지.이벤트명 == discord_test    처리.procDiscordTest
  FLOW.수신메시지.이벤트명 == github_test    처리.procGithubIssuesTest
  GITHUB.수신메시지.메소드 == GET    처리.procGithubIssuesResponse
  FLOW.수신메시지.이벤트명 == slack_test    처리.procSlackTest
  SLACK.수신메시지.메소드 == POST    처리.procSlackResponse
  BSKY.수신메시지.handle == dorafather.bsky.social    처리.procBskyTokRcv
  LINEAR.수신메시지.주소.경로 == /graphql    처리.procLinearRsp
  FLOW.수신메시지.이벤트명 == notion_test    처리.procNotionTest
}
처리::FLOW.procRestInit
{
  만약에(참)
    함수.날짜(cur-date,%Y-%m-%d)
    로그.출력(Current Date : 세션.cur-date)
    함수.저장(tg_offset,0)
    처리.procTelegramPoll
}
처리::FLOW.procNotionTest
{
  만약에(참)
    로그.출력(notion test start)
    전송.sndNotionMyQuery
}
처리::LINEAR.procLinearRsp
{
  만약에(참)
    로그.출력(Linear Rsp)
    로그.출력(0 수신메시지.data.issues.nodes[0].title)
    로그.출력(0 수신메시지.data.issues.nodes[1].title)
    로그.출력(0 수신메시지.data.issues.nodes[2].title)
}
처리::BSKY.procBskyTokRcv
{
  만약에(참)
    함수.저장(bsky_access,수신메시지.accessJwt)
    함수.저장(bsky_did,수신메시지.did)
}
처리::TELEGRAM.procTelegramPoll
{
  만약에(참)
    함수.저장(tg_token,설정.TELEGRAM.bot_token)
    전송.sndTelegramPoll
    타이머.telegram_tick
}
처리::TELEGRAM.procTelegramResponse
{
  만약에(수신메시지.result[0].update_id != NULL)
    처리.procTelegramMsgFound
  그외
    로그.출력(telegram 새 메시지 없음)
}
처리::TELEGRAM.procTelegramMsgFound
{
  만약에(수신메시지.result[0].message.from.id == 설정.TELEGRAM.내chat_id)
    함수.더하기(tg_offset,수신메시지.result[0].update_id,1)
    전송.sndTelegramReply
  그외
    함수.더하기(tg_offset,수신메시지.result[0].update_id,1)
    로그.출력(알 수 없는 발신자로부터의 메시지 무시)
}
처리::NTFY.procNtfyTest
{
  만약에(참)
    전송.sndNtfyAlert
}
처리::NTFY.procNtfyResponse
{
  만약에(참)
    로그.출력(ntfy 알림 전송 완료)
}
처리::DISCORD.procDiscordTest
{
  만약에(참)
    전송.sndDiscordAlert
}
처리::GITHUB.procGithubIssuesTest
{
  만약에(참)
    함수.저장(gh_token,설정.GITHUB.token)
    전송.sndGithubIssues
}
처리::GITHUB.procGithubIssuesResponse
{
  만약에(참)
    로그.출력(github 이슈 조회 완료)
}
처리::SLACK.procSlackTest
{
  만약에(참)
    전송.sndSlackAlert
}
처리::SLACK.procSlackResponse
{
  만약에(참)
    로그.출력(slack 알림 전송 완료)
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
전송::LINEAR.sndLinearIssue
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.LINEAR.domain
  전송메시지.주소.경로 = /graphql
  전송메시지.헤더[0].key = Authorization
  전송메시지.헤더[0].val = 설정.LINEAR.api_key
  전송메시지.query = 문장.stmtLinearQuery
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
  전송메시지.주소.경로 = /xrpc/com.atproto.repo.createRecord
  전송메시지.헤더[0].key = Authorization
  전송메시지.헤더[0].val = 세션.bsky_access
  전송메시지.repo = 1
  전송메시지.collection = app.bsky.feed.post
  전송메시지.record.$type = app.bsky.feed.post
  전송메시지.record.text = test aaa message
  전송메시지.record.createdAt = 2026-09-08T12:00:00.000Z
}
타이머::TELEGRAM.telegram_tick
{
  전송메시지.이벤트명 = telegram_tick
  전송메시지.시간 = 5000
}
전송::TELEGRAM.sndTelegramPoll
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.telegramPollPath
  전송메시지.주소.파라미터[0].key = offset
  전송메시지.주소.파라미터[0].val = 세션.tg_offset
}
전송::TELEGRAM.sndTelegramReply
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.TELEGRAM.domain
  전송메시지.주소.경로 = 문장.telegramReplyPath
  전송메시지.주소.파라미터[0].key = chat_id
  전송메시지.주소.파라미터[0].val = 수신메시지.result[0].message.chat.id
  전송메시지.주소.파라미터[1].key = text
  전송메시지.주소.파라미터[1].val = 문장.telegramReplyText
}
전송::NTFY.sndNtfyAlert
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.NTFY.domain
  전송메시지.주소.경로 = /
  전송메시지.topic = 설정.NTFY.topic
  전송메시지.message = 수신메시지.PARAMS.알림내용
}
전송::DISCORD.sndDiscordAlert
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.DISCORD.webhook_url
  전송메시지.content = 수신메시지.PARAMS.알림내용
}
전송::GITHUB.sndGithubIssues
{
  전송메시지.메소드 = GET
  전송메시지.주소.도메인 = 설정.GITHUB.domain
  전송메시지.주소.경로 = /repos/dorafather/NotebookRAG/issues
  전송메시지.헤더[0].key = Authorization
  전송메시지.헤더[0].val = 문장.githubAuthHeader
  전송메시지.헤더[1].key = User-Agent
  전송메시지.헤더[1].val = NotebookFlow
}
전송::SLACK.sndSlackAlert
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.SLACK.webhook_url
  전송메시지.text = 수신메시지.PARAMS.알림내용
}
전송::TESTNS.sndBugReproTest
{
}
문장::NOTION.buildNotionAuth
{Bearer $$$설정.NOTION.token$$$}
문장::LINEAR.stmtLinearQuery
{$$$<$$$ issues(first: 10) $$$<$$$ nodes $$$<$$$ id title identifier state $$$<$$$ name $$$>$$$ $$$>$$$ $$$>$$$ $$$>$$$}
문장::BSKY.actk
{/xrpc/com.atproto.server.createSession}
문장::TELEGRAM.telegramPollPath
{/bot$$$세션.tg_token$$$/getUpdates}
문장::TELEGRAM.telegramReplyPath
{/bot$$$세션.tg_token$$$/sendMessage}
문장::TELEGRAM.telegramReplyText
{받았습니다: $$$수신메시지.result[0].message.text$$$}
문장::GITHUB.githubAuthHeader
{token $$$세션.gh_token$$$}
