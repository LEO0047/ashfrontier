#include "AshfrontierLegalRuleSystemComponent.h"

UAshfrontierLegalRuleSystemComponent::UAshfrontierLegalRuleSystemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    BuildDefaultRules();
}

bool UAshfrontierLegalRuleSystemComponent::EvaluateLegalEvent(const FName& LegalProfileId, EAshfrontierLegalEventType EventType, FAshfrontierLegalRule& OutRule) const
{
    const FAshfrontierLegalRule* Rule = LegalRules.FindByPredicate([LegalProfileId, EventType](const FAshfrontierLegalRule& Entry)
    {
        return Entry.LegalProfileId == LegalProfileId && Entry.EventType == EventType;
    });

    if (!Rule)
    {
        return false;
    }

    OutRule = *Rule;
    return true;
}

FString UAshfrontierLegalRuleSystemComponent::GetReactionLabelZh(EAshfrontierGuardReaction Reaction) const
{
    switch (Reaction)
    {
    case EAshfrontierGuardReaction::Warning:
        return TEXT("警告");
    case EAshfrontierGuardReaction::Hostile:
        return TEXT("敵對");
    case EAshfrontierGuardReaction::Pursuit:
        return TEXT("追捕");
    case EAshfrontierGuardReaction::Allow:
    default:
        return TEXT("放行");
    }
}

FString UAshfrontierLegalRuleSystemComponent::GetEventLabelZh(EAshfrontierLegalEventType EventType) const
{
    switch (EventType)
    {
    case EAshfrontierLegalEventType::Theft:
        return TEXT("偷竊");
    case EAshfrontierLegalEventType::Attack:
        return TEXT("攻擊");
    case EAshfrontierLegalEventType::SelfDefense:
        return TEXT("自衛");
    case EAshfrontierLegalEventType::RestrictedArea:
    default:
        return TEXT("禁區闖入");
    }
}

void UAshfrontierLegalRuleSystemComponent::BuildDefaultRules()
{
    LegalRules.Reset();

    AddRule(TEXT("legal_saltwardens_city"), TEXT("faction_saltwardens"), EAshfrontierLegalEventType::Theft, EAshfrontierGuardReaction::Warning, 300, -3);
    AddRule(TEXT("legal_saltwardens_city"), TEXT("faction_saltwardens"), EAshfrontierLegalEventType::Attack, EAshfrontierGuardReaction::Pursuit, 900, -12);
    AddRule(TEXT("legal_saltwardens_city"), TEXT("faction_saltwardens"), EAshfrontierLegalEventType::SelfDefense, EAshfrontierGuardReaction::Allow, 120, 0);
    AddRule(TEXT("legal_saltwardens_city"), TEXT("faction_saltwardens"), EAshfrontierLegalEventType::RestrictedArea, EAshfrontierGuardReaction::Warning, 240, -2);

    AddRule(TEXT("legal_glasshouse_market"), TEXT("faction_glasshouse"), EAshfrontierLegalEventType::Theft, EAshfrontierGuardReaction::Pursuit, 600, -8);
    AddRule(TEXT("legal_glasshouse_market"), TEXT("faction_glasshouse"), EAshfrontierLegalEventType::Attack, EAshfrontierGuardReaction::Hostile, 900, -15);
    AddRule(TEXT("legal_glasshouse_market"), TEXT("faction_glasshouse"), EAshfrontierLegalEventType::SelfDefense, EAshfrontierGuardReaction::Allow, 120, 0);
    AddRule(TEXT("legal_glasshouse_market"), TEXT("faction_glasshouse"), EAshfrontierLegalEventType::RestrictedArea, EAshfrontierGuardReaction::Warning, 180, -1);

    AddRule(TEXT("legal_dustrunners_camp"), TEXT("faction_dustrunners"), EAshfrontierLegalEventType::Theft, EAshfrontierGuardReaction::Hostile, 480, -10);
    AddRule(TEXT("legal_dustrunners_camp"), TEXT("faction_dustrunners"), EAshfrontierLegalEventType::Attack, EAshfrontierGuardReaction::Hostile, 900, -16);
    AddRule(TEXT("legal_dustrunners_camp"), TEXT("faction_dustrunners"), EAshfrontierLegalEventType::SelfDefense, EAshfrontierGuardReaction::Allow, 90, 0);
    AddRule(TEXT("legal_dustrunners_camp"), TEXT("faction_dustrunners"), EAshfrontierLegalEventType::RestrictedArea, EAshfrontierGuardReaction::Warning, 180, -2);
}

void UAshfrontierLegalRuleSystemComponent::AddRule(const FName& LegalProfileId, const FName& AuthorityFactionId, EAshfrontierLegalEventType EventType, EAshfrontierGuardReaction Reaction, int32 MemorySeconds, int32 RelationDelta)
{
    FAshfrontierLegalRule Rule;
    Rule.LegalProfileId = LegalProfileId;
    Rule.AuthorityFactionId = AuthorityFactionId;
    Rule.EventType = EventType;
    Rule.Reaction = Reaction;
    Rule.MemorySeconds = MemorySeconds;
    Rule.RelationDelta = RelationDelta;
    LegalRules.Add(Rule);
}
