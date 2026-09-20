import { useEffect, useState } from 'react';
import { listActions, decodeTimerAction, saveTimerAction, addAction } from '../restModel.js';

export default function TimerTab({ restConfig, ns, onSave, saving }) {
  const timers = listActions(restConfig, ns, '타이머');
  const [selectedName, setSelectedName] = useState(timers[0]?.name || '');
  const [ui, setUi] = useState(null);
  const [newName, setNewName] = useState('');

  useEffect(() => {
    const list = listActions(restConfig, ns, '타이머');
    if (!list.find((a) => a.name === selectedName)) setSelectedName(list[0]?.name || '');
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [restConfig, ns]);

  useEffect(() => {
    const found = timers.find((a) => a.name === selectedName);
    setUi(found ? decodeTimerAction(found.action) : null);
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [selectedName, restConfig, ns]);

  function handleSave() {
    if (!selectedName || !ui) return;
    onSave(saveTimerAction(restConfig, ns, selectedName, ui));
  }
  function handleAdd() {
    const name = newName.trim();
    if (!name) return;
    setNewName('');
    onSave(addAction(restConfig, ns, name, '타이머'));
    setSelectedName(name);
  }

  return (
    <div className="subpane">
      <div className="obj-layout">
        <div className="obj-list">
          <div className="obj-list-title">타이머 ({timers.length})</div>
          {timers.map((t) => (
            <button key={t.name} className={`obj-item${t.name === selectedName ? ' active' : ''}`} onClick={() => setSelectedName(t.name)}>
              {t.name}
            </button>
          ))}
          <div className="kv-row" style={{ marginTop: 6 }}>
            <input placeholder="새 타이머 이름" value={newName} onChange={(e) => setNewName(e.target.value)} style={{ fontSize: 12 }} />
          </div>
          <button className="obj-add" onClick={handleAdd}>＋ 새 타이머</button>
        </div>
        <div className="obj-editor">
          {!ui && <div className="empty-hint">좌측에서 타이머를 선택하거나 새로 추가하세요.</div>}
          {ui && (
            <div className="panel">
              <div className="panel-title"><span className="ns-badge">{ns}</span>타이머::{selectedName}</div>
              <div className="panel-desc">
                일회성 타이머를 설정 합니다.<br />
                타임아웃이 발생 하면 해당 이벤트명으로 이벤트 분기 로직으로 호출 됩니다.<br />
                이벤트 설정을 추가 해 줘야 합니다.
              </div>
              <div className="kv-row">
                <input value="이벤트명" disabled style={{ color: '#a3a7b8' }} />
                <input value={ui.eventName} onChange={(e) => setUi({ ...ui, eventName: e.target.value })} />
              </div>
              <div className="kv-row">
                <input value="시간(ms)" disabled style={{ color: '#a3a7b8' }} />
                <input value={ui.ms} onChange={(e) => setUi({ ...ui, ms: e.target.value })} />
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
