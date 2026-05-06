#include "AshfrontierWorldBlockoutDirector.h"

#include "AshfrontierRouteAgent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/DecalComponent.h"
#include "Components/LightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "Dom/JsonObject.h"
#include "Engine/DecalActor.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/SkyLight.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
bool LoadContentJsonObject(const FString& RelativePath, TSharedPtr<FJsonObject>& OutObject)
{
    const FString AbsolutePath = FPaths::Combine(FPaths::ProjectContentDir(), RelativePath);
    FString JsonText;
    if (!FFileHelper::LoadFileToString(JsonText, *AbsolutePath))
    {
        return false;
    }

    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
    return FJsonSerializer::Deserialize(Reader, OutObject) && OutObject.IsValid();
}

bool ReadStringField(const TSharedPtr<FJsonObject>& Object, const FString& FieldName, FString& OutValue)
{
    if (!Object.IsValid() || !Object->HasTypedField<EJson::String>(FieldName))
    {
        return false;
    }
    OutValue = Object->GetStringField(FieldName);
    return !OutValue.IsEmpty();
}
}

AAshfrontierWorldBlockoutDirector::AAshfrontierWorldBlockoutDirector()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AAshfrontierWorldBlockoutDirector::BuildPrototypeWorld()
{
    DestroySpawnedBlockout();

    CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
    DefineWorldRecords();
    LoadArtSlotData();
    SpawnStartupLighting();
    SpawnWorldGeometry();
    SpawnWorldArtMarkers();
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

int32 AAshfrontierWorldBlockoutDirector::GetSpawnedActorCount() const
{
    return SpawnedActors.Num();
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

void AAshfrontierWorldBlockoutDirector::SpawnStartupLighting()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    SpawnParams.Name = TEXT("AF_Startup_SkyAtmosphere");
    if (ASkyAtmosphere* Atmosphere = World->SpawnActor<ASkyAtmosphere>(ASkyAtmosphere::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams))
    {
        SpawnedActors.Add(Atmosphere);
    }

    SpawnParams.Name = TEXT("AF_Startup_DustHaze");
    if (AExponentialHeightFog* Fog = World->SpawnActor<AExponentialHeightFog>(AExponentialHeightFog::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams))
    {
        SpawnedActors.Add(Fog);
    }

    SpawnParams.Name = TEXT("AF_Startup_Sun");
    ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>(ADirectionalLight::StaticClass(), FVector(-1600.0f, -1400.0f, 2200.0f), FRotator(-52.0f, -35.0f, 0.0f), SpawnParams);
    if (Sun)
    {
        if (ULightComponent* LightComponent = Sun->GetLightComponent())
        {
            LightComponent->SetIntensity(8.0f);
            LightComponent->SetLightColor(FLinearColor(1.0f, 0.92f, 0.78f, 1.0f));
            LightComponent->SetCastShadows(false);
        }
        SpawnedActors.Add(Sun);
    }

    SpawnParams.Name = TEXT("AF_Startup_SkyLight");
    ASkyLight* SkyLight = World->SpawnActor<ASkyLight>(ASkyLight::StaticClass(), FVector(0.0f, 0.0f, 900.0f), FRotator::ZeroRotator, SpawnParams);
    if (SkyLight)
    {
        if (USkyLightComponent* LightComponent = SkyLight->GetLightComponent())
        {
            LightComponent->SetIntensity(2.2f);
            LightComponent->SetLightColor(FLinearColor(0.86f, 0.88f, 0.78f, 1.0f));
            LightComponent->SetCastShadows(false);
            LightComponent->RecaptureSky();
        }
        SpawnedActors.Add(SkyLight);
    }
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
    SpawnBlock(TEXT("AF_City_Shop_District_Floor"), FVector(220.0f, 360.0f, 18.0f), FVector(620.0f, 360.0f, 55.0f), FLinearColor(0.28f, 0.27f, 0.23f, 1.0f));
    SpawnBlock(TEXT("AF_City_Guard_Post"), FVector(-420.0f, -520.0f, 80.0f), FVector(320.0f, 120.0f, 170.0f), FLinearColor(0.24f, 0.25f, 0.28f, 1.0f));
    SpawnBlock(TEXT("AF_Outpost_Tower"), FVector(3000.0f, -350.0f, 140.0f), FVector(180.0f, 180.0f, 340.0f), FLinearColor(0.28f, 0.31f, 0.34f, 1.0f));
    SpawnBlock(TEXT("AF_Outpost_Storehouse"), FVector(2820.0f, -170.0f, 55.0f), FVector(360.0f, 180.0f, 130.0f), FLinearColor(0.24f, 0.22f, 0.19f, 1.0f));
    SpawnBlock(TEXT("AF_Wilderness_Work_Yard"), FVector(1600.0f, 1800.0f, 60.0f), FVector(360.0f, 260.0f, 150.0f), FLinearColor(0.24f, 0.29f, 0.24f, 1.0f));
    SpawnBlock(TEXT("AF_Wilderness_Scrap_Mine"), FVector(1280.0f, 1660.0f, 45.0f), FVector(260.0f, 220.0f, 120.0f), FLinearColor(0.27f, 0.24f, 0.2f, 1.0f));
}

void AAshfrontierWorldBlockoutDirector::SpawnWorldArtMarkers()
{
    SpawnBlock(TEXT("AF_Banner_Saltwardens_CityGate"), FVector(900.0f, -240.0f, 245.0f), FVector(28.0f, 140.0f, 180.0f), FLinearColor(0.62f, 0.64f, 0.58f, 1.0f));
    SpawnBlock(TEXT("AF_Banner_Glasshouse_Market"), FVector(350.0f, 250.0f, 165.0f), FVector(28.0f, 150.0f, 160.0f), FLinearColor(0.58f, 0.42f, 0.22f, 1.0f));
    SpawnBlock(TEXT("AF_Banner_Dustrunners_Outpost"), FVector(2860.0f, -500.0f, 165.0f), FVector(28.0f, 150.0f, 160.0f), FLinearColor(0.55f, 0.28f, 0.19f, 1.0f));

    SpawnArtPlane(TEXT("AF_ArtPlane_Banner_Saltwardens_CityGate"), FVector(880.0f, -250.0f, 285.0f), FVector(-900.0f, -1260.0f, 470.0f), FVector2D(145.0f, 220.0f), TEXT("ArtSlot_FactionBanner_01"), FLinearColor(0.62f, 0.64f, 0.58f, 1.0f));
    SpawnArtPlane(TEXT("AF_ArtPlane_Banner_Glasshouse_Market"), FVector(350.0f, 250.0f, 215.0f), FVector(-250.0f, 920.0f, 430.0f), FVector2D(150.0f, 210.0f), TEXT("ArtSlot_FactionBanner_02"), FLinearColor(0.58f, 0.42f, 0.22f, 1.0f));
    SpawnArtPlane(TEXT("AF_ArtPlane_Banner_Dustrunners_Outpost"), FVector(2860.0f, -500.0f, 220.0f), FVector(2300.0f, -1080.0f, 440.0f), FVector2D(150.0f, 210.0f), TEXT("ArtSlot_FactionBanner_03"), FLinearColor(0.55f, 0.28f, 0.19f, 1.0f));
    SpawnArtPlane(TEXT("AF_ArtPlane_City_Gate_Mark"), FVector(905.0f, -20.0f, 245.0f), FVector(-900.0f, -1260.0f, 470.0f), FVector2D(190.0f, 190.0f), TEXT("ArtSlot_CitySignage"), FLinearColor(0.7f, 0.68f, 0.56f, 1.0f));
    SpawnArtPlane(TEXT("AF_ArtPlane_City_Shop_Sign"), FVector(220.0f, 178.0f, 155.0f), FVector(-250.0f, 920.0f, 430.0f), FVector2D(180.0f, 180.0f), TEXT("ArtSlot_Decal_ShopSign"), FLinearColor(0.75f, 0.55f, 0.32f, 1.0f));
    SpawnArtPlane(TEXT("AF_ArtPlane_Outpost_Warehouse"), FVector(2820.0f, -264.0f, 150.0f), FVector(2300.0f, -1080.0f, 440.0f), FVector2D(190.0f, 190.0f), TEXT("ArtSlot_Decal_WarehouseMark"), FLinearColor(0.58f, 0.42f, 0.28f, 1.0f));
    SpawnArtPlane(TEXT("AF_ArtPlane_Wilderness_Workshop"), FVector(1600.0f, 1668.0f, 165.0f), FVector(820.0f, 1020.0f, 520.0f), FVector2D(190.0f, 190.0f), TEXT("ArtSlot_Decal_WorkshopMark"), FLinearColor(0.64f, 0.48f, 0.28f, 1.0f));
    SpawnArtPlane(TEXT("AF_ArtPlane_Wilderness_Danger"), FVector(1250.0f, 1550.0f, 160.0f), FVector(820.0f, 1020.0f, 520.0f), FVector2D(190.0f, 190.0f), TEXT("ArtSlot_Decal_WildDanger"), FLinearColor(0.65f, 0.36f, 0.25f, 1.0f));

    SpawnDecalMarker(TEXT("AF_Decal_City_Gate_Mark"), FVector(902.0f, 0.0f, 210.0f), FRotator(0.0f, 180.0f, 0.0f), FVector(48.0f, 260.0f, 260.0f), FLinearColor(0.7f, 0.68f, 0.56f, 1.0f));
    SpawnDecalMarker(TEXT("AF_Decal_City_Restricted_Warning"), FVector(-420.0f, -588.0f, 155.0f), FRotator(0.0f, 180.0f, 0.0f), FVector(48.0f, 210.0f, 210.0f), FLinearColor(0.72f, 0.62f, 0.45f, 1.0f));
    SpawnDecalMarker(TEXT("AF_Decal_City_Guard_Notice"), FVector(-250.0f, -588.0f, 155.0f), FRotator(0.0f, 180.0f, 0.0f), FVector(48.0f, 210.0f, 210.0f), FLinearColor(0.68f, 0.68f, 0.58f, 1.0f));
    SpawnDecalMarker(TEXT("AF_Decal_City_Shop_Sign"), FVector(220.0f, 178.0f, 130.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(48.0f, 220.0f, 220.0f), FLinearColor(0.75f, 0.55f, 0.32f, 1.0f));
    SpawnDecalMarker(TEXT("AF_Decal_City_Trade_Notice"), FVector(360.0f, 178.0f, 130.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(48.0f, 220.0f, 220.0f), FLinearColor(0.72f, 0.5f, 0.28f, 1.0f));
    SpawnDecalMarker(TEXT("AF_Decal_Outpost_Direction"), FVector(2660.0f, -350.0f, 125.0f), FRotator(0.0f, 90.0f, 0.0f), FVector(48.0f, 210.0f, 210.0f), FLinearColor(0.58f, 0.46f, 0.32f, 1.0f));
    SpawnDecalMarker(TEXT("AF_Decal_Outpost_Warehouse"), FVector(2820.0f, -264.0f, 110.0f), FRotator(0.0f, 180.0f, 0.0f), FVector(48.0f, 220.0f, 220.0f), FLinearColor(0.58f, 0.42f, 0.28f, 1.0f));
    SpawnDecalMarker(TEXT("AF_Decal_Wilderness_Workshop"), FVector(1600.0f, 1668.0f, 120.0f), FRotator(0.0f, 180.0f, 0.0f), FVector(48.0f, 220.0f, 220.0f), FLinearColor(0.64f, 0.48f, 0.28f, 1.0f));
    SpawnDecalMarker(TEXT("AF_Decal_Wilderness_Water_Warning"), FVector(1410.0f, 1840.0f, 105.0f), FRotator(0.0f, 90.0f, 0.0f), FVector(48.0f, 210.0f, 210.0f), FLinearColor(0.62f, 0.62f, 0.52f, 1.0f));
    SpawnDecalMarker(TEXT("AF_Decal_Wilderness_Danger"), FVector(1250.0f, 1550.0f, 115.0f), FRotator(0.0f, 35.0f, 0.0f), FVector(48.0f, 230.0f, 230.0f), FLinearColor(0.65f, 0.36f, 0.25f, 1.0f));
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
        ApplyArtSlotMaterial(MeshComponent, ActorName, DebugColor);
    }

    SpawnedActors.Add(Block);
    return Block;
}

AActor* AAshfrontierWorldBlockoutDirector::SpawnDecalMarker(const FName& ActorName, const FVector& Location, const FRotator& Rotation, const FVector& DecalSize, const FLinearColor& DebugColor)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Name = ActorName;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ADecalActor* Decal = World->SpawnActor<ADecalActor>(ADecalActor::StaticClass(), Location, Rotation, SpawnParams);
    if (!Decal)
    {
        return nullptr;
    }

    if (UDecalComponent* DecalComponent = Decal->GetDecal())
    {
        DecalComponent->DecalSize = DecalSize;
        if (UMaterialInterface* Material = ResolveMaterialForSlot(FindArtSlotForActor(ActorName)))
        {
            DecalComponent->SetDecalMaterial(Material);
        }
        else if (UMaterialInstanceDynamic* DynamicMaterial = DecalComponent->CreateDynamicMaterialInstance())
        {
            DynamicMaterial->SetVectorParameterValue(TEXT("Color"), DebugColor);
            DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), DebugColor);
        }
    }

    SpawnedActors.Add(Decal);
    return Decal;
}

AActor* AAshfrontierWorldBlockoutDirector::SpawnArtPlane(const FName& ActorName, const FVector& Location, const FVector& Target, const FVector2D& Size, const FName& ArtSlot, const FLinearColor& DebugColor)
{
    UWorld* World = GetWorld();
    if (!World || !PlaneMesh)
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Name = ActorName;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    const FVector Facing = (Target - Location).GetSafeNormal();
    const FRotator Rotation = FRotationMatrix::MakeFromZ(Facing.IsNearlyZero() ? FVector::UpVector : Facing).Rotator();
    AStaticMeshActor* Plane = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation, SpawnParams);
    if (!Plane)
    {
        return nullptr;
    }

    Plane->SetActorScale3D(FVector(Size.X / 100.0f, Size.Y / 100.0f, 1.0f));
    UStaticMeshComponent* MeshComponent = Plane->GetStaticMeshComponent();
    if (MeshComponent)
    {
        MeshComponent->SetStaticMesh(PlaneMesh);
        MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        MeshComponent->SetCastShadow(false);
        MeshComponent->SetMobility(EComponentMobility::Static);
        if (UMaterialInterface* Material = ResolvePlaneMaterialForSlot(ArtSlot))
        {
            MeshComponent->SetMaterial(0, Material);
        }
        else if (UMaterialInstanceDynamic* DynamicMaterial = MeshComponent->CreateAndSetMaterialInstanceDynamic(0))
        {
            DynamicMaterial->SetVectorParameterValue(TEXT("Color"), DebugColor);
            DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), DebugColor);
        }
    }

    SpawnedActors.Add(Plane);
    return Plane;
}

void AAshfrontierWorldBlockoutDirector::LoadArtSlotData()
{
    ArtSlotMaterialPaths.Reset();
    ActorArtSlots.Reset();

    TSharedPtr<FJsonObject> MappingRoot;
    if (LoadContentJsonObject(TEXT("Data/Art/ArtSlotMapping.json"), MappingRoot))
    {
        const TArray<TSharedPtr<FJsonValue>>* Records = nullptr;
        if (MappingRoot->TryGetArrayField(TEXT("records"), Records))
        {
            for (const TSharedPtr<FJsonValue>& Value : *Records)
            {
                const TSharedPtr<FJsonObject> Record = Value.IsValid() ? Value->AsObject() : nullptr;
                FString SlotId;
                FString MaterialPath;
                if (ReadStringField(Record, TEXT("slot_id"), SlotId) && ReadStringField(Record, TEXT("material_instance_path"), MaterialPath))
                {
                    ArtSlotMaterialPaths.Add(FName(*SlotId), MaterialPath);
                }
            }
        }
    }

    TSharedPtr<FJsonObject> AssignmentsRoot;
    if (LoadContentJsonObject(TEXT("Data/Art/EnvironmentArtAssignments.json"), AssignmentsRoot))
    {
        const TArray<TSharedPtr<FJsonValue>>* Records = nullptr;
        if (AssignmentsRoot->TryGetArrayField(TEXT("records"), Records))
        {
            for (const TSharedPtr<FJsonValue>& Value : *Records)
            {
                const TSharedPtr<FJsonObject> Record = Value.IsValid() ? Value->AsObject() : nullptr;
                FString ActorName;
                FString SlotId;
                if (ReadStringField(Record, TEXT("actor_name"), ActorName) && ReadStringField(Record, TEXT("art_slot"), SlotId))
                {
                    ActorArtSlots.Add(FName(*ActorName), FName(*SlotId));
                }
            }
        }
    }
}

FName AAshfrontierWorldBlockoutDirector::FindArtSlotForActor(const FName& ActorName) const
{
    if (const FName* Slot = ActorArtSlots.Find(ActorName))
    {
        return *Slot;
    }
    return NAME_None;
}

UMaterialInterface* AAshfrontierWorldBlockoutDirector::ResolveMaterialForSlot(const FName& ArtSlot) const
{
    if (ArtSlot.IsNone())
    {
        return nullptr;
    }
    const FString* MaterialPath = ArtSlotMaterialPaths.Find(ArtSlot);
    if (!MaterialPath || MaterialPath->IsEmpty())
    {
        return nullptr;
    }
    return LoadObject<UMaterialInterface>(nullptr, **MaterialPath);
}

UMaterialInterface* AAshfrontierWorldBlockoutDirector::ResolvePlaneMaterialForSlot(const FName& ArtSlot) const
{
    const FString* MaterialPath = ArtSlotMaterialPaths.Find(ArtSlot);
    if (MaterialPath && MaterialPath->Contains(TEXT("MI_AfV02_Decal_")))
    {
        FString PlaneMaterialPath = *MaterialPath;
        PlaneMaterialPath.ReplaceInline(TEXT("MI_AfV02_Decal_"), TEXT("MI_AfV02_Signage_"));
        if (UMaterialInterface* PlaneMaterial = LoadObject<UMaterialInterface>(nullptr, *PlaneMaterialPath))
        {
            return PlaneMaterial;
        }
    }
    return ResolveMaterialForSlot(ArtSlot);
}

void AAshfrontierWorldBlockoutDirector::ApplyArtSlotMaterial(UStaticMeshComponent* MeshComponent, const FName& ActorName, const FLinearColor& DebugColor) const
{
    if (!MeshComponent)
    {
        return;
    }

    if (UMaterialInterface* Material = ResolveMaterialForSlot(FindArtSlotForActor(ActorName)))
    {
        MeshComponent->SetMaterial(0, Material);
        return;
    }

    if (UMaterialInstanceDynamic* DynamicMaterial = MeshComponent->CreateAndSetMaterialInstanceDynamic(0))
    {
        DynamicMaterial->SetVectorParameterValue(TEXT("Color"), DebugColor);
        DynamicMaterial->SetVectorParameterValue(TEXT("BaseColor"), DebugColor);
    }
}
