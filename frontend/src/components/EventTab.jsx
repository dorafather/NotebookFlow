import { useEffect, useState } from 'react';
import { listEvents, saveEvents, listProcedures } from '../restModel.js';

// "모드"(raw/method/addr/rspcode)는 입력 UI를 고르기 위한 화면 전용 상태일
// 뿐, 실제로 저장되는 데이터 모양(fieldName/value/procedureName)에는 영향을
// 주지 않는다 - restModel.js의 saveEvents()는 이 3개 필드만 읽고 쓴다.
const HTTP_METHODS = ['GET', 'POST', 'PUT', 'DELETE'];
const ADDR_SUBS = [
  { key: '도메인', field: '주소.도메인' },
  { key: '경로', field: '주소.경로' },
  { key: 'URL파라미터', field: '주소.파라미터' },
];

function deriveMode(fieldName) {
  if (fieldName === '메소드') return 'method';
  if (fieldName === '응답코드') return 'rspcode';
  if (fieldName.startsWith('주소.')) return 'addr';
  return 'raw';
}
function deriveAddrSub(fieldName) {
  const found = ADDR_SUBS.find((a) => fieldName === a.field || fieldName.startsWith(a.field + '.'));
  return found ? found.key : '경로';
}
function deriveParamKey(fieldName, addrSub) {
  if (addrSub !== 'URL파라미터') return '';
  const prefix = '주소.파라미터.';
  return fieldName.startsWith(prefix) ? fieldName.slice(prefix.length) : '';
}

export default function EventTab({ restConfig, ns, onSave, saving }) {
  const [rows, setRows] = useState([]);
  const procNames = listProcedures(restConfig, ns).map((p) => p.name);

  useEffect(() => {
    setRows(listEvents(restConfig, ns).map((r) => ({
      ...r,
      mode: deriveMode(r.fieldName),
      addrSub: deriveAddrSub(r.fieldName),
      paramKey: deriveParamKey(r.fieldName, deriveAddrSub(r.fieldName)),
    })));
  }, [restConfig, ns]);

  function update(i, field, value) {
    setRows((prev) => prev.map((r, idx) => (idx === i ? { ...r, [field]: value } : r)));
  }
  // 모드/서브선택이 바뀌면 실제로 저장될 fieldName도 같이 다시 계산한다.
  function setMode(i, mode) {
    setRows((prev) => prev.map((r, idx) => {
      if (idx !== i) return r;
      if (mode === 'method') return { ...r, mode, fieldName: '메소드' };
      if (mode === 'rspcode') return { ...r, mode, fieldName: '응답코드' };
      if (mode === 'addr') {
        const sub = ADDR_SUBS.find((a) => a.key === r.addrSub) || ADDR_SUBS[1];
        return { ...r, mode, fieldName: sub.key === 'URL파라미터' ? `주소.파라미터.${r.paramKey}` : sub.field };
      }
      // raw(필드명/바디 공용) - 기존 fieldName 값을 그대로 두고 자유 입력만 노출
      return { ...r, mode };
    }));
  }
  function setAddrSub(i, subKey) {
    setRows((prev) => prev.map((r, idx) => {
      if (idx !== i) return r;
      const sub = ADDR_SUBS.find((a) => a.key === subKey) || ADDR_SUBS[1];
      const fieldName = subKey === 'URL파라미터' ? `주소.파라미터.${r.paramKey}` : sub.field;
      return { ...r, addrSub: subKey, fieldName };
    }));
  }
  function setParamKey(i, key) {
    setRows((prev) => prev.map((r, idx) => (idx === i ? { ...r, paramKey: key, fieldName: `주소.파라미터.${key}` } : r)));
  }
  function removeRow(i) {
    setRows((prev) => prev.filter((_, idx) => idx !== i));
  }
  function addRow() {
    setRows((prev) => [...prev, {
      fieldName: '이벤트명', value: '', procedureName: procNames[0] || '',
      mode: 'raw', addrSub: '경로', paramKey: '',
    }]);
  }
  function handleSave() {
    onSave(saveEvents(restConfig, ns, rows));
  }

  return (
    <div className="subpane">
      <div className="panel">
        <div className="panel-title"><span className="ns-badge">{ns}</span>이벤트</div>
        <div className="panel-desc">
          {ns}.수신메시지.&lt;필드&gt; == &lt;값&gt; 형태로 매칭되면, 지정한 처리문으로 라우팅됩니다. (네임스페이스는 현재 선택된 연동에 자동으로 귀속됩니다)<br /><br />
          <b>메소드</b>는 <u>요청 분기용</u>입니다 — 외부가 우리 서버로 실제 요청을 보낼 때(웹훅 등) 그 요청의 메소드를 구분할 때만 쓰세요.
          우리가 보낸 요청의 <u>응답을 처리하는 분기</u>는 항상 <b>응답코드</b>를 쓰세요(성공/실패 HTTP 상태 코드로 구분 - 메소드는 우리가 보낸 값이 그대로 되돌아온 것이라 응답 쪽에서는 필터링 의미가 없습니다).
          <b>주소</b>는 도메인/경로/URL파라미터로 분기할 때, 그 외 JSON 필드(단순 키인 ok/이벤트명이든 result[0].message.text 같은 중첩 경로든 - 수신메시지는 어차피 전부 JSON이라 기술적으로 동일합니다)는 <b>필드명</b>에 경로를 직접 입력하세요.<br />
          접근자(.)를 통해 하위 계층 접근 가능 합니다. 예&gt; result[0].message.text
        </div>
        {rows.map((r, i) => (
          <div className="branch-row-block" key={i}>
            <div className="branch-mode-row">
              {[
                { key: 'raw', label: '필드명' },
                { key: 'method', label: '메소드' },
                { key: 'addr', label: '주소' },
                { key: 'rspcode', label: '응답코드' },
              ].map((m) => (
                <label key={m.key} className="mode-radio">
                  <input
                    type="radio"
                    name={`mode-${i}`}
                    checked={r.mode === m.key}
                    onChange={() => setMode(i, m.key)}
                  />
                  <span style={{ marginLeft: 4 }}>{m.label}</span>
                </label>
              ))}
            </div>
            <div className="branch-row">
              {r.mode === 'raw' && (
                <input
                  value={r.fieldName}
                  onChange={(e) => update(i, 'fieldName', e.target.value)}
                  placeholder="예: 이벤트명, ok, result[0].message.text"
                  style={{ width: 240 }}
                />
              )}
              {r.mode === 'addr' && (
                <div style={{ display: 'flex', gap: 6, alignItems: 'center' }}>
                  <select style={{ width: 130 }} value={r.addrSub} onChange={(e) => setAddrSub(i, e.target.value)}>
                    {ADDR_SUBS.map((a) => <option key={a.key} value={a.key}>{a.key}</option>)}
                  </select>
                  {r.addrSub === 'URL파라미터' && (
                    <input
                      value={r.paramKey}
                      onChange={(e) => setParamKey(i, e.target.value)}
                      placeholder="파라미터 이름(예: chat_id)"
                      style={{ width: 130 }}
                    />
                  )}
                </div>
              )}
              <span className="op">==</span>
              <span className="mini-label">값</span>
              {r.mode === 'method' ? (
                <div style={{ display: 'flex', gap: 6, alignItems: 'center' }}>
                  <select
                    style={{ width: 140 }}
                    value={HTTP_METHODS.includes(r.value) ? r.value : '기타'}
                    onChange={(e) => update(i, 'value', e.target.value === '기타' ? '' : e.target.value)}
                  >
                    {HTTP_METHODS.map((m) => <option key={m} value={m}>{m}</option>)}
                    <option value="기타">기타(직접 입력)</option>
                  </select>
                  {!HTTP_METHODS.includes(r.value) && (
                    <input
                      value={r.value}
                      onChange={(e) => update(i, 'value', e.target.value)}
                      placeholder="예: PATCH"
                      style={{ width: 100 }}
                    />
                  )}
                </div>
              ) : (
                <input value={r.value} onChange={(e) => update(i, 'value', e.target.value)} style={{ width: 140 }} />
              )}
              <span className="arrow">→</span>
              <span className="mini-label">처리문</span>
              <select style={{ minWidth: 190 }} value={r.procedureName} onChange={(e) => update(i, 'procedureName', e.target.value)}>
                {!procNames.includes(r.procedureName) && r.procedureName && (
                  <option value={r.procedureName}>{r.procedureName}</option>
                )}
                {procNames.map((p) => <option key={p} value={p}>{p}</option>)}
              </select>
              <button className="kv-remove" onClick={() => removeRow(i)}>×</button>
            </div>
            {r.mode === 'addr' && r.addrSub === 'URL파라미터' && (
              <div className="panel-desc" style={{ marginTop: -6, marginBottom: 10, fontSize: 11 }}>
                ⓘ 저장되는 필드 경로: <code>주소.파라미터.{r.paramKey || '?'}</code> — URL 파라미터를 이름으로 찾아 값을 비교하는
                경로가 실제 엔진에서 이 형태로 동작하는지 아직 실측 검증 전입니다. 저장 후 트레이스로 꼭 확인해주세요.
              </div>
            )}
          </div>
        ))}
        {rows.length === 0 && <div className="empty-hint">등록된 이벤트 분기가 없습니다.</div>}
        <div className="row-actions">
          <button className="btn btn-ghost" onClick={addRow}>＋ 분기 추가</button>
          <button className="btn btn-primary" style={{ marginLeft: 'auto' }} onClick={handleSave} disabled={saving}>
            {saving ? '저장 중…' : '저장'}
          </button>
        </div>
      </div>
    </div>
  );
}
