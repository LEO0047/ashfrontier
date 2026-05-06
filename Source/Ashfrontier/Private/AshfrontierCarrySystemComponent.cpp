#include "AshfrontierCarrySystemComponent.h"

#include "AshfrontierCharacter.h"
#include "AshfrontierDamageModelComponent.h"

UAshfrontierCarrySystemComponent::UAshfrontierCarrySystemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UAshfrontierCarrySystemComponent::CarryDownedCharacter(AAshfrontierCharacter* Carrier, AAshfrontierCharacter* Patient) const
{
    if (!IsValid(Carrier) || !IsValid(Patient) || Carrier == Patient || Carrier->GetCarriedTarget())
    {
        return false;
    }

    const UAshfrontierDamageModelComponent* DamageModel = Patient->GetDamageModel();
    if (!DamageModel || !DamageModel->IsDownedOrUnconscious())
    {
        return false;
    }

    Carrier->SetCarriedTarget(Patient);
    Patient->SetCarrier(Carrier);
    Patient->CommandHold();
    Patient->SetActorEnableCollision(false);
    return true;
}

bool UAshfrontierCarrySystemComponent::DropCarriedCharacter(AAshfrontierCharacter* Carrier) const
{
    if (!IsValid(Carrier))
    {
        return false;
    }

    AAshfrontierCharacter* Patient = Carrier->GetCarriedTarget();
    if (!IsValid(Patient))
    {
        return false;
    }

    Patient->SetCarrier(nullptr);
    Patient->SetActorEnableCollision(true);
    Carrier->SetCarriedTarget(nullptr);
    return true;
}
