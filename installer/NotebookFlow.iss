; NotebookFlow.iss — 사용자 모드 설치 프로그램 (Inno Setup 6)
; (업무지침_InnoSetup_설계_1차릴리스.md — NotebookRAG.iss 패턴 재사용 +
;  NotebookFlow 고유 요구사항: Telegram 검증 마법사 페이지 / AGENT_CWD 등록)
;
; 이 파일은 UTF-8(BOM 포함)로 저장돼 있어야 한글이 깨지지 않는다.
; build_installer.ps1이 컴파일 직전에 BOM을 강제로 붙여준다 — 직접
; 텍스트 에디터로 저장할 때도 "UTF-8 with BOM"으로 저장할 것.
;
; 전제: 컴파일 전에 반드시 build_installer.ps1을 실행해
;   1) frontend(Vite) 빌드
;   2) agent-flow/ssh-flow PyInstaller 산출물 최신화
;   3) tray-flow.exe를 "설치 레이아웃 지원" 버전으로 재빌드
;      (tray-src\build-installer\tray-flow.exe — 라이브 운영 중인
;       tray-src\tray-flow.exe와는 별개의 산출물)
;   4) ..\bin\{notebookflow,agent-flow,ssh-flow,tray,config,assets,onboarding}\
;      배포 디렉토리 구조를 조립
; 해 둬야 한다. 이 .iss를 build_installer.ps1을 거치지 않고 바로 ISCC로
; 컴파일하면 ..\bin\ 아래 최신 산출물이 없을 수 있다.

#define MyAppName "NotebookFlow"
#define MyAppVersion "1.2.1"
#define MyAppPublisher "dorafather"
#define MyTrayExeName "bin\tray\tray-flow.exe"
#define MyNotebookFlowExeName "bin\notebookflow\NotebookFlow.exe"

[Setup]
AppId={{9B3E7C1A-4F2D-4E6B-8C1A-2D6F9B7E5A31}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={localappdata}\Programs\NotebookFlow
DefaultGroupName=NotebookFlow
DisableProgramGroupPage=yes
; [1차 확정 설계] 사용자 모드, 관리자 권한 불필요 — 회사 노트북(관리자
; 권한 없음)에서도 설치돼야 한다는 목표와 직결(NotebookRAG와 동일 원칙).
PrivilegesRequired=lowest
OutputDir=output
OutputBaseFilename=NotebookFlow_Setup
Compression=lzma2
SolidCompression=yes
SetupIconFile=..\assets\notebookflow.ico
UninstallDisplayIcon={app}\{#MyTrayExeName}
; [범위 밖] 코드 서명 인증서 없음 — 서명 없이 배포. SmartScreen 경고가
; 뜰 수 있음(NotebookRAG와 동일).
WizardStyle=modern

[Languages]
Name: "korean"; MessagesFile: "compiler:Languages\Korean.isl"

[Tasks]
; [확정 설계] 기본 체크(ON) — checkedonce: 최초 설치 시엔 체크된 채로
; 시작하되, 이후 재설치/업그레이드 시엔 사용자가 이전에 끈 선택을 존중.
Name: "autostart"; Description: "Windows 시작 시 NotebookFlow 자동 실행"; Flags: checkedonce

[Files]
Source: "..\bin\notebookflow\*"; DestDir: "{app}\bin\notebookflow"; Flags: recursesubdirs ignoreversion
Source: "..\bin\agent-flow\*"; DestDir: "{app}\bin\agent-flow"; Flags: recursesubdirs ignoreversion
Source: "..\bin\ssh-flow\*"; DestDir: "{app}\bin\ssh-flow"; Flags: recursesubdirs ignoreversion
Source: "..\bin\tray\*"; DestDir: "{app}\bin\tray"; Flags: recursesubdirs ignoreversion
Source: "..\bin\config\*"; DestDir: "{app}\bin\config"; Flags: ignoreversion
Source: "..\bin\assets\*"; DestDir: "{app}\bin\assets"; Flags: ignoreversion
Source: "..\bin\onboarding\*"; DestDir: "{app}\bin\onboarding"; Flags: recursesubdirs ignoreversion
; ⚠️ addr.ini는 [Files]에 없다 — 배포판에는 항상 addr.ini.template(빈 값)만
; 담는다. 실제 addr.ini는 (a) 아래 [Code]의 Telegram 검증 마법사가 통과한
; 값으로 ssPostInstall에서 생성되거나, (b) 마법사에서 건너뛴 경우
; NotebookFlow.exe 최초 실행 시 스스로 addr.ini.template에서 생성한다
; (이미 구현된 동작 — Main.cpp의 s_fnEnsureAddrIniExists, 손댈 필요 없음).
;
; Telegram 검증 마법사 페이지(아래 [Code])는 마법사 진행 "중"(파일이 아직
; {app}에 복사되지 않은 시점)에 검증을 수행해야 하므로, NotebookFlow.exe를
; {tmp}에도 별도로 추출해 둔다 — 이 dontcopy 항목은 {app}에 설치되는
; 파일과는 무관한, 검증 전용 임시 사본이다. --verify-telegram 모드는
; addr.ini/rest.sce 등 어떤 배포 경로에도 의존하지 않으므로(Main.cpp에서
; 확인됨 — SetCurrentDirectoryW/addr.ini 로딩보다 앞선 완전히 독립된
; 분기) {tmp}에서 실행해도 안전하다.
; 2026-09-25: OutboundClient가 OS 인증서 저장소 대신 자체 번들 cacert.pem으로
; TLS 검증을 하도록 바뀌면서, NotebookFlow.exe는 실행 파일 옆에서 이 파일을
; 찾는다(exe 디렉터리 기준 절대경로 — CWD와 무관) — {tmp} 검증용 사본도
; 반드시 같이 추출해야 한다.
Source: "..\bin\notebookflow\NotebookFlow.exe"; DestDir: "{tmp}"; Flags: dontcopy
Source: "..\bin\notebookflow\cacert.pem"; DestDir: "{tmp}"; Flags: dontcopy

[Registry]
; [확정 설계] 자동시작 방식 1 — HKCU Run 키.
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; \
  ValueType: string; ValueName: "NotebookFlow"; \
  ValueData: """{app}\{#MyTrayExeName}"""; \
  Tasks: autostart; Flags: uninsdeletevalue

; [4. AGENT_CWD 환경변수, 신규] agent-flow가 claude -p 실행 시 CLAUDE.md를
; 자동으로 읽으려면 AGENT_CWD가 정확히 "{app}\bin\notebookflow"(NotebookFlow.exe
; 와 CLAUDE.md가 있는 폴더)를 가리켜야 한다 — 설치 경로가 사용자마다
; 다르므로 설치 시점에 실제 경로로 채워 등록한다. HKCU\Environment에
; 값을 쓴 것만으로는 이미 떠 있는 프로세스에 즉시 반영되지 않지만,
; agent-flow 자신도 이 설치 직후에 tray-flow.exe가 새로 띄우는 새
; 프로세스이므로 문제 없다(안내 텍스트에도 명시).
Root: HKCU; Subkey: "Environment"; ValueType: string; \
  ValueName: "AGENT_CWD"; ValueData: "{app}\bin\notebookflow"; \
  Flags: preservestringtype uninsdeletevalue

[Icons]
; [확정 설계] 자동시작 방식 2 — 시작프로그램 폴더 바로가기. NotebookRAG가
; 실사용 중 겪은 "Run 키만으론 보안 에이전트에 필터링될 수 있음" 교훈을
; 그대로 적용(이중 등록).
Name: "{userstartup}\NotebookFlow"; Filename: "{app}\{#MyTrayExeName}"; \
  Tasks: autostart
Name: "{group}\NotebookFlow"; Filename: "{app}\{#MyTrayExeName}"
Name: "{group}\NotebookFlow 제거"; Filename: "{uninstallexe}"

[Run]
; 설치 완료 직후 안내 텍스트를 메모장으로 자동으로 띄운다 — 무인 설치
; 시에만 건너뜀. CurStepChanged(ssPostInstall)에서 이 파일을 먼저
; 생성하므로 이 [Run] 항목이 그 뒤에 실행되는 순서가 보장된다(Inno는
; [Run]을 설치 완료 후에만 실행).
Filename: "{win}\notepad.exe"; Parameters: """{app}\설치후_안내.txt"""; \
  Flags: nowait skipifsilent

; 설치 완료 후 즉시 실행 옵션(트레이 앱 = 진입점).
Filename: "{app}\{#MyTrayExeName}"; Description: "NotebookFlow 지금 실행"; \
  Flags: postinstall nowait skipifsilent

[Code]
var
  TelegramPage: TInputQueryWizardPage;
  TelegramSkipCheckBox: TNewCheckBox;
  TelegramVerified: Boolean;
  TelegramBotToken, TelegramChatId: String;

procedure TelegramSkipCheckBoxClick(Sender: TObject);
begin
  // 건너뛰기를 체크하면 입력 필드를 비활성화해 혼동을 줄인다(선택 사항성
  // 강조) — 값이 남아있어도 NextButtonClick에서 체크박스를 최종 판단
  // 기준으로 쓰므로 동작에는 영향 없지만, UX상 명확히 하기 위함.
  TelegramPage.Edits[0].Enabled := not TelegramSkipCheckBox.Checked;
  TelegramPage.Edits[1].Enabled := not TelegramSkipCheckBox.Checked;
end;

procedure InitializeWizard;
begin
  TelegramVerified := False;

  // [3. Telegram 검증 마법사 페이지, 신규] 봇 토큰/chat_id 두 필드 +
  // 건너뛰기 체크박스. wpSelectTasks(자동시작 체크) 다음, wpReady 이전에
  // 배치 — 사용자가 "지금 당장 텔레그램 토큰이 없는" 경우를 위해
  // 건너뛰기를 항상 열어 둔다.
  TelegramPage := CreateInputQueryPage(wpSelectTasks,
    'Telegram 봇 연동 설정',
    'NotebookFlow가 알림을 보내고 명령을 받을 Telegram 봇을 연결합니다',
    'BotFather(@BotFather)에게 "/newbot" 명령으로 새 봇을 만들고 발급받은 토큰을 ' +
    '아래에 입력하세요. chat_id는 그 봇에게 아무 메시지나 한 번 보낸 뒤, ' +
    '브라우저로 https://api.telegram.org/bot<토큰>/getUpdates 에 접속하면 ' +
    '"chat":{"id":...} 형태로 확인할 수 있습니다.' + #13#10#13#10 +
    '지금 준비되지 않았다면 아래 체크박스를 선택해 건너뛸 수 있습니다 — ' +
    '나중에 NotebookFlow가 자동 생성한 addr.ini 파일을 직접 편집하거나, ' +
    'Telegram 대화를 통해 값을 채워 넣을 수 있습니다.');
  TelegramPage.Add('Telegram 봇 토큰 (Bot Token):', False);
  TelegramPage.Add('chat_id:', False);

  TelegramSkipCheckBox := TNewCheckBox.Create(TelegramPage);
  TelegramSkipCheckBox.Parent := TelegramPage.Surface;
  TelegramSkipCheckBox.Caption := '지금 설정하지 않고 건너뛰기';
  TelegramSkipCheckBox.Top := TelegramPage.Edits[1].Top + TelegramPage.Edits[1].Height + 24;
  TelegramSkipCheckBox.Width := TelegramPage.SurfaceWidth;
  TelegramSkipCheckBox.Checked := False;
  TelegramSkipCheckBox.OnClick := @TelegramSkipCheckBoxClick;
end;

function NextButtonClick(CurPageID: Integer): Boolean;
var
  ResultCode: Integer;
  ExePath, Params, Token, ChatId: String;
begin
  Result := True;
  if CurPageID <> TelegramPage.ID then
    Exit;

  if TelegramSkipCheckBox.Checked then
  begin
    // [요구사항 5] 건너뛰면 addr.ini를 만들지 않는다 — NotebookFlow.exe가
    // 최초 실행 시 빈 템플릿에서 자동 생성하도록 그대로 둔다(기존 구현,
    // 손댈 필요 없음).
    TelegramVerified := False;
    Exit;
  end;

  Token := Trim(TelegramPage.Values[0]);
  ChatId := Trim(TelegramPage.Values[1]);
  if (Token = '') or (ChatId = '') then
  begin
    MsgBox('봇 토큰과 chat_id를 모두 입력하거나, "지금 설정하지 않고 건너뛰기"를 체크하세요.',
      mbError, MB_OK);
    Result := False;
    Exit;
  end;

  // [2. Exec()로 --verify-telegram 동기 실행] 이 시점엔 아직 {app}에 파일이
  // 복사되지 않았으므로(설치는 마법사가 끝난 뒤에 진행됨), {tmp}에 미리
  // 추출해 둔 검증 전용 사본을 사용한다([Files]의 dontcopy 항목 참고) —
  // --verify-telegram 모드는 배포 경로에 의존하지 않으므로 {tmp}에서
  // 실행해도 동작이 동일하다.
  // 2026-09-25: cacert.pem도 dontcopy로 등록해뒀는데 여기서 실제로
  // ExtractTemporaryFile()을 안 불러서 {tmp}에 안 풀리는 버그가 있었다 -
  // OutboundClient가 exe와 같은 폴더의 cacert.pem을 찾으므로, 이걸 빠뜨리면
  // "SSL certificate loading failed"로 검증이 항상 실패한다(실사용자
  // 테스트로 재현·확인).
  ExtractTemporaryFile('NotebookFlow.exe');
  ExtractTemporaryFile('cacert.pem');
  ExePath := ExpandConstant('{tmp}\NotebookFlow.exe');
  Params := '--verify-telegram "' + Token + '" "' + ChatId + '"';

  WizardForm.Cursor := crHourglass;
  try
    if not Exec(ExePath, Params, '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then
    begin
      MsgBox('검증 프로그램을 실행할 수 없습니다 (오류 코드: ' + IntToStr(ResultCode) + ').',
        mbError, MB_OK);
      Result := False;
      Exit;
    end;
  finally
    WizardForm.Cursor := crDefault;
  end;

  // [3. exit code 확인] 0이면 통과, 아니면 에러 메시지를 띄우고 같은
  // 페이지에 머무른다(재입력 유도) — Result := False로 페이지 이동을 막음.
  if ResultCode <> 0 then
  begin
    MsgBox('Telegram 봇 토큰 또는 chat_id가 올바르지 않습니다.' + #13#10 +
      '토큰/chat_id를 다시 확인한 뒤 재입력하거나, "건너뛰기"를 체크하세요.' + #13#10#13#10 +
      '(검증 종료 코드: ' + IntToStr(ResultCode) + ')',
      mbError, MB_OK);
    Result := False;
    Exit;
  end;

  TelegramVerified := True;
  TelegramBotToken := Token;
  TelegramChatId := ChatId;
end;

// [인코딩 안전장치, 2026-09-19 실측 후 재작성] addr.ini.template은 UTF-8로
// 저장돼 있고 자리표시자 문구("<여기에 ... 입력하세요>")에 한글이 들어있다.
// ⚠️ 처음에 AnsiString(원시 바이트) + Pos/Copy/Delete/Insert로 '<'/'>' 사이만
// 바이트 단위로 잘라내는 방식을 시도했으나, 실제 설치 테스트에서 [TELEGRAM]
// 섹션 헤더까지 뭉개지는 심각한 손상이 실측으로 확인됐다 — Inno Pascal
// Script의 AnsiString 관련 문자열 함수가 기대와 달리 순수 바이트 배열로
// 동작하지 않는 것으로 보인다(원인을 정확히 특정하지 못했지만 재현은 확실함).
// 대신 Inno가 UTF-8 BOM을 인식해 올바르게 디코딩해주는 "줄 단위" 함수
// (LoadStringsFromFile/SaveStringsToFile, TArrayOfString = 온전한 Unicode
// String 배열)를 쓴다 — 한글이 포함된 줄 전체를 승인된 새 줄로 통째로
// 교체할 뿐, '<'/'>' 사이의 바이트를 직접 잘라내지 않으므로 인코딩 손상
// 여지가 전혀 없다. ASCII 접두어("bot_token=<")로 줄을 식별하기만 하면
// 되고, 그 줄의 나머지(자리표시자 전체)는 그냥 버리고 새 줄로 교체한다.
procedure ReplaceIniPlaceholderLine(var Lines: TArrayOfString; const KeyAscii: String;
  const NewValueAscii: String; var Found: Boolean);
var
  I: Integer;
begin
  Found := False;
  for I := 0 to GetArrayLength(Lines) - 1 do
  begin
    if Pos(KeyAscii + '=<', Lines[I]) = 1 then
    begin
      Lines[I] := KeyAscii + '=' + NewValueAscii;
      Found := True;
    end;
  end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
var
  Guide: TStringList;
  GuidePath: String;
  TemplatePath, AddrIniPath: String;
  Lines: TArrayOfString;
  FoundToken, FoundChatId: Boolean;
begin
  if CurStep = ssPostInstall then
  begin
    // [4. 검증 통과한 값을 addr.ini에 반영] config/addr.ini.template의
    // [TELEGRAM] 섹션 bot_token/my_chat_id 자리를 채워 넣은 뒤
    // {app}\bin\notebookflow\addr.ini로 저장. 건너뛴 경우(TelegramVerified
    // = False)에는 아무것도 하지 않는다 — NotebookFlow.exe가 최초 실행
    // 시 스스로 템플릿에서 addr.ini를 생성한다(이미 구현됨).
    if TelegramVerified then
    begin
      TemplatePath := ExpandConstant('{app}\bin\config\addr.ini.template');
      AddrIniPath := ExpandConstant('{app}\bin\notebookflow\addr.ini');
      if LoadStringsFromFile(TemplatePath, Lines) then
      begin
        ReplaceIniPlaceholderLine(Lines, 'bot_token', TelegramBotToken, FoundToken);
        ReplaceIniPlaceholderLine(Lines, 'my_chat_id', TelegramChatId, FoundChatId);
        if FoundToken and FoundChatId then
          SaveStringsToFile(AddrIniPath, Lines, False)
        else
          MsgBox('addr.ini.template에서 bot_token/my_chat_id 자리표시자를 찾지 못해 ' +
            'Telegram 값을 자동으로 채우지 못했습니다(템플릿 형식이 바뀌었을 수 있습니다).' + #13#10 +
            'NotebookFlow 최초 실행 시 빈 addr.ini가 생성되며, 직접 채워 넣을 수 있습니다.',
            mbError, MB_OK);
      end
      else
      begin
        MsgBox('addr.ini.template을 읽지 못해 Telegram 값을 자동으로 채우지 못했습니다.' + #13#10 +
          'NotebookFlow 최초 실행 시 빈 addr.ini가 생성되며, 직접 채워 넣을 수 있습니다.',
          mbError, MB_OK);
      end;
    end;

    // [5. 설치 후 안내 텍스트] Claude Code MCP 등록과 마찬가지로, 자동화가
    // 아니라 실제 설치 경로가 반영된 정확한 안내를 텍스트 파일로 남긴다.
    Guide := TStringList.Create;
    try
      Guide.Add('NotebookFlow 설치가 완료되었습니다.');
      Guide.Add('');
      Guide.Add('■ 트레이 아이콘 사용법');
      Guide.Add('  화면 우측 하단 시스템 트레이의 NotebookFlow 아이콘을 좌클릭하면');
      Guide.Add('  연동 관리 웹 화면이 열립니다. 우클릭하면 종료 메뉴가 나타납니다.');
      Guide.Add('');
      Guide.Add('■ Telegram으로 바로 확인해보기');
      Guide.Add('  등록해 둔 Telegram 봇에게 "help"라고 보내면 사용 가능한 명령어');
      Guide.Add('  목록을 이미지로 받아볼 수 있습니다.');
      Guide.Add('');
      if TelegramVerified then
      begin
        Guide.Add('  Telegram 봇 연동이 설치 중 검증을 완료했습니다 - 바로 사용 가능합니다.');
      end
      else
      begin
        Guide.Add('■ Telegram 봇을 나중에 연동하려면');
        Guide.Add('  NotebookFlow를 한 번 실행하면 아래 위치에 addr.ini가 자동으로');
        Guide.Add('  생성됩니다. 메모장으로 열어 [TELEGRAM] 섹션의 bot_token/my_chat_id');
        Guide.Add('  값을 채워 넣고 NotebookFlow를 재시작하세요.');
        Guide.Add('    ' + ExpandConstant('{app}\bin\notebookflow\addr.ini'));
      end;
      Guide.Add('');
      Guide.Add('■ 더 자세한 안내');
      Guide.Add('  ' + ExpandConstant('{app}\bin\onboarding\welcome.md') + ' 파일을 열어보세요.');
      GuidePath := ExpandConstant('{app}\설치후_안내.txt');
      Guide.SaveToFile(GuidePath);
    finally
      Guide.Free;
    end;
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usUninstall then
  begin
    // [Files]로 추적되지 않는(설치 후 런타임/마법사가 생성한) 파일들 —
    // 항상 정리한다(이걸 안 하면 {app} 폴더가 안 비어서 Inno가 폴더
    // 자체를 못 지우고 남긴다).
    DeleteFile(ExpandConstant('{app}\설치후_안내.txt'));

    // [확정 설계] addr.ini/rest.sce 등 사용자가 설정한 값이 담긴 파일은
    // 되돌릴 수 없는 삭제이므로 사용자 확인이 필요하다. 무인(silent)
    // 제거 시엔 물어볼 사용자가 없으므로 묻지 않고 보존한다(NotebookRAG와
    // 동일 정책 — 확인 없이 삭제하는 것보다 보존이 안전한 기본값).
    if UninstallSilent then
      Exit;
    if MsgBox('설정 파일(addr.ini 등 Telegram/GitHub/Slack 등 자격증명 포함)을' + #13#10 +
             '삭제하시겠습니까?' + #13#10 +
             '삭제하지 않으면 나중에 재설치 시 다시 사용할 수 있습니다.',
             mbConfirmation, MB_YESNO) = IDYES then
    begin
      DeleteFile(ExpandConstant('{app}\bin\notebookflow\addr.ini'));
    end;
  end;
end;
