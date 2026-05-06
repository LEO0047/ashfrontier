#if WITH_DEV_AUTOMATION_TESTS

#include "AshfrontierCharacter.h"
#include "AshfrontierCrimeEventMemoryComponent.h"
#include "AshfrontierFactionSystemComponent.h"
#include "AshfrontierGuardAIComponent.h"
#include "AshfrontierLegalRuleSystemComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/AutomationTest.h"

namespace AshfrontierLegalTest
{
UWorld* FindEditorTestWorld()
{
    if (!GEngine)
    {
        return nullptr;
    }

    for (const FWorldContext& Context : GEngine->GetWorldContexts())
    {
        UWorld* World = Context.World();
        if (World && (Context.WorldType == EWorldType::Editor || Context.WorldType == EWorldType::PIE || Context.WorldType == EWorldType::Game))
        {
            return World;
        }
    }

    return nullptr;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FAshfrontierLegalGuardReactionTest,
    "Ashfrontier.Legal.GuardReactionMemory",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAshfrontierLegalGuardReactionTest::RunTest(const FString& Parameters)
{
    using namespace AshfrontierLegalTest;

    UWorld* World = FindEditorTestWorld();
    if (!TestNotNull(TEXT("Editor world should be available"), World))
    {
        return false;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.ObjectFlags |= RF_Transient;

    AAshfrontierCharacter* Guard = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), FVector(0.0f, 0.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
    AAshfrontierCharacter* Offender = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), FVector(120.0f, 0.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
    AAshfrontierCharacter* Victim = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), FVector(240.0f, 0.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);

    UObject* ComponentOuter = Guard ? Cast<UObject>(Guard) : GetTransientPackage();
    UAshfrontierFactionSystemComponent* Factions = NewObject<UAshfrontierFactionSystemComponent>(ComponentOuter);
    UAshfrontierLegalRuleSystemComponent* LegalRules = NewObject<UAshfrontierLegalRuleSystemComponent>(ComponentOuter);
    UAshfrontierCrimeEventMemoryComponent* Memory = NewObject<UAshfrontierCrimeEventMemoryComponent>(ComponentOuter);
    UAshfrontierGuardAIComponent* GuardAI = NewObject<UAshfrontierGuardAIComponent>(ComponentOuter);

    bool bPassed = TestNotNull(TEXT("Guard should spawn"), Guard)
        && TestNotNull(TEXT("Offender should spawn"), Offender)
        && TestNotNull(TEXT("Victim should spawn"), Victim)
        && TestNotNull(TEXT("Faction system should exist"), Factions)
        && TestNotNull(TEXT("Legal rules should exist"), LegalRules)
        && TestNotNull(TEXT("Crime memory should exist"), Memory)
        && TestNotNull(TEXT("Guard AI should exist"), GuardAI);

    if (!bPassed)
    {
        if (Guard) { Guard->Destroy(); }
        if (Offender) { Offender->Destroy(); }
        if (Victim) { Victim->Destroy(); }
        return false;
    }

    Guard->SetFactionId(TEXT("faction_saltwardens"));
    Guard->SetCharacterTeam(EAshfrontierCharacterTeam::Neutral);
    Offender->SetFactionId(TEXT("faction_player_squad"));
    Offender->SetCharacterTeam(EAshfrontierCharacterTeam::PlayerSquad);
    Victim->SetFactionId(TEXT("faction_glasshouse"));

    bPassed &= TestEqual(TEXT("Player should start neutral with Saltwardens"), Factions->GetRelation(TEXT("faction_player_squad"), TEXT("faction_saltwardens")), 0);

    EAshfrontierGuardReaction Reaction = GuardAI->HandleObservedEvent(Guard, Offender, Victim, EAshfrontierLegalEventType::Theft, TEXT("legal_saltwardens_city"), LegalRules, Factions, Memory);
    bPassed &= TestTrue(TEXT("Theft should trigger warning in Saltwardens city"), Reaction == EAshfrontierGuardReaction::Warning);
    bPassed &= TestTrue(TEXT("Guard state should be warning"), GuardAI->GetGuardState() == EAshfrontierGuardState::Warning);
    bPassed &= TestEqual(TEXT("Theft should reduce Saltwardens relation"), Factions->GetRelation(TEXT("faction_player_squad"), TEXT("faction_saltwardens")), -3);
    bPassed &= TestTrue(TEXT("Theft should create event memory"), Memory->HasMemoryFor(TEXT("faction_player_squad"), EAshfrontierLegalEventType::Theft));
    bPassed &= TestFalse(TEXT("Warning should not make guard hostile"), Guard->IsHostileToPlayer());

    Reaction = GuardAI->HandleObservedEvent(Guard, Offender, Victim, EAshfrontierLegalEventType::Attack, TEXT("legal_saltwardens_city"), LegalRules, Factions, Memory);
    bPassed &= TestTrue(TEXT("Attack should trigger pursuit"), Reaction == EAshfrontierGuardReaction::Pursuit);
    bPassed &= TestTrue(TEXT("Guard state should be pursuit"), GuardAI->GetGuardState() == EAshfrontierGuardState::Pursuit);
    bPassed &= TestEqual(TEXT("Attack should further reduce relation"), Factions->GetRelation(TEXT("faction_player_squad"), TEXT("faction_saltwardens")), -15);
    bPassed &= TestTrue(TEXT("Pursuit should make guard hostile to player"), Guard->IsHostileToPlayer());

    const int32 RelationBeforeSelfDefense = Factions->GetRelation(TEXT("faction_player_squad"), TEXT("faction_saltwardens"));
    Reaction = GuardAI->HandleObservedEvent(Guard, Offender, Victim, EAshfrontierLegalEventType::SelfDefense, TEXT("legal_saltwardens_city"), LegalRules, Factions, Memory);
    bPassed &= TestTrue(TEXT("Self defense should be allowed"), Reaction == EAshfrontierGuardReaction::Allow);
    bPassed &= TestEqual(TEXT("Self defense should not reduce relation"), Factions->GetRelation(TEXT("faction_player_squad"), TEXT("faction_saltwardens")), RelationBeforeSelfDefense);
    bPassed &= TestFalse(TEXT("Allowed self defense should clear guard hostility"), Guard->IsHostileToPlayer());

    Reaction = GuardAI->HandleObservedEvent(Guard, Offender, Victim, EAshfrontierLegalEventType::RestrictedArea, TEXT("legal_saltwardens_city"), LegalRules, Factions, Memory);
    bPassed &= TestTrue(TEXT("Restricted area should trigger warning"), Reaction == EAshfrontierGuardReaction::Warning);
    bPassed &= TestEqual(TEXT("Restricted area should reduce relation"), Factions->GetRelation(TEXT("faction_player_squad"), TEXT("faction_saltwardens")), -17);

    Reaction = GuardAI->HandleObservedEvent(Guard, Offender, Victim, EAshfrontierLegalEventType::Theft, TEXT("legal_glasshouse_market"), LegalRules, Factions, Memory);
    bPassed &= TestTrue(TEXT("Glasshouse theft should trigger pursuit"), Reaction == EAshfrontierGuardReaction::Pursuit);
    bPassed &= TestEqual(TEXT("Glasshouse relation should be affected independently"), Factions->GetRelation(TEXT("faction_player_squad"), TEXT("faction_glasshouse")), -8);
    bPassed &= TestTrue(TEXT("Memory should contain multiple legal events"), Memory->GetMemoryCount() >= 4);

    Memory->TickMemory(1000.0f);
    bPassed &= TestEqual(TEXT("Expired legal memory should be removed"), Memory->GetMemoryCount(), 0);

    Guard->Destroy();
    Offender->Destroy();
    Victim->Destroy();

    return bPassed;
}

#endif
