import { useEffect, useState } from 'react';
import { listStmts, saveStmt } from '../restModel.js';

export default function StmtTab({ restConfig, ns, onSave, saving }) {
  const stmts = listStmts(restConfig, ns);
  const [selectedName, setSelectedName] = useState(stmts[0]?.name || '');
  const [data, setData] = useState('');
  const [newName, setNewName] = useState('');

  useEffect(() => {
    const list = listStmts(restConfig, ns);
    if (!list.find((s) => s.name === selectedName)) setSelectedName(list[0]?.name || '');
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [restConfig, ns]);

  useEffect(() => {
    const found = stmts.find((s) => s.name === selectedName);
    setData(found ? found.data : '');
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [selectedName, restConfig, ns]);

  function handleSave() {
    if (!selectedName) return;
    onSave(saveStmt(restConfig, ns, selectedName, data));
  }
  function handleAdd() {
    const name = newName.trim();
    if (!name) return;
    setNewName('');
    onSave(saveStmt(restConfig, ns, name, ''));
    setSelectedName(name);
  }

  return (
    <div className="subpane">
      <div className="obj-layout">
        <div className="obj-list">
          <div className="obj-list-title">문장 ({stmts.length})</div>
          {stmts.map((s) => (
            <button key={s.name} className={`obj-item${s.name === selectedName ? ' active' : ''}`} onClick={() => setSelectedName(s.name)}>
              {s.name}
            </button>
          ))}
          <div className="kv-row" style={{ marginTop: 6 }}>
            <input placeholder="새 문장 이름" value={newName} onChange={(e) => setNewName(e.target.value)} style={{ fontSize: 12 }} />
          </div>
          <button className="obj-add" onClick={handleAdd}>＋ 새 문장</button>
        </div>
        <div className="obj-editor">
          {!selectedName && <div className="empty-hint">좌측에서 문장을 선택하거나 새로 추가하세요.</div>}
          {selectedName && (
            <div className="panel">
              <div className="panel-title"><span className="ns-badge">{ns}</span>문장::{selectedName}</div>
              <div className="panel-desc">$$$세션.변수명$$$ / $$$수신메시지.필드$$$ 형태로 값을 동적으로 끼워 넣을 수 있습니다.</div>
              <div className="kv-row">
                <input value="경로 템플릿" disabled style={{ flex: '4 4 0%', color: '#a3a7b8' }} />
                <input value={data} onChange={(e) => setData(e.target.value)} style={{ flex: '6 6 0%' }} />
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
