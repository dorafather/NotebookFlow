/************************** Process Include ******************/
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include "Main.h"
#include "ConnRegistry.h"
#include "OutboundClient.h"
#include "TraceHub.h"
#include "httplib.h"
#include "BASICPARSER.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <chrono>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <algorithm>
using std::min;
using std::max;
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace nsUtil;
using namespace NotebookFlowHttp;
HelpFileReader * HelpFileReader::m_pInst = NULL;
HelpFileReader::HelpFileReader()
{
}
HelpFileReader::~HelpFileReader()
{
}
HelpFileReader & HelpFileReader::OBJ()
{
	if(m_pInst==NULL)
	{
		m_pInst = new HelpFileReader;
		m_pInst->Read("./help.json");
	}
	return *m_pInst;
}
void HelpFileReader::Read(KCSTR  _path)
{
	m_lock.WLOCK();
	m_file.init(_path);
	m_msg.PARSE(m_file.m_pszRawData);
	m_file.enablecheckchanged();
	m_lock.UNLOCK();
	Gpolling::setTimer(this, "json", 1000, jsonfiletimeout);
}
void HelpFileReader::jsonfiletimeout(Gpolling::info * _info)
{
	HelpFileReader * pFile = (HelpFileReader*)(_info->m_pOwner);
	pFile->m_lock.WLOCK();
	if(pFile->m_file.checkchanged())
	{
		pFile->m_msg.PARSE(pFile->m_file.m_pszRawData);
	}
	pFile->m_lock.UNLOCK();
}
static const std::string & s_fnGetExeDirUtf8()
{
	static std::string s_dir;
	static std::once_flag s_once;
	std::call_once(s_once, []()
	{
		wchar_t wpath[MAX_PATH] = { 0 };
		DWORD len = GetModuleFileNameW(NULL, wpath, MAX_PATH);
		std::wstring wdir(L".");
		if (len > 0 && len < MAX_PATH)
		{
			std::wstring wfull(wpath, len);
			size_t pos = wfull.find_last_of(L"\\/");
			if (pos != std::wstring::npos) wdir = wfull.substr(0, pos);
		}
		int u8len = WideCharToMultiByte(CP_UTF8, 0, wdir.c_str(), (int)wdir.size(), NULL, 0, NULL, NULL);
		std::string dir;
		if (u8len > 0)
		{
			dir.resize((size_t)u8len);
			WideCharToMultiByte(CP_UTF8, 0, wdir.c_str(), (int)wdir.size(), &dir[0], u8len, NULL, NULL);
		}
		else
		{
			dir = ".";
		}
		dir += "\\";
		s_dir = dir;
	});
	return s_dir;
}
static std::wstring s_fnGetExeDirWide()
{
	wchar_t wpath[MAX_PATH] = { 0 };
	DWORD len = GetModuleFileNameW(NULL, wpath, MAX_PATH);
	if (len == 0 || len >= MAX_PATH) return L".";
	std::wstring wfull(wpath, len);
	size_t pos = wfull.find_last_of(L"\\/");
	return (pos == std::wstring::npos) ? L"." : wfull.substr(0, pos);
}
static void s_fnEnsureAddrIniExists(const std::wstring & _exeDirWide)
{
	std::wstring addrIniPath = _exeDirWide + L"\\addr.ini";
	if (GetFileAttributesW(addrIniPath.c_str()) != INVALID_FILE_ATTRIBUTES)
	{
		return;
	}
	std::wstring candidates[] = {
		_exeDirWide + L"\\addr.ini.template",
		_exeDirWide + L"\\..\\config\\addr.ini.template",
		_exeDirWide + L"\\..\\..\\config\\addr.ini.template",
	};
	for (const std::wstring & tmpl : candidates)
	{
		if (GetFileAttributesW(tmpl.c_str()) == INVALID_FILE_ATTRIBUTES) continue;
		if (CopyFileW(tmpl.c_str(), addrIniPath.c_str(), 1))
		{
			printf("[NotebookFlow] addr.ini not found - created from template.\r\n");
		}
		else
		{
			printf("[NotebookFlow] addr.ini not found and template copy FAILED (err=%lu).\r\n", GetLastError());
		}
		return;
	}
	printf("[NotebookFlow] addr.ini not found and no addr.ini.template available - engine will boot with empty config.\r\n");
}
// claude-code 2026-09-26: "GitHub Pull 방식 업데이트 확인" 티켓 - 하루 1회 GitHub
// 조회를 "썬더링 허드" 없이 사용자마다 자연스럽게 분산시키기 위한 앵커값.
// 최초 부팅 시(딱 한 번) 그 순간의 로컬 "시(0~23)"를 addr.ini [INFO] 섹션에
// install_hour=NN(2자리, 함수.날짜(...,%%H)의 strftime 출력과 자릿수를 맞춤 -
// rest.sce가 매시 함수.날짜(x,%%H)로 뽑은 현재 시각과 문자열 그대로 비교하므로
// 자릿수가 다르면 절대 일치하지 않는다)로 기록한다. 이미 값이 있으면(재기동
// 포함) 절대 덮어쓰지 않는다 - "최초 1회만" 요구사항의 핵심.
static void s_fnEnsureInstallHourRecorded(const std::wstring & _exeDirWide)
{
	std::wstring addrIniPath = _exeDirWide + L"\\addr.ini";
	std::ifstream ifs(addrIniPath, std::ios::binary);
	if (!ifs)
	{
		printf("[NotebookFlow] install_hour: addr.ini를 아직 열 수 없어 건너뜁니다(다음 기동 시 재시도).\r\n");
		return;
	}
	std::ostringstream ss;
	ss << ifs.rdbuf();
	std::string content = ss.str();
	ifs.close();

	if (content.find("install_hour=") != std::string::npos)
	{
		// 이미 기록되어 있음 - 재기동이어도 절대 덮어쓰지 않는다.
		return;
	}

	SYSTEMTIME st;
	GetLocalTime(&st);
	char hourBuf[8];
	sprintf_s(hourBuf, "%02u", (unsigned)st.wHour);
	std::string newLine = std::string("install_hour=") + hourBuf + "\r\n";

	std::string newContent;
	size_t infoPos = content.find("[INFO]");
	if (infoPos != std::string::npos)
	{
		size_t lineEnd = content.find('\n', infoPos);
		if (lineEnd == std::string::npos) lineEnd = content.size() > 0 ? content.size() - 1 : 0;
		newContent = content.substr(0, lineEnd + 1) + newLine + content.substr(lineEnd + 1);
	}
	else
	{
		// [INFO] 섹션 자체가 없는 극단적으로 오래된 addr.ini 대비 방어적 fallback.
		newContent = std::string("[INFO]\r\n") + newLine + "\r\n" + content;
	}

	std::ofstream ofs(addrIniPath, std::ios::binary | std::ios::trunc);
	if (!ofs)
	{
		printf("[NotebookFlow] install_hour: addr.ini 쓰기 실패(err=%lu) - 다음 기동 시 재시도됩니다.\r\n", GetLastError());
		return;
	}
	ofs.write(newContent.data(), (std::streamsize)newContent.size());
	ofs.close();
	printf("[NotebookFlow] install_hour=%s 기록 완료(최초 1회, 이후 재기동에도 유지됩니다).\r\n", hourBuf);
}
static std::string s_fnFindFrontendDir()
{
	const std::string & exeDir = s_fnGetExeDirUtf8();
	std::string candidates[] = {
		exeDir + "frontend",   
		exeDir + "..\\frontend\\dist", 
	};
	for (const std::string & dir : candidates)
	{
		DWORD attr = GetFileAttributesA(dir.c_str());
		if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY))
		{
			return dir;
		}
	}
	return "";
}
static std::string s_fnUrlEncodeComponent(const std::string & _s)
{
	static const char hex[] = "0123456789ABCDEF";
	std::string out;
	out.reserve(_s.size());
	for (unsigned char c : _s)
	{
		bool unreserved = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
						|| (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~';
		if (unreserved) out += (char)c;
		else
		{
			out += '%';
			out += hex[(c >> 4) & 0xF];
			out += hex[c & 0xF];
		}
	}
	return out;
}
static std::string s_fnBuildAddrObjUrl(RestParam & _addrObj, std::string * _outDomain = nullptr)
{
	std::string url;
	std::string realDomain, realPath;
	KCSTR rawDomain = (KCSTR)_addrObj.GET("도메인").VAL();
	deserialPath(rawDomain, realDomain);
	KCSTR rawPath = (KCSTR)_addrObj.GET("경로").VAL();
	deserialPath(rawPath, realPath);
	if (_outDomain) *_outDomain = realDomain;
	url += realDomain;
	url += realPath;
	RestParam & params = _addrObj.GET("파라미터");
	unsigned int n = params.NUMS();
	for (unsigned int i = 0; i < n; i++)
	{
		RestParam & item = params[i];
		std::string realKey, realVal;
		deserialPath((KCSTR)item.GET("key").VAL(), realKey);
		deserialPath((KCSTR)item.GET("val").VAL(), realVal);
		url += (i == 0) ? "?" : "&";
		url += s_fnUrlEncodeComponent(realKey);
		url += "=";
		url += s_fnUrlEncodeComponent(realVal);
	}
	return url;
}
static void s_fnBuildAddrObjFromReq(const httplib::Request & _req, int _idx, RestParam & _addrObj)
{
	std::string rawDomain = "http://" + _req.get_header_value("Host");
	_addrObj.SET("도메인").VAL() = rawDomain.c_str();
	_addrObj.SET("경로").VAL() = _req.path.c_str();
	for (const auto & kv : _req.params)
	{
		RestParam & item = _addrObj.SET("파라미터").ARR();
		item.SET("key").VAL() = kv.first.c_str();
		item.SET("val").VAL() = kv.second.c_str();
	}
	_addrObj.SET("idx").VAL() = _idx;
}
static const std::string & s_fnDownloadBaseDir()
{
	static std::string s_path;
	static std::once_flag s_once;
	std::call_once(s_once, []() { s_path = s_fnGetExeDirUtf8() + "downloads\\"; });
	return s_path;
}
static bool s_fnBuildSafeDownloadPath(const std::string & _rawFileName, std::string & _outFullPath)
{
	std::string safe;
	for (char c : _rawFileName)
	{
		if (c == '/' || c == '\\' || c == ':') continue; 
		safe += c;
	}
	std::string cleaned;
	for (size_t i = 0; i < safe.size(); i++)
	{
		if (safe[i] == '.' && i + 1 < safe.size() && safe[i+1] == '.') { i++; continue; }
		cleaned += safe[i];
	}
	if (cleaned.empty()) return false;
	CreateDirectoryA(s_fnDownloadBaseDir().c_str(), NULL); 
	_outFullPath = s_fnDownloadBaseDir() + cleaned;
	return true;
}
static std::wstring s_fnUtf8ToWide(const std::string & _s)
{
	if (_s.empty()) return std::wstring();
	int len = MultiByteToWideChar(CP_UTF8, 0, _s.c_str(), (int)_s.size(), NULL, 0);
	if (len <= 0) return std::wstring();
	std::wstring w((size_t)len, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, _s.c_str(), (int)_s.size(), &w[0], len);
	return w;
}
static bool s_fnFindJpegEncoderClsid(CLSID & _outClsid)
{
	UINT num = 0, size = 0;
	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0) return false;
	std::vector<BYTE> buf(size);
	Gdiplus::ImageCodecInfo * pInfo = (Gdiplus::ImageCodecInfo *)buf.data();
	Gdiplus::GetImageEncoders(num, size, pInfo);
	for (UINT i = 0; i < num; i++)
	{
		if (wcscmp(pInfo[i].MimeType, L"image/jpeg") == 0)
		{
			_outClsid = pInfo[i].Clsid;
			return true;
		}
	}
	return false;
}
static void s_fnEnsureGdiplusStarted()
{
	static std::once_flag s_once;
	static ULONG_PTR s_token;
	std::call_once(s_once, []()
	{
		Gdiplus::GdiplusStartupInput input;
		Gdiplus::GdiplusStartup(&s_token, &input, NULL);
	});
}
static bool s_fnGenerateHelpImage(const std::string & _outPathUtf8)
{
	#if 1
	HelpFileReader & help = HelpFileReader::OBJ();
	help.m_lock.RLOCK();
	RestParam & rows = help.m_msg.GET("rows");
	const int nRows = rows.NUMS();
	const int headerH = 56;
	const int rowH = 42;
	const int padX = 20;
	const int col1W = 300;
	const int col2W = 460;
	const int width = padX * 3 + col1W + col2W;
	const int height = headerH + rowH * nRows + padX;
	s_fnEnsureGdiplusStarted();
	Gdiplus::Bitmap bmp(width, height, PixelFormat24bppRGB);
	Gdiplus::Graphics g(&bmp);
	g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	g.Clear(Gdiplus::Color(255, 255, 255, 255));
	Gdiplus::FontFamily fam(L"맑은 고딕");
	Gdiplus::Font fontTitle(&fam, 20, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::Font fontCmd(&fam, 15, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::Font fontDesc(&fam, 15, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::SolidBrush brushBody(Gdiplus::Color(255, 40, 42, 54));
	Gdiplus::SolidBrush brushHeaderBg(Gdiplus::Color(255, 47, 58, 82));
	Gdiplus::SolidBrush brushHeaderText(Gdiplus::Color(255, 255, 255, 255));
	Gdiplus::SolidBrush brushAltRow(Gdiplus::Color(255, 244, 246, 250));
	Gdiplus::Pen pen(Gdiplus::Color(255, 224, 226, 232), 1.0f);
	g.FillRectangle(&brushHeaderBg, 0, 0, width, headerH);
	std::wstring title = s_fnUtf8ToWide("NotebookFlow 텔레그램 명령어");
	g.DrawString(title.c_str(), -1, &fontTitle,
		Gdiplus::PointF((float)padX, (float)(headerH / 2 - 13)), &brushHeaderText);
	int y = headerH;
	for (int i = 0; i < nRows; i++)
	{
		if (i % 2 == 1) g.FillRectangle(&brushAltRow, 0, y, width, rowH);
		RestParam & item = rows[i];
		std::string szCmd = (KCSTR)item.GET("cmd").VAL();
		std::string szDesc = (KCSTR)item.GET("desc").VAL();;
		std::wstring cmd = s_fnUtf8ToWide(szCmd);
		std::wstring desc = s_fnUtf8ToWide(szDesc);
		g.DrawString(cmd.c_str(), -1, &fontCmd,
			Gdiplus::PointF((float)padX, (float)(y + rowH / 2 - 11)), &brushBody);
		g.DrawString(desc.c_str(), -1, &fontDesc,
			Gdiplus::PointF((float)(padX * 2 + col1W), (float)(y + rowH / 2 - 11)), &brushBody);
		g.DrawLine(&pen, 0.0f, (float)(y + rowH), (float)width, (float)(y + rowH));
		y += rowH;
	}
	help.m_lock.UNLOCK();
	g.DrawLine(&pen, (float)(padX * 2 + col1W - 10), (float)headerH, (float)(padX * 2 + col1W - 10), (float)height);
	CLSID jpegClsid;
	if (!s_fnFindJpegEncoderClsid(jpegClsid)) return false;
	std::wstring wpath = s_fnUtf8ToWide(_outPathUtf8);
	return bmp.Save(wpath.c_str(), &jpegClsid, NULL) == Gdiplus::Ok;
	#else
	struct Row { const char * cmd; const char * desc; };
	static const Row s_rows[] = {
		{"help", "도움말 보기"},
		{"깃허브 이슈", "NotebookFlow 저장소 이슈 목록 조회"},
		{"깃허브 이슈등록 [제목]", "NotebookFlow 저장소에 이슈 등록"},
		{"슬랙 알람 [문구]", "Slack 알림 전송"},
		{"디스코드 알람 [문구]", "Discord 알림 전송"},
		{"노티파이 알람 [문구]", "ntfy 알림 전송"},
		{"블루스카이 알람 [문구]", "Bluesky 포스팅"},
		{"클루드코드 [지시]", "Claude Code에게 작업 지시"},
		{"텔넷 [전송객체명] [명령어]", "원격 서버 셸 명령 실행 (대상 생략 시 기본 서버)"},
		{"파일 얻기 [경로]", "로컬 파일 전송받기"},
		{"파일 폴더 [경로]", "로컬 폴더 목록 조회 (최대 20개)"},
		{"사진 얻기 [경로]", "로컬 이미지 전송받기 (사진으로)"},
		{"문서/사진을 채팅창에 올리기", "downloads 폴더(exe와 같은 폴더 기준)에 자동 저장"},
	};
	const int nRows = sizeof(s_rows) / sizeof(s_rows[0]);
	const int headerH = 56;
	const int rowH = 42;
	const int padX = 20;
	const int col1W = 300;
	const int col2W = 460;
	const int width = padX * 3 + col1W + col2W;
	const int height = headerH + rowH * nRows + padX;
	s_fnEnsureGdiplusStarted();
	Gdiplus::Bitmap bmp(width, height, PixelFormat24bppRGB);
	Gdiplus::Graphics g(&bmp);
	g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
	g.Clear(Gdiplus::Color(255, 255, 255, 255));
	Gdiplus::FontFamily fam(L"맑은 고딕");
	Gdiplus::Font fontTitle(&fam, 20, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::Font fontCmd(&fam, 15, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::Font fontDesc(&fam, 15, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	Gdiplus::SolidBrush brushBody(Gdiplus::Color(255, 40, 42, 54));
	Gdiplus::SolidBrush brushHeaderBg(Gdiplus::Color(255, 47, 58, 82));
	Gdiplus::SolidBrush brushHeaderText(Gdiplus::Color(255, 255, 255, 255));
	Gdiplus::SolidBrush brushAltRow(Gdiplus::Color(255, 244, 246, 250));
	Gdiplus::Pen pen(Gdiplus::Color(255, 224, 226, 232), 1.0f);
	g.FillRectangle(&brushHeaderBg, 0, 0, width, headerH);
	std::wstring title = s_fnUtf8ToWide("NotebookFlow 텔레그램 명령어");
	g.DrawString(title.c_str(), -1, &fontTitle,
		Gdiplus::PointF((float)padX, (float)(headerH / 2 - 13)), &brushHeaderText);
	int y = headerH;
	for (int i = 0; i < nRows; i++)
	{
		if (i % 2 == 1) g.FillRectangle(&brushAltRow, 0, y, width, rowH);
		std::wstring cmd = s_fnUtf8ToWide(s_rows[i].cmd);
		std::wstring desc = s_fnUtf8ToWide(s_rows[i].desc);
		g.DrawString(cmd.c_str(), -1, &fontCmd,
			Gdiplus::PointF((float)padX, (float)(y + rowH / 2 - 11)), &brushBody);
		g.DrawString(desc.c_str(), -1, &fontDesc,
			Gdiplus::PointF((float)(padX * 2 + col1W), (float)(y + rowH / 2 - 11)), &brushBody);
		g.DrawLine(&pen, 0.0f, (float)(y + rowH), (float)width, (float)(y + rowH));
		y += rowH;
	}
	g.DrawLine(&pen, (float)(padX * 2 + col1W - 10), (float)headerH, (float)(padX * 2 + col1W - 10), (float)height);
	CLSID jpegClsid;
	if (!s_fnFindJpegEncoderClsid(jpegClsid)) return false;
	std::wstring wpath = s_fnUtf8ToWide(_outPathUtf8);
	return bmp.Save(wpath.c_str(), &jpegClsid, NULL) == Gdiplus::Ok;
	#endif
}
static void s_fnEmitTraceJson(const char * _dir, const char * _json)
{
	RestMsg env;
	env.SET("dir").VAL() = _dir;
	env.SET("payload").VAL() = _json;
	NotebookFlowHttp::GTraceHub().Emit(std::string(env.STR()));
}
static void s_fnEmitTraceWire(const char * _dir, const std::string & _method, const std::string & _url,
				int _status, const NotebookFlowHttp::OutboundClient::HeaderList & _headers,
				const std::string & _body)
{
	RestMsg env;
	env.SET("dir").VAL() = _dir;
	if (!_method.empty()) env.SET("method").VAL() = _method.c_str();
	if (!_url.empty()) env.SET("url").VAL() = _url.c_str();
	if (_status >= 0) env.SET("status").VAL() = _status;
	RestParam & hdrArr = env.SET("headers");
	for (const auto & kv : _headers)
	{
		RestParam & item = hdrArr.ARR();
		item.SET("key").VAL() = kv.first.c_str();
		item.SET("val").VAL() = kv.second.c_str();
	}
	env.SET("body").VAL() = _body.c_str();
	NotebookFlowHttp::GTraceHub().Emit(std::string(env.STR()));
}
/************************** App *****************************/
App::App() {}
App::~App() {}
void App::ACTION(nsUtil::QTHREAD & _wk,
					nsUtil::POOL::POOLDATA & _rPool,
					nsUtil::RestMsg & _msg)
{
	RestParam & folderObj = _msg.GET("폴더");
	if (!folderObj.EMPTY())
	{
		KCSTR rawPath = (KCSTR)folderObj.GET("경로").VAL();
		std::string realPath;
		deserialPath(rawPath, realPath);
		RestMsg rspMsg;
		RestParam & listArr = rspMsg.SET("목록");
		std::string summary;
		int shown = 0;
		int total = 0;
		bool bOk = false;
		if (!realPath.empty())
		{
			std::string pattern = realPath;
			char last = pattern[pattern.size() - 1];
			if (last != '\\' && last != '/') pattern += "\\";
			pattern += "*";
			WIN32_FIND_DATAA fd;
			HANDLE hFind = FindFirstFileA(pattern.c_str(), &fd);
			bOk = (hFind != INVALID_HANDLE_VALUE);
			if (bOk)
			{
				do
				{
					std::string name = fd.cFileName;
					if (name == "." || name == "..") continue;
					total++;
					bool bDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
					if (shown < 20)
					{
						RestParam & item = listArr.ARR();
						item.SET("이름").VAL() = name.c_str();
						item.SET("폴더여부").VAL() = bDir ? 1 : 0;
						summary += bDir ? "[폴더] " : "[파일] ";
						summary += name;
						if (!bDir)
						{
							ULARGE_INTEGER sz; sz.LowPart = fd.nFileSizeLow; sz.HighPart = fd.nFileSizeHigh;
							item.SET("크기").VAL() = (KUINT)sz.QuadPart;
							summary += " (" + std::to_string((unsigned long long)sz.QuadPart) + " bytes)";
						}
						summary += "\n";
						shown++;
					}
				} while (FindNextFileA(hFind, &fd));
				FindClose(hFind);
			}
		}
		if (bOk && total > shown)
		{
			summary += "... (전체 " + std::to_string(total) + "개 중 " + std::to_string(shown) + "개만 표시)";
		}
		rspMsg.SET("성공").VAL() = bOk ? 1 : 0;
		rspMsg.SET("전체개수").VAL() = total;
		rspMsg.SET("요약").VAL() = summary.c_str();
		rspMsg.SET("폴더모드").VAL() = 1;
		RestParam & addrEcho = _msg.GET(DEF_DSL_K_ADDR_eng);
		if (!addrEcho.EMPTY())
		{
			RestParam & dstAddr = rspMsg.SET(DEF_DSL_K_ADDR_kor);
			dstAddr = addrEcho;
			dstAddr.setkey(DEF_DSL_K_ADDR_kor);
			dstAddr.KEY() = DEF_DSL_K_ADDR_kor;
		}
		Flow::OBJ().PUT("", rspMsg.STR());
		return;
	}
	RestParam & helpImgObj = _msg.GET("헬프이미지");
	if (!helpImgObj.EMPTY())
	{
		const std::string kGeneratedDir = s_fnGetExeDirUtf8() + "generated\\";
		CreateDirectoryA(kGeneratedDir.c_str(), NULL);
		std::string outPath = kGeneratedDir + "help.jpg";
		bool bOk = s_fnGenerateHelpImage(outPath);

		RestMsg rspMsg;
		rspMsg.SET("성공").VAL() = bOk ? 1 : 0;
		if (bOk) rspMsg.SET("이미지경로").VAL() = outPath.c_str();
		rspMsg.SET("이미지모드").VAL() = 1;
		RestParam & addrEcho2 = _msg.GET(DEF_DSL_K_ADDR_eng);
		if (!addrEcho2.EMPTY())
		{
			RestParam & dstAddr2 = rspMsg.SET(DEF_DSL_K_ADDR_kor);
			dstAddr2 = addrEcho2;
			dstAddr2.setkey(DEF_DSL_K_ADDR_kor);
			dstAddr2.KEY() = DEF_DSL_K_ADDR_kor;
		}
		Flow::OBJ().PUT("", rspMsg.STR());
		return;
	}
	RestParam & addrParam = _msg.GET(DEF_DSL_K_ADDR_eng);
	if (addrParam.EMPTY())
	{
		return;
	}
	KSTRING & Method = _msg.GET(DEF_DSL_K_METHOD_eng).VAL();
	if (Method.LENGTH() > 0) 
	{
		std::string traceDomain;
		std::string url = s_fnBuildAddrObjUrl(addrParam, &traceDomain);
		std::string method = (KCSTR)Method;
		bool bTraceThis = NotebookFlowHttp::GTraceHub().IsActive()
						&& NotebookFlowHttp::GTraceHub().Matches(traceDomain);
		if (bTraceThis)
		{
			s_fnEmitTraceJson("1", _msg.STR());
		}
		std::shared_ptr<RestParam> addrCopy = std::make_shared<RestParam>();
		*addrCopy = addrParam;
		std::string saveToPath;
		RestParam & saveObj = _msg.GET("저장");
		if (!saveObj.EMPTY())
		{
			KCSTR rawFileName = (KCSTR)saveObj.GET("파일명").VAL();
			if (rawFileName && rawFileName[0])
			{
				s_fnBuildSafeDownloadPath(rawFileName, saveToPath);
			}
		}
		auto onComplete = [method,addrCopy,bTraceThis,saveToPath](bool _ok, int _status, const std::string & _resp)
		{
			RestMsg rspMsg;
			if (!saveToPath.empty())
			{
				rspMsg.SET("저장모드").VAL() = 1;
				rspMsg.SET("저장경로").VAL() = saveToPath.c_str();
				rspMsg.SET("저장성공").VAL() = _ok ? 1 : 0;
			}
			else
			{
				rspMsg.PARSE(_resp.c_str());
			}
			rspMsg.SET(DEF_DSL_K_METHOD_kor).VAL() = method.c_str();
			rspMsg.SET(DEF_DSL_K_RSP_CODE_kor).VAL() = _status;
			RestParam & dstAddr = rspMsg.SET(DEF_DSL_K_ADDR_kor);
			dstAddr = *addrCopy;
			dstAddr.setkey(DEF_DSL_K_ADDR_kor);
			dstAddr.KEY() = DEF_DSL_K_ADDR_kor;
			if (bTraceThis) s_fnEmitTraceJson("4", rspMsg.STR());
			Flow::OBJ().PUT("", rspMsg.STR());
		};
		NotebookFlowHttp::OutboundClient::HeaderList headers;
		RestParam & hdrObj = _msg.GET("헤더");
		unsigned int hdrN = hdrObj.NUMS();
		for (unsigned int hi = 0; hi < hdrN; hi++)
		{
			RestParam & hItem = hdrObj[hi];
			headers.emplace_back((KCSTR)hItem.GET("key").VAL(),
								(KCSTR)hItem.GET("val").VAL());
		}
		NotebookFlowHttp::OutboundClient::TraceRespHook traceHook = nullptr;
		if (bTraceThis)
		{
			traceHook = [](int _status, const NotebookFlowHttp::OutboundClient::HeaderList & _hdrs, const std::string & _body)
			{
				s_fnEmitTraceWire("3", "", "", _status, _hdrs, _body);
			};
		}
		if (method == "GET")
		{
			if (bTraceThis) s_fnEmitTraceWire("2", method, url, -1, headers, "");
			GOutboundClient().GetAsync(url, onComplete, headers, traceHook, saveToPath);
		}
		else
		{
			_msg.DEL(DEF_DSL_K_METHOD_eng);
			_msg.DEL(DEF_DSL_K_ADDR_eng);
			if (hdrN > 0) _msg.DEL("헤더");
			RestParam & docObj = _msg.GET("문서");
			std::string body;
			if (!docObj.EMPTY())
			{
				std::string fileBytes;
				bool bHaveContent = false;
				KCSTR rawContent = (KCSTR)docObj.GET("내용").VAL();
				if (rawContent && rawContent[0])
				{
					fileBytes = rawContent;
					bHaveContent = true;
				}
				std::string filePath;
				KCSTR rawPath = (KCSTR)docObj.GET("경로").VAL();
				if (!bHaveContent && rawPath && rawPath[0])
				{
					deserialPath(rawPath, filePath);
					std::ifstream ifs(filePath, std::ios::binary);
					if (ifs)
					{
						std::ostringstream ss;
						ss << ifs.rdbuf();
						fileBytes = ss.str();
						bHaveContent = true;
					}
					else
					{
						printf("[NotebookFlow] sendDocument: failed to open file: %s\r\n", filePath.c_str());
					}
				}
				if (!bHaveContent)
				{
					return;
				}
				KCSTR rawFieldName = (KCSTR)docObj.GET("필드명").VAL();
				std::string fieldName = "document";
				if (rawFieldName && rawFieldName[0]) deserialPath(rawFieldName, fieldName);
				KCSTR rawFileName = (KCSTR)docObj.GET("파일명").VAL();
				std::string fileName;
				if (rawFileName && rawFileName[0])
				{
					deserialPath(rawFileName, fileName);
				}
				else if (!filePath.empty())
				{
					size_t slashPos = filePath.find_last_of("/\\");
					fileName = (slashPos == std::string::npos) ? filePath : filePath.substr(slashPos + 1);
				}
				else
				{
					fileName = "document.txt";
				}
				KCSTR rawContentType = (KCSTR)docObj.GET("타입").VAL();
				std::string contentType = "text/plain";
				if (rawContentType && rawContentType[0]) deserialPath(rawContentType, contentType);
				httplib::UploadFormDataItems items;
				items.push_back({fieldName, fileBytes, fileName, contentType});
				KCSTR rawCaption = (KCSTR)docObj.GET("캡션").VAL();
				if (rawCaption && rawCaption[0])
				{
					std::string caption; deserialPath(rawCaption, caption);
					items.push_back({"caption", caption, "", ""});
				}
				_msg.DEL("문서");
				const std::string boundary = httplib::detail::make_multipart_data_boundary();
				body = httplib::detail::serialize_multipart_formdata(items, boundary);
				headers.emplace_back("Content-Type",
								httplib::detail::serialize_multipart_formdata_get_content_type(boundary));
			}
			else
			{
				body = _msg.STR();
			}

			if (bTraceThis) s_fnEmitTraceWire("2", method, url, -1, headers, body);
			GOutboundClient().PostAsync(url, body, onComplete, headers, traceHook);
		}
		return;
	}
	else    
	{
		int idx = (KINT)addrParam.GET("idx").VAL();
		std::shared_ptr<ConnHandle> conn = GConnRegistry().Find(idx);
		if (conn)
		{
			conn->Deliver((KCSTR)_msg.STR());
			if (conn->IsOneShot())
			{
				GConnRegistry().Remove(idx);
			}
		}
	}
}
struct SyncHttpCtx
{
	std::mutex m_mtx;
	std::condition_variable m_cv;
	bool m_bDone = false;
	bool m_bOk = false;
	int m_status = 0;
	std::string m_body;
};
static bool s_fnSyncHttpsGet(const std::string & _url, bool & _outOk, int & _outStatus,
				std::string & _outBody, int _timeoutSec = 15)
{
	auto ctx = std::make_shared<SyncHttpCtx>();
	GOutboundClient().GetAsync(_url, [ctx](bool _ok, int _status, const std::string & _body)
	{
		std::lock_guard<std::mutex> lk(ctx->m_mtx);
		ctx->m_bOk = _ok;
		ctx->m_status = _status;
		ctx->m_body = _body;
		ctx->m_bDone = true;
		ctx->m_cv.notify_one();
	});

	std::unique_lock<std::mutex> lk(ctx->m_mtx);
	bool bCompleted = ctx->m_cv.wait_for(lk, std::chrono::seconds(_timeoutSec),
					[&ctx] { return ctx->m_bDone; });
	_outOk = ctx->m_bOk;
	_outStatus = ctx->m_status;
	_outBody = ctx->m_body;
	return bCompleted;
}
static void s_fnFillChildHealthStatus(RestParam & _outObj, const std::string & _url,
				int _timeoutSec,
				std::function<void(RestParam &, RestMsg &)> _extraFieldsFromBody = nullptr)
{
	bool ok = false; int status = 0; std::string body;
	auto t0 = std::chrono::steady_clock::now();
	bool completed = s_fnSyncHttpsGet(_url, ok, status, body, _timeoutSec);
	auto t1 = std::chrono::steady_clock::now();
	long long latencyMs = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
	if (!completed)
	{
		_outObj.SET("status").VAL() = "down";
		_outObj.SET("error").VAL() = "timeout";
		return;
	}
	if (!ok || status < 200 || status >= 300)
	{
		_outObj.SET("status").VAL() = "down";
		std::string errMsg = (status > 0) ? ("http " + std::to_string(status))
							: (body.empty() ? "connection refused" : body);
		_outObj.SET("error").VAL() = errMsg.c_str();
		return;
	}
	_outObj.SET("status").VAL() = "up";
	_outObj.SET("latency_ms").VAL() = (int)latencyMs;
	if (_extraFieldsFromBody)
	{
		RestMsg rspBody;
		if (rspBody.PARSE(body.c_str()))
		{
			_extraFieldsFromBody(_outObj, rspBody);
		}
	}
}
static std::string s_fnSseFrame(const std::string & _chunk)
{
	std::string frame;
	size_t start = 0;
	while (true)
	{
		size_t nl = _chunk.find('\n', start);
		std::string line = (nl == std::string::npos) ? _chunk.substr(start) : _chunk.substr(start, nl - start);
		if (!line.empty() && line.back() == '\r') line.pop_back();  // CRLF 대응
		frame += "data: ";
		frame += line;
		frame += "\n";
		if (nl == std::string::npos) break;
		start = nl + 1;
	}
	frame += "\n";
	return frame;
}
static void s_fnRegisterConfigRoute(httplib::Server & _svr, const char * _path,
	void (nsUtil::Flow::*_pfnApply)(nsUtil::RestMsg &),
	void (nsUtil::Flow::*_pfnQuery)(nsUtil::RestMsg &))
{
	_svr.Put(_path, [_pfnApply](const httplib::Request & req, httplib::Response & res)
	{
		nsUtil::RestMsg reqMsg;
		if (!reqMsg.PARSE(req.body.c_str()))
		{
			res.status = 400;
			res.set_content("{\"RESULT\":\"1\",\"REASON\":\"invalid json\"}", "application/json");
			return;
		}
		(nsUtil::Flow::OBJ().*_pfnApply)(reqMsg);
		res.set_content("{\"RESULT\":\"0\"}", "application/json");
	});
	_svr.Get(_path, [_pfnQuery](const httplib::Request & /*req*/, httplib::Response & res)
	{
		nsUtil::RestMsg respMsg;
		(nsUtil::Flow::OBJ().*_pfnQuery)(respMsg);
		res.set_content(std::string(respMsg.STR()), "application/json");
	});
}
static void SetupHttpServer(App & _app, httplib::Server & _svr)
{
	std::string frontendDir = s_fnFindFrontendDir();
	if (!frontendDir.empty())
	{
		_svr.set_mount_point("/", frontendDir.c_str());
		printf("[NotebookFlow] serving static frontend from: %s\r\n", frontendDir.c_str());
	}
	else
	{
		printf("[NotebookFlow] WARNING: frontend build output not found - web UI will not be served (REST bridge still works).\r\n");
	}
	auto genericHandler = [&_app](const httplib::Request & req, httplib::Response & res)
	{
		auto conn = std::make_shared<PromiseConn>();
		std::future<std::string> fut = conn->m_promise.get_future();
		int idx = GConnRegistry().Register(conn);
		RestMsg msg;
		msg.PARSE(req.body.c_str());
		s_fnBuildAddrObjFromReq(req, idx, msg.SET(DEF_DSL_K_ADDR_kor));
		msg.SET(DEF_DSL_K_METHOD_kor).VAL() = req.method.c_str();
		char * bodyPtr = msg.STR();
		_app.PUT("", bodyPtr);
		auto status = fut.wait_for(std::chrono::seconds(15));
		if (status == std::future_status::ready)
		{
			std::string body = fut.get();
			RestMsg rspBody;
			rspBody.PARSE(body.c_str());
			RestParam & rspCode = rspBody.GET(DEF_DSL_K_RSP_CODE_kor);
			if (!rspCode.EMPTY())
			{
				res.status = (int)rspCode.VAL();
				rspBody.DEL(DEF_DSL_K_RSP_CODE_kor);
				body = std::string(rspBody.STR());
			}
			res.set_content(body, "application/json");
		}
		else
		{
			res.status = 504;
			res.set_content("{\"RESULT\":\"1\",\"REASON\":\"timeout\"}", "application/json");
		}
		GConnRegistry().Remove(idx);
	};
	_svr.Get(R"(/notebookflow/events/(.+))", [&_app](const httplib::Request & req, httplib::Response & res)
	{
		std::string asId = req.matches[1];
		auto conn = std::make_shared<SseConn>();
		int idx = GConnRegistry().Register(conn);
       	RestMsg subMsg;
		if (req.has_param("SCE_EVENT"))
			subMsg.SET(DEF_SCE_EVENT).VAL() = req.get_param_value("SCE_EVENT").c_str();
		if (req.has_param("ACTION_EVENT"))
			subMsg.SET(DEF_ACTION_EVENT).VAL() = req.get_param_value("ACTION_EVENT").c_str();
		KSTRING sceId; sceId.PRINT("IC-%s", asId.c_str());
		subMsg.SET(DEF_SCE_ID).VAL() = (KCSTR)sceId;
		subMsg.SET(DEF_AS_ID).VAL() = asId.c_str();
		s_fnBuildAddrObjFromReq(req, idx, subMsg.SET(DEF_DSL_K_ADDR_kor));
		subMsg.SET(DEF_DSL_K_METHOD_kor).VAL() = req.method.c_str();
		char * pkRaw = subMsg.STR();
		KCSTR pkPtr = pkRaw;
		_app.PUT("", pkPtr);

		res.set_chunked_content_provider("text/event-stream",
			[conn](size_t /*offset*/, httplib::DataSink & sink) -> bool
			{
				std::string chunk;
				if (conn->WaitAndPop(chunk, 15000))
				{
					std::string frame = s_fnSseFrame(chunk);
					sink.write(frame.data(), frame.size());
					return true;
				}
				if (conn->IsClosed())
				{
					sink.done();
					return false;
				}
				static const char s_ping[] = ": ping\n\n";
				sink.write(s_ping, sizeof(s_ping) - 1);
				return true;
			},
			[idx](bool /*success*/)
			{
				GConnRegistry().Remove(idx);
			});
	});
	_svr.Get(R"(/notebookflow/trace/(.+))", [](const httplib::Request & req, httplib::Response & res)
	{
		std::string name = req.matches[1];
		auto conn = std::make_shared<SseConn>();
		NotebookFlowHttp::GTraceHub().Attach(name, conn);

		res.set_chunked_content_provider("text/event-stream",
			[conn](size_t /*offset*/, httplib::DataSink & sink) -> bool
			{
				std::string chunk;
				if (conn->WaitAndPop(chunk, 15000))
				{
					std::string frame = s_fnSseFrame(chunk);
					sink.write(frame.data(), frame.size());
					return true;
				}
				if (conn->IsClosed())
				{
					sink.done();
					return false;
				}
				static const char s_ping[] = ": ping\n\n";
				sink.write(s_ping, sizeof(s_ping) - 1);
				return true;
			},
			[conn](bool /*success*/)
			{
				NotebookFlowHttp::GTraceHub().Detach(conn.get());
			});
	});
	s_fnRegisterConfigRoute(_svr, "/notebookflow/config/addr", &nsUtil::Flow::applyAddrConfig, &nsUtil::Flow::queryAddrConfig);
	s_fnRegisterConfigRoute(_svr, "/notebookflow/config/flow", &nsUtil::Flow::applyFlowConfig, &nsUtil::Flow::queryFlowConfig);
	s_fnRegisterConfigRoute(_svr, "/notebookflow/config/rest", &nsUtil::Flow::applyRestConfig, &nsUtil::Flow::queryRestConfig);
      _svr.Get("/notebookflow/status", [](const httplib::Request & /*req*/, httplib::Response & res)
	{
		RestMsg rspMsg;
		rspMsg.SET("notebookflow").SET("status").VAL() = "up";
		KSTRING agentDomain; ExeCore::OBJ().GETINI("AGENT", "domain", agentDomain);
		std::string agentUrl = std::string((KCSTR)agentDomain) + "/health";
		s_fnFillChildHealthStatus(rspMsg.SET("agent-flow"), agentUrl, 3,
			[](RestParam & _obj, RestMsg & _body)
			{
				RestParam & aj = _body.GET("active_jobs");
				if (!aj.EMPTY()) _obj.SET("active_jobs").VAL() = (KCSTR)aj.VAL();
			});
		KSTRING sshDomain; ExeCore::OBJ().GETINI("SSH", "domain", sshDomain);
		std::string sshUrl = std::string((KCSTR)sshDomain) + "/";
		s_fnFillChildHealthStatus(rspMsg.SET("ssh-flow"), sshUrl, 3);
		KSTRING fileDomain; ExeCore::OBJ().GETINI("FILE", "domain", fileDomain);
		std::string fileUrl = std::string((KCSTR)fileDomain) + "/health";
		s_fnFillChildHealthStatus(rspMsg.SET("file-ra"), fileUrl, 3,
			[](RestParam & _obj, RestMsg & _body)
			{
				RestParam & aj = _body.GET("active_jobs");
				if (!aj.EMPTY()) _obj.SET("active_jobs").VAL() = (KCSTR)aj.VAL();
			});

		res.set_content(std::string(rspMsg.STR()), "application/json");
	});
	_svr.Get(R"(/.*)", genericHandler);
	_svr.Put(R"(/.*)", genericHandler);
	_svr.Post(R"(/.*)", genericHandler);
}
static int s_fnVerifyTelegramMode(const std::string & _token, const std::string & _chatId)
{
	try
	{
		if (_token.empty())
		{
			fprintf(stderr, "[verify-telegram] FAIL: bot_token argument is empty\r\n");
			return 1;
		}
		std::string getMeUrl = "https://api.telegram.org/bot" + _token + "/getMe";
		bool ok = false; int status = 0; std::string body;
		bool completed = s_fnSyncHttpsGet(getMeUrl, ok, status, body, 15);
		if (!completed)
		{
			fprintf(stderr, "[verify-telegram] FAIL: getMe timed out (network unreachable or DNS failure)\r\n");
			return 1;
		}
		if (!ok || status != 200)
		{
			fprintf(stderr, "[verify-telegram] FAIL: getMe returned status=%d body=%s (token invalid)\r\n",
							status, body.c_str());
			return 1;
		}
		printf("[verify-telegram] OK: bot token is valid (getMe status=%d)\r\n", status);

		if (_chatId.empty())
		{			
			printf("[verify-telegram] OK: chat_id not provided, skipped chat_id verification\r\n");
			return 0;
		}
		std::string getChatUrl = "https://api.telegram.org/bot" + _token + "/getChat?chat_id=" + _chatId;
		bool ok2 = false; int status2 = 0; std::string body2;
		bool completed2 = s_fnSyncHttpsGet(getChatUrl, ok2, status2, body2, 15);
		if (!completed2)
		{
			fprintf(stderr, "[verify-telegram] FAIL: getChat timed out (network unreachable or DNS failure)\r\n");
			return 1;
		}
		if (!ok2 || status2 != 200)
		{
			fprintf(stderr, "[verify-telegram] FAIL: getChat returned status=%d body=%s (chat_id invalid or bot has no access to it)\r\n",
							status2, body2.c_str());
			return 1;
		}
		printf("[verify-telegram] OK: bot_token and chat_id combination is valid (getChat status=%d)\r\n", status2);
		return 0;
	}
	catch (const std::exception & e)
	{
		fprintf(stderr, "[verify-telegram] FAIL: unexpected exception: %s\r\n", e.what());
		return 1;
	}
	catch (...)
	{
		fprintf(stderr, "[verify-telegram] FAIL: unexpected unknown exception\r\n");
		return 1;
	}
}

/************************** main ***************************/
int main(int _argc, char ** _argv)
{
	setvbuf(stdout, NULL, _IONBF, 0);
	if (_argc >= 2 && std::string(_argv[1]) == "--verify-telegram")
	{
		std::string token = (_argc >= 3) ? _argv[2] : "";
		std::string chatId = (_argc >= 4) ? _argv[3] : "";
		if (token.empty())
		{
			fprintf(stderr, "[verify-telegram] FAIL: usage: NotebookFlow.exe --verify-telegram <bot_token> <chat_id>\r\n");
			ExitProcess(1);
		}
		int verifyResult = s_fnVerifyTelegramMode(token, chatId);
		ExitProcess((UINT)verifyResult);
	}
	std::wstring exeDirWide = s_fnGetExeDirWide();
	if (!SetCurrentDirectoryW(exeDirWide.c_str()))
	{
		printf("[NotebookFlow] WARNING: SetCurrentDirectoryW(%ls) failed (err=%lu) - relative paths(addr.ini/rest.sce/user.sce) may resolve against the launch cwd instead of the exe folder.\r\n",
			exeDirWide.c_str(), GetLastError());
	}
	else
	{
		printf("[NotebookFlow] working directory pinned to exe folder: %s\r\n", s_fnGetExeDirUtf8().c_str());
	}
	s_fnEnsureAddrIniExists(exeDirWide);
	s_fnEnsureInstallHourRecorded(exeDirWide);
	App app;
	httplib::Server svr;
	SetupHttpServer(app, svr);
	int httpPort = 18099;
	std::thread httpThread([&svr, httpPort]
	{
		if (!svr.listen("0.0.0.0", httpPort))
		{
			printf("NotebookFlow HTTP FAILED to bind 0.0.0.0:%d (port already in use?)\r\n", httpPort);
		}
	});
	httpThread.detach();
	printf("NotebookFlow HTTP listening on 0.0.0.0:%d (GET/PUT/POST any path, GET /notebookflow/events/:as_id for SSE, GET /notebookflow/trace/:name for tracing SSE)\r\n", httpPort);
	app.RUNFLOW(_argc, _argv);
	return 0;
}
