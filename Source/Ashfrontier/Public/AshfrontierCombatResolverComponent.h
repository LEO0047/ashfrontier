#pragma once

#include "CoreMinimal.h"
#include "AshfrontierCombatTypes.h"
#include "Components/ActorComponent.h"
#include "AshfrontierCombatResolverComponent.generated.h"

class AAshfrontierCharacter;

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierCombatResolverComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierCombatResolverComponent();

    float ResolveMeleeAttack(AAshfrontierCharacter* Attacker, AAshfrontierCharacter* Target, EAshfrontierBodyPart TargetPart = EAshfrontierBodyPart::Chest, float DamageAmount = 38.0f) const;
};
