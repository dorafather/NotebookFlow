import { useEffect, useRef, useState } from 'react';
import { api } from '../api.js';

// NotebookFlow.exe가 띄우는 자식 RA 프로세스(agent-flow/ssh-flow/file-ra)가
// 실제로 살아있는지 웹 UI에서 한눈에 보이게 하는 전용 탭. 사이드바의
// "연동" 목록과는 성격이 다른 전역 화면이라(addr.ini 카테고리가 아니라
// 프로세스 헬스체크), App.jsx가 별도 최상위 뷰로 렌더링한다(사이드바
// 맨 위에 별도 항목으로 노출 - Sidebar.jsx 참고).
const POLL_MS = 7000;

// 카드에 표시할 순서/한글 라벨 - 백엔드 응답 키(notebookflow/agent-flow/
// ssh-flow/file-ra)는 그대로 두고 화면 라벨만 한글로 매핑한다.
const SERVICES = [
  { key: 'notebookflow', label: 'NotebookFlow', desc: '이 화면을 서빙 중인 프로세스 자신' },
  { key: 'agent-flow', label: 'agent-flow', desc: 'Claude Code 작업 위임 (클루드코드 명령)' },
  { key: 'ssh-flow', label: 'ssh-flow', desc: '원격 서버 셸 명령 실행 (텔넷 명령)' },
  { key: 'file-ra', label: 'file-ra', desc: '로컬 파일 규칙/파일 관련 명령' },
];

function StatusDot({ status }) {
  const cls = status === 'up' ? 'sys-dot-ok' : status === 'down' ? 'sys-dot-down' : 'sys-dot-unknown';
  return <span className={`sys-dot ${cls}`} />;
}

function ServiceCard({ meta, data }) {
  // data가 아예 없다면(폴링 실패, 또는 백엔드 응답에 이 키가 없음) "확인 불가"로
  // 표시한다 - 화면이 깨지지 않게 방어적으로 처리.
  const status = data?.status === 'up' ? 'up' : data?.status === 'down' ? 'down' : 'unknown';
  const statusLabel = status === 'up' ? '정상' : status === 'down' ? '응답 없음' : '확인 중…';
  return (
    <div className={`sys-card sys-card-${status}`}>
      <div className="sys-card-head">
        <StatusDot status={status} />
        <span className="sys-card-name">{meta.label}</span>
        <span className={`sys-card-badge sys-card-badge-${status}`}>{statusLabel}</span>
      </div>
      <div className="sys-card-desc">{meta.desc}</div>
      {status === 'up' && (
        <div className="sys-card-meta">
          {data?.latency_ms !== undefined && <span>응답시간 {data.latency_ms}ms</span>}
          {data?.active_jobs !== undefined && <span>작업 중 {data.active_jobs}건</span>}
        </div>
      )}
      {status === 'down' && data?.error && (
        <div className="sys-card-error">{String(data.error)}</div>
      )}
    </div>
  );
}

export default function SystemStatusTab() {
  const [status, setStatus] = useState(null);
  const [lastOk, setLastOk] = useState(null);
  const [pollError, setPollError] = useState(null);
  const timerRef = useRef(null);

  useEffect(() => {
    let cancelled = false;

    async function poll() {
      try {
        const data = await api.getSystemStatus();
        if (cancelled) return;
        setStatus(data);
        setLastOk(Date.now());
        setPollError(null);
      } catch (e) {
        // 폴링 요청 자체가 실패해도(네트워크 순간 끊김 등) 화면을 비우지
        // 않는다 - 마지막으로 성공한 status를 그대로 유지하고, 상단에만
        // 조용히 에러 문구를 얹는다.
        if (cancelled) return;
        setPollError(String(e.message || e));
      }
    }

    poll();
    timerRef.current = setInterval(poll, POLL_MS);
    return () => {
      cancelled = true;
      if (timerRef.current) clearInterval(timerRef.current);
    };
  }, []);

  const secsAgo = lastOk ? Math.max(0, Math.round((Date.now() - lastOk) / 1000)) : null;

  return (
    <div className="tabpane">
      <div className="sys-toolbar">
        <div className="sys-toolbar-title">자식 프로세스 헬스체크</div>
        <div className="save-hint">
          {lastOk ? `마지막 확인: ${secsAgo}초 전` : '확인 중…'}
          {pollError && <span className="sys-poll-err"> · 폴링 실패 ({pollError})</span>}
        </div>
      </div>
      <div className="sys-grid">
        {SERVICES.map((meta) => (
          <ServiceCard key={meta.key} meta={meta} data={status ? status[meta.key] : null} />
        ))}
      </div>
      {!status && !pollError && (
        <div className="empty-hint">불러오는 중…</div>
      )}
    </div>
  );
}
