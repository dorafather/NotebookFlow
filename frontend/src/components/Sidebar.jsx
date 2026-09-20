import { iconFor } from '../api.js';

// "시스템 상태"는 addr.ini 카테고리(연동)가 아니라 자식 프로세스
// 헬스체크라는 별개 성격의 전역 화면이라, 연동 목록과 나란히 두지 않고
// 사이드바 맨 위(브랜드 바로 아래)에 독립된 항목으로 고정 배치한다 -
// isSystemStatus가 true면 이 항목이 활성 표시되고, onSelectSystemStatus로
// 전환한다(App.jsx가 최상위 뷰를 분기).
export default function Sidebar({ integrations, selected, onSelect, onAddClick, isSystemStatus, onSelectSystemStatus }) {
  return (
    <div className="sidebar">
      <div className="brand">
        <div className="brand-mark" />
        <div>
          <div className="brand-name">NotebookFlow</div>
          <div className="brand-sub">연동 관리</div>
        </div>
      </div>
      <button
        className={`conn-item sys-status-item${isSystemStatus ? ' active' : ''}`}
        onClick={onSelectSystemStatus}
      >
        <span className={`dot ${isSystemStatus ? 'ok' : 'off'}`} />
        <div className="conn-icon" style={{ background: '#2e9b6f' }}>S</div>
        <span className="conn-name">시스템 상태</span>
      </button>
      <div className="nav-label">등록된 연동 ({integrations.length})</div>
      <div className="conn-list">
        {integrations.map((it) => {
          const icon = iconFor(it.key);
          return (
            <button
              key={it.key}
              className={`conn-item${selected === it.key ? ' active' : ''}`}
              onClick={() => onSelect(it.key)}
            >
              <span className={`dot ${it.configured ? 'ok' : 'off'}`} />
              <div className="conn-icon" style={{ background: icon.color }}>{icon.letter}</div>
              <span className="conn-name">{it.key}</span>
              {!it.configured && <span className="conn-badge">미설정</span>}
            </button>
          );
        })}
      </div>
      <button className="add-conn-btn" onClick={onAddClick}>＋ 연동 추가</button>
    </div>
  );
}
