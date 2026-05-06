#include "AshfrontierGameMode.h"

#include "AshfrontierCharacter.h"
#include "AshfrontierHUD.h"
#include "AshfrontierPlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"

AAshfrontierGameMode::AAshfrontierGameMode()
{
    DefaultPawnClass = AAshfrontierCharacter::StaticClass();
    PlayerControllerClass = AAshfrontierPlayerController::StaticClass();
    HUDClass = AAshfrontierHUD::StaticClass();
}

void AAshfrontierGameMode::BeginPlay()
{
    Super::BeginPlay();

    SpawnPrototypeFloor();
}

void AAshfrontierGameMode::SpawnPrototypeFloor() const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (!CubeMesh)
    {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = TEXT("AF_Prototype_Command_Floor");
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AStaticMeshActor* FloorActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector(0.0f, 0.0f, -60.0f), FRotator::ZeroRotator, SpawnParams);
    if (!FloorActor)
    {
        return;
    }

    FloorActor->SetActorScale3D(FVector(32.0f, 32.0f, 0.08f));
    UStaticMeshComponent* MeshComponent = FloorActor->GetStaticMeshComponent();
    if (MeshComponent)
    {
        MeshComponent->SetStaticMesh(CubeMesh);
        MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
        MeshComponent->SetMobility(EComponentMobility::Static);
    }
}
