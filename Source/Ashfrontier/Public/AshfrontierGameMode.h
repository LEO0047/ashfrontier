#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AshfrontierGameMode.generated.h"

class AAshfrontierCharacter;
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
    void SpawnGate05Hostile();

    UPROPERTY(Transient)
    TObjectPtr<AAshfrontierWorldBlockoutDirector> WorldBlockoutDirector;

    UPROPERTY(Transient)
    TObjectPtr<AAshfrontierCharacter> Gate05Hostile;
};
