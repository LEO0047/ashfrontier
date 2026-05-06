#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AshfrontierCarrySystemComponent.generated.h"

class AAshfrontierCharacter;

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierCarrySystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierCarrySystemComponent();

    bool CarryDownedCharacter(AAshfrontierCharacter* Carrier, AAshfrontierCharacter* Patient) const;
    bool DropCarriedCharacter(AAshfrontierCharacter* Carrier) const;
};
