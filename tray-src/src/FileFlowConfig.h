#pragma once

// NotebookFlow.exe/File RA 고정 접속 정보.
// 이 앱 자신이 곧 NotebookFlow 전용 트레이이므로 별도 설정 파일 없이
// 상수로 둔다(과설계 금지) - 여러 대상을 설정 가능하게 할 필요가 생기면
// 그때 확장.
static const wchar_t* const kNotebookFlowHost = L"127.0.0.1";
static const int kNotebookFlowPort = 18099;
static const wchar_t* const kFileRaHost = L"127.0.0.1";
static const int kFileRaPort = 8425;

// ssh-flow(SSH 원격 명령 실행)/agent-flow(Telegram<->Claude Code 역방향
// 연동)도 동일하게 tray-flow.exe가 자식 프로세스로 관리한다 - 위
// NotebookFlow/File RA와 같은 원칙(과설계 금지, 상수로 고정).
static const wchar_t* const kSshFlowHost = L"127.0.0.1";
static const int kSshFlowPort = 8450;
static const wchar_t* const kAgentFlowHost = L"127.0.0.1";
static const int kAgentFlowPort = 8430;

// Tray가 NotebookFlow에 START_REQ로 만드는 세션의 고정 식별자 - "규칙목록의
// 진실의 원천은 Tray"이므로 NotebookFlow 쪽 세션 자체는 매번 새로 시작해도
// 무방하다(재개할 상태가 없음, File RA가 실제 스케줄 상태를 들고 있음).
static const wchar_t* const kFileFlowAsId = L"filetray";
