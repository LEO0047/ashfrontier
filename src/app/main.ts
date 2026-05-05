import './styles.css';

const app = document.querySelector<HTMLDivElement>('#app');

if (!app) {
  throw new Error('找不到 Ashfrontier app 掛載點。');
}

app.innerHTML = `
  <main class="boot-screen">
    <section class="boot-panel" aria-label="Ashfrontier 啟動狀態">
      <p class="eyebrow">Ashfrontier Prototype</p>
      <h1>Ashfrontier</h1>
      <p>荒原小隊沙盒原型已建立。階段 1 會接上 Three.js 場景、攝影機與角色控制。</p>
      <dl>
        <div>
          <dt>工程狀態</dt>
          <dd>階段 0 骨架</dd>
        </div>
        <div>
          <dt>驗證管線</dt>
          <dd>format / lint / typecheck / unit / content lint / build</dd>
        </div>
      </dl>
    </section>
  </main>
`;
