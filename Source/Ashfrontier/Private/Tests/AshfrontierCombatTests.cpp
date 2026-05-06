#if WITH_DEV_AUTOMATION_TESTS

#include "AshfrontierCarrySystemComponent.h"
#include "AshfrontierCharacter.h"
#include "AshfrontierCombatResolverComponent.h"
#include "AshfrontierCombatTypes.h"
#include "AshfrontierDamageModelComponent.h"
#include "AshfrontierMedicalSystemComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/AutomationTest.h"

namespace AshfrontierCombatTest
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
    FAshfrontierCombatDamageCarryMedicalTest,
    "Ashfrontier.Combat.DamageCarryMedical",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAshfrontierCombatDamageCarryMedicalTest::RunTest(const FString& Parameters)
{
    using namespace AshfrontierCombatTest;

    UWorld* World = FindEditorTestWorld();
    if (!TestNotNull(TEXT("Editor world should be available"), World))
    {
        return false;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.ObjectFlags |= RF_Transient;

    AAshfrontierCharacter* Attacker = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), FVector(0.0f, 0.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
    AAshfrontierCharacter* Patient = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), FVector(160.0f, 0.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
    UAshfrontierCombatResolverComponent* Combat = NewObject<UAshfrontierCombatResolverComponent>(Attacker);
    UAshfrontierCarrySystemComponent* Carry = NewObject<UAshfrontierCarrySystemComponent>(Attacker);
    UAshfrontierMedicalSystemComponent* Medical = NewObject<UAshfrontierMedicalSystemComponent>(Attacker);

    bool bPassed = TestNotNull(TEXT("Attacker should spawn"), Attacker)
        && TestNotNull(TEXT("Patient should spawn"), Patient)
        && TestNotNull(TEXT("Combat resolver should exist"), Combat)
        && TestNotNull(TEXT("Carry system should exist"), Carry)
        && TestNotNull(TEXT("Medical system should exist"), Medical);

    if (!bPassed)
    {
        if (Attacker)
        {
            Attacker->Destroy();
        }
        if (Patient)
        {
            Patient->Destroy();
        }
        return false;
    }

    Attacker->SetCharacterTeam(EAshfrontierCharacterTeam::PlayerSquad);
    Patient->SetCharacterTeam(EAshfrontierCharacterTeam::Hostile);

    UAshfrontierDamageModelComponent* DamageModel = Patient->GetDamageModel();
    bPassed &= TestNotNull(TEXT("Patient should have damage model"), DamageModel);
    if (!DamageModel)
    {
        Attacker->Destroy();
        Patient->Destroy();
        return false;
    }

    const float ChestBefore = DamageModel->GetBodyPartHealth(EAshfrontierBodyPart::Chest);
    Combat->ResolveMeleeAttack(Attacker, Patient, EAshfrontierBodyPart::Chest, 42.0f);
    bPassed &= TestTrue(TEXT("Chest should take body part damage"), DamageModel->GetBodyPartHealth(EAshfrontierBodyPart::Chest) < ChestBefore);
    bPassed &= TestTrue(TEXT("Damage should start bleeding"), DamageModel->GetBleedingRate() > 0.0f);

    Combat->ResolveMeleeAttack(Attacker, Patient, EAshfrontierBodyPart::Chest, 42.0f);
    Combat->ResolveMeleeAttack(Attacker, Patient, EAshfrontierBodyPart::Chest, 42.0f);
    bPassed &= TestTrue(TEXT("Patient should become downed or unconscious"), DamageModel->IsDownedOrUnconscious());

    bPassed &= TestTrue(TEXT("Carrier should be able to carry downed patient"), Carry->CarryDownedCharacter(Attacker, Patient));
    bPassed &= TestEqual(TEXT("Patient carrier should be attacker"), Patient->GetCarrier(), Attacker);
    bPassed &= TestEqual(TEXT("Attacker carried target should be patient"), Attacker->GetCarriedTarget(), Patient);

    bPassed &= TestTrue(TEXT("Medical system should bandage patient"), Medical->Bandage(Attacker, Patient));
    bPassed &= TestEqual(TEXT("Bleeding should stop after bandage"), DamageModel->GetBleedingRate(), 0.0f);
    bPassed &= TestEqual(TEXT("Patient should be stable after bandage"), DamageModel->GetConsciousState(), EAshfrontierConsciousState::Stable);

    bPassed &= TestTrue(TEXT("Carrier should be able to drop patient"), Carry->DropCarriedCharacter(Attacker));
    bPassed &= TestFalse(TEXT("Patient should no longer be carried"), Patient->IsBeingCarried());

    Attacker->Destroy();
    Patient->Destroy();

    return bPassed;
}

#endif
