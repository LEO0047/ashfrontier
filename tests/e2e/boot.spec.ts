import { expect, test } from '@playwright/test';

test('Ashfrontier 頁面可以啟動', async ({ page }) => {
  await page.goto('/');
  await expect(page.getByRole('heading', { name: 'Ashfrontier' })).toBeVisible();
  await expect(page.getByText('階段 0 骨架')).toBeVisible();
});
