// 처리(procedure) 조건문 문자열 <-> UI(좌항/연산자/우항) 상호 변환.
//
// 실제 연산자 심볼은 libUtil/parser/IFPARSER.cpp의 s_compList[]/s_cmpString[]에서
// 확인했다(목업의 한글 라벨 순서와 정확히 일치):
//   TRUE->참, FALSE->거짓, ">"->크다, "<"->작다, ">="->크거나같다,
//   "<="->작거나같다, "=="->완전일치, "!="->불일치, "==="->앞자리일치(prefix),
//   "!!!"->부분일치(partial/substring)
// 조건 블록 접두어: 만약에(...) / 그외그외(...) / 그외()
export const OPS = [
  { label: '참', sym: 'TRUE', needsRhs: false },
  { label: '거짓', sym: 'FALSE', needsRhs: false },
  { label: '크다', sym: '>', needsRhs: true },
  { label: '작다', sym: '<', needsRhs: true },
  { label: '크거나같다', sym: '>=', needsRhs: true },
  { label: '작거나같다', sym: '<=', needsRhs: true },
  { label: '완전일치', sym: '==', needsRhs: true },
  { label: '불일치', sym: '!=', needsRhs: true },
  { label: '앞자리일치', sym: '===', needsRhs: true },
  { label: '부분일치', sym: '!!!', needsRhs: true },
];
const SYM_TO_LABEL = new Map(OPS.map((o) => [o.sym, o.label]));

// condition 문자열 -> {kind:'if'|'elseif'|'else', lhs, opLabel, rhs}
export function parseCondition(cond) {
  const c = (cond || '').trim();
  let kind = 'if';
  let inner = '';
  if (c.startsWith('그외그외(') && c.endsWith(')')) { kind = 'elseif'; inner = c.slice(5, -1); }
  else if (c.startsWith('그외(') && c.endsWith(')')) { kind = 'else'; inner = c.slice(3, -1); }
  else if (c.startsWith('만약에(') && c.endsWith(')')) { kind = 'if'; inner = c.slice(4, -1); }
  else { inner = c; }

  if (inner === 'TRUE' || inner === '참') return { kind, lhs: '', opLabel: '참', rhs: '' };
  if (inner === 'FALSE' || inner === '거짓') return { kind, lhs: '', opLabel: '거짓', rhs: '' };
  if (inner === '') return { kind, lhs: '', opLabel: '참', rhs: '' };

  // 연산자를 길이가 긴 것부터 찾아야 "==="/"==" 오판을 피한다.
  const symsByLenDesc = [...OPS].filter((o) => o.needsRhs).sort((a, b) => b.sym.length - a.sym.length);
  for (const op of symsByLenDesc) {
    const at = inner.indexOf(op.sym);
    if (at > -1) {
      return {
        kind,
        lhs: inner.slice(0, at).trim(),
        opLabel: SYM_TO_LABEL.get(op.sym),
        rhs: inner.slice(at + op.sym.length).trim(),
      };
    }
  }
  // 인식 못한 형태 - 원문을 lhs에 그대로 두고 "참"으로 표시(안전한 fallback).
  return { kind, lhs: inner, opLabel: '참', rhs: '' };
}

export function buildCondition({ kind, lhs, opLabel, rhs }) {
  const op = OPS.find((o) => o.label === opLabel) || OPS[0];
  let inner;
  if (!op.needsRhs) inner = op.label === '거짓' ? '거짓' : '참';
  else inner = `${lhs}${op.sym}${rhs}`;
  if (kind === 'else') return '그외()';
  if (kind === 'elseif') return `그외그외(${inner})`;
  return `만약에(${inner})`;
}
