#include "TrayApp.h"
#include "FileFlowConfig.h"
#include "resource.h"
#include <shellapi.h>
#include <afxdisp.h> // AfxOleInit() - COM 초기화(ShellExecute 등 셸 API 안전성 목적으로 유지)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h> // _beginthreadex (워치독 스레드)
#pragma comment(lib, "ws2_32.lib")

namespace {

// [워치독] 죽었는지 확인하는 주기 - 너무 짧으면 불필요한 폴링, 너무 길면
// 복구까지 체감 공백이 길어진다. tray의 다른 폴링(제안 조회 8초)과 비슷한
// 수준으로 잡는다.
const UINT_PTR kWatchdogTimerId = 1;
const UINT kWatchdogIntervalMs = 5000;

// [업무지침_TrayFlow_프로세스관리.md 요구사항 1] NotebookFlow.exe의 정상
// 기동 확인 - NotebookFlow.exe는 /health 같은 상태조회 엔드포인트가 없고
// (PUT /flow, GET /events/:as_id뿐), PUT /flow도 매칭되는 ACTION_EVENT가
// 없으면 응답을 안 주는 이벤트가 있어(예: 임의 프로브 메시지) 프로토콜
// 레벨로 "정상 응답"을 확인하려 하면 오히려 오탐이 난다. 그래서 여기서는
// 더 낮은 레벨 - "리스닝 소켓이 열렸는지"(TCP connect 성공 여부)만 확인한다.
// httplib::Server가 listen()을 마치고 accept 가능한 상태가 되면 이 connect가
// 성공하므로, "PUT /flow를 받을 준비가 됐는지"를 확인하는 목적에는 충분하다.
bool WaitForTcpPort(const wchar_t* host, int port, DWORD timeoutMs)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return false;

    char hostA[64] = {0};
    WideCharToMultiByte(CP_UTF8, 0, host, -1, hostA, sizeof(hostA), nullptr, nullptr);

    DWORD start = GetTickCount();
    bool connected = false;
    while (GetTickCount() - start < timeoutMs)
    {
        SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s != INVALID_SOCKET)
        {
            sockaddr_in addr = {};
            addr.sin_family = AF_INET;
            addr.sin_port = htons((u_short)port);
            inet_pton(AF_INET, hostA, &addr.sin_addr);
            if (connect(s, (sockaddr*)&addr, sizeof(addr)) == 0)
            {
                connected = true;
            }
            closesocket(s);
        }
        if (connected) break;
        Sleep(300);
    }
    WSACleanup();
    return connected;
}

} // namespace

// ── CTrayWnd ─────────────────────────────────────────────────────────────

CTrayWnd::CTrayWnd()
{
    ZeroMemory(&m_nid, sizeof(m_nid));
}

CTrayWnd::~CTrayWnd()
{
}

BEGIN_MESSAGE_MAP(CTrayWnd, CWnd)
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_MESSAGE(WM_TRAYICON, &CTrayWnd::OnTrayIcon)
    ON_COMMAND(ID_TRAY_OPEN, &CTrayWnd::OnTrayOpen)
    ON_COMMAND(ID_TRAY_EXIT, &CTrayWnd::OnTrayExit)
END_MESSAGE_MAP()

BOOL CTrayWnd::Init()
{
    CString className = AfxRegisterWndClass(0);
    if (!CreateEx(0, className, L"NotebookFlowTrayHidden", WS_OVERLAPPED,
                  CRect(0, 0, 0, 0), nullptr, 0))
    {
        return FALSE;
    }

    m_nid.cbSize = sizeof(m_nid);
    m_nid.hWnd = m_hWnd;
    m_nid.uID = 1;
    m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nid.uCallbackMessage = WM_TRAYICON;
    m_nid.hIcon = AfxGetApp()->LoadIcon(IDI_TRAY_ICON);
    wcscpy_s(m_nid.szTip, L"NotebookFlow");
    Shell_NotifyIconW(NIM_ADD, &m_nid);

    // [업무지침_TrayFlow_프로세스관리.md] NotebookFlow.exe/File RA를
    // 여기서 직접 기동한다 - 다이얼로그의 StartupThreadProc(①+②)가 이
    // 둘에 의존하므로 반드시 그 전에 먼저 떠 있어야 한다.
    LaunchChildProcesses();

    SetTimer(kWatchdogTimerId, kWatchdogIntervalMs, nullptr);

    return TRUE;
}

namespace {

// [업무지침_InnoSetup_설계_1차릴리스.md, 2026-09-19] 배포 설치 구조가
// "bin/notebookflow, bin/agent-flow, bin/ssh-flow, bin/tray"처럼 펼쳐진
// (flat) 형태로 확정됐다 - 지금까지의 개발 트리 레이아웃
// (NotebookFlow-release/{tray-src,NotebookFlow,file-ra/dist/file-ra,
// ssh-flow/dist/ssh-flow,agent-flow/dist/agent-flow})과는 폴더 모양이
// 다르다. 이 프로세스는 지금 실제로 라이브 서비스로 떠서 개발 트리
// 레이아웃을 그대로 감시하고 있으므로, 여기서 설치 레이아웃 전용으로
// 못박아 바꾸면 라이브 서비스가 깨진다 - 대신 "설치 레이아웃 후보를
// 먼저 파일 존재로 확인하고, 없으면 개발 트리 경로로 폴백"하는 방식으로
// 두 레이아웃을 동시에 지원한다(존재 확인 실패 시에도 기존과 동일하게
// 폴백 경로를 그대로 반환하므로, Launch() 실패 처리는 기존과 100%
// 동일하게 유지된다).
std::wstring PickExeDir(const std::wstring& preferredDir, const wchar_t* exeName,
                         const std::wstring& fallbackDir)
{
    std::wstring preferredExe = preferredDir + L"\\" + exeName;
    DWORD attr = GetFileAttributesW(preferredExe.c_str());
    if (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY))
        return preferredDir;
    return fallbackDir;
}

} // namespace

void CTrayWnd::LaunchChildProcesses()
{
    // [요구사항 3] tray-flow.exe 자신의 위치 기준 상대경로 - 하드코딩 금지.
    wchar_t exePathBuf[MAX_PATH];
    GetModuleFileNameW(nullptr, exePathBuf, MAX_PATH);
    std::wstring trayExeDir = exePathBuf;
    size_t slash1 = trayExeDir.find_last_of(L'\\');
    trayExeDir = trayExeDir.substr(0, slash1);                 // 자기 자신의 폴더(tray-src 또는 bin\tray)
    size_t slash2 = trayExeDir.find_last_of(L'\\');
    std::wstring parentDir = trayExeDir.substr(0, slash2);      // 한 단계 위(개발 트리: NotebookFlow-release, 설치: bin)

    // NotebookFlow.exe: 설치 레이아웃(parentDir\notebookflow\) 우선,
    // 개발 트리(parentDir\NotebookFlow\) 폴백.
    m_notebookFlowDir = PickExeDir(parentDir + L"\\notebookflow", L"NotebookFlow.exe",
                                    parentDir + L"\\NotebookFlow");
    m_notebookFlowExe = m_notebookFlowDir + L"\\NotebookFlow.exe";

    // [요구사항 4] file-ra는 PyInstaller로 빌드된 배포물(dist/file-ra/) 경로를
    // 쓴다 - 개발 중 `python file_ra.py`로 직접 실행하던 방식은 Python
    // 미설치 환경에서 동작하지 않으므로 배포 실행파일을 기본으로 한다.
    // file-ra는 1차 릴리스 설치 레이아웃에 아예 포함되지 않으므로
    // (패키징설계정리_1차릴리스 - 규칙기반 자동정리는 2차로 연기) 설치
    // 레이아웃 후보가 존재하지 않아 항상 개발 트리 경로로 폴백된다 -
    // 즉 라이브 서비스(dev tree) 동작은 이전과 완전히 동일하고, 설치
    // 패키지에서는 file-ra Launch()가 조용히 실패(로그만 남고 다이얼로그
    // 기동은 막지 않음)하는 것이 의도된 동작이다.
    m_fileRaDir = PickExeDir(parentDir + L"\\file-ra", L"file-ra.exe",
                              parentDir + L"\\file-ra\\dist\\file-ra");
    m_fileRaExe = m_fileRaDir + L"\\file-ra.exe";
    // [ssh-flow/agent-flow 통합, 2026-09-17] 이 둘도 file-ra와 동일하게
    // PyInstaller 배포물(dist/<name>/) 경로를 쓴다 - python.exe를 직접
    // 호출하지 않는다(기존 관례 그대로). 설치 레이아웃에서는 PyInstaller
    // onedir 결과물이 dist/ 하위 폴더 없이 bin\ssh-flow\, bin\agent-flow\
    // 바로 아래 펼쳐져 배치되므로 그 경로를 우선 시도한다.
    m_sshFlowDir = PickExeDir(parentDir + L"\\ssh-flow", L"ssh-flow.exe",
                               parentDir + L"\\ssh-flow\\dist\\ssh-flow");
    m_sshFlowExe = m_sshFlowDir + L"\\ssh-flow.exe";
    m_agentFlowDir = PickExeDir(parentDir + L"\\agent-flow", L"agent-flow.exe",
                                 parentDir + L"\\agent-flow\\dist\\agent-flow");
    m_agentFlowExe = m_agentFlowDir + L"\\agent-flow.exe";

    m_notebookFlowMgr.Launch(m_notebookFlowExe, m_notebookFlowDir);

    // [요구사항 1, 6] NotebookFlow.exe가 리스닝을 시작할 때까지 기다린 뒤
    // File RA를 띄운다 - 순서가 꼬여도(대기 타임아웃) 아래에서 File RA는
    // 어차피 기동을 계속 진행한다(둘 다 서로 독립된 HTTP 서버라 기동
    // 순서 자체가 정확성에 필수는 아님 - 이 대기는 "느슨한 순서 보장"
    // 목적일 뿐, 실패해도 무조건 진행해 견고하게 처리).
    WaitForTcpPort(kNotebookFlowHost, kNotebookFlowPort, 10000);

    m_fileRaMgr.Launch(m_fileRaExe, m_fileRaDir);

    // [요구사항 1, 6] File RA도 리스닝 시작까지 기다린다 - 다른 자식
    // 프로세스와 동일하게 "느슨한 순서 보장"만 목적이며(실패해도 무조건
    // 진행), File-Flow 설정 다이얼로그가 있던 시절의 규칙목록 relay 레이스
    // 방지 목적은 다이얼로그 제거(2026-09-20)로 더 이상 해당되지 않는다 -
    // 다만 기동 순서 안정성 자체는 여전히 유효하므로 대기는 그대로 둔다.
    WaitForTcpPort(kFileRaHost, kFileRaPort, 10000);

    // [ssh-flow/agent-flow 통합, 2026-09-17] 이 둘은 File-Flow 다이얼로그
    // 기동 시퀀스와 의존관계가 없으므로(기존 ①START_REQ+②규칙목록 흐름은
    // NotebookFlow/File RA만 씀) 순서상 여기서 기동하되, 실패해도 다이얼로그
    // 기동을 막지 않도록 동일하게 "느슨한 순서 보장"만 적용한다.
    m_sshFlowMgr.Launch(m_sshFlowExe, m_sshFlowDir);
    WaitForTcpPort(kSshFlowHost, kSshFlowPort, 10000);

    m_agentFlowMgr.Launch(m_agentFlowExe, m_agentFlowDir);
    WaitForTcpPort(kAgentFlowHost, kAgentFlowPort, 10000);
}

LRESULT CTrayWnd::OnTrayIcon(WPARAM /*wParam*/, LPARAM lParam)
{
    switch (lParam)
    {
    case WM_LBUTTONUP:
        OpenWebUI();
        break;
    case WM_RBUTTONUP:
        ShowContextMenu();
        break;
    default:
        break;
    }
    return 0;
}

// [업무지침_mcp_file_flow_배포제외_tray다이얼로그제거.md, 2026-09-20]
// 자체 다이얼로그를 완전히 제거하고, 기본 브라우저로 웹 UI를 여는 것으로
// 대체했다 - NotebookFlow.exe가 서빙하는 프론트엔드(포트 kNotebookFlowPort).
// ShellExecuteW(SW_SHOWNORMAL)는 기본 브라우저 연결 프로그램으로 URL을
// 넘기는 표준 방식이며, 실패해도(브라우저 미설치 등) 트레이 앱 자체의
// 동작에는 영향이 없다(반환값을 별도로 확인하지 않는 이유).
void CTrayWnd::OpenWebUI()
{
    wchar_t url[64];
    swprintf_s(url, L"http://%ls:%d/", kNotebookFlowHost, kNotebookFlowPort);
    ShellExecuteW(nullptr, L"open", url, nullptr, nullptr, SW_SHOWNORMAL);
}

void CTrayWnd::ShowContextMenu()
{
    CMenu menu;
    menu.CreatePopupMenu();
    menu.AppendMenuW(MF_STRING, ID_TRAY_OPEN, L"연동 관리 열기");
    menu.AppendMenuW(MF_STRING, ID_TRAY_EXIT, L"종료");

    CPoint pt;
    GetCursorPos(&pt);
    SetForegroundWindow();
    menu.TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
    PostMessage(WM_NULL);
}

void CTrayWnd::OnTrayOpen()
{
    OpenWebUI();
}

void CTrayWnd::OnTrayExit()
{
    DestroyWindow(); // → OnDestroy에서 정리 후 PostQuitMessage
}

void CTrayWnd::OnDestroy()
{
    KillTimer(kWatchdogTimerId);
    Shell_NotifyIconW(NIM_DELETE, &m_nid);
    // [업무지침_TrayFlow_프로세스관리.md] 정상 종료 경로에서 명시적으로
    // 한 번 정리 - Job Object 소멸자의 KILL_ON_JOB_CLOSE가 어차피 다시
    // 한번 보장해주는 이중 안전장치(NotebookRAG tray.exe와 동일 패턴).
    m_notebookFlowMgr.Terminate();
    m_fileRaMgr.Terminate();
    m_sshFlowMgr.Terminate();
    m_agentFlowMgr.Terminate();
    CWnd::OnDestroy();
    PostQuitMessage(0);
}

void CTrayWnd::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == kWatchdogTimerId)
    {
        // 이전 워치독 스레드가 아직 재기동/포트대기 중이면(WaitForTcpPort
        // 최대 10초) 겹쳐 돌리지 않는다 - 다음 틱에서 다시 시도.
        bool expected = false;
        if (m_watchdogBusy.compare_exchange_strong(expected, true))
        {
            HANDLE h = (HANDLE)_beginthreadex(nullptr, 0, &CTrayWnd::WatchdogThreadProc, this, 0, nullptr);
            if (h) CloseHandle(h);
            else m_watchdogBusy = false;
        }
    }
    CWnd::OnTimer(nIDEvent);
}

// [워치독, 2026-09-10] 외부 요인으로 NotebookFlow.exe/File RA가 예고 없이
// 죽어도 자동으로 다시 띄운다 - 근본 원인(현재 미해결, 결과보고 참고)을
// 코드로 못 고치는 대신 "잠깐 끊겼다 자동 복구"로 격하시키는 목적.
unsigned __stdcall CTrayWnd::WatchdogThreadProc(void* param)
{
    CTrayWnd* self = static_cast<CTrayWnd*>(param);

    if (!self->m_notebookFlowMgr.IsRunning())
    {
        self->m_notebookFlowMgr.Launch(self->m_notebookFlowExe, self->m_notebookFlowDir);
        WaitForTcpPort(kNotebookFlowHost, kNotebookFlowPort, 10000);
    }
    if (!self->m_fileRaMgr.IsRunning())
    {
        self->m_fileRaMgr.Launch(self->m_fileRaExe, self->m_fileRaDir);
        WaitForTcpPort(kFileRaHost, kFileRaPort, 10000);
    }
    if (!self->m_sshFlowMgr.IsRunning())
    {
        self->m_sshFlowMgr.Launch(self->m_sshFlowExe, self->m_sshFlowDir);
        WaitForTcpPort(kSshFlowHost, kSshFlowPort, 10000);
    }
    if (!self->m_agentFlowMgr.IsRunning())
    {
        self->m_agentFlowMgr.Launch(self->m_agentFlowExe, self->m_agentFlowDir);
        WaitForTcpPort(kAgentFlowHost, kAgentFlowPort, 10000);
    }

    self->m_watchdogBusy = false;
    return 0;
}

// ── CTrayApp ─────────────────────────────────────────────────────────────

BOOL CTrayApp::InitInstance()
{
    // [별도 앱으로 분리] NotebookRAG tray.exe와는 다른 뮤텍스 이름을 써야
    // 두 트레이 앱이 서로 간섭 없이 동시에 뜬다.
    m_hSingleInstanceMutex = CreateMutexW(nullptr, TRUE, L"Global\\NotebookFlowTray_SingleInstance");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        if (m_hSingleInstanceMutex)
        {
            CloseHandle(m_hSingleInstanceMutex);
            m_hSingleInstanceMutex = nullptr;
        }
        return FALSE;
    }

    CWinApp::InitInstance();

    if (!AfxOleInit())
    {
        return FALSE;
    }

    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_LISTVIEW_CLASSES | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icc);

    m_pTrayWnd = new CTrayWnd();
    if (!m_pTrayWnd->Init())
    {
        delete m_pTrayWnd;
        m_pTrayWnd = nullptr;
        return FALSE;
    }

    m_pMainWnd = m_pTrayWnd;
    return TRUE;
}

int CTrayApp::ExitInstance()
{
    if (m_hSingleInstanceMutex)
    {
        CloseHandle(m_hSingleInstanceMutex);
        m_hSingleInstanceMutex = nullptr;
    }
    return CWinApp::ExitInstance();
}

CTrayApp theApp;
