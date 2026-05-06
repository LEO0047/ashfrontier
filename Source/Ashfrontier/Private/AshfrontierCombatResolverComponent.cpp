#include "AshfrontierCombatResolverComponent.h"

#include "AshfrontierCharacter.h"
#include "AshfrontierDamageModelComponent.h"

UAshfrontierCombatResolverComponent::UAshfrontierCombatResolverComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

float UAshfrontierCombatResolverComponent::ResolveMeleeAttack(AAshfrontierCharacter* Attacker, AAshfrontierCharacter* Target, EAshfrontierBodyPart TargetPart, float DamageAmount) const
{
    if (!IsValid(Attacker) || !IsValid(Target) || Attacker == Target)
    {
        return 0.0f;
    }

    UAshfrontierDamageModelComponent* DamageModel = Target->GetDamageModel();
    if (!DamageModel || DamageModel->IsDownedOrUnconscious())
    {
        return 0.0f;
    }

    return DamageModel->ApplyDamage(TargetPart, DamageAmount);
}
