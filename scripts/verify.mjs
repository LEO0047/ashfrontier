import { spawnSync } from 'node:child_process';

const steps = [
  ['pnpm', ['run', 'format:check']],
  ['pnpm', ['run', 'lint']],
  ['pnpm', ['run', 'typecheck']],
  ['pnpm', ['run', 'test']],
  ['pnpm', ['run', 'content:lint']],
  ['pnpm', ['run', 'build']]
];

for (const [command, args] of steps) {
  console.log(`\n[verify] ${command} ${args.join(' ')}`);
  const result = spawnSync(command, args, {
    stdio: 'inherit',
    shell: process.platform === 'win32'
  });

  if (result.status !== 0) {
    console.error(`[verify] 失敗：${command} ${args.join(' ')}`);
    process.exit(result.status ?? 1);
  }
}

console.log('\n[verify] 全部檢查通過。');
