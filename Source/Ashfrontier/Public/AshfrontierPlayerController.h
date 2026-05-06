#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AshfrontierPlayerController.generated.h"

UCLASS()
class ASHFRONTIER_API AAshfrontierPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AAshfrontierPlayerController();

protected:
    virtual void BeginPlay() override;
};
