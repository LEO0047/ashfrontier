#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AshfrontierMedicalSystemComponent.generated.h"

class AAshfrontierCharacter;

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierMedicalSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierMedicalSystemComponent();

    bool Bandage(AAshfrontierCharacter* Medic, AAshfrontierCharacter* Patient) const;
};
