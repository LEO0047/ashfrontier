#include "AshfrontierHUD.h"

#include "AshfrontierCameraControllerComponent.h"
#include "AshfrontierCharacter.h"
#include "AshfrontierDamageModelComponent.h"
#include "AshfrontierInventoryComponent.h"
#include "AshfrontierCrimeEventMemoryComponent.h"
#include "AshfrontierGuardAIComponent.h"
#include "AshfrontierPlacedBuilding.h"
#include "AshfrontierPlayerController.h"
#include "AshfrontierSquadManagerComponent.h"
#include "Engine/Canvas.h"

void AAshfrontierHUD::DrawHUD()
{
    Super::DrawHUD();

    const float PanelWidth = Canvas ? FMath::Min(Canvas->SizeX - 36.0f, 1040.0f) : 1040.0f;
    DrawRect(FLinearColor(0.02f, 0.025f, 0.03f, 0.82f), 18.0f, 18.0f, PanelWidth, 188.0f);
    DrawText(TEXT("Ashfrontier Gate 09 可玩原型已啟動"), FLinearColor(0.98f, 0.86f, 0.28f, 1.0f), 32.0f, 30.0f, nullptr, 1.45f);
    DrawText(TEXT("滑鼠左鍵選取｜右鍵移動 / 攻擊｜Tab 切換鏡頭｜R 招募｜T 交易｜G 採集｜B 建造｜P 生產｜F5 存檔｜F9 讀檔"), FLinearColor::White, 32.0f, 72.0f, nullptr, 0.95f);
    DrawText(TEXT("若看到這行，packaged build 已經進入 prototype map。黑畫面修補：啟用開場光源、固定俯視相機與大型 HUD。"), FLinearColor(0.6f, 0.95f, 0.95f, 1.0f), 32.0f, 104.0f, nullptr, 0.9f);

    const AAshfrontierPlayerController* AshController = Cast<AAshfrontierPlayerController>(PlayerOwner);
    if (!AshController)
    {
        return;
    }

    const UAshfrontierSquadManagerComponent* SquadManager = AshController->GetSquadManager();
    const UAshfrontierCameraControllerComponent* CameraController = AshController->GetCameraController();
    if (!SquadManager)
    {
        return;
    }

    const FString CameraLabel = CameraController && CameraController->IsTacticalCamera() ? TEXT("戰術視角") : TEXT("第三人稱視角");
    DrawText(FString::Printf(TEXT("小隊：%d / 選取：%d / 鏡頭：%s"), SquadManager->GetSquadCount(), SquadManager->GetSelectedCount(), *CameraLabel), FLinearColor::White, 32.0f, 142.0f, nullptr, 1.05f);

    const TArray<AAshfrontierCharacter*> Members = SquadManager->GetSquadMembers();
    float Y = 218.0f;
    for (const AAshfrontierCharacter* Member : Members)
    {
        if (!Member)
        {
            continue;
        }

        const FLinearColor Color = Member->IsSelected() ? FLinearColor(0.2f, 0.9f, 0.6f, 1.0f) : FLinearColor::White;
        const FString Marker = Member->IsSelected() ? TEXT("[X]") : TEXT("[ ]");
        const UAshfrontierDamageModelComponent* DamageModel = Member->GetDamageModel();
        const FString HealthLabel = DamageModel ? DamageModel->GetStateLabelZh() : TEXT("未知");
        const UAshfrontierInventoryComponent* Inventory = Member->GetInventory();
        const int32 Credits = Inventory ? Inventory->GetItemCount(TEXT("item_ash_credit")) : 0;
        const int32 Planks = Inventory ? Inventory->GetItemCount(TEXT("item_salvaged_plank")) : 0;
        const int32 Ore = Inventory ? Inventory->GetItemCount(TEXT("item_scrap_ore")) : 0;
        const int32 Grain = Inventory ? Inventory->GetItemCount(TEXT("item_raw_grain")) : 0;
        const int32 Water = Inventory ? Inventory->GetItemCount(TEXT("item_water_flask")) : 0;
        const int32 Rations = Inventory ? Inventory->GetItemCount(TEXT("item_cooked_ration")) : 0;
        DrawText(FString::Printf(TEXT("%s %d  %s  %s  %s  灰印幣:%d 木板:%d 碎鐵:%d 穀:%d 水:%d 糧:%d"), *Marker, Member->GetSquadIndex() + 1, *Member->GetSquadDisplayName(), *Member->GetOrderLabelZh(), *HealthLabel, Credits, Planks, Ore, Grain, Water, Rations), Color, 32.0f, Y);
        Y += 24.0f;
    }

    const AAshfrontierPlacedBuilding* LastBuilding = AshController->GetLastPlacedBuilding();
    if (LastBuilding)
    {
        const FString QueueLabel = LastBuilding->GetProductionQueueCount() > 0
            ? FString::Printf(TEXT("生產中 %.1f 秒"), LastBuilding->GetCurrentProductionRemainingSeconds())
            : TEXT("無生產隊列");
        DrawText(FString::Printf(TEXT("最近建築：%s / %s"), *LastBuilding->GetBuildingNameZh(), *QueueLabel), FLinearColor(0.8f, 0.95f, 0.6f, 1.0f), 32.0f, Y + 8.0f);
        Y += 32.0f;
    }

    const UAshfrontierGuardAIComponent* GuardAI = AshController->GetGuardAI();
    const UAshfrontierCrimeEventMemoryComponent* CrimeMemory = AshController->GetCrimeEventMemory();
    if (GuardAI && CrimeMemory)
    {
        DrawText(FString::Printf(TEXT("城市反應：%s / 事件記憶:%d"), *GuardAI->GetStateLabelZh(), CrimeMemory->GetMemoryCount()), FLinearColor(0.95f, 0.75f, 0.45f, 1.0f), 32.0f, Y + 8.0f);
    }
}
