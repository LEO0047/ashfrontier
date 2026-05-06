#pragma once

#include "CoreMinimal.h"
#include "AshfrontierLegalTypes.h"
#include "Components/ActorComponent.h"
#include "AshfrontierGuardAIComponent.generated.h"

class AAshfrontierCharacter;
class UAshfrontierCrimeEventMemoryComponent;
class UAshfrontierFactionSystemComponent;
class UAshfrontierLegalRuleSystemComponent;

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierGuardAIComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierGuardAIComponent();

    EAshfrontierGuardReaction HandleObservedEvent(
        AAshfrontierCharacter* Guard,
        AAshfrontierCharacter* Offender,
        AAshfrontierCharacter* Victim,
        EAshfrontierLegalEventType EventType,
        const FName& LegalProfileId,
        const UAshfrontierLegalRuleSystemComponent* LegalRules,
        UAshfrontierFactionSystemComponent* Factions,
        UAshfrontierCrimeEventMemoryComponent* Memory);

    EAshfrontierGuardState GetGuardState() const;
    EAshfrontierGuardReaction GetLastReaction() const;
    FString GetStateLabelZh() const;

private:
    UPROPERTY(VisibleAnywhere, Category = "Guard")
    EAshfrontierGuardState GuardState = EAshfrontierGuardState::Idle;

    UPROPERTY(VisibleAnywhere, Category = "Guard")
    EAshfrontierGuardReaction LastReaction = EAshfrontierGuardReaction::Allow;
};
