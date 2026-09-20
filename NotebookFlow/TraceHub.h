#ifndef _TRACE_HUB_H
#define _TRACE_HUB_H
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include "ConnRegistry.h"

namespace NotebookFlowHttp
{
class TraceHub
{
public:
	void Attach(const std::string & _name, std::shared_ptr<SseConn> _conn);
	void Detach(SseConn * _conn);
	bool IsActive() const { return m_bActive.load(std::memory_order_relaxed); }
	bool Matches(const std::string & _domain);
	void Emit(const std::string & _json);
private:
	std::atomic<bool> m_bActive{false};
	std::mutex m_mtx;
	std::string m_name;
	std::shared_ptr<SseConn> m_conn;
};
TraceHub & GTraceHub();
}
#endif
