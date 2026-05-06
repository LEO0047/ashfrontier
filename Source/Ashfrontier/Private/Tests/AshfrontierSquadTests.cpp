#if WITH_DEV_AUTOMATION_TESTS

#include "AshfrontierCameraControllerComponent.h"
#include "AshfrontierCharacter.h"
#include "AshfrontierOrderSystemComponent.h"
#include "AshfrontierPlayerController.h"
#include "AshfrontierSquadManagerComponent.h"
#include "AshfrontierSquadTypes.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/AutomationTest.h"

namespace AshfrontierSquadTest
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
    FAshfrontierSquadOrderStateFlowTest,
    "Ashfrontier.Squad.OrderStateFlow",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAshfrontierSquadOrderStateFlowTest::RunTest(const FString& Parameters)
{
    using namespace AshfrontierSquadTest;

    UWorld* World = FindEditorTestWorld();
    if (!TestNotNull(TEXT("Editor world should be available"), World))
    {
        return false;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.ObjectFlags |= RF_Transient;

    AAshfrontierPlayerController* Controller = World->SpawnActor<AAshfrontierPlayerController>(AAshfrontierPlayerController::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    if (!TestNotNull(TEXT("Player controller should spawn"), Controller))
    {
        return false;
    }

    UAshfrontierSquadManagerComponent* SquadManager = Controller->GetSquadManager();
    UAshfrontierOrderSystemComponent* OrderSystem = Controller->GetOrderSystem();
    UAshfrontierCameraControllerComponent* CameraController = Controller->GetCameraController();
    bool bPassed = TestNotNull(TEXT("SquadManager should exist"), SquadManager)
        && TestNotNull(TEXT("OrderSystem should exist"), OrderSystem)
        && TestNotNull(TEXT("CameraController should exist"), CameraController);

    if (!bPassed)
    {
        Controller->Destroy();
        return false;
    }

    SquadManager->EnsureStartingSquad(Controller, AAshfrontierCharacter::StaticClass(), 2);
    TArray<AAshfrontierCharacter*> Members = SquadManager->GetSquadMembers();
    bPassed &= TestEqual(TEXT("Starting squad should contain two members"), Members.Num(), 2);
    bPassed &= TestEqual(TEXT("Default selection should contain one member"), SquadManager->GetSelectedCount(), 1);

    SquadManager->SelectAll();
    TArray<AAshfrontierCharacter*> SelectedMembers = SquadManager->GetSelectedMembers();
    bPassed &= TestEqual(TEXT("SelectAll should select two members"), SelectedMembers.Num(), 2);

    const FVector MoveDestination(700.0f, 240.0f, 120.0f);
    bPassed &= TestEqual(TEXT("Move command should affect both members"), OrderSystem->IssueMove(SelectedMembers, MoveDestination), 2);
    for (AAshfrontierCharacter* Member : SelectedMembers)
    {
        if (!TestNotNull(TEXT("Selected member should be valid after move"), Member))
        {
            bPassed = false;
            continue;
        }
        bPassed &= TestEqual(TEXT("Member should enter moving order"), Member->GetCurrentOrder(), EAshfrontierSquadOrder::Moving);
    }

    AAshfrontierCharacter* Leader = SquadManager->GetLeader();
    bPassed &= TestNotNull(TEXT("Leader should exist"), Leader);
    bPassed &= TestEqual(TEXT("Follow command should affect both members"), OrderSystem->IssueFollow(SelectedMembers, Leader), 2);
    if (Members.IsValidIndex(0))
    {
        bPassed &= TestEqual(TEXT("Leader should hold when selected as follow target"), Members[0]->GetCurrentOrder(), EAshfrontierSquadOrder::Holding);
    }
    if (Members.IsValidIndex(1))
    {
        bPassed &= TestEqual(TEXT("Second member should follow leader"), Members[1]->GetCurrentOrder(), EAshfrontierSquadOrder::Following);
        bPassed &= TestEqual(TEXT("Second member follow target should be leader"), Members[1]->GetFollowTarget(), Leader);
    }

    bPassed &= TestEqual(TEXT("Hold command should affect both members"), OrderSystem->IssueHold(SelectedMembers), 2);
    for (AAshfrontierCharacter* Member : SelectedMembers)
    {
        if (Member)
        {
            bPassed &= TestEqual(TEXT("Member should enter holding order"), Member->GetCurrentOrder(), EAshfrontierSquadOrder::Holding);
        }
    }

    CameraController->ToggleTacticalCamera(Controller, Leader);
    bPassed &= TestTrue(TEXT("Tactical camera should be enabled"), CameraController->IsTacticalCamera());
    if (Leader)
    {
        bPassed &= TestTrue(TEXT("Leader camera arm should use tactical length"), Leader->GetCameraArmLength() > 1000.0f);
    }

    for (AAshfrontierCharacter* Member : Members)
    {
        if (Member)
        {
            Member->Destroy();
        }
    }
    Controller->Destroy();

    return bPassed;
}

#endif
