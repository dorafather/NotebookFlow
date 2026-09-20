#include "ConnRegistry.h"
#include <chrono>

namespace NotebookFlowHttp
{
void PromiseConn::Deliver(const std::string & _json)
{
	try
	{
		m_promise.set_value(_json);
	}
	catch (const std::future_error &)
	{	
	}
}
void SseConn::Deliver(const std::string & _json)
{
	{
		std::lock_guard<std::mutex> lk(m_mtx);
		m_q.push(_json);
	}
	m_cv.notify_one();
}
bool SseConn::WaitAndPop(std::string & _out, int _timeoutMs)
{
	std::unique_lock<std::mutex> lk(m_mtx);
	if (!m_cv.wait_for(lk, std::chrono::milliseconds(_timeoutMs),
			[this] { return m_bClosed || !m_q.empty(); }))
	{
		return false; 
	}
	if (!m_q.empty())
	{
		_out = m_q.front();
		m_q.pop();
		return true;
	}
	return false; 
}
void SseConn::Close()
{
	{
		std::lock_guard<std::mutex> lk(m_mtx);
		m_bClosed = true;
	}
	m_cv.notify_all();
}
bool SseConn::IsClosed()
{
	std::lock_guard<std::mutex> lk(m_mtx);
	return m_bClosed;
}
int ConnRegistry::Register(std::shared_ptr<ConnHandle> _h)
{
	int idx = m_nextIdx.fetch_add(1);
	std::lock_guard<std::mutex> lk(m_mtx);
	m_table[idx] = _h;
	return idx;
}
std::shared_ptr<ConnHandle> ConnRegistry::Find(int _idx)
{
	std::lock_guard<std::mutex> lk(m_mtx);
	auto it = m_table.find(_idx);
	if (it == m_table.end()) return nullptr;
	return it->second;
}
void ConnRegistry::Remove(int _idx)
{
	std::lock_guard<std::mutex> lk(m_mtx);
	m_table.erase(_idx);
}
ConnRegistry & GConnRegistry()
{
	static ConnRegistry s_inst;
	return s_inst;
}
}
