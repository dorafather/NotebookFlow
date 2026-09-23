#include "OutboundClient.h"
#include "httplib.h"
#include <winhttp.h>
#include <vector>
#include <fstream>
#pragma comment(lib, "winhttp.lib")

namespace NotebookFlowHttp
{
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
static std::wstring s_fnBuildHeaderBlock(const OutboundClient::HeaderList & _headers)
{
	std::wstring out;
	for (const auto & kv : _headers)
	{
		std::wstring wkey(kv.first.begin(), kv.first.end());
		std::wstring wval(kv.second.begin(), kv.second.end());
		out += wkey; out += L": "; out += wval; out += L"\r\n";
	}
	return out;
}
static void s_fnParseRawHeaders(const std::wstring & _raw, OutboundClient::HeaderList & _out)
{
	std::wstring line;
	for (size_t i = 0; i <= _raw.size(); i++)
	{
		if (i == _raw.size() || _raw[i] == L'\r' || _raw[i] == L'\n')
		{
			if (!line.empty())
			{
				size_t colon = line.find(L": ");
				if (colon != std::wstring::npos)
				{
					std::wstring wk = line.substr(0, colon);
					std::wstring wv = line.substr(colon + 2);
					_out.emplace_back(std::string(wk.begin(), wk.end()), std::string(wv.begin(), wv.end()));
				}
			}
			line.clear();
		}
		else
		{
			line += _raw[i];
		}
	}
}
static bool s_fnHttpsRequest(const std::string & _url, bool _isPost, const std::string & _body,
				const OutboundClient::HeaderList & _headers,
				bool & _ok, std::string & _respBody,
				int * _outStatus = nullptr, OutboundClient::HeaderList * _outRespHeaders = nullptr,
				const std::string * _pSaveToPath = nullptr)
{
	_ok = false;
	std::string encoded = s_fnPercentEncodeSpaceAndNonAscii(_url);
	std::wstring wurl(encoded.begin(), encoded.end());
	URL_COMPONENTS uc = {};
	wchar_t hostName[256] = {0};
	wchar_t urlPath[4096] = {0};
	uc.dwStructSize = sizeof(uc);
	uc.lpszHostName = hostName;
	uc.dwHostNameLength = 256;
	uc.lpszUrlPath = urlPath;
	uc.dwUrlPathLength = 4096;
	uc.dwSchemeLength = (DWORD)-1;
	if (!WinHttpCrackUrl(wurl.c_str(), (DWORD)wurl.size(), 0, &uc))
	{
		_respBody = "WinHttpCrackUrl failed";
		return false;
	}
	HINTERNET hSession = WinHttpOpen(L"NotebookFlow/1.0",
					WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
					WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (!hSession)
	{
		_respBody = "WinHttpOpen failed";
		return false;
	}
	// Windows 8 이하는 WinHTTP 기본 협상 프로토콜이 SSL3/TLS1.0까지만 켜져
	// 있어 TLS1.2 이상만 받는 서버(Telegram 등)와 핸드셰이크가 실패한다
	// (반면 브라우저는 자체 TLS 스택을 쓰므로 영향 없음). 리터럴 값 사용은
	// WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_1/1_2가 winhttp.h에서
	// _WIN32_WINNT>=0x0602 조건부로만 정의돼 빌드 환경에 따라 매크로가
	// 없을 수 있기 때문.
	DWORD secureProtocols = 0x00000080 /*TLS1_0*/ | 0x00000200 /*TLS1_1*/ | 0x00000800 /*TLS1_2*/;
	WinHttpSetOption(hSession, WINHTTP_OPTION_SECURE_PROTOCOLS, &secureProtocols, sizeof(secureProtocols));
	HINTERNET hConnect = WinHttpConnect(hSession, hostName, uc.nPort, 0);
	if (!hConnect)
	{
		WinHttpCloseHandle(hSession);
		_respBody = "WinHttpConnect failed";
		return false;
	}
	DWORD flags = (uc.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;
	HINTERNET hRequest = WinHttpOpenRequest(hConnect, _isPost ? L"POST" : L"GET",
					urlPath, NULL, WINHTTP_NO_REFERER,
					WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
	if (!hRequest)
	{
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);
		_respBody = "WinHttpOpenRequest failed";
		return false;
	}
	std::wstring headerBlock;
	if (_isPost && !s_fnHasContentTypeHeader(_headers)) headerBlock = L"Content-Type: application/json\r\n";
	headerBlock += s_fnBuildHeaderBlock(_headers);
	LPCWSTR headersPtr = headerBlock.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS : headerBlock.c_str();
	DWORD headersLen = headerBlock.empty() ? 0 : (DWORD)-1L;

	BOOL bResults;
	if (_isPost)
	{
		bResults = WinHttpSendRequest(hRequest, headersPtr, headersLen,
						(LPVOID)_body.data(), (DWORD)_body.size(),
						(DWORD)_body.size(), 0);
	}
	else
	{
		bResults = WinHttpSendRequest(hRequest, headersPtr, headersLen,
						WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	}
	if (bResults) bResults = WinHttpReceiveResponse(hRequest, NULL);

	if (bResults)
	{
		DWORD statusCode = 0; DWORD statusSize = sizeof(statusCode);
		WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_FLAG_NUMBER | WINHTTP_QUERY_STATUS_CODE,
						WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusSize, WINHTTP_NO_HEADER_INDEX);
		if (_outStatus) *_outStatus = (int)statusCode;
		if (_outRespHeaders)
		{
			DWORD hdrSize = 0;
			WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF,
							WINHTTP_HEADER_NAME_BY_INDEX, WINHTTP_NO_OUTPUT_BUFFER, &hdrSize, WINHTTP_NO_HEADER_INDEX);
			if (hdrSize > 0)
			{
				std::wstring hdrBuf(hdrSize / sizeof(wchar_t), L'\0');
				if (WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF,
								WINHTTP_HEADER_NAME_BY_INDEX, &hdrBuf[0], &hdrSize, WINHTTP_NO_HEADER_INDEX))
				{
					s_fnParseRawHeaders(hdrBuf, *_outRespHeaders);
				}
			}
		}
		std::ofstream ofsSave;
		bool bStreamToFile = (_pSaveToPath != nullptr && !_pSaveToPath->empty());
		bool bSaveOpenFailed = false;
		if (bStreamToFile)
		{
			ofsSave.open(*_pSaveToPath, std::ios::binary | std::ios::trunc);
			if (!ofsSave) bSaveOpenFailed = true;
		}
		DWORD dwSize = 0;
		do
		{
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
			if (dwSize == 0) break;
			std::vector<char> buf(dwSize);
			DWORD dwRead = 0;
			if (WinHttpReadData(hRequest, buf.data(), dwSize, &dwRead))
			{
				if (bStreamToFile)
				{
					if (!bSaveOpenFailed) ofsSave.write(buf.data(), dwRead);
				}
				else
				{
					_respBody.append(buf.data(), dwRead);
				}
			}
		} while (dwSize > 0);

		if (bStreamToFile) ofsSave.close();

		_ok = (statusCode >= 200 && statusCode < 300) && !bSaveOpenFailed;
	}
	else
	{
		_respBody = "WinHttp request failed, err=" + std::to_string(GetLastError());
	}
	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	WinHttpCloseHandle(hSession);
	return true;
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
		if (task.m_url.rfind("https://", 0) == 0)
		{
			OutboundClient::HeaderList * pTraceHeaders = task.m_traceHook ? &traceHeaders : nullptr;
			const std::string * pSaveToPath = task.m_saveToPath.empty() ? nullptr : &task.m_saveToPath;
			try
			{
				s_fnHttpsRequest(task.m_url, task.m_method == HttpMethod::Post, task.m_body, task.m_headers, ok, respBody, &respStatus, pTraceHeaders, pSaveToPath);
			}
			catch (const std::exception & e)
			{
				ok = false;
				respBody = std::string("outbound https exception: ") + e.what();
			}
			catch (...)
			{
				ok = false;
				respBody = "outbound https unknown exception";
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
			continue;
		}
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
				httplib::Headers hdrs;
				for (const auto & kv : task.m_headers) hdrs.emplace(kv.first, kv.second);
				httplib::Result res = (task.m_method == HttpMethod::Get)
					? cli.Get(path.c_str(), hdrs)
					: cli.Post(path.c_str(), hdrs, task.m_body,
									s_fnHasContentTypeHeader(task.m_headers) ? "" : "application/json");
				if (res)
				{
					respBody = res->body;
					ok = (res->status >= 200 && res->status < 300);
					respStatus = res->status;
					if (task.m_traceHook)
					{
						for (const auto & h : res->headers) traceHeaders.emplace_back(h.first, h.second);
					}
				}
				else
				{
					respBody = "outbound request failed: ";
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

		if (task.m_cb) task.m_cb(ok, respStatus, respBody);
	}
}
OutboundClient & GOutboundClient()
{
	static OutboundClient s_inst(2);
	return s_inst;
}

}
