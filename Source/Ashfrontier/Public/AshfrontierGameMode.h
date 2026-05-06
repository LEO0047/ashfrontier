#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AshfrontierGameMode.generated.h"

class AAshfrontierWorldBlockoutDirector;

UCLASS()
class ASHFRONTIER_API AAshfrontierGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AAshfrontierGameMode();

protected:
    virtual void BeginPlay() override;

private:
    void SpawnPrototypeWorld();

    UPROPERTY(Transient)
    TObjectPtr<AAshfrontierWorldBlockoutDirector> WorldBlockoutDirector;
};
