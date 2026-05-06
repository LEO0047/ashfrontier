#include "AshfrontierGuardAIComponent.h"

#include "AshfrontierCharacter.h"
#include "AshfrontierCrimeEventMemoryComponent.h"
#include "AshfrontierFactionSystemComponent.h"
#include "AshfrontierLegalRuleSystemComponent.h"

namespace AshfrontierLegal
{
const FName PlayerFactionId(TEXT("faction_player_squad"));
}

UAshfrontierGuardAIComponent::UAshfrontierGuardAIComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

EAshfrontierGuardReaction UAshfrontierGuardAIComponent::HandleObservedEvent(
    AAshfrontierCharacter* Guard,
    AAshfrontierCharacter* Offender,
    AAshfrontierCharacter* Victim,
    EAshfrontierLegalEventType EventType,
    const FName& LegalProfileId,
    const UAshfrontierLegalRuleSystemComponent* LegalRules,
    UAshfrontierFactionSystemComponent* Factions,
    UAshfrontierCrimeEventMemoryComponent* Memory)
{
    (void)Victim;

    if (!IsValid(Guard) || !IsValid(Offender) || !LegalRules || !Factions || !Memory)
    {
        LastReaction = EAshfrontierGuardReaction::Allow;
        GuardState = EAshfrontierGuardState::Idle;
        return LastReaction;
    }

    FAshfrontierLegalRule Rule;
    if (!LegalRules->EvaluateLegalEvent(LegalProfileId, EventType, Rule))
    {
        LastReaction = EAshfrontierGuardReaction::Allow;
        GuardState = EAshfrontierGuardState::Allowing;
        return LastReaction;
    }

    const FName OffenderFactionId = Offender->GetFactionId().IsNone() ? AshfrontierLegal::PlayerFactionId : Offender->GetFactionId();
    if (Rule.RelationDelta != 0)
    {
        Factions->AdjustRelation(OffenderFactionId, Rule.AuthorityFactionId, Rule.RelationDelta);
    }

    FAshfrontierCrimeEventRecord EventRecord;
    EventRecord.EventType = EventType;
    EventRecord.Reaction = Rule.Reaction;
    EventRecord.OffenderFactionId = OffenderFactionId;
    EventRecord.AuthorityFactionId = Rule.AuthorityFactionId;
    EventRecord.RelationDelta = Rule.RelationDelta;
    EventRecord.RemainingMemorySeconds = static_cast<float>(Rule.MemorySeconds);
    EventRecord.SummaryZh = FString::Printf(TEXT("%s -> %s"), *LegalRules->GetEventLabelZh(EventType), *LegalRules->GetReactionLabelZh(Rule.Reaction));
    Memory->RecordEvent(EventRecord);

    LastReaction = Rule.Reaction;
    switch (Rule.Reaction)
    {
    case EAshfrontierGuardReaction::Warning:
        GuardState = EAshfrontierGuardState::Warning;
        Guard->SetCharacterTeam(EAshfrontierCharacterTeam::Neutral);
        break;
    case EAshfrontierGuardReaction::Hostile:
        GuardState = EAshfrontierGuardState::Hostile;
        Guard->SetCharacterTeam(EAshfrontierCharacterTeam::Hostile);
        break;
    case EAshfrontierGuardReaction::Pursuit:
        GuardState = EAshfrontierGuardState::Pursuit;
        Guard->SetCharacterTeam(EAshfrontierCharacterTeam::Hostile);
        break;
    case EAshfrontierGuardReaction::Allow:
    default:
        GuardState = EAshfrontierGuardState::Allowing;
        Guard->SetCharacterTeam(EAshfrontierCharacterTeam::Neutral);
        break;
    }

    return LastReaction;
}

EAshfrontierGuardState UAshfrontierGuardAIComponent::GetGuardState() const
{
    return GuardState;
}

EAshfrontierGuardReaction UAshfrontierGuardAIComponent::GetLastReaction() const
{
    return LastReaction;
}

FString UAshfrontierGuardAIComponent::GetStateLabelZh() const
{
    switch (GuardState)
    {
    case EAshfrontierGuardState::Warning:
        return TEXT("警告");
    case EAshfrontierGuardState::Hostile:
        return TEXT("敵對");
    case EAshfrontierGuardState::Pursuit:
        return TEXT("追捕");
    case EAshfrontierGuardState::Allowing:
        return TEXT("放行");
    case EAshfrontierGuardState::Idle:
    default:
        return TEXT("巡守");
    }
}
