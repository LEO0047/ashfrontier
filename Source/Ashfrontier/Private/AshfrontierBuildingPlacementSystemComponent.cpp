#include "AshfrontierBuildingPlacementSystemComponent.h"

#include "AshfrontierCharacter.h"
#include "AshfrontierConstructionSystemComponent.h"
#include "AshfrontierInventoryComponent.h"
#include "AshfrontierPlacedBuilding.h"
#include "Engine/World.h"

namespace AshfrontierBuildZones
{
const FName WildernessBuildZone(TEXT("wilderness_build_zone"));
const FVector WildernessCenter(1600.0f, 1800.0f, 120.0f);
const FVector WildernessSize(1400.0f, 1000.0f, 240.0f);
}

UAshfrontierBuildingPlacementSystemComponent::UAshfrontierBuildingPlacementSystemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    AshfrontierBaseData::BuildDefaultBuildingDefinitions(BuildingDefinitions);
}

const FAshfrontierBuildingDefinition* UAshfrontierBuildingPlacementSystemComponent::FindBuildingDefinition(const FName& BuildingId) const
{
    return BuildingDefinitions.Find(BuildingId);
}

bool UAshfrontierBuildingPlacementSystemComponent::CanPlaceBuilding(AAshfrontierCharacter* Builder, const FName& BuildingId, const FVector& Location, const UAshfrontierConstructionSystemComponent* ConstructionSystem) const
{
    if (!IsValid(Builder) || !ConstructionSystem)
    {
        return false;
    }

    const FAshfrontierBuildingDefinition* Definition = FindBuildingDefinition(BuildingId);
    if (!Definition || !IsLocationInAllowedZone(Definition->AllowedZone, Location))
    {
        return false;
    }

    return ConstructionSystem->CanAffordCost(Builder->GetInventory(), Definition->Cost);
}

AAshfrontierPlacedBuilding* UAshfrontierBuildingPlacementSystemComponent::PlaceBuilding(AAshfrontierCharacter* Builder, const FName& BuildingId, const FVector& Location, const UAshfrontierConstructionSystemComponent* ConstructionSystem) const
{
    if (!CanPlaceBuilding(Builder, BuildingId, Location, ConstructionSystem))
    {
        return nullptr;
    }

    const FAshfrontierBuildingDefinition* Definition = FindBuildingDefinition(BuildingId);
    if (!Definition || !ConstructionSystem->SpendCost(Builder->GetInventory(), Definition->Cost))
    {
        return nullptr;
    }

    UWorld* World = Builder->GetWorld();
    if (!World)
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Builder;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    const FVector SpawnLocation(Location.X, Location.Y, 120.0f);
    AAshfrontierPlacedBuilding* Building = World->SpawnActor<AAshfrontierPlacedBuilding>(AAshfrontierPlacedBuilding::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
    if (Building)
    {
        Building->InitializeBuilding(*Definition);
    }

    return Building;
}

bool UAshfrontierBuildingPlacementSystemComponent::IsLocationInAllowedZone(const FName& AllowedZone, const FVector& Location) const
{
    if (AllowedZone != AshfrontierBuildZones::WildernessBuildZone)
    {
        return false;
    }

    const FVector Delta = Location - AshfrontierBuildZones::WildernessCenter;
    return FMath::Abs(Delta.X) <= AshfrontierBuildZones::WildernessSize.X * 0.5f
        && FMath::Abs(Delta.Y) <= AshfrontierBuildZones::WildernessSize.Y * 0.5f;
}
