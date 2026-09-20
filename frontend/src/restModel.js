// restConfig(GET /notebookflow/config/rest 전체 JSON)에서 "현재 선택된 연동
// (name-space)"에 해당하는 조각만 뽑아 쓰기 위한 헬퍼들. 저장할 때는 항상
// restConfig 전체를 복제해서 해당 조각만 갈아끼운 뒤 전체를 PUT한다
// (ExeScen::IMPORT가 매번 전체 리스트를 clear() 하기 때문 - api.js 상단
// 주석 참고).

import { decodeSlash, encodeSlash } from './api.js';

export function cloneRest(restConfig) {
  return JSON.parse(JSON.stringify(restConfig));
}

// ── 이벤트 (state-list[*].event-list) ───────────────────────────────────
export function listEvents(restConfig, ns) {
  const out = [];
  const states = restConfig['state-list'] || [];
  states.forEach((st, stateIdx) => {
    (st['event-list'] || []).forEach((ev, evIdx) => {
      if (ev['name-space'] === ns) {
        out.push({
          stateIdx,
          evIdx,
          fieldName: decodeSlash(ev['event-param']),
          value: decodeSlash(ev['event-name']),
          procedureName: ev['procedure-name'],
        });
      }
    });
  });
  return out;
}
export function saveEvents(restConfig, ns, rows) {
  const next = cloneRest(restConfig);
  const states = next['state-list'] || [];
  // 1) 이 ns에 속했던 기존 이벤트를 전부 제거(state-list[0] 기준 - 지금까지
  //    실제로 확인된 데이터에는 state가 "초기" 하나뿐이라 0번 state에
  //    몰아서 씀. state가 여러 개로 늘어나면 이 부분을 확장해야 한다).
  if (states.length === 0) states.push({ 'state-name-space': 'FLOW', 'state-name': '초기', 'event-list': [] });
  const target = states[0];
  target['event-list'] = (target['event-list'] || []).filter((ev) => ev['name-space'] !== ns);
  // 2) 편집된 rows를 다시 채워 넣는다.
  rows.forEach((r) => {
    target['event-list'].push({
      'name-space': ns,
      'event-param': encodeSlash(r.fieldName),
      'event-name': encodeSlash(r.value),
      'procedure-name': r.procedureName,
    });
  });
  return next;
}

// ── 처리 (procedure-list) ───────────────────────────────────────────────
export function listProcedures(restConfig, ns) {
  return (restConfig['procedure-list'] || [])
    .map((p, idx) => ({ idx, name: p['procedure-name'], ns: p['name-space'], proc: p }))
    .filter((p) => p.ns === ns);
}
export function decodeProcForEdit(proc) {
  // exe-list의 condition/exe-name을 화면 표시용으로 디코딩한 깊은 복사본을 만든다.
  const copy = JSON.parse(JSON.stringify(proc));
  (copy['exe-list'] || []).forEach((block) => {
    block.condition = decodeSlash(block.condition);
    ['if-list', 'elseif-list', 'else-list'].forEach((key) => {
      (block[key] || []).forEach((exe) => {
        exe['exe-name'] = decodeSlash(exe['exe-name']);
      });
    });
  });
  return copy;
}
export function saveProcedure(restConfig, editedProc) {
  // editedProc은 decodeProcForEdit()로 디코딩된 상태 - 다시 인코딩해서
  // procedure-list의 같은 procedure-name 항목을 교체한다.
  const next = cloneRest(restConfig);
  const encoded = JSON.parse(JSON.stringify(editedProc));
  (encoded['exe-list'] || []).forEach((block) => {
    block.condition = encodeSlash(block.condition);
    ['if-list', 'elseif-list', 'else-list'].forEach((key) => {
      (block[key] || []).forEach((exe) => {
        exe['exe-name'] = encodeSlash(exe['exe-name']);
      });
    });
  });
  const list = next['procedure-list'] || [];
  const idx = list.findIndex((p) => p['procedure-name'] === editedProc['procedure-name']);
  if (idx >= 0) list[idx] = encoded;
  else list.push(encoded);
  next['procedure-list'] = list;
  return next;
}
export function addProcedure(restConfig, ns, name) {
  const next = cloneRest(restConfig);
  const list = next['procedure-list'] || [];
  list.push({
    'procedure-name': name,
    'name-space': ns,
    'exe-list': [
      { condition: 'TRUE', 'if-list': [] },
    ],
  });
  next['procedure-list'] = list;
  return next;
}

// ── 전송 / 타이머 (action-list) ─────────────────────────────────────────
export function listActions(restConfig, ns, actionType) {
  return (restConfig['action-list'] || [])
    .map((a, idx) => ({ idx, name: a['action-name'], ns: a['name-space'], type: a['action-type'], action: a }))
    .filter((a) => a.ns === ns && a.type === actionType);
}

// 전송 오브젝트를 UI가 다루기 쉬운 구조로 분해한다:
// { method, domain, path, params:[{key,val}], headers:[{key,val}], body:[{field,val}] }
export function decodeSendAction(action) {
  const params = { method: '', domain: '', path: '', params: [], headers: [], body: [] };
  const paramList = action['param-list'] || [];
  const headerMap = new Map();
  const paramMap = new Map();
  paramList.forEach((p) => {
    const key = p['param-key'];
    const val = decodeSlash(p['param-val']);
    if (key === '전송메시지.메소드') params.method = val;
    else if (key === '전송메시지.주소.도메인') params.domain = val;
    else if (key === '전송메시지.주소.경로') params.path = val;
    // "전송메시지.주소"(하위 경로 없이 통째로) - "응답" 모드 전용 관용구로,
    // 원본 요청 주소 객체를 그대로 복사해 회신하는 데 쓴다(수신메시지.주소
    // 자체가 도메인/경로/파라미터를 가진 오브젝트라 실제로는 body에 들어갈
    // 문자열 값이 아님 - UI 바디 필드/미리보기에 "주소": "수신메시지.주소" 처럼
    // 문자열로 잘못 나타나는 걸 막기 위해 body에 넣지 않는다). 실측 확인:
    // rest.sce의 TELEGRAM/DISCORD/NTFY/SLACK/BSKY "~직접응답" 5개 전송::
    // 전부 이 줄을 동일하게 쓰고 있고, isEcho 모드에서만 등장한다 - 이미
    // 화면에 "수신메시지.주소로 그대로 응답합니다" 안내문이 있으므로 값을
    // 그대로 버려도(encodeSendAction에서 isEcho일 때 다시 채워 넣음) 정보
    // 손실이 없다.
    else if (key === '전송메시지.주소') { /* 의도적으로 무시 - 위 주석 참고 */ }
    else {
      let m = key.match(/^전송메시지\.주소\.파라미터\[(\d+)\]\.(key|val)$/);
      if (m) {
        const idx = Number(m[1]);
        if (!paramMap.has(idx)) paramMap.set(idx, { key: '', val: '' });
        paramMap.get(idx)[m[2]] = val;
        return;
      }
      m = key.match(/^전송메시지\.헤더\[(\d+)\]\.(key|val)$/);
      if (m) {
        const idx = Number(m[1]);
        if (!headerMap.has(idx)) headerMap.set(idx, { key: '', val: '' });
        headerMap.get(idx)[m[2]] = val;
        return;
      }
      // 나머지는 전부 바디 필드 - "전송메시지." 접두어를 뗀 나머지가 필드명
      // (점 표기 중첩 경로 허용, 예: record.text)
      const field = key.replace(/^전송메시지\./, '');
      params.body.push({ field, val });
    }
  });
  params.params = [...paramMap.keys()].sort((a, b) => a - b).map((i) => paramMap.get(i));
  params.headers = [...headerMap.keys()].sort((a, b) => a - b).map((i) => headerMap.get(i));
  params.isEcho = !paramList.some((p) => p['param-key'] === '전송메시지.메소드');
  return params;
}

// UI 구조(decodeSendAction 결과 형태) -> action-list의 param-list로 재조립.
// "메소드=응답"이면 "전송메시지.메소드" 키 자체를 만들지 않는다(업무지침
// 확정 사항 2번).
export function encodeSendAction(ui) {
  const list = [];
  if (ui.isEcho) {
    // decodeSendAction에서 일부러 버린 "전송메시지.주소 = 수신메시지.주소"를
    // 그대로 복원한다(응답 모드의 고정 관용구 - 위 decodeSendAction 주석 참고).
    list.push({ 'param-key': '전송메시지.주소', 'param-val': '수신메시지.주소' });
  }
  if (!ui.isEcho) {
    list.push({ 'param-key': '전송메시지.메소드', 'param-val': ui.method || 'GET' });
    list.push({ 'param-key': '전송메시지.주소.도메인', 'param-val': encodeSlash(ui.domain) });
    list.push({ 'param-key': '전송메시지.주소.경로', 'param-val': encodeSlash(ui.path) });
    ui.params.forEach((p, i) => {
      list.push({ 'param-key': `전송메시지.주소.파라미터[${i}].key`, 'param-val': encodeSlash(p.key) });
      list.push({ 'param-key': `전송메시지.주소.파라미터[${i}].val`, 'param-val': encodeSlash(p.val) });
    });
  }
  ui.headers.forEach((h, i) => {
    list.push({ 'param-key': `전송메시지.헤더[${i}].key`, 'param-val': encodeSlash(h.key) });
    list.push({ 'param-key': `전송메시지.헤더[${i}].val`, 'param-val': encodeSlash(h.val) });
  });
  ui.body.forEach((b) => {
    list.push({ 'param-key': `전송메시지.${b.field}`, 'param-val': encodeSlash(b.val) });
  });
  return list;
}
export function saveAction(restConfig, ns, actionName, actionType, uiSendState) {
  const next = cloneRest(restConfig);
  const list = next['action-list'] || [];
  const idx = list.findIndex((a) => a['action-name'] === actionName && a['name-space'] === ns);
  const rebuilt = {
    'action-type': actionType,
    'name-space': ns,
    'action-name': actionName,
    'param-list': encodeSendAction(uiSendState),
  };
  if (idx >= 0) list[idx] = rebuilt;
  else list.push(rebuilt);
  next['action-list'] = list;
  return next;
}
// 타이머는 항상 "전송메시지.이벤트명"/"전송메시지.시간" 2개 필드 고정 구조.
export function decodeTimerAction(action) {
  const out = { eventName: '', ms: '' };
  (action['param-list'] || []).forEach((p) => {
    if (p['param-key'] === '전송메시지.이벤트명') out.eventName = decodeSlash(p['param-val']);
    if (p['param-key'] === '전송메시지.시간') out.ms = decodeSlash(p['param-val']);
  });
  return out;
}
export function saveTimerAction(restConfig, ns, actionName, ui) {
  const next = cloneRest(restConfig);
  const list = next['action-list'] || [];
  const idx = list.findIndex((a) => a['action-name'] === actionName && a['name-space'] === ns);
  const rebuilt = {
    'action-type': '타이머',
    'name-space': ns,
    'action-name': actionName,
    'param-list': [
      { 'param-key': '전송메시지.이벤트명', 'param-val': encodeSlash(ui.eventName) },
      { 'param-key': '전송메시지.시간', 'param-val': encodeSlash(ui.ms) },
    ],
  };
  if (idx >= 0) list[idx] = rebuilt;
  else list.push(rebuilt);
  next['action-list'] = list;
  return next;
}
export function addAction(restConfig, ns, name, actionType) {
  const next = cloneRest(restConfig);
  const list = next['action-list'] || [];
  if (actionType === '타이머') {
    list.push({
      'action-type': '타이머', 'name-space': ns, 'action-name': name,
      'param-list': [
        { 'param-key': '전송메시지.이벤트명', 'param-val': name },
        { 'param-key': '전송메시지.시간', 'param-val': '5000' },
      ],
    });
  } else {
    list.push({
      'action-type': '전송', 'name-space': ns, 'action-name': name,
      'param-list': [
        { 'param-key': '전송메시지.메소드', 'param-val': 'GET' },
        { 'param-key': '전송메시지.주소.도메인', 'param-val': '' },
        { 'param-key': '전송메시지.주소.경로', 'param-val': '' },
      ],
    });
  }
  next['action-list'] = list;
  return next;
}

// ── 문장 (stmt-list) ─────────────────────────────────────────────────────
// StmtParser는 serialPath()를 전혀 타지 않는다(실측 확인) - 그대로 주고받는다.
export function listStmts(restConfig, ns) {
  return (restConfig['stmt-list'] || [])
    .map((s, idx) => ({ idx, name: s['stmt-name'], ns: s['name-space'], data: s['stmt-data'] }))
    .filter((s) => s.ns === ns);
}
export function saveStmt(restConfig, ns, name, data) {
  const next = cloneRest(restConfig);
  const list = next['stmt-list'] || [];
  const idx = list.findIndex((s) => s['stmt-name'] === name && s['name-space'] === ns);
  const rebuilt = { 'stmt-name': name, 'name-space': ns, 'stmt-data': data };
  if (idx >= 0) list[idx] = rebuilt;
  else list.push(rebuilt);
  next['stmt-list'] = list;
  return next;
}
