#ifndef _OUTBOUND_CLIENT_H
#define _OUTBOUND_CLIENT_H
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

namespace NotebookFlowHttp
{
class OutboundClient
{
public:
	explicit OutboundClient(int _numWorkers = 2);
	~OutboundClient();
	typedef std::vector<std::pair<std::string,std::string>> HeaderList;
	typedef std::function<void(int _statusCode, const HeaderList & _respHeaders,
					const std::string & _respBody)> TraceRespHook;
	void PostAsync(const std::string & _url, const std::string & _jsonBody,
					std::function<void(bool _ok, int _status, const std::string & _body)> _onComplete,
					const HeaderList & _headers = HeaderList(),
					TraceRespHook _traceHook = nullptr);
	void GetAsync(const std::string & _url,
					std::function<void(bool _ok, int _status, const std::string & _body)> _onComplete,
					const HeaderList & _headers = HeaderList(),
					TraceRespHook _traceHook = nullptr,
					const std::string & _saveToPath = std::string());
private:
	enum class HttpMethod { Post, Get };
	struct Task
	{
		HttpMethod m_method = HttpMethod::Post;
		std::string m_url;
		std::string m_body;
		HeaderList m_headers;
		std::function<void(bool, int, const std::string &)> m_cb;
		TraceRespHook m_traceHook;
		std::string m_saveToPath;
	};
	void WorkerLoop();

	std::vector<std::thread> m_workers;
	std::mutex m_mtx;
	std::condition_variable m_cv;
	std::queue<Task> m_q;
	bool m_bStop = false;
};
OutboundClient & GOutboundClient();
}
#endif
