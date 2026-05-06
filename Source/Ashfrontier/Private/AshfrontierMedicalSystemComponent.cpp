#include "AshfrontierMedicalSystemComponent.h"

#include "AshfrontierCharacter.h"
#include "AshfrontierDamageModelComponent.h"

UAshfrontierMedicalSystemComponent::UAshfrontierMedicalSystemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UAshfrontierMedicalSystemComponent::Bandage(AAshfrontierCharacter* Medic, AAshfrontierCharacter* Patient) const
{
    if (!IsValid(Medic) || !IsValid(Patient))
    {
        return false;
    }

    UAshfrontierDamageModelComponent* DamageModel = Patient->GetDamageModel();
    if (!DamageModel || DamageModel->GetBleedingRate() <= 0.0f)
    {
        return false;
    }

    DamageModel->StabilizeBleeding();
    return true;
}
