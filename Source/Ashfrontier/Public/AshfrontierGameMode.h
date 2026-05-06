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
    void SpawnGate06Characters();
    void SpawnGate08Guards();

    UPROPERTY(Transient)
    TObjectPtr<AAshfrontierWorldBlockoutDirector> WorldBlockoutDirector;

    UPROPERTY(Transient)
    TObjectPtr<AAshfrontierCharacter> Gate05Hostile;

    UPROPERTY(Transient)
    TArray<TObjectPtr<AAshfrontierCharacter>> Gate06Characters;

    UPROPERTY(Transient)
    TArray<TObjectPtr<AAshfrontierCharacter>> Gate08Guards;
};
