#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AshfrontierHUD.generated.h"

UCLASS()
class ASHFRONTIER_API AAshfrontierHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;
};
