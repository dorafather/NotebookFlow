import { useEffect, useState } from 'react';
import { decodeSlash, encodeSlash } from '../api.js';

export default function AccountTab({ addrConfig, selected, onSave, saving }) {
  const [rows, setRows] = useState([]);

  useEffect(() => {
    const cate = addrConfig[selected] || [];
    setRows(cate.map((it) => ({ key: it.key, val: decodeSlash(it.val) })));
  }, [addrConfig, selected]);

  function updateRow(i, field, value) {
    setRows((prev) => prev.map((r, idx) => (idx === i ? { ...r, [field]: value } : r)));
  }
  function removeRow(i) {
    setRows((prev) => prev.filter((_, idx) => idx !== i));
  }
  function addRow() {
    setRows((prev) => [...prev, { key: '', val: '' }]);
  }
  function handleSave() {
    // addr.ini는 카테고리별 값에 "도메인/domain"처럼 사실상 고정된 항목도
    // 있지만, IniParser::IMPORT는 그냥 들어온 그대로 저장하므로 프론트
    // 단에서 특별 취급할 필요는 없다 - key/val 그대로 인코딩해서 보낸다.
    const encoded = rows
      .filter((r) => r.key.trim() !== '')
      .map((r) => ({ key: r.key, val: encodeSlash(r.val) }));
    const full = { ...addrConfig, [selected]: encoded };
    onSave(full);
  }

  return (
    <div className="tabpane">
      <div className="panel">
        <div className="panel-title">계정 / 접속 정보</div>
        <div className="panel-desc">
          다양한 설정 정보를 시나리오 에서 참조 가능 합니다.<br />
          설정.{selected}.{'{키값}'} 으로 사용 하시면 됩니다.
        </div>
        {rows.map((r, i) => (
          <div className="kv-row" key={i}>
            <input value={r.key} onChange={(e) => updateRow(i, 'key', e.target.value)} placeholder="키" />
            <input value={r.val} onChange={(e) => updateRow(i, 'val', e.target.value)} placeholder="값" />
            <button className="kv-remove" onClick={() => removeRow(i)}>×</button>
          </div>
        ))}
        {rows.length === 0 && <div className="empty-hint">등록된 값이 없습니다.</div>}
        <div className="row-actions">
          <button className="btn btn-ghost" onClick={addRow}>＋ 필드 추가</button>
          <button className="btn btn-primary" style={{ marginLeft: 'auto' }} onClick={handleSave} disabled={saving}>
            {saving ? '저장 중…' : '변경사항 저장'}
          </button>
        </div>
      </div>
    </div>
  );
}
