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
}
