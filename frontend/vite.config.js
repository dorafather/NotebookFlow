import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';

// NotebookFlow.exe(백엔드, 포트 18099)는 CORS 헤더를 전혀 세팅하지 않는다
// (Main.cpp 확인 - Access-Control-Allow-Origin 등 관련 코드 없음). 브라우저에서
// 개발 서버(5180) -> 백엔드(18099)로 직접 fetch하면 교차 출처로 막히므로,
// "/notebookflow" 경로는 Vite dev 서버가 백엔드로 프록시하게 한다(백엔드 C++
// 코드는 건드리지 않는다 - 범위 밖: "vite build 산출물을 NotebookFlow.exe가
// 서빙하는 것"과는 별개로, 개발 중에는 이 프록시로만 CORS 문제를 우회한다).
export default defineConfig({
  plugins: [react()],
  server: {
    host: '0.0.0.0',
    port: 5180,
    proxy: {
      '/notebookflow': {
        target: 'http://127.0.0.1:18099',
        changeOrigin: true,
      },
    },
  },
});
