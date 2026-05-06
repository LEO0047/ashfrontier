#pragma once

#include "CoreMinimal.h"
#include "AshfrontierCombatTypes.h"
#include "Components/ActorComponent.h"
#include "AshfrontierDamageModelComponent.generated.h"

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierDamageModelComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierDamageModelComponent();

    virtual void BeginPlay() override;

    float ApplyDamage(EAshfrontierBodyPart BodyPart, float DamageAmount);
    void StabilizeBleeding();
    void ResetHealth();

    float GetBodyPartHealth(EAshfrontierBodyPart BodyPart) const;
    float GetBleedingRate() const;
    EAshfrontierConsciousState GetConsciousState() const;
    const TArray<FAshfrontierBodyPartHealth>& GetBodyPartsForSave() const;
    void RestoreFromSave(const TArray<FAshfrontierBodyPartHealth>& SavedBodyParts, float SavedBleedingRate, EAshfrontierConsciousState SavedConsciousState);
    bool IsDownedOrUnconscious() const;
    FString GetStateLabelZh() const;

private:
    void EnsureBodyParts();
    FAshfrontierBodyPartHealth* FindBodyPart(EAshfrontierBodyPart BodyPart);
    const FAshfrontierBodyPartHealth* FindBodyPart(EAshfrontierBodyPart BodyPart) const;
    void RecalculateState();
    float GetTotalHealth() const;

    UPROPERTY(VisibleAnywhere, Category = "Damage")
    TArray<FAshfrontierBodyPartHealth> BodyParts;

    UPROPERTY(VisibleAnywhere, Category = "Damage")
    float BleedingRate = 0.0f;

    UPROPERTY(VisibleAnywhere, Category = "Damage")
    EAshfrontierConsciousState ConsciousState = EAshfrontierConsciousState::Conscious;
};
