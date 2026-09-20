// ── 백엔드(NotebookFlow.exe, 포트 18099) 연동 계층 ──────────────────────────
//
// 실제 소스(코드 리딩으로 확인한 사실, 문서에는 없던 내용 포함):
//
// 1) GET/PUT /notebookflow/config/addr
//    - Flow::applyAddrConfig() -> ExeCore.m_ini.IMPORT() -> IniParser::IMPORT()가
//      매번 카테고리 리스트를 전부 clear() 하고 메시지로 다시 채운다.
//      즉 PUT 바디에는 "지금 고치는 연동 하나"가 아니라 addr.ini의 모든
//      카테고리(INFO/RA_ADDRESS/FLOW/RA_SESSION_ID + 8개 연동)를 전부
//      실어 보내야 한다 - 일부만 보내면 나머지 카테고리가 통째로 사라진다.
//      그래서 이 프론트는 GET으로 받은 전체 객체를 그대로 들고 있다가,
//      편집한 카테고리만 갈아끼운 뒤 전체를 다시 PUT한다.
//    - IniObject::JSON()/IMPORT()가 각 val에 serialPath()/deserialPath()를
//      적용한다 - "/"가 "^^^"로 인코딩되어 오고, 보낼 때도 그 형태여야
//      한다(디스크 addr.ini에는 실제 "/"로 저장됨). 그래서 모든 값에
//      대해 encodeSlash/decodeSlash를 건다(가리지 않고 전부 - 슬래시가
//      없는 값에는 어차피 no-op).
//
// 2) GET/PUT /notebookflow/config/rest
//    - Flow::applyRestConfig() -> ExeScen::IMPORT()가 state-list/procedure-list/
//      action-list/stmt-list를 전부 clear() 하고 다시 채운다 - addr와 동일하게
//      "시나리오 전체"를 매번 통째로 주고받아야 한다.
//    - ActionParser/ProcParser/StateParser는 .sce 원문을 처음 파싱할 때
//      한 줄 전체에 serialPath()를 걸어 리터럴 '/'을 내부적으로 "^^^"로
//      저장해둔다(문자 단위 상태머신이 '/'를 다른 구분자로 오인하지 않게
//      하는 용도 - EXESCEN.cpp 주석 확인). JSON()/IMPORT()는 이 인코딩을
//      풀거나 다시 걸지 않고 그대로 통과시킨다. 실측 확인(LINEAR 이벤트의
//      event-name이 "^^^graphql"로, GitHub 전송의 주소.경로가
//      "^^^repos^^^..."로 옴) - 그래서 이벤트(event-name/event-param),
//      처리(condition/exe-name), 전송/타이머(param-key/param-val) 전부
//      encodeSlash/decodeSlash 대상이다.
//    - 예외: 문장(stmt-list의 stmt-data)은 StmtParser가 이 인코딩을 전혀
//      타지 않는다(PARSE/IMPORT/JSON 어디에도 serialPath 호출 없음) -
//      실측으로도 "telegramPollPath"의 stmt-data가 실제 "/"로 그대로 옴.
//      따라서 문장 값은 인코딩하지 않고 그대로 주고받는다.
//
// 이 두 사실(전체 재전송 필요, ^^^ 인코딩 범위)은 업무지침 문서에 없던
// 내용이라 여기 주석으로 남긴다 - 작업지침 4번 항목("^^^ 인코딩 처리")의
// 실측 확인 결과이기도 하다.

export function decodeSlash(s) {
  if (typeof s !== 'string') return s;
  return s.split('^^^').join('/');
}
export function encodeSlash(s) {
  if (typeof s !== 'string') return s;
  return s.split('/').join('^^^');
}

// addr.ini 카테고리 중 "연동"이 아니라 시스템/내부 설정인 것들 - 사이드바
// 목록에서 제외한다(실측: GET 응답에 INFO/RA_ADDRESS/FLOW/RA_SESSION_ID +
// 8개 연동 카테고리가 함께 온다).
// [TRACE].name은 "실시간 추적" 탭이 addr.ini의 기존 설정 API를
// 재사용해서 쓰는 제어용 값(현재 추적 중인 연동 이름)일 뿐 실제 연동이
// 아니다 - 빼지 않으면 사이드바에 9번째 가짜 "연동"으로 나타난다.
export const ADDR_SYSTEM_KEYS = ['INFO', 'RA_ADDRESS', 'FLOW', 'RA_SESSION_ID', 'TRACE'];

// 연동 카탈로그 메타데이터 - GET /notebookflow/catalog API는 아직 없음(범위
// 밖, 업무지침 "범위 밖" 3번 항목). "연동 추가" 모달은 참고용 정적 목록만
// 보여준다(목업과 동일하게 5개, 클릭해도 실제로 추가되지 않음 - 온보딩
// 플로우는 후속 작업).
export const CATALOG = [
  { key: 'TELEGRAM', name: 'Telegram', color: '#26a1e0', letter: 'T', fields: 'bot_token, 내chat_id' },
  { key: 'NTFY', name: 'ntfy', color: '#5586e6', letter: 'N', fields: 'topic' },
  { key: 'DISCORD', name: 'Discord', color: '#5865f2', letter: 'D', fields: 'webhook_url' },
  { key: 'GITHUB', name: 'GitHub', color: '#333', letter: 'G', fields: 'token, repo' },
  { key: 'SLACK', name: 'Slack', color: '#611f69', letter: 'S', fields: 'webhook_url' },
];

// 사이드바에 보여줄 아이콘 색상/이니셜 - 카탈로그에 있으면 그걸 쓰고,
// 카탈로그에 없는 연동(BSKY/LINEAR/NOTION 등)은 이름 첫 글자로 대체한다.
export function iconFor(key) {
  const found = CATALOG.find((c) => c.key === key);
  if (found) return { color: found.color, letter: found.letter };
  const palette = ['#2e9b6f', '#a3562b', '#8a4fd6', '#4f7cff', '#c0455a'];
  const idx = Math.abs(hashCode(key)) % palette.length;
  return { color: palette[idx], letter: key.slice(0, 1).toUpperCase() };
}
function hashCode(s) {
  let h = 0;
  for (let i = 0; i < s.length; i++) h = (h << 5) - h + s.charCodeAt(i);
  return h;
}

async function getJSON(path) {
  const res = await fetch(path);
  if (!res.ok) throw new Error(`GET ${path} -> ${res.status}`);
  return res.json();
}
async function putJSON(path, body) {
  const res = await fetch(path, {
    method: 'PUT',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(body),
  });
  if (!res.ok) throw new Error(`PUT ${path} -> ${res.status}`);
  return res.json();
}

export const api = {
  getAddrConfig: () => getJSON('/notebookflow/config/addr'),
  putAddrConfig: (fullObj) => putJSON('/notebookflow/config/addr', fullObj),
  getRestConfig: () => getJSON('/notebookflow/config/rest'),
  putRestConfig: (fullObj) => putJSON('/notebookflow/config/rest', fullObj),
  // NotebookFlow.exe/agent-flow/ssh-flow/file-ra 4개 프로세스의 헬스체크
  // 결과를 한 번에 받아온다. 백엔드가 각 자식에게 짧은 타임아웃(2~3초)으로
  // 이미 순서대로 물어본 결과를 모아 반환하므로, 여기서는 추가 재시도/병렬화
  // 없이 그대로 한 번 호출한다(SystemStatusTab이 주기적으로 폴링).
  getSystemStatus: () => getJSON('/notebookflow/status'),
};
