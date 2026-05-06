#pragma once

#include "CoreMinimal.h"
#include "AshfrontierBaseTypes.h"
#include "Components/ActorComponent.h"
#include "AshfrontierBuildingPlacementSystemComponent.generated.h"

class AAshfrontierCharacter;
class AAshfrontierPlacedBuilding;
class UAshfrontierConstructionSystemComponent;

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierBuildingPlacementSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierBuildingPlacementSystemComponent();

    const FAshfrontierBuildingDefinition* FindBuildingDefinition(const FName& BuildingId) const;
    bool CanPlaceBuilding(AAshfrontierCharacter* Builder, const FName& BuildingId, const FVector& Location, const UAshfrontierConstructionSystemComponent* ConstructionSystem) const;
    AAshfrontierPlacedBuilding* PlaceBuilding(AAshfrontierCharacter* Builder, const FName& BuildingId, const FVector& Location, const UAshfrontierConstructionSystemComponent* ConstructionSystem) const;
    bool IsLocationInAllowedZone(const FName& AllowedZone, const FVector& Location) const;

private:
    UPROPERTY(VisibleAnywhere, Category = "Building")
    TMap<FName, FAshfrontierBuildingDefinition> BuildingDefinitions;
};
