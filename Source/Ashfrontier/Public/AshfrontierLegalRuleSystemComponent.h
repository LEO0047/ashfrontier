#pragma once

#include "CoreMinimal.h"
#include "AshfrontierLegalTypes.h"
#include "Components/ActorComponent.h"
#include "AshfrontierLegalRuleSystemComponent.generated.h"

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierLegalRuleSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierLegalRuleSystemComponent();

    bool EvaluateLegalEvent(const FName& LegalProfileId, EAshfrontierLegalEventType EventType, FAshfrontierLegalRule& OutRule) const;
    FString GetReactionLabelZh(EAshfrontierGuardReaction Reaction) const;
    FString GetEventLabelZh(EAshfrontierLegalEventType EventType) const;

private:
    void BuildDefaultRules();
    void AddRule(const FName& LegalProfileId, const FName& AuthorityFactionId, EAshfrontierLegalEventType EventType, EAshfrontierGuardReaction Reaction, int32 MemorySeconds, int32 RelationDelta);

    UPROPERTY(VisibleAnywhere, Category = "Legal")
    TArray<FAshfrontierLegalRule> LegalRules;
};
