#include "AshfrontierHUD.h"

#include "AshfrontierCameraControllerComponent.h"
#include "AshfrontierCharacter.h"
#include "AshfrontierPlayerController.h"
#include "AshfrontierSquadManagerComponent.h"

void AAshfrontierHUD::DrawHUD()
{
    Super::DrawHUD();

    DrawText(TEXT("Ashfrontier Gate 03 原型 HUD"), FLinearColor::White, 32.0f, 32.0f);

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
        DrawText(FString::Printf(TEXT("%s %d  %s  %s"), *Marker, Member->GetSquadIndex() + 1, *Member->GetSquadDisplayName(), *Member->GetOrderLabelZh()), Color, 32.0f, Y);
        Y += 24.0f;
    }
}
