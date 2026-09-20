import { useState } from 'react';
import EventTab from './EventTab.jsx';
import ProcTab from './ProcTab.jsx';
import SendTab from './SendTab.jsx';
import TimerTab from './TimerTab.jsx';
import StmtTab from './StmtTab.jsx';
import HelpTab from './HelpTab.jsx';

const SUBTABS = [
  { key: 'branch', label: '이벤트' },
  { key: 'proc', label: '처리' },
  { key: 'send', label: '전송' },
  { key: 'timer', label: '타이머' },
  { key: 'stmt', label: '문장' },
  { key: 'help', label: '도움말' },
];

export default function ScenarioTab({ restConfig, ns, onSave, saving, addrConfig }) {
  const [sub, setSub] = useState('proc');
  return (
    <div className="tabpane">
      <div className="subtabs">
        {SUBTABS.map((s) => (
          <div
            key={s.key}
            className={`subtab${sub === s.key ? ' active' : ''}`}
            onClick={() => setSub(s.key)}
          >{s.label}</div>
        ))}
      </div>
      {sub === 'branch' && <EventTab restConfig={restConfig} ns={ns} onSave={onSave} saving={saving} />}
      {sub === 'proc' && <ProcTab restConfig={restConfig} ns={ns} onSave={onSave} saving={saving} />}
      {sub === 'send' && <SendTab restConfig={restConfig} ns={ns} onSave={onSave} saving={saving} addrConfig={addrConfig} />}
      {sub === 'timer' && <TimerTab restConfig={restConfig} ns={ns} onSave={onSave} saving={saving} />}
      {sub === 'stmt' && <StmtTab restConfig={restConfig} ns={ns} onSave={onSave} saving={saving} />}
      {sub === 'help' && <HelpTab />}
    </div>
  );
}
