#if WITH_DEV_AUTOMATION_TESTS

#include "AshfrontierCharacter.h"
#include "AshfrontierCrimeEventMemoryComponent.h"
#include "AshfrontierDamageModelComponent.h"
#include "AshfrontierFactionSystemComponent.h"
#include "AshfrontierInventoryComponent.h"
#include "AshfrontierPlacedBuilding.h"
#include "AshfrontierSaveGame.h"
#include "AshfrontierSaveGameSystemComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"

namespace AshfrontierSaveGameTest
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
    FAshfrontierSaveGameRoundTripTest,
    "Ashfrontier.SaveGame.RoundTripCoreState",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAshfrontierSaveGameRoundTripTest::RunTest(const FString& Parameters)
{
    using namespace AshfrontierSaveGameTest;

    UWorld* World = FindEditorTestWorld();
    if (!TestNotNull(TEXT("Editor world should be available"), World))
    {
        return false;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.ObjectFlags |= RF_Transient;

    AAshfrontierCharacter* Leader = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), FVector(10.0f, 20.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
    AAshfrontierCharacter* Ally = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), FVector(140.0f, 20.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
    AAshfrontierPlacedBuilding* Building = World->SpawnActor<AAshfrontierPlacedBuilding>(AAshfrontierPlacedBuilding::StaticClass(), FVector(1600.0f, 1800.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
    UObject* ComponentOuter = Leader ? Cast<UObject>(Leader) : GetTransientPackage();
    UAshfrontierFactionSystemComponent* Factions = NewObject<UAshfrontierFactionSystemComponent>(ComponentOuter);
    UAshfrontierCrimeEventMemoryComponent* Memory = NewObject<UAshfrontierCrimeEventMemoryComponent>(ComponentOuter);
    UAshfrontierSaveGameSystemComponent* SaveSystem = NewObject<UAshfrontierSaveGameSystemComponent>(ComponentOuter);

    bool bPassed = TestNotNull(TEXT("Leader should spawn"), Leader)
        && TestNotNull(TEXT("Ally should spawn"), Ally)
        && TestNotNull(TEXT("Building should spawn"), Building)
        && TestNotNull(TEXT("Faction system should exist"), Factions)
        && TestNotNull(TEXT("Crime memory should exist"), Memory)
        && TestNotNull(TEXT("Save system should exist"), SaveSystem);

    if (!bPassed)
    {
        if (Leader) { Leader->Destroy(); }
        if (Ally) { Ally->Destroy(); }
        if (Building) { Building->Destroy(); }
        return false;
    }

    Leader->SetSquadDisplayName(TEXT("灰燼前鋒"));
    Leader->SetFactionId(TEXT("faction_player_squad"));
    Leader->SetCharacterTeam(EAshfrontierCharacterTeam::PlayerSquad);
    Leader->GetInventory()->AddItem(TEXT("item_ash_credit"), 73);
    Leader->GetInventory()->AddItem(TEXT("item_cooked_ration"), 2);
    Leader->GetDamageModel()->ApplyDamage(EAshfrontierBodyPart::Chest, 44.0f);

    Ally->SetSquadDisplayName(TEXT("鹽路斥候"));
    Ally->SetArtCharacterId(TEXT("recruit_scout_jianan"));
    Ally->SetPortraitTexturePath(TEXT("/Game/GeneratedArt/Characters/T_AfV02_Portrait_RecruitScout"));
    Ally->SetFactionId(TEXT("faction_player_squad"));
    Ally->SetCharacterTeam(EAshfrontierCharacterTeam::PlayerSquad);
    Ally->GetInventory()->AddItem(TEXT("item_field_bandage"), 1);

    FAshfrontierBuildingDefinition Definition;
    Definition.BuildingId = TEXT("building_camp_kitchen");
    Definition.NameZh = TEXT("營火灶");
    Definition.Category = TEXT("production");
    Building->InitializeBuilding(Definition);
    Building->GetStorageInventory()->AddItem(TEXT("item_raw_grain"), 2);
    Building->AddProductionJob(TEXT("recipe_grain_to_ration"), 13.0f);

    Factions->AdjustRelation(TEXT("faction_player_squad"), TEXT("faction_saltwardens"), -12);
    FAshfrontierCrimeEventRecord CrimeEvent;
    CrimeEvent.EventType = EAshfrontierLegalEventType::Attack;
    CrimeEvent.Reaction = EAshfrontierGuardReaction::Pursuit;
    CrimeEvent.OffenderFactionId = TEXT("faction_player_squad");
    CrimeEvent.AuthorityFactionId = TEXT("faction_saltwardens");
    CrimeEvent.RelationDelta = -12;
    CrimeEvent.RemainingMemorySeconds = 500.0f;
    CrimeEvent.SummaryZh = TEXT("攻擊 -> 追捕");
    Memory->RecordEvent(CrimeEvent);

    TArray<AAshfrontierCharacter*> Characters = {Leader, Ally};
    TArray<AAshfrontierPlacedBuilding*> Buildings = {Building};
    UAshfrontierSaveGame* Captured = SaveSystem->CaptureState(Characters, Buildings, Factions, Memory);
    bPassed &= TestNotNull(TEXT("Save capture should create object"), Captured);
    bPassed &= TestEqual(TEXT("Save schema version should be 1"), Captured ? Captured->SchemaVersion : 0, 1);
    bPassed &= TestEqual(TEXT("Save should include two characters"), Captured ? Captured->Characters.Num() : 0, 2);
    bPassed &= TestEqual(TEXT("Save should include one building"), Captured ? Captured->Buildings.Num() : 0, 1);

    const FString SlotName = TEXT("AshfrontierAutomationSave");
    bPassed &= TestTrue(TEXT("SaveGameToSlot should succeed"), SaveSystem->SaveToSlot(SlotName, 0, Characters, Buildings, Factions, Memory));
    UAshfrontierSaveGame* LoadedFromDisk = SaveSystem->LoadFromSlot(SlotName, 0);
    bPassed &= TestNotNull(TEXT("LoadGameFromSlot should return save"), LoadedFromDisk);

    Leader->SetActorLocation(FVector(9000.0f, 9000.0f, 120.0f));
    Leader->GetInventory()->ClearInventory();
    Leader->GetDamageModel()->ResetHealth();
    Ally->SetActorLocation(FVector(8000.0f, 8000.0f, 120.0f));
    Ally->GetInventory()->ClearInventory();
    Factions->SetRelation(TEXT("faction_player_squad"), TEXT("faction_saltwardens"), 0);
    Memory->TickMemory(1000.0f);
    Building->Destroy();
    Buildings.Reset();

    bPassed &= TestTrue(TEXT("ApplyState should restore mutated actors and building"), SaveSystem->ApplyState(LoadedFromDisk, Characters, Buildings, Factions, Memory));
    bPassed &= TestTrue(TEXT("Leader location should restore X"), FMath::IsNearlyEqual(Characters[0]->GetActorLocation().X, 10.0f, 1.0f));
    bPassed &= TestEqual(TEXT("Leader credits should restore"), Characters[0]->GetInventory()->GetItemCount(TEXT("item_ash_credit")), 73);
    bPassed &= TestEqual(TEXT("Leader ration should restore"), Characters[0]->GetInventory()->GetItemCount(TEXT("item_cooked_ration")), 2);
    bPassed &= TestTrue(TEXT("Leader chest health should restore damaged state"), Characters[0]->GetDamageModel()->GetBodyPartHealth(EAshfrontierBodyPart::Chest) < 100.0f);
    bPassed &= TestTrue(TEXT("Leader bleeding should restore"), Characters[0]->GetDamageModel()->GetBleedingRate() > 0.0f);
    bPassed &= TestEqual(TEXT("Ally bandage should restore"), Characters[1]->GetInventory()->GetItemCount(TEXT("item_field_bandage")), 1);
    bPassed &= TestEqual(TEXT("Ally art character id should restore"), Characters[1]->GetArtCharacterId(), FString(TEXT("recruit_scout_jianan")));
    bPassed &= TestEqual(TEXT("Ally portrait path should restore"), Characters[1]->GetPortraitTexturePath(), FString(TEXT("/Game/GeneratedArt/Characters/T_AfV02_Portrait_RecruitScout")));
    bPassed &= TestEqual(TEXT("Faction relation should restore"), Factions->GetRelation(TEXT("faction_player_squad"), TEXT("faction_saltwardens")), -12);
    bPassed &= TestTrue(TEXT("Crime memory should restore"), Memory->HasMemoryFor(TEXT("faction_player_squad"), EAshfrontierLegalEventType::Attack));
    bPassed &= TestEqual(TEXT("Building should be recreated"), Buildings.Num(), 1);
    bPassed &= TestEqual(TEXT("Building id should restore"), Buildings[0]->GetBuildingId(), FName(TEXT("building_camp_kitchen")));
    bPassed &= TestEqual(TEXT("Building storage should restore"), Buildings[0]->GetStorageInventory()->GetItemCount(TEXT("item_raw_grain")), 2);
    bPassed &= TestEqual(TEXT("Production queue should restore"), Buildings[0]->GetProductionQueueCount(), 1);

    UGameplayStatics::DeleteGameInSlot(SlotName, 0);

    for (AAshfrontierPlacedBuilding* RestoredBuilding : Buildings)
    {
        if (IsValid(RestoredBuilding))
        {
            RestoredBuilding->Destroy();
        }
    }
    if (IsValid(Leader)) { Leader->Destroy(); }
    if (IsValid(Ally)) { Ally->Destroy(); }

    return bPassed;
}

#endif
