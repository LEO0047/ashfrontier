#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AshfrontierStorageSystemComponent.generated.h"

class AAshfrontierCharacter;
class AAshfrontierPlacedBuilding;

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierStorageSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierStorageSystemComponent();

    bool DepositItem(AAshfrontierCharacter* FromCharacter, AAshfrontierPlacedBuilding* ToBuilding, const FName& ItemId, int32 Count) const;
    bool WithdrawItem(AAshfrontierPlacedBuilding* FromBuilding, AAshfrontierCharacter* ToCharacter, const FName& ItemId, int32 Count) const;
};
