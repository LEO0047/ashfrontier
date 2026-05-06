#include "AshfrontierDamageModelComponent.h"

UAshfrontierDamageModelComponent::UAshfrontierDamageModelComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    EnsureBodyParts();
}

void UAshfrontierDamageModelComponent::BeginPlay()
{
    Super::BeginPlay();
    EnsureBodyParts();
}

float UAshfrontierDamageModelComponent::ApplyDamage(EAshfrontierBodyPart BodyPart, float DamageAmount)
{
    EnsureBodyParts();
    DamageAmount = FMath::Max(0.0f, DamageAmount);

    FAshfrontierBodyPartHealth* TargetPart = FindBodyPart(BodyPart);
    if (!TargetPart || DamageAmount <= 0.0f)
    {
        return 0.0f;
    }

    const float PreviousHealth = TargetPart->CurrentHealth;
    TargetPart->CurrentHealth = FMath::Clamp(TargetPart->CurrentHealth - DamageAmount, 0.0f, TargetPart->MaxHealth);
    TargetPart->bCrippled = TargetPart->CurrentHealth <= 0.0f;
    BleedingRate += DamageAmount * 0.035f;
    RecalculateState();

    return PreviousHealth - TargetPart->CurrentHealth;
}

void UAshfrontierDamageModelComponent::StabilizeBleeding()
{
    BleedingRate = 0.0f;
    if (ConsciousState == EAshfrontierConsciousState::Unconscious || ConsciousState == EAshfrontierConsciousState::Downed)
    {
        ConsciousState = EAshfrontierConsciousState::Stable;
    }
}

void UAshfrontierDamageModelComponent::ResetHealth()
{
    BodyParts.Reset();
    BleedingRate = 0.0f;
    ConsciousState = EAshfrontierConsciousState::Conscious;
    EnsureBodyParts();
}

float UAshfrontierDamageModelComponent::GetBodyPartHealth(EAshfrontierBodyPart BodyPart) const
{
    const FAshfrontierBodyPartHealth* TargetPart = FindBodyPart(BodyPart);
    return TargetPart ? TargetPart->CurrentHealth : 0.0f;
}

float UAshfrontierDamageModelComponent::GetBleedingRate() const
{
    return BleedingRate;
}

EAshfrontierConsciousState UAshfrontierDamageModelComponent::GetConsciousState() const
{
    return ConsciousState;
}

bool UAshfrontierDamageModelComponent::IsDownedOrUnconscious() const
{
    return ConsciousState == EAshfrontierConsciousState::Downed
        || ConsciousState == EAshfrontierConsciousState::Unconscious
        || ConsciousState == EAshfrontierConsciousState::Stable;
}

FString UAshfrontierDamageModelComponent::GetStateLabelZh() const
{
    switch (ConsciousState)
    {
    case EAshfrontierConsciousState::Downed:
        return TEXT("倒地");
    case EAshfrontierConsciousState::Unconscious:
        return TEXT("昏迷");
    case EAshfrontierConsciousState::Stable:
        return TEXT("已穩定");
    case EAshfrontierConsciousState::Conscious:
    default:
        return BleedingRate > 0.0f ? TEXT("流血") : TEXT("正常");
    }
}

void UAshfrontierDamageModelComponent::EnsureBodyParts()
{
    if (BodyParts.Num() > 0)
    {
        return;
    }

    const EAshfrontierBodyPart Parts[] = {
        EAshfrontierBodyPart::Head,
        EAshfrontierBodyPart::Chest,
        EAshfrontierBodyPart::Abdomen,
        EAshfrontierBodyPart::LeftArm,
        EAshfrontierBodyPart::RightArm,
        EAshfrontierBodyPart::LeftLeg,
        EAshfrontierBodyPart::RightLeg
    };

    for (EAshfrontierBodyPart Part : Parts)
    {
        FAshfrontierBodyPartHealth Entry;
        Entry.BodyPart = Part;
        Entry.MaxHealth = (Part == EAshfrontierBodyPart::Head) ? 70.0f : 100.0f;
        Entry.CurrentHealth = Entry.MaxHealth;
        BodyParts.Add(Entry);
    }
}

FAshfrontierBodyPartHealth* UAshfrontierDamageModelComponent::FindBodyPart(EAshfrontierBodyPart BodyPart)
{
    EnsureBodyParts();
    return BodyParts.FindByPredicate([BodyPart](const FAshfrontierBodyPartHealth& Entry)
    {
        return Entry.BodyPart == BodyPart;
    });
}

const FAshfrontierBodyPartHealth* UAshfrontierDamageModelComponent::FindBodyPart(EAshfrontierBodyPart BodyPart) const
{
    return BodyParts.FindByPredicate([BodyPart](const FAshfrontierBodyPartHealth& Entry)
    {
        return Entry.BodyPart == BodyPart;
    });
}

void UAshfrontierDamageModelComponent::RecalculateState()
{
    const float Head = GetBodyPartHealth(EAshfrontierBodyPart::Head);
    const float Chest = GetBodyPartHealth(EAshfrontierBodyPart::Chest);
    const float Abdomen = GetBodyPartHealth(EAshfrontierBodyPart::Abdomen);
    const float LeftLeg = GetBodyPartHealth(EAshfrontierBodyPart::LeftLeg);
    const float RightLeg = GetBodyPartHealth(EAshfrontierBodyPart::RightLeg);

    if (Head <= 0.0f || Chest <= 0.0f || Abdomen <= 0.0f)
    {
        ConsciousState = EAshfrontierConsciousState::Unconscious;
        return;
    }

    if (LeftLeg <= 0.0f || RightLeg <= 0.0f || GetTotalHealth() <= 360.0f)
    {
        ConsciousState = EAshfrontierConsciousState::Downed;
        return;
    }

    ConsciousState = EAshfrontierConsciousState::Conscious;
}

float UAshfrontierDamageModelComponent::GetTotalHealth() const
{
    float Total = 0.0f;
    for (const FAshfrontierBodyPartHealth& Entry : BodyParts)
    {
        Total += Entry.CurrentHealth;
    }
    return Total;
}
