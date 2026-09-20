import { useEffect, useState } from 'react';
import { listProcedures, decodeProcForEdit, saveProcedure, addProcedure } from '../restModel.js';
import { parseCondition, buildCondition, OPS } from '../conditionModel.js';

// rest.sce가 실제로 쓰는 수행문 종류들(단어분리/단어합치기/붙이기/더하기/
// 빼기/객체저장/로그.출력 포함). "로그.출력"만 exe-type이 "함수"가 아니라
// "로그"다(LOGPARSER.h의 DEF_DSL_K_LOG_kor="로그") - 저장된 실제 값은
// {"exe-type":"로그","exe-name":"출력(ssh 응답)"} 모양이라 함수.저장과
// 완전히 같은 {exe-type,exe-name} 틀을 공유한다 - 값만 다를 뿐 별도 처리
// 분기가 필요 없다.
const EXE_KINDS = [
  '함수.저장', '함수.주소저장', '함수.단어분리', '함수.단어합치기',
  '함수.붙이기', '함수.더하기', '함수.빼기', '함수.객체저장', '로그.출력',
  '전송', '타이머', '처리',
];

// 수행문 kind를 바꿀 때 인자 입력창에 채워줄 기본 예시(실제 rest.sce
// 사용례 기반 - 각 함수 실제 호출부에서 그대로 따옴).
const EXE_DEFAULT_ARGS = {
  '함수.저장': '변수명,상수값 또는 (수신메시지 json 파라미터 값 예: body.reason)',
  '함수.주소저장': '변수명,수신메시지.주소',
  '함수.단어분리': '리스트변수명,수신메시지.result[0].message.text',
  '함수.단어합치기': '대상변수명,세션.리스트.리스트변수명,시작단어인덱스',
  '함수.붙이기': '대상변수명,이어붙일값1,이어붙일값2',
  '함수.더하기': '대상변수명,수신메시지.result[0].update_id,1',
  '함수.빼기': '대상변수명,값1,값2',
  '함수.객체저장': '오브젝트변수명,수신메시지.결과',
  '로그.출력': '로그로 남길 문구 (쉼표가 아니라 공백으로 여러 값 구분)',
};

// 실제 rest.sce의 고정 디스패처: 상태::FLOW.초기 { FLOW.수신메시지.이벤트명==시작
// 처리.procRestInit }는 항상 고정이며, 이 컴포넌트는 그 안에서 호출되는
// 처리::FLOW.procRestInit 의 "내용"(호출 목록)만 다룬다.
const GLOBAL_NS = 'FLOW';
const GLOBAL_PROC = 'procRestInit';
// 전역 초기화 슬롯은 조건문 없이 항상 단일 "만약에(참)" 블록 하나로
// 직렬화된다(실측: 실제 rest.sce 원본도 이 형태).
const GLOBAL_CONDITION = '만약에(참)';

// exe-type "함수" 안에서 인식하는 함수명 ↔ 화면 kind 매핑.
const FUNC_NAME_TO_KIND = {
  '저장': '함수.저장',
  '주소저장': '함수.주소저장',
  '단어분리': '함수.단어분리',
  '단어합치기': '함수.단어합치기',
  '붙이기': '함수.붙이기',
  '더하기': '함수.더하기',
  '빼기': '함수.빼기',
  '객체저장': '함수.객체저장',
};
const KIND_TO_FUNC_NAME = Object.fromEntries(Object.entries(FUNC_NAME_TO_KIND).map(([k, v]) => [v, k]));

function toRow(exe) {
  if (exe['exe-type'] === '함수') {
    const name = exe['exe-name'] || '';
    const m = name.match(/^([^(]*)\(([\s\S]*)\)\s*$/);
    const fn = m ? m[1] : name;
    const args = m ? m[2] : '';
    if (FUNC_NAME_TO_KIND[fn]) return { kind: FUNC_NAME_TO_KIND[fn], args };
    return { kind: 'OTHER', otherExeType: '함수', otherRaw: name, displayLabel: `함수.${fn}` };
  }
  // "로그.출력"은 exe-type이 "함수"가 아니라 "로그"다(LOGPARSER.h -
  // exe-name은 여전히 "출력(...)" 모양).
  if (exe['exe-type'] === '로그') {
    const name = exe['exe-name'] || '';
    const m = name.match(/^([^(]*)\(([\s\S]*)\)\s*$/);
    const fn = m ? m[1] : name;
    const args = m ? m[2] : '';
    if (fn === '출력') return { kind: '로그.출력', args };
    return { kind: 'OTHER', otherExeType: '로그', otherRaw: name, displayLabel: `로그.${fn}` };
  }
  if (['전송', '타이머', '처리'].includes(exe['exe-type'])) {
    return { kind: exe['exe-type'], args: exe['exe-name'] || '' };
  }
  return { kind: 'OTHER', otherExeType: exe['exe-type'], otherRaw: exe['exe-name'] || '', displayLabel: exe['exe-type'] };
}
function toExe(row) {
  if (KIND_TO_FUNC_NAME[row.kind]) return { 'exe-type': '함수', 'exe-name': `${KIND_TO_FUNC_NAME[row.kind]}(${row.args || ''})` };
  if (row.kind === '로그.출력') return { 'exe-type': '로그', 'exe-name': `출력(${row.args || ''})` };
  if (['전송', '타이머', '처리'].includes(row.kind)) return { 'exe-type': row.kind, 'exe-name': row.args || '' };
  return { 'exe-type': row.otherExeType, 'exe-name': row.otherRaw };
}
function listKeyOf(kind) {
  if (kind === 'else') return 'else-list';
  if (kind === 'elseif') return 'elseif-list';
  return 'if-list';
}
function blockToUi(block) {
  const cond = parseCondition(block.condition);
  const key = listKeyOf(cond.kind);
  const rows = (block[key] || []).map(toRow);
  return { cond, rows };
}
function uiToBlock(ui) {
  const key = listKeyOf(ui.cond.kind);
  return { condition: buildCondition(ui.cond), [key]: ui.rows.map(toExe) };
}
// 전역 초기화 슬롯 전용: 어떤 kind든 상관없이 화면에 보여줄 라벨/값을
// 뽑아낸다(기존 데이터에 함수.날짜/로그.출력처럼 5종 밖의 항목이 섞여
// 있어도 원본 그대로 표시하기 위함 - toExe()를 그대로 재사용).
function describeRow(row) {
  const exe = toExe(row);
  return { label: exe['exe-type'], value: exe['exe-name'] };
}

export default function ProcTab({ restConfig, ns, onSave, saving }) {
  const procs = listProcedures(restConfig, ns);
  const [selectedName, setSelectedName] = useState(procs[0]?.name || '');
  const [blocks, setBlocks] = useState([]);
  const [newName, setNewName] = useState('');
  // "🌐 전역 초기화" 고정 슬롯 - 현재 선택된 연동(ns)과 무관하게 항상
  // FLOW.procRestInit 하나만 다룬다.
  const [globalMode, setGlobalMode] = useState(false);
  const [globalRows, setGlobalRows] = useState([]);

  useEffect(() => {
    const list = listProcedures(restConfig, ns);
    if (!list.find((p) => p.name === selectedName)) {
      setSelectedName(list[0]?.name || '');
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [restConfig, ns]);

  useEffect(() => {
    const found = procs.find((p) => p.name === selectedName);
    if (!found) { setBlocks([]); return; }
    const decoded = decodeProcForEdit(found.proc);
    setBlocks((decoded['exe-list'] || []).map(blockToUi));
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [selectedName, restConfig, ns]);

  // restConfig.procedure-list 전체(연동 필터 없이)에서 FLOW.procRestInit을
  // 직접 찾는다 - 이 슬롯은 사이드바에서 어떤 연동을 선택했든 항상 같은
  // 대상을 편집해야 하기 때문에 ns로 필터링된 procs를 쓰지 않는다.
  useEffect(() => {
    const found = (restConfig['procedure-list'] || [])
      .find((p) => p['name-space'] === GLOBAL_NS && p['procedure-name'] === GLOBAL_PROC);
    if (!found) { setGlobalRows([]); return; }
    const decoded = decodeProcForEdit(found);
    const block = (decoded['exe-list'] || [])[0];
    setGlobalRows((block?.['if-list'] || []).map(toRow));
  }, [restConfig]);

  // 처리객체명 선택 드롭다운 후보: 전역 초기화 슬롯 자기 자신을 뺀
  // 등록된 모든 처리(procedure-list) 전체.
  const procCandidates = (restConfig['procedure-list'] || [])
    .filter((p) => !(p['name-space'] === GLOBAL_NS && p['procedure-name'] === GLOBAL_PROC))
    .map((p) => ({ ns: p['name-space'], name: p['procedure-name'] }));
  // 전송/타이머 오브젝트명 선택 드롭다운 후보 - action-list 전체에서 해당
  // action-type만 추려낸다(네임스페이스 무관, 처리객체명과 동일한 방식).
  const sendCandidates = (restConfig['action-list'] || [])
    .filter((a) => a['action-type'] === '전송')
    .map((a) => ({ ns: a['name-space'], name: a['action-name'] }));
  const timerCandidates = (restConfig['action-list'] || [])
    .filter((a) => a['action-type'] === '타이머')
    .map((a) => ({ ns: a['name-space'], name: a['action-name'] }));
  function candidatesForKind(kind) {
    if (kind === '전송') return sendCandidates;
    if (kind === '타이머') return timerCandidates;
    if (kind === '처리') return procCandidates;
    return [];
  }

  function updateBlockCond(bi, field, value) {
    setBlocks((prev) => prev.map((b, i) => (i === bi ? { ...b, cond: { ...b.cond, [field]: value } } : b)));
  }
  function addExeRow(bi) {
    setBlocks((prev) => prev.map((b, i) => (i === bi ? { ...b, rows: [...b.rows, { kind: '전송', args: '' }] } : b)));
  }
  function updateExeRow(bi, ri, field, value) {
    setBlocks((prev) => prev.map((b, i) => {
      if (i !== bi) return b;
      const rows = b.rows.map((r, j) => (j === ri ? { ...r, [field]: value } : r));
      return { ...b, rows };
    }));
  }
  // kind를 바꿀 때: 함수.저장/함수.주소저장으로 바꾸면 인자란에 기본
  // 예시를 채워주고, 그 외(전송/타이머/처리)로 바꾸면 오브젝트명 콤보박스가
  // 빈 선택 상태로 시작하도록 인자값을 비운다(이전 kind의 값이 그대로
  // 남아있으면 새 kind 기준으로는 무의미하기 때문).
  function updateExeRowKind(bi, ri, newKind) {
    setBlocks((prev) => prev.map((b, i) => {
      if (i !== bi) return b;
      const rows = b.rows.map((r, j) => (j === ri ? { ...r, kind: newKind, args: EXE_DEFAULT_ARGS[newKind] || '' } : r));
      return { ...b, rows };
    }));
  }
  function removeExeRow(bi, ri) {
    setBlocks((prev) => prev.map((b, i) => (i === bi ? { ...b, rows: b.rows.filter((_, j) => j !== ri) } : b)));
  }
  function addElseif() {
    setBlocks((prev) => {
      const hasElse = prev.some((b) => b.cond.kind === 'else');
      const newBlock = { cond: { kind: 'elseif', lhs: '', opLabel: '완전일치', rhs: '' }, rows: [] };
      if (hasElse) {
        const out = [...prev];
        out.splice(out.length - 1, 0, newBlock);
        return out;
      }
      return [...prev, newBlock];
    });
  }
  function handleSaveProc() {
    if (!selectedName) return;
    const proc = { 'procedure-name': selectedName, 'name-space': ns, 'exe-list': blocks.map(uiToBlock) };
    onSave(saveProcedure(restConfig, proc));
  }
  function handleAddProc() {
    const name = newName.trim();
    if (!name) return;
    setNewName('');
    onSave(addProcedure(restConfig, ns, name));
    setSelectedName(name);
  }

  // ── 전역 초기화(FLOW.procRestInit) 전용 핸들러 ──────────────────────────
  // 새로 추가하는 수행문은 "처리" 한 종류만 허용.
  function addGlobalRow() {
    setGlobalRows((prev) => [...prev, { kind: '처리', args: '' }]);
  }
  function updateGlobalRow(ri, field, value) {
    setGlobalRows((prev) => prev.map((r, j) => (j === ri ? { ...r, [field]: value } : r)));
  }
  function removeGlobalRow(ri) {
    setGlobalRows((prev) => prev.filter((_, j) => j !== ri));
  }
  function handleSaveGlobal() {
    // if/else 구조 없이, 항상 단일 "만약에(참)" 블록 하나에 수행문만
    // 순서대로 담아 직렬화한다(조건문을 새로 만들지 않음).
    const proc = {
      'procedure-name': GLOBAL_PROC,
      'name-space': GLOBAL_NS,
      'exe-list': [
        { condition: GLOBAL_CONDITION, 'if-list': globalRows.map(toExe) },
      ],
    };
    onSave(saveProcedure(restConfig, proc));
  }

  return (
    <div className="subpane">
      <div className="obj-layout">
        <div className="obj-list">
          {/* 연동 선택(ns)과 무관하게 항상 맨 위에 고정 표시되는 전역 초기화 슬롯. */}
          <button
            className={`obj-item${globalMode ? ' active' : ''}`}
            style={{ fontWeight: 700, borderBottom: '1px dashed var(--border)', borderRadius: '7px 7px 0 0', marginBottom: 4 }}
            onClick={() => setGlobalMode(true)}
          >🌐 전역 초기화</button>
          <div className="obj-list-title">처리 ({procs.length})</div>
          {procs.map((p) => (
            <button
              key={p.name}
              className={`obj-item${!globalMode && p.name === selectedName ? ' active' : ''}`}
              onClick={() => { setGlobalMode(false); setSelectedName(p.name); }}
            >{p.name}</button>
          ))}
          <div className="kv-row" style={{ marginTop: 6 }}>
            <input placeholder="새 처리 이름" value={newName} onChange={(e) => setNewName(e.target.value)} style={{ fontSize: 12 }} />
          </div>
          <button className="obj-add" onClick={handleAddProc}>＋ 새 처리</button>
        </div>
        <div className="obj-editor">
          {globalMode && (
            <>
              <div className="panel" style={{ padding: '4px 4px 12px 4px', border: 'none', background: 'transparent', margin: 0 }}>
                <div className="panel-desc" style={{ marginBottom: 0 }}>
                  모든 연동에 앞서 한 번 수행되는 전역 초기화 디스패처입니다.<br />
                  조건문 없이, 등록된 각 연동의 초기화 처리문을 순서대로 호출합니다.
                </div>
              </div>
              <div className="proc-card">
                <div className="proc-head">
                  <span className="ns-badge">{GLOBAL_NS}</span>
                  <span style={{ fontSize: 12, color: '#6b7086' }}>처리 이름</span>
                  <input value={GLOBAL_PROC} readOnly />
                </div>
                <div className="cond-block">
                  {globalRows.map((r, ri) => (
                    <div className="stmt-row" key={ri}>
                      {r.kind === '처리' ? (
                        <>
                          <select value="처리" onChange={() => {}}>
                            <option value="처리">처리</option>
                          </select>
                          <select
                            value={r.args}
                            onChange={(e) => updateGlobalRow(ri, 'args', e.target.value)}
                          >
                            <option value="">(처리객체명 선택)</option>
                            {r.args && !procCandidates.some((c) => c.name === r.args) && (
                              <option value={r.args}>{r.args} (알 수 없음)</option>
                            )}
                            {procCandidates.map((c) => (
                              <option key={`${c.ns}.${c.name}`} value={c.name}>{c.ns}.{c.name}</option>
                            ))}
                          </select>
                        </>
                      ) : (
                        <>
                          <span
                            className="ns-badge"
                            title="전역 초기화 슬롯에서는 새 수행문으로 '처리'만 추가할 수 있습니다. 이 항목은 기존 데이터로 원본 그대로 보존되어 표시됩니다(읽기전용)."
                          >{describeRow(r).label}</span>
                          <input value={describeRow(r).value} readOnly />
                        </>
                      )}
                      <span className="stmt-remove" onClick={() => removeGlobalRow(ri)}>×</span>
                    </div>
                  ))}
                  <button className="add-stmt" onClick={addGlobalRow}>＋ 수행문 추가</button>
                </div>
              </div>
              <div className="row-actions">
                <button className="btn btn-primary" style={{ marginLeft: 'auto' }} onClick={handleSaveGlobal} disabled={saving}>
                  {saving ? '저장 중…' : '저장'}
                </button>
              </div>
            </>
          )}
          {!globalMode && !selectedName && <div className="empty-hint">좌측에서 처리를 선택하거나 새로 추가하세요.</div>}
          {!globalMode && selectedName && (
            <>
              <div className="panel" style={{ padding: '4px 4px 12px 4px', border: 'none', background: 'transparent', margin: 0 }}>
                <div className="panel-desc" style={{ marginBottom: 0 }}>
                  이벤트 분기에 의해 호출 되는 처리함수 입니다.<br />
                  조건문이 충족되면 하위 수행문들이 순차적으로 수행 됩니다.
                </div>
              </div>
              <div className="proc-card">
                <div className="proc-head">
                  <span className="ns-badge">{ns}</span>
                  <span style={{ fontSize: 12, color: '#6b7086' }}>처리 이름</span>
                  <input value={selectedName} readOnly />
                </div>
                {blocks.map((b, bi) => (
                  <div className="cond-block" key={bi}>
                    <div className={`cond-tag ${b.cond.kind === 'if' ? 'if' : 'else'}`}>
                      {b.cond.kind === 'if' ? 'if' : b.cond.kind === 'elseif' ? 'else if' : 'else (조건 없음)'}
                    </div>
                    {b.cond.kind !== 'else' && (() => {
                      const isConst = b.cond.opLabel === '참' || b.cond.opLabel === '거짓';
                      return (
                      <div className="cond-line">
                        <input
                          value={isConst ? '' : b.cond.lhs}
                          onChange={(e) => updateBlockCond(bi, 'lhs', e.target.value)}
                          readOnly={isConst}
                          style={{ width: 290, ...(isConst ? { background: '#f0f0f3', color: '#9a9fae' } : {}) }}
                          placeholder="좌항 (예: 수신메시지.ok)"
                        />
                        <select value={b.cond.opLabel} onChange={(e) => updateBlockCond(bi, 'opLabel', e.target.value)}>
                          {OPS.map((o) => <option key={o.label} value={o.label}>{o.label}</option>)}
                        </select>
                        <input
                          value={isConst ? '' : b.cond.rhs}
                          onChange={(e) => updateBlockCond(bi, 'rhs', e.target.value)}
                          readOnly={isConst}
                          style={{ width: 200, ...(isConst ? { background: '#f0f0f3', color: '#9a9fae' } : {}) }}
                          placeholder="우항"
                        />
                      </div>
                      );
                    })()}
                    {b.rows.map((r, ri) => (
                      <div className="stmt-row" key={ri}>
                        {r.kind === 'OTHER' ? (
                          <>
                            <span className="ns-badge" title="기존 데이터의 원래 타입 - 새로 추가할 때는 목록에 있는 종류만 선택 가능">{r.displayLabel}</span>
                            <input
                              value={r.otherRaw}
                              onChange={(e) => updateExeRow(bi, ri, 'otherRaw', e.target.value)}
                            />
                          </>
                        ) : (
                          <>
                            <select value={r.kind} onChange={(e) => updateExeRowKind(bi, ri, e.target.value)}>
                              {EXE_KINDS.map((k) => <option key={k} value={k}>{k}</option>)}
                            </select>
                            {(r.kind.startsWith('함수') || r.kind === '로그.출력') ? (
                              <input
                                value={r.args}
                                onChange={(e) => updateExeRow(bi, ri, 'args', e.target.value)}
                                placeholder={`인자 (예: ${EXE_DEFAULT_ARGS[r.kind] || ''})`}
                              />
                            ) : (
                              <select value={r.args} onChange={(e) => updateExeRow(bi, ri, 'args', e.target.value)}>
                                <option value="">({r.kind} 오브젝트 선택)</option>
                                {r.args && !candidatesForKind(r.kind).some((c) => c.name === r.args) && (
                                  <option value={r.args}>{r.args} (알 수 없음)</option>
                                )}
                                {candidatesForKind(r.kind).map((c) => (
                                  <option key={`${c.ns}.${c.name}`} value={c.name}>{c.ns}.{c.name}</option>
                                ))}
                              </select>
                            )}
                          </>
                        )}
                        <span className="stmt-remove" onClick={() => removeExeRow(bi, ri)}>×</span>
                      </div>
                    ))}
                    <button className="add-stmt" onClick={() => addExeRow(bi)}>＋ 수행문 추가</button>
                  </div>
                ))}
                <button className="add-elseif" onClick={addElseif}>＋ else if 조건 추가</button>
              </div>
              <div className="row-actions">
                <button className="btn btn-primary" style={{ marginLeft: 'auto' }} onClick={handleSaveProc} disabled={saving}>
                  {saving ? '저장 중…' : '저장'}
                </button>
              </div>
            </>
          )}
        </div>
      </div>
    </div>
  );
}
