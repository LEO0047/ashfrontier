#include "AshfrontierGameMode.h"

#include "AshfrontierCharacter.h"
#include "AshfrontierHUD.h"
#include "AshfrontierPlayerController.h"
#include "AshfrontierWorldBlockoutDirector.h"

AAshfrontierGameMode::AAshfrontierGameMode()
{
    DefaultPawnClass = AAshfrontierCharacter::StaticClass();
    PlayerControllerClass = AAshfrontierPlayerController::StaticClass();
    HUDClass = AAshfrontierHUD::StaticClass();
}

void AAshfrontierGameMode::BeginPlay()
{
    Super::BeginPlay();

    SpawnPrototypeWorld();
}

void AAshfrontierGameMode::SpawnPrototypeWorld()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = TEXT("AF_WorldBlockoutDirector");
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    WorldBlockoutDirector = World->SpawnActor<AAshfrontierWorldBlockoutDirector>(AAshfrontierWorldBlockoutDirector::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    if (WorldBlockoutDirector)
    {
        WorldBlockoutDirector->BuildPrototypeWorld();
    }

    SpawnGate05Hostile();
}

void AAshfrontierGameMode::SpawnGate05Hostile()
{
    UWorld* World = GetWorld();
    if (!World || IsValid(Gate05Hostile))
    {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    Gate05Hostile = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), FVector(420.0f, -260.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
    if (Gate05Hostile)
    {
        Gate05Hostile->SetSquadDisplayName(TEXT("塵路劫手"));
        Gate05Hostile->SetCharacterTeam(EAshfrontierCharacterTeam::Hostile);
        Gate05Hostile->SpawnDefaultController();
    }
}
