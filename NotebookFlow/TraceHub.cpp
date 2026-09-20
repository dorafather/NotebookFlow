#include "TraceHub.h"
#include "EXECORE.h"

namespace NotebookFlowHttp
{
void TraceHub::Attach(const std::string & _name, std::shared_ptr<SseConn> _conn)
{
	std::shared_ptr<SseConn> old;
	{
		std::lock_guard<std::mutex> lk(m_mtx);
		old = m_conn;
		m_name = _name;
		m_conn = _conn;
		m_bActive.store(true, std::memory_order_relaxed);
	}
	if (old && old != _conn) old->Close();
}
void TraceHub::Detach(SseConn * _conn)
{
	std::lock_guard<std::mutex> lk(m_mtx);
	if (m_conn.get() == _conn)
	{
		m_bActive.store(false, std::memory_order_relaxed);
		m_conn.reset();
		m_name.clear();
	}
}
bool TraceHub::Matches(const std::string & _domain)
{
	if (!IsActive()) return false;
	std::string name;
	{
		std::lock_guard<std::mutex> lk(m_mtx);
		name = m_name;
	}
	if (name.empty() || _domain.empty()) return false;
	return nsUtil::ExeCore::OBJ().isNamedDomain(name.c_str(), _domain.c_str());
}
void TraceHub::Emit(const std::string & _json)
{
	std::shared_ptr<SseConn> conn;
	{
		std::lock_guard<std::mutex> lk(m_mtx);
		conn = m_conn;
	}
	if (!conn) return;
	conn->Deliver(_json);
}
TraceHub & GTraceHub()
{
	static TraceHub s_inst;
	return s_inst;
}
}
