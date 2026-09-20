# NotebookFlow

> 당신의 노트북이 모든 서비스를 지휘하는 오케스트라가 됩니다.

NotebookFlow는 여러분의 Windows PC를 하나의 자동화 허브로 만들어주는
브릿지 프로그램입니다. Telegram으로 말을 걸면 GitHub 이슈를 조회하고,
Slack·Discord·ntfy·Bluesky로 알림을 보내고, 원격 서버(SSH)에 명령을
내리고, 파일을 자동으로 정리하고, 심지어 Claude Code를 불러 코드를
직접 고치게 할 수도 있습니다.

---

## 핵심 기능

- **Telegram 통합 소통창구** — 별도 앱 없이 Telegram 대화 한 줄로
  전체 기능을 제어합니다. `help`라고 입력하면 지금 쓸 수 있는 명령어
  전체를 안내해줍니다.
- **8개 이상의 외부 서비스 카탈로그** — Telegram, ntfy, Discord, GitHub,
  Slack, Bluesky, Linear, Notion 등 자주 쓰는 서비스를 웹 연동 관리
  화면에서 등록하고 바로 연결할 수 있습니다.
- **agent-flow** — Telegram에서 바로 Claude Code를 호출해 코드를
  읽고 고치고 커밋하게 합니다.
- **ssh-flow** — 원격 서버에 명령을 보내고 결과(stdout/stderr)를
  돌려받습니다. 여러 대의 원격 서버를 별칭으로 등록해 골라서 보낼 수
  있습니다.
- **file-ra** — 폴더를 감시하다가 조건에 맞는 파일이 생기면 자동으로
  옮기거나 알림을 보내는 파일 자동화 엔진입니다.
- **React 기반 웹 연동 관리 화면** — 시나리오(DSL)와 서비스 연동
  정보를 코드 없이 눈으로 보고 편집할 수 있습니다.

내부적으로는 C++로 작성된 DSL(시나리오 스크립트) 엔진이 위 모든
연동을 오케스트레이션하며, 각 서비스 브릿지(agent-flow/ssh-flow/
file-ra 등)는 독립된 프로세스로 떠 있다가 REST로 명령을 받습니다.
동작 원리와 DSL 문법을 더 깊이 알고 싶다면 저장소의 `CLAUDE.md`를
참고하세요 — 이 프로젝트를 처음부터 함께 만들어온 Claude Code용
전체 가이드이자, 사람에게도 그대로 유효한 아키텍처 문서입니다.

---

## 설치 방법

가장 쉬운 방법은 [Releases](../../releases) 페이지에서 최신
`NotebookFlow_Setup.exe` 설치 파일을 내려받아 실행하는 것입니다
(Inno Setup 기반 설치 프로그램, Windows 전용).

소스에서 직접 빌드하고 싶다면:

1. `libUtil/`을 먼저 정적 라이브러리로 빌드합니다 (`libUtil/build_util.bat`,
   MSVC Build Tools 필요).
2. `NotebookFlow/1. build_NotebookFlowt.bat`으로 메인 실행 파일을
   빌드합니다.
3. `agent-flow/`, `ssh-flow/`, `file-ra/`, `mcp-flow/`는 각각
   PyInstaller로 패키징된 독립 프로세스입니다(`*.spec` 참고).
4. `installer/build_installer.ps1`을 실행하면 위 산출물을 모아
   `installer/NotebookFlow.iss`로 설치 파일까지 만들어줍니다.

## 빠른 시작

1. 설치 후 처음 실행하면 `config/addr.ini.template`을 기반으로
   `addr.ini`가 자동 생성됩니다. 이 파일에 실제로 쓸 서비스의
   자격증명(Telegram bot token, GitHub 토큰 등)을 채워 넣으세요.
   **이 파일에는 실제 비밀번호/토큰이 들어가므로 절대 공유하거나
   커밋하지 마세요.**
2. [BotFather](https://t.me/BotFather)로 Telegram 봇을 하나
   만들고, bot token과 본인의 chat_id를 `addr.ini`의 `[TELEGRAM]`
   섹션에 넣습니다.
3. NotebookFlow를 실행(또는 재시작)한 뒤, 등록한 봇에게 `help`라고
   보내보세요. 지금 쓸 수 있는 명령어 목록이 돌아오면 준비 완료입니다.
4. 웹 연동 관리 화면(`frontend/`, 기본 포트는 `addr.ini`의 `[FLOW]`
   섹션 참고)에서 나머지 서비스(Slack, Discord, GitHub, SSH 대상
   등)를 하나씩 추가할 수 있습니다.

---

## Notebook AX 생태계

NotebookFlow는 "Notebook AX" 생태계의 한 축입니다. 같은 생태계의
[NotebookRAG](https://github.com/dorafather/NotebookRAG)가 문서를
이해하고 검색하는 두뇌 역할이라면, NotebookFlow는 그 두뇌와 세상의
여러 서비스를 이어주는 손발 역할을 합니다. 두 프로젝트는 독립적으로
쓸 수 있지만, 함께 쓰면 "문서를 찾아 요약해서 Telegram으로 알려줘"
같은 흐름도 자연스럽게 이어집니다.

---

## 커뮤니티 / 소통

NotebookFlow는 원래 개인용으로 쓰던 자동화 도구를, 사람과 AI 에이전트가
함께 만들어가는 앱 생태계로 넓혀보려는 시도입니다. 버그를 발견했거나,
새로운 서비스 연동이 필요하거나, 그냥 아이디어가 있다면 편하게
[Issues](../../issues)에 등록해주세요. 저희가 적극적으로 살펴보고
소통하겠습니다.

---

## 라이선스

[GNU Affero General Public License v3.0](LICENSE) — 같은 Notebook AX
생태계인 [NotebookRAG](https://github.com/dorafather/NotebookRAG)와
동일한 라이선스입니다. 이 프로젝트가 포함하는 일부 서드파티 코드
(cpp-httplib, json-c 기반 JSON 파서)의 라이선스는 [NOTICE.md](NOTICE.md)에
별도로 명시되어 있습니다.
