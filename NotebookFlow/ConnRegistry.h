#ifndef _CONN_REGISTRY_H
#define _CONN_REGISTRY_H
#include <atomic>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>

namespace NotebookFlowHttp
{
class ConnHandle
{
public:
	virtual ~ConnHandle() {}
	virtual void Deliver(const std::string & _json) = 0;
	virtual bool IsOneShot() const = 0;
};
class PromiseConn : public ConnHandle
{
public:
	void Deliver(const std::string & _json) override;
	bool IsOneShot() const override { return true; }
	std::promise<std::string> m_promise;
};
class SseConn : public ConnHandle
{
public:
	void Deliver(const std::string & _json) override;
	bool IsOneShot() const override { return false; }
	bool WaitAndPop(std::string & _out, int _timeoutMs);
	void Close();
	bool IsClosed();
private:
	std::mutex m_mtx;
	std::condition_variable m_cv;
	std::queue<std::string> m_q;
	bool m_bClosed = false;
};
class ConnRegistry
{
public:
	int Register(std::shared_ptr<ConnHandle> _h);
	std::shared_ptr<ConnHandle> Find(int _idx);
	void Remove(int _idx);
private:
	std::mutex m_mtx;
	std::unordered_map<int, std::shared_ptr<ConnHandle>> m_table;
	std::atomic<int> m_nextIdx{1};
};
ConnRegistry & GConnRegistry();

}
#endif
