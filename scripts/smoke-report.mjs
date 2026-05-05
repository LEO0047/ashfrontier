import { mkdirSync, writeFileSync } from 'node:fs';

mkdirSync('Reports', { recursive: true });

const report = [
  '# Ashfrontier Smoke Report',
  '',
  '- 狀態：階段 0 工程骨架可建置',
  '- 畫面：啟動畫面可由 Vite 載入',
  '- 後續：階段 1 會加入 Three.js canvas 與互動 smoke test',
  ''
].join('\n');

writeFileSync('Reports/perf-summary.md', report);
console.log('smoke report 已寫入 Reports/perf-summary.md');
