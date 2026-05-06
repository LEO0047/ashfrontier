#include "AshfrontierHUD.h"

#include "AshfrontierCameraControllerComponent.h"
#include "AshfrontierCharacter.h"
#include "AshfrontierDamageModelComponent.h"
#include "AshfrontierInventoryComponent.h"
#include "AshfrontierPlacedBuilding.h"
#include "AshfrontierPlayerController.h"
#include "AshfrontierSquadManagerComponent.h"

void AAshfrontierHUD::DrawHUD()
{
    Super::DrawHUD();

    DrawText(TEXT("Ashfrontier Gate 07 原型 HUD"), FLinearColor::White, 32.0f, 32.0f);

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
    DrawText(FString::Printf(TEXT("小隊：%d / 選取：%d / 鏡頭：%s"), SquadManager->GetSquadCount(), SquadManager->GetSelectedCount(), *CameraLabel), FLinearColor::White, 32.0f, 58.0f);

    const TArray<AAshfrontierCharacter*> Members = SquadManager->GetSquadMembers();
    float Y = 88.0f;
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
    }
}
