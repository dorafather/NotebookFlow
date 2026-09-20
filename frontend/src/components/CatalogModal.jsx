import { CATALOG } from '../api.js';

// 카탈로그 메타데이터 API(GET /notebookflow/catalog)는 아직 없다. 이 모달은
// 목업과 동일하게 정적 참고 목록만 보여준다 - 클릭해도 실제로 연동이 추가되지
// 않는다(새 연동 온보딩 플로우는 후속 작업).
export default function CatalogModal({ show, onClose }) {
  return (
    <div className={`modal-overlay${show ? ' show' : ''}`} onClick={(e) => { if (e.target === e.currentTarget) onClose(); }}>
      <div className="modal">
        <div className="modal-head">
          <h2>연동 추가</h2>
          <button className="modal-close" onClick={onClose}>×</button>
        </div>
        <div className="modal-body">
          {CATALOG.map((c) => (
            <button
              key={c.key}
              className="catalog-item"
              onClick={() => alert('새 연동 추가(온보딩) 기능은 이번 범위 밖입니다. addr.ini에 카테고리를 직접 추가한 뒤 다시 시작하면 사이드바에 자동으로 나타납니다.')}
            >
              <div className="conn-icon" style={{ background: c.color }}>{c.letter}</div>
              <div>
                <div className="catalog-name">{c.name}</div>
                <div className="catalog-fields">{c.fields}</div>
              </div>
            </button>
          ))}
        </div>
        <div className="catalog-note">
          ⓘ 이미 addr.ini에 등록된 연동은 사이드바에 자동으로 표시됩니다(현재 API 기준 동적 렌더링). 이 목록은 참고용 카탈로그이며, 새 연동을 처음부터 추가하는 기능은 후속 작업입니다.
        </div>
      </div>
    </div>
  );
}
