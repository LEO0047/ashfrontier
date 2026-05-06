#pragma once

#include "CoreMinimal.h"
#include "AshfrontierBaseTypes.h"
#include "Components/ActorComponent.h"
#include "AshfrontierConstructionSystemComponent.generated.h"

class UAshfrontierInventoryComponent;

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierConstructionSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierConstructionSystemComponent();

    bool CanAffordCost(const UAshfrontierInventoryComponent* Inventory, const TArray<FAshfrontierItemQuantity>& Cost) const;
    bool SpendCost(UAshfrontierInventoryComponent* Inventory, const TArray<FAshfrontierItemQuantity>& Cost) const;
};
