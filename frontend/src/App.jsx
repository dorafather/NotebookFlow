import { useEffect, useState, useCallback, useMemo } from 'react';
import Sidebar from './components/Sidebar.jsx';
import CatalogModal from './components/CatalogModal.jsx';
import AccountTab from './components/AccountTab.jsx';
import ScenarioTab from './components/ScenarioTab.jsx';
import TraceTab from './components/TraceTab.jsx';
import SystemStatusTab from './components/SystemStatusTab.jsx';
import { api, ADDR_SYSTEM_KEYS } from './api.js';

const TABS = [
  { key: 'addr', label: '계정설정' },
  { key: 'scenario', label: '시나리오 설정' },
  { key: 'trace', label: '실시간 추적' },
];

export default function App() {
  const [addrConfig, setAddrConfig] = useState(null);
  const [restConfig, setRestConfig] = useState(null);
  const [loadError, setLoadError] = useState(null);
  const [selected, setSelected] = useState(null);
  const [tab, setTab] = useState('scenario');
  // "시스템 상태"는 특정 연동(selected)과 무관한 전역 화면이라, 기존
  // selected/tab과 별개의 최상위 뷰 플래그로 분기한다. true인 동안은
  // selected가 무엇이든 상관없이 SystemStatusTab만 보여준다.
  const [showSystemStatus, setShowSystemStatus] = useState(false);
  const [showCatalog, setShowCatalog] = useState(false);
  const [saving, setSaving] = useState(false);
  const [toast, setToast] = useState(null);
  const [lastChecked, setLastChecked] = useState(null);
  const [, setTick] = useState(0);

  const loadAll = useCallback(async () => {
    try {
      const [addr, rest] = await Promise.all([api.getAddrConfig(), api.getRestConfig()]);
      setAddrConfig(addr);
      setRestConfig(rest);
      setLastChecked(Date.now());
      setLoadError(null);
      return { addr, rest };
    } catch (e) {
      setLoadError(String(e.message || e));
      throw e;
    }
  }, []);

  useEffect(() => { loadAll(); }, [loadAll]);
  // "마지막 확인: n초 전" 표시를 위해 5초마다 다시 그린다.
  useEffect(() => {
    const t = setInterval(() => setTick((x) => x + 1), 5000);
    return () => clearInterval(t);
  }, []);

  const integrations = useMemo(() => {
    if (!addrConfig) return [];
    return Object.keys(addrConfig)
      .filter((k) => !ADDR_SYSTEM_KEYS.includes(k))
      .map((k) => ({ key: k, configured: (addrConfig[k] || []).length > 0 }));
  }, [addrConfig]);

  useEffect(() => {
    if (!selected && integrations.length > 0) setSelected(integrations[0].key);
  }, [integrations, selected]);

  function showToast(text, isErr) {
    setToast({ text, isErr });
    setTimeout(() => setToast(null), 3500);
  }

  async function handleSaveAddr(fullAddrConfig) {
    setSaving(true);
    try {
      await api.putAddrConfig(fullAddrConfig);
      await loadAll();
      showToast('저장되었습니다 (addr.ini 반영됨)');
    } catch (e) {
      showToast(`저장 실패: ${e.message || e}`, true);
    } finally {
      setSaving(false);
    }
  }
  async function handleSaveRest(fullRestConfig) {
    setSaving(true);
    try {
      await api.putRestConfig(fullRestConfig);
      await loadAll();
      showToast('저장되었습니다');
    } catch (e) {
      showToast(`저장 실패: ${e.message || e}`, true);
    } finally {
      setSaving(false);
    }
  }

  if (loadError) {
    return (
      <div className="error-screen">
        <div>백엔드(NotebookFlow.exe, :18099)에 연결할 수 없습니다.</div>
        <div style={{ fontSize: 12 }}>{loadError}</div>
        <button className="btn" onClick={() => loadAll()}>다시 시도</button>
      </div>
    );
  }
  if (!addrConfig || !restConfig || !selected) {
    return <div className="loading-screen">불러오는 중…</div>;
  }

  const secsAgo = lastChecked ? Math.max(0, Math.round((Date.now() - lastChecked) / 1000)) : null;

  return (
    <div className="app">
      <Sidebar
        integrations={integrations}
        selected={selected}
        onSelect={(k) => { setShowSystemStatus(false); setSelected(k); }}
        onAddClick={() => setShowCatalog(true)}
        isSystemStatus={showSystemStatus}
        onSelectSystemStatus={() => setShowSystemStatus(true)}
      />
      <div className="main">
        {showSystemStatus ? (
          <>
            <div className="topbar">
              <div className="topbar-row">
                <div className="topbar-title">
                  <h1>시스템 상태</h1>
                </div>
              </div>
            </div>
            <div className="content">
              <SystemStatusTab />
            </div>
          </>
        ) : (
          <>
            <div className="topbar">
              <div className="topbar-row">
                <div className="topbar-title">
                  <h1>{selected}</h1>
                  <div className="status-pill"><span className="dot" />정상 연결됨</div>
                </div>
                <div className="save-hint">마지막 확인: {secsAgo != null ? `${secsAgo}초 전` : '-'}</div>
              </div>
              <div className="tabs">
                {TABS.map((t) => (
                  <div key={t.key} className={`tab${tab === t.key ? ' active' : ''}`} onClick={() => setTab(t.key)}>
                    {t.label}
                  </div>
                ))}
              </div>
            </div>
            <div className="content">
              {tab === 'addr' && (
                <AccountTab addrConfig={addrConfig} selected={selected} onSave={handleSaveAddr} saving={saving} />
              )}
              {tab === 'scenario' && (
                <ScenarioTab restConfig={restConfig} ns={selected} onSave={handleSaveRest} saving={saving} addrConfig={addrConfig} />
              )}
              {tab === 'trace' && <TraceTab addrConfig={addrConfig} selected={selected} />}
            </div>
          </>
        )}
      </div>
      <CatalogModal show={showCatalog} onClose={() => setShowCatalog(false)} />
      {toast && <div className={`toast${toast.isErr ? ' err' : ''}`}>{toast.text}</div>}
    </div>
  );
}
