#include "AshfrontierWorldBlockoutDirector.h"

#include "AshfrontierRouteAgent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"

AAshfrontierWorldBlockoutDirector::AAshfrontierWorldBlockoutDirector()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AAshfrontierWorldBlockoutDirector::BuildPrototypeWorld()
{
    DestroySpawnedBlockout();

    CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    DefineWorldRecords();
    SpawnWorldGeometry();
    SpawnRouteMarkers();
    SpawnRouteAgents();
}

void AAshfrontierWorldBlockoutDirector::DestroySpawnedBlockout()
{
    for (AActor* Actor : SpawnedActors)
    {
        if (IsValid(Actor))
        {
            Actor->Destroy();
        }
    }
    SpawnedActors.Reset();
    RouteAgents.Reset();
}

const TArray<FAshfrontierWorldZoneRecord>& AAshfrontierWorldBlockoutDirector::GetZoneRecords() const
{
    return ZoneRecords;
}

const TArray<FAshfrontierRouteRecord>& AAshfrontierWorldBlockoutDirector::GetRouteRecords() const
{
    return RouteRecords;
}

int32 AAshfrontierWorldBlockoutDirector::GetRouteAgentCount() const
{
    return RouteAgents.Num();
}

void AAshfrontierWorldBlockoutDirector::DefineWorldRecords()
{
    ZoneRecords = {
        {TEXT("zone_salt_spine_city"), TEXT("鹽脊主城"), EAshfrontierWorldZoneType::MainCity, FVector(0.0f, 0.0f, 0.0f), FVector(1800.0f, 1300.0f, 90.0f)},
        {TEXT("zone_cinder_outpost"), TEXT("燼沙前哨"), EAshfrontierWorldZoneType::Outpost, FVector(3000.0f, -350.0f, 0.0f), FVector(900.0f, 650.0f, 80.0f)},
        {TEXT("zone_wilderness_yard"), TEXT("裂土建造區"), EAshfrontierWorldZoneType::WildernessBuildZone, FVector(1600.0f, 1800.0f, 0.0f), FVector(1400.0f, 1000.0f, 70.0f)}
    };

    RouteRecords = {
        {
            TEXT("route_city_guard_patrol"),
            TEXT("鹽脊城門巡邏"),
            {
                FVector(-760.0f, -520.0f, 120.0f),
                FVector(760.0f, -520.0f, 120.0f),
                FVector(760.0f, 520.0f, 120.0f),
                FVector(-760.0f, 520.0f, 120.0f)
            },
            false
        },
        {
            TEXT("route_glasshouse_caravan"),
            TEXT("玻璃屋商隊路線"),
            {
                FVector(0.0f, 220.0f, 120.0f),
                FVector(1050.0f, 860.0f, 120.0f),
                FVector(1600.0f, 1800.0f, 120.0f),
                FVector(3000.0f, -350.0f, 120.0f)
            },
            true
        }
    };
}

void AAshfrontierWorldBlockoutDirector::SpawnWorldGeometry()
{
    SpawnBlock(TEXT("AF_Blockout_World_Floor"), FVector(1300.0f, 650.0f, -70.0f), FVector(5600.0f, 4200.0f, 30.0f), FLinearColor(0.18f, 0.18f, 0.16f, 1.0f));

    for (const FAshfrontierWorldZoneRecord& Zone : ZoneRecords)
    {
        SpawnBlock(Zone.ZoneId, Zone.Center + FVector(0.0f, 0.0f, -35.0f), Zone.Size, FLinearColor(0.25f, 0.25f, 0.22f, 1.0f));
    }

    SpawnBlock(TEXT("AF_City_North_Wall"), FVector(0.0f, 690.0f, 90.0f), FVector(1900.0f, 90.0f, 240.0f), FLinearColor(0.35f, 0.34f, 0.3f, 1.0f));
    SpawnBlock(TEXT("AF_City_South_Wall"), FVector(0.0f, -690.0f, 90.0f), FVector(1900.0f, 90.0f, 240.0f), FLinearColor(0.35f, 0.34f, 0.3f, 1.0f));
    SpawnBlock(TEXT("AF_City_East_Gate_Block"), FVector(950.0f, 0.0f, 80.0f), FVector(90.0f, 380.0f, 220.0f), FLinearColor(0.36f, 0.33f, 0.28f, 1.0f));
    SpawnBlock(TEXT("AF_Outpost_Tower"), FVector(3000.0f, -350.0f, 140.0f), FVector(180.0f, 180.0f, 340.0f), FLinearColor(0.28f, 0.31f, 0.34f, 1.0f));
    SpawnBlock(TEXT("AF_Wilderness_Work_Yard"), FVector(1600.0f, 1800.0f, 60.0f), FVector(360.0f, 260.0f, 150.0f), FLinearColor(0.24f, 0.29f, 0.24f, 1.0f));
}

void AAshfrontierWorldBlockoutDirector::SpawnRouteMarkers()
{
    for (const FAshfrontierRouteRecord& Route : RouteRecords)
    {
        for (int32 Index = 0; Index < Route.Waypoints.Num(); ++Index)
        {
            const FName MarkerName(*FString::Printf(TEXT("AF_%s_WP_%02d"), *Route.RouteId.ToString(), Index));
            SpawnBlock(MarkerName, Route.Waypoints[Index] + FVector(0.0f, 0.0f, -40.0f), FVector(70.0f, 70.0f, 50.0f), FLinearColor(0.15f, 0.45f, 0.75f, 1.0f));
        }
    }
}

void AAshfrontierWorldBlockoutDirector::SpawnRouteAgents()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    for (const FAshfrontierRouteRecord& Route : RouteRecords)
    {
        if (Route.Waypoints.Num() == 0)
        {
            continue;
        }

        AAshfrontierRouteAgent* Agent = World->SpawnActor<AAshfrontierRouteAgent>(AAshfrontierRouteAgent::StaticClass(), Route.Waypoints[0], FRotator::ZeroRotator, SpawnParams);
        if (!Agent)
        {
            continue;
        }

        Agent->SetRoute(Route.RouteId, Route.NameZh, Route.Waypoints, true);
        Agent->SpawnDefaultController();
        RouteAgents.Add(Agent);
        SpawnedActors.Add(Agent);
    }
}

AActor* AAshfrontierWorldBlockoutDirector::SpawnBlock(const FName& ActorName, const FVector& Location, const FVector& Size, const FLinearColor& DebugColor)
{
    UWorld* World = GetWorld();
    if (!World || !CubeMesh)
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Name = ActorName;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AStaticMeshActor* Block = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);
    if (!Block)
    {
        return nullptr;
    }

    Block->SetActorScale3D(FVector(Size.X / 100.0f, Size.Y / 100.0f, Size.Z / 100.0f));
    UStaticMeshComponent* MeshComponent = Block->GetStaticMeshComponent();
    if (MeshComponent)
    {
        MeshComponent->SetStaticMesh(CubeMesh);
        MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        MeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
        MeshComponent->SetMobility(EComponentMobility::Static);
        MeshComponent->SetCustomPrimitiveDataFloat(0, DebugColor.R);
    }

    SpawnedActors.Add(Block);
    return Block;
}
