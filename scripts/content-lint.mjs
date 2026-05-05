import { mkdirSync, writeFileSync } from 'node:fs';

mkdirSync('Reports', { recursive: true });

const report = [
  '# Ashfrontier 內容檢查',
  '',
  '- 狀態：通過',
  '- 階段：0',
  '- 說明：資料驅動內容將在階段 3 起由 schema 與引用檢查驗證。',
  ''
].join('\n');

writeFileSync('Reports/content-lint.md', report);
console.log('content lint 通過：階段 0 尚無資料表，已產出初始報告。');
