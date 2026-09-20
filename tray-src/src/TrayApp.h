#pragma once
#include <afxwin.h>
#include <atomic>
#include <string>
#include "ProcessManager.h"

// Shell_NotifyIcon 콜백 메시지 - NotebookRAG tray.exe와는 별개 프로세스이므로
// 값이 겹쳐도 무방하지만 관례상 WM_APP대(+100)를 그대로 씀.
#define WM_TRAYICON (WM_APP + 100)

// tray-flow.exe 하나만 실행하면 NotebookFlow.exe/File RA(및 ssh-flow/
// agent-flow)가 자동으로 같이 뜨고 같이 정리된다 - NotebookRAG의 tray.exe가
// notebookrag.exe를 Job Object로 관리하는 것과 동일한 패턴(ProcessManager.h/
// .cpp 재사용).
//
// tray-flow.exe의 역할은 ①트레이 아이콘 표시 ②우클릭 메뉴로 웹 UI
// (NotebookFlow.exe가 서빙하는 프론트엔드)를 기본 브라우저로 열기 ③자식
// 프로세스 기동/워치독 관리, 이 세 가지다.
class CTrayWnd : public CWnd
{
public:
    CTrayWnd();
    virtual ~CTrayWnd();

    BOOL Init();

protected:
    afx_msg LRESULT OnTrayIcon(WPARAM wParam, LPARAM lParam);
    afx_msg void OnTrayOpen();
    afx_msg void OnTrayExit();
    afx_msg void OnDestroy();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    DECLARE_MESSAGE_MAP()

private:
    void ShowContextMenu();
    // 트레이 아이콘 좌클릭/우클릭 메뉴 "연동 관리 열기" 공용 핸들러 -
    // 기본 브라우저로 웹 UI(NotebookFlow.exe, 포트 kNotebookFlowPort)를 연다.
    void OpenWebUI();
    void LaunchChildProcesses();
    // 외부 요인으로 NotebookFlow.exe/File RA가 예고 없이 죽어도, 근본 원인을
    // 코드로 못 고치는 대신 "몇 초 내 자동 재기동"으로 격하시킨다.
    // UI 스레드(OnTimer)를 블로킹하면 안 되므로(WaitForTcpPort가 최대 10초
    // 걸림) 별도 스레드에서 수행한다.
    static unsigned __stdcall WatchdogThreadProc(void* param);

    NOTIFYICONDATAW m_nid;
    CProcessManager m_notebookFlowMgr;
    CProcessManager m_fileRaMgr;
    // NotebookFlow/File RA와 동일한 패턴으로 재사용 - CProcessManager
    // 인스턴스만 추가, 클래스 자체는 수정 없음.
    CProcessManager m_sshFlowMgr;
    CProcessManager m_agentFlowMgr;
    std::wstring m_notebookFlowExe, m_notebookFlowDir;
    std::wstring m_fileRaExe, m_fileRaDir;
    std::wstring m_sshFlowExe, m_sshFlowDir;
    std::wstring m_agentFlowExe, m_agentFlowDir;
    std::atomic<bool> m_watchdogBusy{false};
};

class CTrayApp : public CWinApp
{
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();

private:
    CTrayWnd* m_pTrayWnd = nullptr;

    // NotebookRAG tray.exe의 뮤텍스("Global\\NotebookRAG_TrayApp_SingleInstance")와
    // 이름이 겹치면 서로 다른 앱인데 한쪽만 뜨는 사고가 나므로 반드시 다른
    // 이름을 쓴다.
    HANDLE m_hSingleInstanceMutex = nullptr;
};
