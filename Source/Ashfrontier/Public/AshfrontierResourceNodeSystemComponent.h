#pragma once

#include "CoreMinimal.h"
#include "AshfrontierBaseTypes.h"
#include "Components/ActorComponent.h"
#include "AshfrontierResourceNodeSystemComponent.generated.h"

class AAshfrontierCharacter;
class AAshfrontierPlacedBuilding;

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierResourceNodeSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierResourceNodeSystemComponent();

    const FAshfrontierResourceDefinition* FindResourceDefinition(const FName& ResourceId) const;
    bool GatherResource(AAshfrontierCharacter* Gatherer, const FName& ResourceId, int32 Cycles) const;
    bool GatherFromBuilding(AAshfrontierCharacter* Gatherer, AAshfrontierPlacedBuilding* Building, int32 Cycles) const;

private:
    UPROPERTY(VisibleAnywhere, Category = "Resources")
    TMap<FName, FAshfrontierResourceDefinition> ResourceDefinitions;
};
