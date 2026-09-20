import { useEffect, useState } from 'react';
import { listActions, decodeSendAction, saveAction, addAction, listStmts } from '../restModel.js';
import { decodeSlash } from '../api.js';

// ── 실시간 미리보기: 참조값 치환 ─────────────────────────────────────────
// 실제 addrConfig(계정설정 탭 상태)/restConfig(문장 탭 상태)에서 참조값을
// 찾아 화면에 보여줄 문자열로 치환한다.
//   "설정.NAMESPACE.키"     -> addrConfig[NAMESPACE]에서 key 일치 항목의 값
//   "문장.이름"             -> restConfig의 stmt-list에서 이름 일치 항목의 데이터.
//                             그 안의 $$$세션.xxx$$$/$$$수신메시지.xxx$$$는
//                             다시 <세션.xxx> 형태로 치환
//   "수신메시지.xxx"/"세션.xxx" -> <그대로> 꺾쇠로 감싸서 강조(placeholder)
//   그 외                   -> 리터럴 그대로
function resolveRef(raw, { addrConfig, restConfig, ns }) {
  raw = (raw || '').trim();
  if (!raw) return { text: '', isPlaceholder: false };
  if (raw.startsWith('설정.')) {
    const m = raw.match(/^설정\.([^.]+)\.(.+)$/);
    if (m) {
      const [, refNs, key] = m;
      const found = (addrConfig?.[refNs] || []).find((it) => it.key === key);
      if (found) return { text: decodeSlash(found.val), isPlaceholder: false };
    }
    return { text: raw, isPlaceholder: false };
  }
  if (raw.startsWith('문장.')) {
    const name = raw.slice('문장.'.length);
    const found = listStmts(restConfig, ns).find((s) => s.name === name);
    let tpl = found ? found.data : raw;
    // $$$<$$$/$$$>$$$는 세션/수신메시지 참조가 아니라 "{"/"}" 리터럴
    // 이스케이프다(문장::이름 { ... } 블록 문법 기호와 충돌 피하려는 용도 -
    // 도움말 탭 2번 섹션 참고). 아래 일반 "$$$...$$$ -> <...>" 치환보다
    // 먼저 이 둘을 진짜 중괄호로 바꿔야 "<<>"/"<>>" 같은 깨진 문자로
    // 잘못 표시되지 않는다.
    tpl = tpl.replace(/\$\$\$<\$\$\$/g, '{').replace(/\$\$\$>\$\$\$/g, '}');
    // 나머지(세션.xxx/수신메시지.xxx 등 실제 참조)만 <...>로 강조 치환.
    tpl = tpl.replace(/\$\$\$(.*?)\$\$\$/g, (m, inner) => `<${inner}>`);
    return { text: tpl, isPlaceholder: tpl.includes('<') };
  }
  if (raw.startsWith('수신메시지.') || raw.startsWith('세션.')) {
    return { text: `<${raw}>`, isPlaceholder: true };
  }
  return { text: raw, isPlaceholder: false };
}

// resolveRef가 만든 <...> placeholder 구간만 pv-ph로 강조하는 React 노드 배열로 변환.
function splitPlaceholders(text) {
  const nodes = [];
  const re = /<([^<>]*)>/g;
  let last = 0, m, i = 0;
  while ((m = re.exec(text))) {
    if (m.index > last) nodes.push(text.slice(last, m.index));
    nodes.push(<span className="pv-ph" key={`ph-${i++}`}>{`<${m[1]}>`}</span>);
    last = m.index + m[0].length;
  }
  nodes.push(text.slice(last));
  return nodes;
}

export default function SendTab({ restConfig, ns, onSave, saving, addrConfig }) {
  const actions = listActions(restConfig, ns, '전송');
  const [selectedName, setSelectedName] = useState(actions[0]?.name || '');
  const [ui, setUi] = useState(null);
  const [newName, setNewName] = useState('');

  useEffect(() => {
    const list = listActions(restConfig, ns, '전송');
    if (!list.find((a) => a.name === selectedName)) setSelectedName(list[0]?.name || '');
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [restConfig, ns]);

  useEffect(() => {
    const found = actions.find((a) => a.name === selectedName);
    setUi(found ? decodeSendAction(found.action) : null);
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [selectedName, restConfig, ns]);

  if (ui === null && selectedName) return null;

  function setField(field, value) {
    setUi((prev) => ({ ...prev, [field]: value }));
  }
  function setMethod(value) {
    setUi((prev) => ({ ...prev, method: value === 'ECHO' ? prev.method : value, isEcho: value === 'ECHO' }));
  }
  function updateListItem(listName, i, field, value) {
    setUi((prev) => {
      const list = prev[listName].map((it, idx) => (idx === i ? { ...it, [field]: value } : it));
      return { ...prev, [listName]: list };
    });
  }
  function removeListItem(listName, i) {
    setUi((prev) => ({ ...prev, [listName]: prev[listName].filter((_, idx) => idx !== i) }));
  }
  function addParam() { setUi((prev) => ({ ...prev, params: [...prev.params, { key: '', val: '' }] })); }
  function addHeader() { setUi((prev) => ({ ...prev, headers: [...prev.headers, { key: '', val: '' }] })); }
  function addBody() { setUi((prev) => ({ ...prev, body: [...prev.body, { field: '', val: '' }] })); }

  // ui/restConfig/ns/addrConfig 중 무엇이 바뀌어도 렌더링 시점에 그대로
  // 다시 계산되는 순수 파생값이라, 디바운스나 별도 useEffect 없이 매
  // 입력마다(키 입력 하나하나) 자연스럽게 최신 상태로 갱신된다.
  function buildPreview() {
    const refCtx = { addrConfig, restConfig, ns };
    const resolved = (raw) => resolveRef(raw, refCtx).text;
    const resolvedNodes = (raw) => splitPlaceholders(resolveRef(raw, refCtx).text);

    if (ui.isEcho) {
      const bodyObj = {};
      // 목업과 동일: "응답" 모드의 바디는 참조 치환을 거치지 않고 원문 그대로 표시한다.
      ui.body.forEach((b) => { if (b.field.trim()) bodyObj[b.field] = b.val; });
      const bodyText = Object.keys(bodyObj).length ? JSON.stringify(bodyObj, null, 2) : '(바디 없음)';
      return {
        title: '응답 미리보기',
        node: (
          <>
            <span className="pv-status">HTTP/1.1 200 OK</span>{'\n'}
            <span className="pv-dim">(전체 응답 헤더는 알 수 없음 — 우리가 설정한 바디만 표시)</span>{'\n\n'}
            {bodyText}
          </>
        ),
      };
    }

    const method = ui.method || 'GET';
    const pathResolved = resolved(ui.path);
    const host = resolved(ui.domain).replace(/^https?:\/\//, '');
    const queryStr = ui.params.filter((p) => p.key.trim()).map((p) => `${p.key}=${resolved(p.val)}`).join('&');
    const pathLine = pathResolved + (queryStr ? '?' + queryStr : '');
    const headerRows = ui.headers.filter((h) => h.key.trim());

    // "문서.*"는 "메소드"/"주소"/"헤더"와 같은 성격의 통신계층 전용
    // 메타(Main.cpp가 소비하고 실제로는 JSON 바디에 안 남는 값)라, 이미
    // 위에서 저 셋을 URL줄/Host/헤더줄로 따로 그리는 것과 동일하게 일반
    // JSON 바디에 섞지 않고 실제로 나가는 진짜 모양(multipart/form-data)
    // 으로 따로 그린다. Main.cpp 쪽 실제 구현(문서 키가 하나라도 있으면
    // 그 전송::는 통째로 멀티파트가 되고, 그 외 JSON 바디 필드는 만들어지지
    // 않음)과 동일하게, "문서.*"가 하나라도 있으면 나머지 body 필드는
    // 무시한다(둘을 섞어 쓰는 시나리오 자체가 실제로 지원 안 됨 - 혼동
    // 방지 차원에서 미리보기도 그대로 반영).
    const docFieldRows = ui.body.filter((b) => b.field.trim().startsWith('문서.'));
    const otherFieldRows = ui.body.filter((b) => b.field.trim() && !b.field.trim().startsWith('문서.'));

    let bodySection = null;
    let contentTypeLine = null;
    if (docFieldRows.length > 0) {
      const doc = {};
      docFieldRows.forEach((b) => { doc[b.field.trim().slice('문서.'.length)] = b.val; });
      const boundary = '----NotebookFlowBoundary';
      const fieldName = doc.필드명 ? resolved(doc.필드명) : '(필드명 미지정)';
      const fileName = doc.파일명 ? resolved(doc.파일명) : '';
      const partContentType = doc.타입 ? resolved(doc.타입) : 'application/octet-stream';
      contentTypeLine = `multipart/form-data; boundary=${boundary}`;
      bodySection = (
        <>
          {`--${boundary}`}{'\n'}
          <span className="pv-hdrkey">Content-Disposition</span>: form-data; name="{fieldName}"{fileName && <>; filename="{resolvedNodes(doc.파일명)}"</>}{'\n'}
          <span className="pv-hdrkey">Content-Type</span>: {partContentType}{'\n\n'}
          {doc.내용 != null && resolvedNodes(doc.내용)}
          {doc.내용 == null && doc.경로 != null && <>파일 내용: {resolvedNodes(doc.경로)}</>}
          {'\n'}{`--${boundary}`}
          {doc.캡션 != null && (
            <>
              {'\n'}
              <span className="pv-hdrkey">Content-Disposition</span>: form-data; name="caption"{'\n\n'}
              {resolvedNodes(doc.캡션)}
              {'\n'}{`--${boundary}`}
            </>
          )}
          {`--`}
        </>
      );
    } else {
      const bodyObj = {};
      otherFieldRows.forEach((b) => { bodyObj[b.field] = resolved(b.val); });
      const bodyText = Object.keys(bodyObj).length ? JSON.stringify(bodyObj, null, 2) : '';
      bodySection = bodyText || null;
    }

    return {
      title: '요청 미리보기',
      node: (
        <>
          <span className="pv-method">{method}</span> <span className="pv-path">{splitPlaceholders(pathLine)}</span> HTTP/1.1{'\n'}
          <span className="pv-hdrkey">Host</span>: <span className="pv-host">{host}</span>
          {contentTypeLine && <span>{'\n'}<span className="pv-hdrkey">Content-Type</span>: {contentTypeLine}</span>}
          {headerRows.map((h, i) => (
            <span key={i}>{'\n'}<span className="pv-hdrkey">{h.key}</span>: {resolvedNodes(h.val)}</span>
          ))}
          {bodySection && <>{'\n\n'}{bodySection}</>}
        </>
      ),
    };
  }

  function handleSave() {
    if (!selectedName || !ui) return;
    onSave(saveAction(restConfig, ns, selectedName, '전송', ui));
  }
  function handleAdd() {
    const name = newName.trim();
    if (!name) return;
    setNewName('');
    onSave(addAction(restConfig, ns, name, '전송'));
    setSelectedName(name);
  }

  return (
    <div className="subpane">
      <div className="obj-layout">
        <div className="obj-list">
          <div className="obj-list-title">전송 ({actions.length})</div>
          {actions.map((a) => (
            <button key={a.name} className={`obj-item${a.name === selectedName ? ' active' : ''}`} onClick={() => setSelectedName(a.name)}>
              {a.name}
            </button>
          ))}
          <div className="kv-row" style={{ marginTop: 6 }}>
            <input placeholder="새 전송 이름" value={newName} onChange={(e) => setNewName(e.target.value)} style={{ fontSize: 12 }} />
          </div>
          <button className="obj-add" onClick={handleAdd}>＋ 새 전송</button>
        </div>
        <div className="obj-editor">
          {!ui && <div className="empty-hint">좌측에서 전송을 선택하거나 새로 추가하세요.</div>}
          {ui && (
            <div className="panel">
              <div className="panel-title"><span className="ns-badge">{ns}</span>전송::{selectedName}</div>
              <div className="panel-desc">
                HTTP 요청/응답을 전송 하는 설정 입니다. "응답"을 선택하면 새 요청을 걸지 않고 수신메시지.주소를 그대로 돌려주며, 이 경우 저장되는 JSON에는 "메소드" 필드 자체가 생성되지 않습니다(화면 표시 전용 구분입니다).
              </div>

              {(() => {
                const preview = buildPreview();
                return (
                  <>
                    <div className="preview-header">
                      <div className="send-section-title" style={{ margin: 0, padding: 0, border: 'none' }}>{preview.title}</div>
                      <div className="preview-live-badge"><span className="pulse-dot" />실시간 반영</div>
                    </div>
                    <div className="panel-desc" style={{ marginTop: 4, marginBottom: 0 }}>
                      실제로 나가는(또는 돌아가는) 패킷 모양입니다.{' '}
                      <span className="pv-ph" style={{ background: '#fff3ee', padding: '1px 5px', borderRadius: 4 }}>주황색</span>
                      은 실행 시점에 채워지는 값(세션·수신메시지)이라 지금은 알 수 없다는 뜻입니다.
                    </div>
                    <div className="preview-box">{preview.node}</div>
                  </>
                );
              })()}

              <div className="kv-row">
                <input value="메소드" disabled style={{ color: '#a3a7b8', flex: '4 4 0%' }} />
                <select
                  style={{ flex: '6 6 0%', padding: '9px 11px', border: '1px solid var(--border)', borderRadius: 8, fontSize: 13 }}
                  value={ui.isEcho ? 'ECHO' : (ui.method || 'GET')}
                  onChange={(e) => setMethod(e.target.value)}
                >
                  <option value="GET">GET</option>
                  <option value="POST">POST</option>
                  <option value="ECHO">응답</option>
                </select>
              </div>

              {!ui.isEcho && (
                <div>
                  <div className="send-section-title">주소</div>
                  <div className="kv-row">
                    <input value="도메인" disabled style={{ color: '#a3a7b8' }} />
                    <input value={ui.domain} onChange={(e) => setField('domain', e.target.value)} />
                  </div>
                  <div className="kv-row">
                    <input value="경로" disabled style={{ color: '#a3a7b8' }} />
                    <input value={ui.path} onChange={(e) => setField('path', e.target.value)} />
                  </div>

                  <div className="send-subgroup-label">URL 쿼리 파라미터</div>
                  {ui.params.map((p, i) => (
                    <div className="param-row" key={i}>
                      <input value={p.key} onChange={(e) => updateListItem('params', i, 'key', e.target.value)} />
                      <input value={p.val} onChange={(e) => updateListItem('params', i, 'val', e.target.value)} />
                      <button className="kv-remove" onClick={() => removeListItem('params', i)}>×</button>
                    </div>
                  ))}
                  <div className="row-actions" style={{ marginTop: 0, marginBottom: 16 }}>
                    <button className="btn btn-ghost btn-sm" onClick={addParam}>＋ 파라미터 추가</button>
                  </div>
                </div>
              )}
              {ui.isEcho && (
                <div className="echo-notice">
                  ⓘ&nbsp;이 전송문은 새 요청을 걸지 않고, 원래 요청이 들어온 주소(수신메시지.주소)로 그대로 응답합니다. 도메인/경로/쿼리 파라미터를 별도로 지정할 필요가 없습니다.
                </div>
              )}

              <div className="send-section-title">헤더</div>
              <div className="panel-desc" style={{ marginBottom: 10 }}>외부 API가 요구하는 HTTP 헤더(인증 토큰 등)를 지정합니다. 헤더[N].key/val 형태로 저장되며, 주소.파라미터와 동일한 구조입니다.</div>
              {ui.headers.map((h, i) => (
                <div className="param-row" key={i}>
                  <input value={h.key} onChange={(e) => updateListItem('headers', i, 'key', e.target.value)} placeholder="예: Authorization" />
                  <input value={h.val} onChange={(e) => updateListItem('headers', i, 'val', e.target.value)} />
                  <button className="kv-remove" onClick={() => removeListItem('headers', i)}>×</button>
                </div>
              ))}
              {ui.headers.length === 0 && (
                <div className="param-row" style={{ opacity: .45 }}>
                  <input value="(헤더 없음)" disabled />
                  <input value="" disabled placeholder="예: Authorization" />
                  <button className="kv-remove" style={{ visibility: 'hidden' }}>×</button>
                </div>
              )}
              <div className="row-actions" style={{ marginTop: 0, marginBottom: 16 }}>
                <button className="btn btn-ghost btn-sm" onClick={addHeader}>＋ 헤더 추가</button>
              </div>

              <div className="send-section-title">바디 (JSON)</div>
              <div className="panel-desc" style={{ marginBottom: 10 }}>전송 내용을 이루는 필드입니다. 필드명에 점(.)을 포함해 중첩 경로도 지정할 수 있습니다(예: record.text). POST/응답 모두에서 사용 가능합니다.</div>
              {ui.body.map((b, i) => (
                <div className="param-row" key={i}>
                  <input value={b.field} onChange={(e) => updateListItem('body', i, 'field', e.target.value)} placeholder="예: record.text" />
                  <input value={b.val} onChange={(e) => updateListItem('body', i, 'val', e.target.value)} />
                  <button className="kv-remove" onClick={() => removeListItem('body', i)}>×</button>
                </div>
              ))}
              {ui.body.length === 0 && (
                <div className="param-row" style={{ opacity: .45 }}>
                  <input value="(바디 없음)" disabled />
                  <input value="" disabled placeholder="예: record.text" />
                  <button className="kv-remove" style={{ visibility: 'hidden' }}>×</button>
                </div>
              )}
              <div className="row-actions" style={{ marginTop: 0 }}>
                <button className="btn btn-ghost btn-sm" onClick={addBody}>＋ 바디 필드 추가</button>
              </div>

              <div className="row-actions">
                <button className="btn btn-primary" style={{ marginLeft: 'auto' }} onClick={handleSave} disabled={saving}>
                  {saving ? '저장 중…' : '저장'}
                </button>
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}
