#include "AshfrontierHUD.h"

void AAshfrontierHUD::DrawHUD()
{
    Super::DrawHUD();

    DrawText(TEXT("Ashfrontier 原型 HUD"), FLinearColor::White, 32.0f, 32.0f);
    DrawText(TEXT("Gate 01：核心模組與 playable map 驗證中"), FLinearColor::White, 32.0f, 56.0f);
}
