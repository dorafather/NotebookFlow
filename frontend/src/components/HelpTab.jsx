// 다른 탭(이벤트/처리/전송/타이머/문장)이 다루는 편집 UI가 아니라, 그 UI들이
// 실제로 어떻게 쓰이는지를 설명하는 정적 문서 탭이라 restConfig를 건드리지
// 않는다(저장 버튼 없음). 모든 예시는 이 프로젝트의 실제 rest.sce에서 그대로
// 가져온 것이다.

function Code({ children }) {
  return <div className="help-code">{children}</div>;
}
function Section({ id, title, children }) {
  return (
    <section className="help-section" id={id}>
      <h3>{title}</h3>
      {children}
    </section>
  );
}

export default function HelpTab() {
  return (
    <div className="subpane">
      <div className="panel">
        <div className="panel-title">도움말</div>
        <div className="panel-desc">
          시나리오(.sce) 문법 상세 설명서입니다. 모든 예시는 이 프로젝트의 실제 rest.sce에서 그대로 가져온 것입니다.
        </div>
        <div className="help-toc">
          <a href="#help-session">1. 세션 전역 설정</a>
          <a href="#help-msg">2. 수신메시지 / 전송메시지</a>
          <a href="#help-cond">3. 조건문</a>
          <a href="#help-func">4. 인라인 함수</a>
        </div>

        <Section id="help-session" title="1. 세션 전역 설정 설명서">
          <p>
            <b>세션</b>은 시나리오가 실행되는 동안 값을 담아두는 저장소입니다. 한 번 저장하면 같은 실행 흐름 안에서는
            물론, 뒤이어 들어오는 다른 메시지를 처리할 때도(예: 텔레그램 파일 업로드 감지 → getFile 응답 → 실제 다운로드처럼
            여러 단계에 걸친 흐름) 계속 값이 남아있습니다. <b>수신메시지</b>는 요청 하나가 끝나면 사라지지만, <b>세션</b>은
            그 요청들 사이에서 값을 이어주는 다리 역할을 합니다.
          </p>

          <p><b>저장하기</b> — <code>함수.저장(변수명, 값)</code></p>
          <Code>{`함수.저장(tg_token,설정.TELEGRAM.bot_token)
함수.저장(download_file_id,수신메시지.result[0].message.document.file_id)`}</Code>
          <p>값 자리에는 리터럴 문자열, <code>설정.네임스페이스.키</code>, <code>수신메시지.경로</code> 무엇이든 올 수 있습니다.</p>

          <p><b>주소 저장하기</b> — <code>함수.주소저장(변수명, 수신메시지.주소)</code></p>
          <Code>{`함수.주소저장(sse_addr,수신메시지.주소)`}</Code>
          <p>나중에 그 주소로 직접 응답을 돌려보내야 할 때(예: 비동기 작업 완료 후 원래 요청자에게 회신) 씁니다.</p>

          <p><b>읽기</b> — 저장한 곳 어디서든 <code>세션.변수명</code>으로 참조합니다.</p>
          <Code>{`전송메시지.문서.경로 = 세션.file_path
전송메시지.주소.파라미터[0].val = 세션.download_file_id
만약에(세션.pending_reply_chat_id != NULL)`}</Code>

          <p>
            <b>리스트(배열)</b> — <code>함수.단어분리</code>/<code>함수.쪼개기</code>로 만든 목록은
            <code>세션.리스트.이름[idx]</code> 형태로 각 원소에 접근합니다(idx는 숫자, <code>반복횟수</code> 키워드,
            또는 다른 세션 변수 참조 모두 가능). <code>함수.단어합치기</code>는 목록 자체를 넘길 때 항상
            <code>세션.리스트.이름</code>이라고 온전히 써야 합니다(그냥 <code>세션.이름</code>이 아님).
          </p>
          <Code>{`함수.단어분리(cmd_word_list,수신메시지.result[0].message.text)
함수.단어합치기(cmd_rest,세션.리스트.cmd_word_list,1)`}</Code>

          <p>
            <b>객체(배열-of-객체)</b> — <code>함수.객체저장</code>으로 만든 목록은
            <code>세션.객체.이름[idx].필드명</code> 형태로 접근합니다.
          </p>
          <Code>{`함수.객체저장(gh_issues,수신메시지.결과)
만약에(세션.객체.gh_issues[세션.idx].title != NULL)`}</Code>
        </Section>

        <Section id="help-msg" title="2. 수신메시지 / 전송메시지 리터럴 설정 설명서">
          <p>
            둘 다 JSON 오브젝트이고, <code>.</code>으로 하위 계층을, <code>[idx]</code>로 배열 인덱스를 접근합니다.
            차이는 방향입니다 — <b>수신메시지</b>는 지금 막 들어온 것(진짜 인바운드 요청이거나, 우리가 보낸 요청의 응답
            에코), <b>전송메시지</b>는 <code>전송::</code> 블록 안에서 지금 만들고 있는 요청입니다.
          </p>

          <p><b>수신메시지의 대표 필드</b></p>
          <ul>
            <li><code>메소드</code> — 요청/응답 모두에 붙는 HTTP 메소드(응답 쪽에서는 우리가 보낸 값이 그대로 되돌아온 것이라 필터링 의미가 적음)</li>
            <li><code>응답코드</code> — 우리가 보낸 요청의 응답에만 존재하는 실제 HTTP 상태 코드(진짜 인바운드 요청에는 없음)</li>
            <li><code>주소.도메인</code> / <code>주소.경로</code> / <code>주소.파라미터[N].key</code>·<code>.val</code></li>
            <li><code>이벤트명</code> — FLOW 도메인 전용(내부 트리거 이름), <code>ok</code> — 텔레그램 전용 성공 플래그</li>
            <li>그 외는 응답 바디의 실제 JSON 필드 그대로(예: <code>result[0].message.text</code>)</li>
          </ul>
          <Code>{`만약에(수신메시지.result[0].message.from.id == 설정.TELEGRAM.my_chat_id)
TELEGRAM.수신메시지.응답코드 == 204    처리.디스코드알람응답처리`}</Code>

          <p><b>전송메시지의 통신계층 전용 키</b> — 아래 키들은 실제로 나가는 JSON 바디에 절대 안 남고, 각자 정해진 자리(URL/헤더/멀티파트)로 빠집니다.</p>
          <ul>
            <li><code>메소드</code> = GET/POST</li>
            <li><code>주소.도메인</code> / <code>주소.경로</code> / <code>주소.파라미터[N].key</code>·<code>.val</code> (URL과 쿼리스트링이 됨)</li>
            <li><code>헤더[N].key</code>·<code>.val</code> (실제 HTTP 헤더가 됨)</li>
            <li><code>문서.내용</code>·<code>경로</code>·<code>파일명</code>·<code>필드명</code>·<code>타입</code>·<code>캡션</code> (이 중 하나라도 있으면 전체가 multipart/form-data가 됨)</li>
            <li><code>저장.파일명</code> (응답 바디를 JSON으로 안 보고 파일로 스트림 저장)</li>
          </ul>
          <p>그 외에 여러분이 붙이는 아무 이름이나(<code>text</code>, <code>content</code> 등)는 전부 실제 JSON 바디 필드가 됩니다.</p>
          <Code>{`전송::SLACK.슬랙알람직접전송
{
  전송메시지.메소드 = POST
  전송메시지.주소.도메인 = 설정.SLACK.webhook_url
  전송메시지.text = 세션.alert_message
}`}</Code>
          <Code>{`전송::TELEGRAM.텔넷회신문서전송
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
}`}</Code>

          <p><b>문장(템플릿) 안에서 참조하기</b> — <code>$$$...$$$</code>로 감싸면 실행 시점에 실제 값으로 치환됩니다.</p>
          <Code>{`문장::TELEGRAM.텔레그램응답문장
{받았습니다: $$$수신메시지.result[0].message.text$$$}`}</Code>

          <p><b>문장 결과에 여는 중괄호 <code>{'{'}</code>나 닫는 중괄호 <code>{'}'}</code>가 들어가야 할 때</b> — 이 두 문자는
            이미 <code>문장::이름 {'{'} ... {'}'}</code> 처럼 블록 자체를 감싸는 문법 기호로 쓰이고 있어서, 문장 본문
            안에 그냥 적으면 블록이 거기서 끝난 것으로 파싱됩니다. 그래서 실제 결과값에 중괄호가 필요할 때는(예: GraphQL
            쿼리) 여는 중괄호 자리에 <code>$$$&lt;$$$</code>를, 닫는 중괄호 자리에 <code>$$$&gt;$$$</code>를 대신
            씁니다 — 실행 시점에 진짜 중괄호로 치환됩니다.</p>
          <Code>{`문장::LINEAR.stmtLinearQuery
{$$$<$$$ issues(first: 10) $$$<$$$ nodes $$$<$$$ id title identifier state $$$<$$$ name $$$>$$$ $$$>$$$ $$$>$$$ $$$>$$$}`}</Code>
        </Section>

        <Section id="help-cond" title="3. 조건문 상세 사용 설명서">
          <p>
            조건문은 <b>두 군데</b>에서 쓰이고, 서로 문법이 다릅니다 — 헷갈리기 쉬운 부분이라 꼭 구분해서 알아두세요.
          </p>

          <p><b>① 상태:: 블록(도메인 라우팅)</b> — <code>도메인.수신메시지.필드 == 값    처리.대상</code> 형태 한 줄짜리이고,
            <b><code>==</code> 연산자만 지원합니다.</b> 다른 연산자(<code>!=</code> 등)는 쓰지 마세요.</p>
          <Code>{`상태::FLOW.초기
{
  TELEGRAM.수신메시지.ok == 1    처리.텔레그램수신처리
  SSH.수신메시지.응답코드 == 200    처리.sshRsp
}`}</Code>

          <p><b>② 처리:: 블록 안의 만약에/그외그외/그외</b> — <code>연산자 9종</code>을 지원합니다(처리 탭에서 드롭다운으로 고름):</p>
          <table className="help-table">
            <tbody>
              <tr><td>참 / 거짓</td><td>항상 참 / 항상 거짓 (우항 없음)</td></tr>
              <tr><td>크다 / 작다</td><td><code>&gt;</code> / <code>&lt;</code></td></tr>
              <tr><td>크거나같다 / 작거나같다</td><td><code>&gt;=</code> / <code>&lt;=</code></td></tr>
              <tr><td>완전일치 / 불일치</td><td><code>==</code> / <code>!=</code></td></tr>
              <tr><td>앞자리일치</td><td><code>===</code> (STRNCMP - 접두어 비교)</td></tr>
              <tr><td>부분일치</td><td><code>!!!</code> (STRSTR - 부분 문자열 포함 여부)</td></tr>
            </tbody>
          </table>
          <Code>{`처리::SSH.sshRsp길이분기
{
  만약에(세션.telnet_rsp.길이 > 4096)
    전송.텔넷회신문서전송
  그외
    전송.텔넷회신
}`}</Code>
          <p>
            여러 조건을 순서대로 검사하려면 <b>그외그외(...)</b>(else if)를 이어 붙이고, 마지막에 <b>그외()</b>(else, 조건 없음)로
            나머지를 받습니다. 좌항/우항 자리에는 <code>수신메시지.*</code>, <code>세션.*</code>, 리터럴 값 아무거나 올 수 있습니다.
          </p>
        </Section>

        <Section id="help-func" title="4. 인라인 함수 상세 사용 설명서">
          <p>"처리" 탭의 수행문 추가 드롭다운에서 바로 고를 수 있는 함수들입니다.</p>

          <p><code>함수.저장(변수명, 값)</code> — 세션 변수에 값을 저장합니다.</p>
          <Code>{`함수.저장(tg_token,설정.TELEGRAM.bot_token)`}</Code>

          <p><code>함수.주소저장(변수명, 수신메시지.주소)</code> — 나중에 회신할 주소를 통째로 저장합니다.</p>
          <Code>{`함수.주소저장(sse_addr,수신메시지.주소)`}</Code>

          <p><code>함수.단어분리(리스트명, 문자열)</code> — 공백 기준으로 단어를 쪼개 <code>세션.리스트.리스트명</code>을 만듭니다.</p>
          <Code>{`함수.단어분리(cmd_word_list,수신메시지.result[0].message.text)`}</Code>

          <p><code>함수.단어합치기(대상변수명, 세션.리스트.리스트명, 시작인덱스)</code> — 그 목록의 시작인덱스부터
            끝까지를 공백으로 다시 이어붙입니다(끝에 불필요한 공백은 안 붙음). "N번째 단어(들)만 건너뛰고 나머지를
            돌려받기"에 씁니다.</p>
          <Code>{`함수.단어합치기(cmd_rest,세션.리스트.cmd_word_list,1)`}</Code>

          <p><code>함수.붙이기(대상변수명, 값1, 값2, ...)</code> — 값들을 <b>구분자 없이 그대로</b> 이어붙입니다
            (단어합치기와 달리 공백이 자동으로 안 들어감).</p>
          <Code>{`함수.붙이기(telnet_rsp,수신메시지.stderr,수신메시지.stdout)`}</Code>

          <p><code>함수.더하기(대상변수명, 값1, 값2, ...)</code> — 숫자로 변환해 전부 더합니다.</p>
          <Code>{`함수.더하기(tg_offset,수신메시지.result[0].update_id,1)`}</Code>

          <p><code>함수.빼기(대상변수명, 값1, 값2, ...)</code> — 값1에서 값2, 값3 순서대로 뺍니다.</p>
          <Code>{`함수.빼기(remain,전체수량,사용수량)`}</Code>

          <p><code>함수.객체저장(오브젝트명, 수신메시지.경로)</code> — 응답의 배열/오브젝트를
            <code>세션.객체.오브젝트명[idx].필드</code>로 순회 가능한 형태로 저장합니다. 두 번째 인자는 반드시
            <code>수신메시지.</code>로 시작해야 합니다.</p>
          <Code>{`함수.객체저장(gh_issues,수신메시지.결과)`}</Code>

          <p><code>로그.출력(문구)</code> — 진단용 로그를 남깁니다. 다른 함수들과 달리 <b>쉼표가 아니라 공백</b>으로 여러 값을
            구분합니다(그래서 문구 안에 <code>세션.xxx</code>를 그냥 섞어 써도 됩니다).</p>
          <Code>{`로그.출력(파일 요청 세션.file_path)`}</Code>
        </Section>
      </div>
    </div>
  );
}
