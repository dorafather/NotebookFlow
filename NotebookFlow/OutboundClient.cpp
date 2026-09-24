#include "OutboundClient.h"
#include "httplib.h"
#include <windows.h>
#include <vector>
#include <fstream>
#include <mutex>

namespace NotebookFlowHttp
{
// 2026-09-25: Windows OS 인증서 저장소(WinHTTP/Schannel)가 오래되거나
// 방치된 PC에서 낡아있으면(루트 CA 미갱신) TLS 핸드셰이크가 실패한다 -
// 실제로 Windows 8.1 테스트 PC에서 err=12175(SSL 인증서 오류)로 재현됨.
// OS 저장소에 의존하지 않도록 자체 CA 번들(exe와 같은 폴더의 cacert.pem)로
// 검증하는 httplib+OpenSSL 경로로 통일했다 - 아래가 그 경로를 찾는 함수.
static const std::string & s_fnGetCaCertPath()
{
	static std::string s_path;
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
		std::string dir(".");
		if (u8len > 0)
		{
			dir.resize((size_t)u8len);
			WideCharToMultiByte(CP_UTF8, 0, wdir.c_str(), (int)wdir.size(), &dir[0], u8len, NULL, NULL);
		}
		s_path = dir + "\\cacert.pem";
	});
	return s_path;
}
static std::string s_fnPercentEncodeSpaceAndNonAscii(const std::string & _url)
{
	static const char hex[] = "0123456789ABCDEF";
	std::string out;
	out.reserve(_url.size());
	for (unsigned char c : _url)
	{
		if (c == ' ' || c >= 0x80)
		{
			out += '%';
			out += hex[(c >> 4) & 0xF];
			out += hex[c & 0xF];
		}
		else
		{
			out += (char)c;
		}
	}
	return out;
}
static bool s_fnHasContentTypeHeader(const OutboundClient::HeaderList & _headers)
{
	for (const auto & kv : _headers)
	{
		if (kv.first.size() != 12) continue;
		bool bMatch = true;
		static const char s_key[] = "content-type";
		for (size_t i = 0; i < 12; i++)
		{
			char c = kv.first[i];
			if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
			if (c != s_key[i]) { bMatch = false; break; }
		}
		if (bMatch) return true;
	}
	return false;
}
static bool s_fnSplitUrl(const std::string & _url, std::string & _schemeHostPort, std::string & _path)
{
	auto schemeEnd = _url.find("://");
	if (schemeEnd == std::string::npos) return false;
	auto pathStart = _url.find('/', schemeEnd + 3);
	if (pathStart == std::string::npos)
	{
		_schemeHostPort = _url;
		_path = "/";
	}
	else
	{
		_schemeHostPort = _url.substr(0, pathStart);
		_path = _url.substr(pathStart);
	}
	return true;
}
OutboundClient::OutboundClient(int _numWorkers)
{
	for (int i = 0; i < _numWorkers; i++)
	{
		m_workers.emplace_back(&OutboundClient::WorkerLoop, this);
	}
}
OutboundClient::~OutboundClient()
{
	{
		std::lock_guard<std::mutex> lk(m_mtx);
		m_bStop = true;
	}
	m_cv.notify_all();
	for (auto & t : m_workers)
	{
		if (t.joinable()) t.join();
	}
}
void OutboundClient::PostAsync(const std::string & _url, const std::string & _jsonBody,
				std::function<void(bool, int, const std::string &)> _onComplete,
				const HeaderList & _headers,
				TraceRespHook _traceHook)
{
	Task task;
	task.m_method = HttpMethod::Post;
	task.m_url = _url;
	task.m_body = _jsonBody;
	task.m_headers = _headers;
	task.m_cb = std::move(_onComplete);
	task.m_traceHook = std::move(_traceHook);
	{
		std::lock_guard<std::mutex> lk(m_mtx);
		m_q.push(std::move(task));
	}
	m_cv.notify_one();
}
void OutboundClient::GetAsync(const std::string & _url,
				std::function<void(bool, int, const std::string &)> _onComplete,
				const HeaderList & _headers,
				TraceRespHook _traceHook,
				const std::string & _saveToPath)
{
	Task task;
	task.m_method = HttpMethod::Get;
	task.m_url = _url;
	task.m_headers = _headers;
	task.m_cb = std::move(_onComplete);
	task.m_traceHook = std::move(_traceHook);
	task.m_saveToPath = _saveToPath;
	{
		std::lock_guard<std::mutex> lk(m_mtx);
		m_q.push(std::move(task));
	}
	m_cv.notify_one();
}
void OutboundClient::WorkerLoop()
{
	for (;;)
	{
		Task task;
		{
			std::unique_lock<std::mutex> lk(m_mtx);
			m_cv.wait(lk, [this] { return m_bStop || !m_q.empty(); });
			if (m_bStop && m_q.empty()) return;
			task = std::move(m_q.front());
			m_q.pop();
		}

		bool ok = false;
		std::string respBody;
		std::string schemeHostPort, path;
		int respStatus = 0;
		OutboundClient::HeaderList traceHeaders;

		try
		{
			if (s_fnSplitUrl(task.m_url, schemeHostPort, path))
			{
				path = s_fnPercentEncodeSpaceAndNonAscii(path);
				httplib::Client cli(schemeHostPort);
				cli.set_connection_timeout(5, 0);
				cli.set_read_timeout(10, 0);
				cli.set_write_timeout(10, 0);
				cli.set_path_encode(false);
				bool bIsHttps = schemeHostPort.rfind("https://", 0) == 0;
				if (bIsHttps)
				{
					// OS 인증서 저장소(WinHTTP/Schannel) 대신 자체 CA 번들로
					// 검증한다 - set_ca_cert_path()를 부르면 httplib이 Windows
					// 기본 인증서 검증(Schannel) 폴백을 자동으로 건너뛴다.
					cli.set_ca_cert_path(s_fnGetCaCertPath());
					cli.enable_server_certificate_verification(true);
				}
				httplib::Headers hdrs;
				for (const auto & kv : task.m_headers) hdrs.emplace(kv.first, kv.second);

				httplib::Result res;
				bool bSaveOpenFailed = false;
				if (task.m_method == HttpMethod::Get)
				{
					if (!task.m_saveToPath.empty())
					{
						std::ofstream ofsSave(task.m_saveToPath, std::ios::binary | std::ios::trunc);
						bSaveOpenFailed = !ofsSave;
						res = cli.Get(path.c_str(), hdrs,
							[&ofsSave, &bSaveOpenFailed](const char * data, size_t len) -> bool
							{
								if (!bSaveOpenFailed) ofsSave.write(data, (std::streamsize)len);
								return true;
							});
						ofsSave.close();
					}
					else
					{
						res = cli.Get(path.c_str(), hdrs);
					}
				}
				else
				{
					res = cli.Post(path.c_str(), hdrs, task.m_body,
									s_fnHasContentTypeHeader(task.m_headers) ? "" : "application/json");
				}

				if (res)
				{
					respStatus = res->status;
					ok = (res->status >= 200 && res->status < 300) && !bSaveOpenFailed;
					if (task.m_saveToPath.empty()) respBody = res->body;
					if (task.m_traceHook)
					{
						for (const auto & h : res->headers) traceHeaders.emplace_back(h.first, h.second);
					}
				}
				else
				{
					respBody = bIsHttps ? "outbound https request failed: " : "outbound request failed: ";
					respBody += httplib::to_string(res.error());
				}
			}
			else
			{
				respBody = "invalid RA url: ";
				respBody += task.m_url;
			}
		}
		catch (const std::exception & e)
		{
			ok = false;
			respBody = std::string("outbound exception: ") + e.what();
		}
		catch (...)
		{
			ok = false;
			respBody = "outbound unknown exception";
		}
		if (task.m_traceHook)
		{
			try
			{
				task.m_traceHook(respStatus, traceHeaders, respBody);
			}
			catch (const std::exception & e)
			{
				printf("[DBG-TRACE] traceHook exception: %s\n", e.what()); fflush(stdout);
			}
			catch (...)
			{
				printf("[DBG-TRACE] traceHook unknown exception\n"); fflush(stdout);
			}
		}

		if (task.m_cb)
		{
			try
			{
				task.m_cb(ok, respStatus, respBody);
			}
			catch (const std::exception & e)
			{
				printf("[DBG-CRASH] onComplete callback exception: %s\n", e.what()); fflush(stdout);
			}
			catch (...)
			{
				printf("[DBG-CRASH] onComplete callback unknown exception\n"); fflush(stdout);
			}
		}
	}
}
OutboundClient & GOutboundClient()
{
	static OutboundClient s_inst(2);
	return s_inst;
}

}
