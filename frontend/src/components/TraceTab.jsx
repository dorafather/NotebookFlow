import { useEffect, useRef, useState } from 'react';
import { api } from '../api.js';

// 백엔드가 흘려보내는 4개 메시지(①Flow엔진→HTTP계층 JSON / ②HTTP계층→외부
// raw 요청 / ③외부→HTTP계층 raw 응답 / ④HTTP계층→Flow엔진 JSON)를 방향별로
// 구분해서 그대로 보여준다 - 가공/요약 없음.
const DIR_META = {
  1: { label: '① 엔진 → 통신계층', cls: 'dir-1', arrow: '➜' },
  2: { label: '② 통신계층 → 외부 (raw 요청)', cls: 'dir-2', arrow: '↗' },
  3: { label: '③ 외부 → 통신계층 (raw 응답)', cls: 'dir-3', arrow: '↙' },
  4: { label: '④ 통신계층 → 엔진', cls: 'dir-4', arrow: '⇐' },
};

// 4개 메시지를 하나의 평평한 줄 목록으로 이어붙이면 화면상 "끊임없이
// 흐르는 스트림"처럼 보여 혼동을 준다(실제로는 매번 완결되는 "단일
// 요청/응답" 한 세트일 뿐 - Telegram 폴링처럼 몇 초 간격을 두고 독립적인
// 호출이 반복되는 것). ①로 시작하는 메시지부터 다음 ①이 오기 전까지를
// 하나의 "콜"로 묶어 카드 하나로 렌더링해서, "이건 방금 끝난 하나의
// 이벤트"라는 것을 시각적으로 분명히 한다.
const MAX_CALLS = 50;

function tryPretty(s) {
  if (typeof s !== 'string') return s == null ? '' : String(s);
  try {
    return JSON.stringify(JSON.parse(s), null, 2);
  } catch {
    return s;
  }
}

function fmtTime(ts) {
  return new Date(ts).toLocaleTimeString('ko-KR', { hour12: false });
}

function TraceLine({ l }) {
  const meta = DIR_META[String(l.dir)] || { label: `dir ${l.dir}`, cls: '', arrow: '?' };
  const isJsonMsg = l.dir === '1' || l.dir === '4';
  return (
    <div className={`trace-entry ${meta.cls}`}>
      <div className="trace-entry-head">
        <span className="trace-dir">{meta.arrow}</span>
        <span className="trace-entry-label">{meta.label}</span>
        {!isJsonMsg && l.method && <span className="trace-status">{l.method}</span>}
        {!isJsonMsg && l.status !== undefined && <span className="trace-status">{l.status}</span>}
      </div>
      {!isJsonMsg && l.url && <div className="trace-url">{l.url}</div>}
      {!isJsonMsg && Array.isArray(l.headers) && l.headers.length > 0 && (
        <div className="trace-headers">
          {l.headers.map((h, i) => (
            <div key={i}>{h.key}: {h.val}</div>
          ))}
        </div>
      )}
      <pre className="trace-body">{tryPretty(isJsonMsg ? l.payload : l.body)}</pre>
    </div>
  );
}

function CallCard({ call }) {
  return (
    <div className="trace-call">
      <div className="trace-call-head">
        <span className="trace-call-time">{fmtTime(call.ts)}</span>
        <span className="trace-call-badge">단일 요청 · 응답</span>
      </div>
      {call.msgs.map((l, i) => (
        <TraceLine l={l} key={i} />
      ))}
    </div>
  );
}

export default function TraceTab({ addrConfig, selected }) {
  const [calls, setCalls] = useState([]);
  // "정지"는 백엔드/SSE 연결과는 무관하다 - 서버는 계속 추적을 흘려보내고,
  // 여기서는 화면 렌더링만 무시한다. 부하 방지를 위해 정지 중 도착한
  // 메시지는 버퍼링하지 않고 그냥 버린다(폐기) - "재개"를 누르는 순간부터
  // 새로 들어오는 메시지만 화면에 반영된다.
  const [paused, setPaused] = useState(false);
  const pausedRef = useRef(false);
  const consoleRef = useRef(null);

  useEffect(() => {
    pausedRef.current = paused;
  }, [paused]);

  function handleTogglePause() {
    setPaused((p) => !p);
  }

  useEffect(() => {
    let cancelled = false;
    let es = null;
    setCalls([]);
    setPaused(false);

    async function connect() {
      // addr.ini [TRACE].name을 지금 선택된 연동으로 갱신 - 기존
      // "계정설정" 탭과 동일한 API(GET으로 받은 전체 객체를 그대로
      // 들고 있다가 TRACE 카테고리만 갈아끼워서 PUT)를 재사용한다.
      // 새 전용 API를 만들지 않는다.
      try {
        const full = { ...addrConfig, TRACE: [{ key: 'name', val: selected }] };
        await api.putAddrConfig(full);
      } catch {
        // 추적 대상 기록에 실패해도 SSE 연결은 시도한다 - 최악의 경우
        // 이전 대상 기준으로 필터링되어 화면에 아무 것도 안 뜨는 정도.
      }
      if (cancelled) return;
      es = new EventSource(`/notebookflow/trace/${encodeURIComponent(selected)}`);
      es.onmessage = (ev) => {
        if (cancelled) return;
        let obj;
        try {
          obj = JSON.parse(ev.data);
        } catch {
          return;
        }
        // 정지 상태여도 연결/수신 자체는 끊지 않는다 - 다만 부하를 늘리지
        // 않기 위해 버퍼링 없이 그냥 폐기한다(재개 시점부터 들어오는
        // 메시지만 화면에 반영됨).
        if (pausedRef.current) return;
        setCalls((prev) => {
          const startsNewCall = String(obj.dir) === '1' || prev.length === 0;
          let next;
          if (startsNewCall) {
            next = [...prev, { id: `${Date.now()}-${Math.random()}`, ts: Date.now(), msgs: [obj] }];
          } else {
            const last = prev[prev.length - 1];
            next = prev.slice(0, -1).concat([{ ...last, msgs: [...last.msgs, obj] }]);
          }
          return next.length > MAX_CALLS ? next.slice(next.length - MAX_CALLS) : next;
        });
      };
    }
    connect();

    // 탭을 벗어나거나(이 컴포넌트 언마운트) 다른 연동을 선택하면(selected
    // 변경) SSE 연결을 정상 종료한다 - EventSource.close()로 클라이언트가
    // 먼저 끊거나, 다음 선택으로 addr.ini [TRACE].name이 바뀌어 서버가
    // 이전 연결을 닫는다(TraceHub::Attach()가 이전 연결을 강제 종료).
    return () => {
      cancelled = true;
      if (es) es.close();
    };
  }, [selected]); // eslint-disable-line react-hooks/exhaustive-deps

  useEffect(() => {
    if (consoleRef.current) consoleRef.current.scrollTop = consoleRef.current.scrollHeight;
  }, [calls]);

  return (
    <div className="tabpane">
      <div className="trace-toolbar">
        {/* 연결 상태 요약 문구는 의도적으로 표시하지 않는다 - "연결 재시도
            중" 같은 상태 문구조차 "연결이 원래 이상했나?" 하는 혼동을
            유발할 수 있어, 연결 성공/실패 여부와 무관하게 화면은 항상
            같은 정적인 제목만 보여준다. */}
        <div className="trace-toolbar-title">{selected} 요청 · 응답 기록</div>
        {paused && <div className="live-badge off">⏸ 정지 중</div>}
        <button className="btn btn-sm" onClick={handleTogglePause}>
          {paused ? '▶ 재개' : '⏸ 정지'}
        </button>
        <button className="btn btn-sm" onClick={() => setCalls([])}>지우기</button>
      </div>
      <div className="trace-console" ref={consoleRef}>
        {calls.length === 0 && (
          <div className="empty-hint">
            아직 기록이 없습니다 - {selected} 연동에 실제 요청이 발생하면, 그
            요청 하나에 대한 응답까지를 한 세트로 여기 기록됩니다(끊임없이
            흐르는 스트림이 아니라, 매번 완결되는 단일 요청 · 응답입니다).
          </div>
        )}
        {calls.map((c) => (
          <CallCard call={c} key={c.id} />
        ))}
      </div>
    </div>
  );
}
