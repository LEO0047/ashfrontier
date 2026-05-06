#if WITH_DEV_AUTOMATION_TESTS

#include "AshfrontierCharacter.h"
#include "AshfrontierInventoryComponent.h"
#include "AshfrontierRecruitSystemComponent.h"
#include "AshfrontierSquadManagerComponent.h"
#include "AshfrontierTradingSystemComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Misc/AutomationTest.h"

namespace AshfrontierEconomyTest
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
    FAshfrontierRecruitInventoryTradeTest,
    "Ashfrontier.Economy.RecruitInventoryTrade",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAshfrontierRecruitInventoryTradeTest::RunTest(const FString& Parameters)
{
    using namespace AshfrontierEconomyTest;

    UWorld* World = FindEditorTestWorld();
    if (!TestNotNull(TEXT("Editor world should be available"), World))
    {
        return false;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.ObjectFlags |= RF_Transient;

    APlayerController* Controller = World->SpawnActor<APlayerController>(APlayerController::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    AAshfrontierCharacter* Leader = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), FVector(0.0f, 0.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
    AAshfrontierCharacter* Recruit = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), FVector(120.0f, 0.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
    AAshfrontierCharacter* Merchant = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), FVector(240.0f, 0.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
    UAshfrontierSquadManagerComponent* Squad = NewObject<UAshfrontierSquadManagerComponent>(Controller);
    UAshfrontierRecruitSystemComponent* RecruitSystem = NewObject<UAshfrontierRecruitSystemComponent>(Controller);
    UAshfrontierTradingSystemComponent* TradingSystem = NewObject<UAshfrontierTradingSystemComponent>(Controller);

    bool bPassed = TestNotNull(TEXT("Leader should spawn"), Leader)
        && TestNotNull(TEXT("Recruit should spawn"), Recruit)
        && TestNotNull(TEXT("Merchant should spawn"), Merchant)
        && TestNotNull(TEXT("Squad manager should exist"), Squad)
        && TestNotNull(TEXT("Recruit system should exist"), RecruitSystem)
        && TestNotNull(TEXT("Trading system should exist"), TradingSystem);

    if (!bPassed)
    {
        if (Controller) { Controller->Destroy(); }
        if (Leader) { Leader->Destroy(); }
        if (Recruit) { Recruit->Destroy(); }
        if (Merchant) { Merchant->Destroy(); }
        return false;
    }

    Leader->GetInventory()->AddItem(TEXT("item_ash_credit"), 160);
    Leader->GetInventory()->AddItem(TEXT("item_cooked_ration"), 1);
    Recruit->SetRecruitable(true, 60);
    Merchant->SetShopkeeper(true);
    Merchant->GetInventory()->AddItem(TEXT("item_field_bandage"), 4);
    Merchant->GetInventory()->AddItem(TEXT("item_ash_credit"), 40);

    bPassed &= TestTrue(TEXT("Leader should join initial squad"), Squad->AddExistingMember(Leader));
    bPassed &= TestTrue(TEXT("Recruit should be recruitable"), Recruit->IsRecruitable());
    bPassed &= TestTrue(TEXT("Recruit should join squad after payment"), RecruitSystem->Recruit(Leader, Recruit, Squad));
    bPassed &= TestEqual(TEXT("Squad should contain recruited member"), Squad->GetSquadCount(), 2);
    bPassed &= TestEqual(TEXT("Recruit payment should be removed"), Leader->GetInventory()->GetItemCount(TEXT("item_ash_credit")), 100);
    bPassed &= TestFalse(TEXT("Recruit should not remain recruitable"), Recruit->IsRecruitable());

    bPassed &= TestTrue(TEXT("Leader should buy a field bandage"), TradingSystem->BuyItem(Leader, Merchant, TEXT("item_field_bandage"), 1));
    bPassed &= TestEqual(TEXT("Bandage should enter leader inventory"), Leader->GetInventory()->GetItemCount(TEXT("item_field_bandage")), 1);
    bPassed &= TestEqual(TEXT("Credits should decrease by price"), Leader->GetInventory()->GetItemCount(TEXT("item_ash_credit")), 82);
    bPassed &= TestTrue(TEXT("Leader should sell cooked ration"), TradingSystem->SellItem(Leader, Merchant, TEXT("item_cooked_ration"), 1));
    bPassed &= TestEqual(TEXT("Cooked ration should leave leader inventory"), Leader->GetInventory()->GetItemCount(TEXT("item_cooked_ration")), 0);
    bPassed &= TestEqual(TEXT("Credits should increase after sale"), Leader->GetInventory()->GetItemCount(TEXT("item_ash_credit")), 96);

    Controller->Destroy();
    Leader->Destroy();
    Recruit->Destroy();
    Merchant->Destroy();

    return bPassed;
}

#endif
