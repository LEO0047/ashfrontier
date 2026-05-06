#if WITH_DEV_AUTOMATION_TESTS

#include "AshfrontierBuildingPlacementSystemComponent.h"
#include "AshfrontierCharacter.h"
#include "AshfrontierConstructionSystemComponent.h"
#include "AshfrontierInventoryComponent.h"
#include "AshfrontierPlacedBuilding.h"
#include "AshfrontierProductionSystemComponent.h"
#include "AshfrontierRecruitSystemComponent.h"
#include "AshfrontierResourceNodeSystemComponent.h"
#include "AshfrontierSquadManagerComponent.h"
#include "AshfrontierStorageSystemComponent.h"
#include "AshfrontierTradingSystemComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Misc/AutomationTest.h"

namespace AshfrontierBuildingProductionTest
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
    FAshfrontierBuildingProductionFlowTest,
    "Ashfrontier.Building.ProductionFlow",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAshfrontierBuildingProductionFlowTest::RunTest(const FString& Parameters)
{
    using namespace AshfrontierBuildingProductionTest;

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

    UObject* ComponentOuter = Controller ? Cast<UObject>(Controller) : GetTransientPackage();
    UAshfrontierSquadManagerComponent* Squad = NewObject<UAshfrontierSquadManagerComponent>(ComponentOuter);
    UAshfrontierRecruitSystemComponent* RecruitSystem = NewObject<UAshfrontierRecruitSystemComponent>(ComponentOuter);
    UAshfrontierTradingSystemComponent* TradingSystem = NewObject<UAshfrontierTradingSystemComponent>(ComponentOuter);
    UAshfrontierConstructionSystemComponent* ConstructionSystem = NewObject<UAshfrontierConstructionSystemComponent>(ComponentOuter);
    UAshfrontierBuildingPlacementSystemComponent* BuildingPlacement = NewObject<UAshfrontierBuildingPlacementSystemComponent>(ComponentOuter);
    UAshfrontierResourceNodeSystemComponent* ResourceSystem = NewObject<UAshfrontierResourceNodeSystemComponent>(ComponentOuter);
    UAshfrontierProductionSystemComponent* ProductionSystem = NewObject<UAshfrontierProductionSystemComponent>(ComponentOuter);
    UAshfrontierStorageSystemComponent* StorageSystem = NewObject<UAshfrontierStorageSystemComponent>(ComponentOuter);

    bool bPassed = TestNotNull(TEXT("Controller should spawn"), Controller)
        && TestNotNull(TEXT("Leader should spawn"), Leader)
        && TestNotNull(TEXT("Recruit should spawn"), Recruit)
        && TestNotNull(TEXT("Merchant should spawn"), Merchant)
        && TestNotNull(TEXT("Squad manager should exist"), Squad)
        && TestNotNull(TEXT("Recruit system should exist"), RecruitSystem)
        && TestNotNull(TEXT("Trading system should exist"), TradingSystem)
        && TestNotNull(TEXT("Construction system should exist"), ConstructionSystem)
        && TestNotNull(TEXT("Building placement should exist"), BuildingPlacement)
        && TestNotNull(TEXT("Resource system should exist"), ResourceSystem)
        && TestNotNull(TEXT("Production system should exist"), ProductionSystem)
        && TestNotNull(TEXT("Storage system should exist"), StorageSystem);

    if (!bPassed)
    {
        if (Controller) { Controller->Destroy(); }
        if (Leader) { Leader->Destroy(); }
        if (Recruit) { Recruit->Destroy(); }
        if (Merchant) { Merchant->Destroy(); }
        return false;
    }

    Leader->GetInventory()->AddItem(TEXT("item_ash_credit"), 220);
    Recruit->SetRecruitable(true, 60);
    Merchant->SetShopkeeper(true);
    Merchant->GetInventory()->AddItem(TEXT("item_field_bandage"), 3);
    Merchant->GetInventory()->AddItem(TEXT("item_ash_credit"), 60);

    bPassed &= TestTrue(TEXT("Leader should enter squad"), Squad->AddExistingMember(Leader));
    bPassed &= TestTrue(TEXT("Recruit should join before base flow"), RecruitSystem->Recruit(Leader, Recruit, Squad));
    bPassed &= TestTrue(TEXT("Trade should buy one bandage before base flow"), TradingSystem->BuyItem(Leader, Merchant, TEXT("item_field_bandage"), 1));
    bPassed &= TestEqual(TEXT("Squad should include recruit"), Squad->GetSquadCount(), 2);
    bPassed &= TestEqual(TEXT("Bandage should enter leader inventory"), Leader->GetInventory()->GetItemCount(TEXT("item_field_bandage")), 1);

    bPassed &= TestTrue(TEXT("Gather two planks"), ResourceSystem->GatherResource(Leader, TEXT("resource_salvage_wood"), 2));
    bPassed &= TestTrue(TEXT("Gather one ore"), ResourceSystem->GatherResource(Leader, TEXT("resource_scrap_ore"), 1));

    const FVector CityLocation(0.0f, 0.0f, 120.0f);
    const FVector BuildZoneLocation(1600.0f, 1800.0f, 120.0f);
    AAshfrontierPlacedBuilding* IllegalKitchen = BuildingPlacement->PlaceBuilding(Leader, TEXT("building_camp_kitchen"), CityLocation, ConstructionSystem);
    bPassed &= TestNull(TEXT("Building outside wilderness zone should fail"), IllegalKitchen);
    bPassed &= TestEqual(TEXT("Failed placement should not consume plank"), Leader->GetInventory()->GetItemCount(TEXT("item_salvaged_plank")), 2);

    AAshfrontierPlacedBuilding* CampKitchen = BuildingPlacement->PlaceBuilding(Leader, TEXT("building_camp_kitchen"), BuildZoneLocation, ConstructionSystem);
    bPassed &= TestNotNull(TEXT("Camp kitchen should place in wilderness zone"), CampKitchen);
    bPassed &= TestEqual(TEXT("Camp kitchen should consume planks"), Leader->GetInventory()->GetItemCount(TEXT("item_salvaged_plank")), 0);
    bPassed &= TestEqual(TEXT("Camp kitchen should consume ore"), Leader->GetInventory()->GetItemCount(TEXT("item_scrap_ore")), 0);

    bPassed &= TestTrue(TEXT("Gather dry grain for food chain"), ResourceSystem->GatherResource(Leader, TEXT("resource_dry_grain"), 2));
    bPassed &= TestTrue(TEXT("Gather water for food chain"), ResourceSystem->GatherResource(Leader, TEXT("resource_brackish_water"), 1));
    bPassed &= TestTrue(TEXT("Food recipe should enqueue"), ProductionSystem->EnqueueRecipe(CampKitchen, Leader, TEXT("recipe_grain_to_ration")));
    bPassed &= TestEqual(TEXT("Production queue should contain food job"), CampKitchen->GetProductionQueueCount(), 1);
    bPassed &= TestTrue(TEXT("Food recipe should complete"), ProductionSystem->CompleteNextProductionImmediately(CampKitchen, Leader));
    bPassed &= TestEqual(TEXT("Cooked ration should enter leader inventory"), Leader->GetInventory()->GetItemCount(TEXT("item_cooked_ration")), 1);
    bPassed &= TestEqual(TEXT("Production queue should be empty after completion"), CampKitchen->GetProductionQueueCount(), 0);

    bPassed &= TestTrue(TEXT("Gather planks for storage crate"), ResourceSystem->GatherResource(Leader, TEXT("resource_salvage_wood"), 4));
    AAshfrontierPlacedBuilding* StorageCrate = BuildingPlacement->PlaceBuilding(Leader, TEXT("building_storage_crate"), BuildZoneLocation + FVector(220.0f, 0.0f, 0.0f), ConstructionSystem);
    bPassed &= TestNotNull(TEXT("Storage crate should place"), StorageCrate);
    bPassed &= TestTrue(TEXT("Ration should deposit to storage"), StorageSystem->DepositItem(Leader, StorageCrate, TEXT("item_cooked_ration"), 1));
    bPassed &= TestEqual(TEXT("Storage should hold ration"), StorageCrate->GetStorageInventory()->GetItemCount(TEXT("item_cooked_ration")), 1);
    bPassed &= TestTrue(TEXT("Ration should withdraw from storage"), StorageSystem->WithdrawItem(StorageCrate, Leader, TEXT("item_cooked_ration"), 1));
    bPassed &= TestEqual(TEXT("Leader should receive ration back"), Leader->GetInventory()->GetItemCount(TEXT("item_cooked_ration")), 1);

    Leader->GetInventory()->AddItem(TEXT("item_salvaged_plank"), 3);
    Leader->GetInventory()->AddItem(TEXT("item_building_plate"), 1);
    AAshfrontierPlacedBuilding* ScrapMine = BuildingPlacement->PlaceBuilding(Leader, TEXT("building_scrap_mine"), BuildZoneLocation + FVector(440.0f, 0.0f, 0.0f), ConstructionSystem);
    bPassed &= TestNotNull(TEXT("Scrap mine should place"), ScrapMine);
    bPassed &= TestTrue(TEXT("Scrap mine should gather ore"), ResourceSystem->GatherFromBuilding(Leader, ScrapMine, 1));
    bPassed &= TestTrue(TEXT("Ore should enter leader inventory from mine"), Leader->GetInventory()->GetItemCount(TEXT("item_scrap_ore")) >= 1);

    Leader->GetInventory()->AddItem(TEXT("item_salvaged_plank"), 5);
    Leader->GetInventory()->AddItem(TEXT("item_crude_ingot"), 2);
    Leader->GetInventory()->AddItem(TEXT("item_scrap_ore"), 3);
    AAshfrontierPlacedBuilding* Workshop = BuildingPlacement->PlaceBuilding(Leader, TEXT("building_field_workshop"), BuildZoneLocation + FVector(660.0f, 0.0f, 0.0f), ConstructionSystem);
    bPassed &= TestNotNull(TEXT("Field workshop should place"), Workshop);
    bPassed &= TestTrue(TEXT("Metal recipe should enqueue"), ProductionSystem->EnqueueRecipe(Workshop, Leader, TEXT("recipe_ore_to_ingot")));
    bPassed &= TestTrue(TEXT("Metal recipe should complete"), ProductionSystem->CompleteNextProductionImmediately(Workshop, Leader));
    bPassed &= TestTrue(TEXT("Ingot should enter leader inventory"), Leader->GetInventory()->GetItemCount(TEXT("item_crude_ingot")) >= 1);

    Leader->GetInventory()->AddItem(TEXT("item_salvaged_plank"), 2);
    bPassed &= TestTrue(TEXT("Plate recipe should enqueue"), ProductionSystem->EnqueueRecipe(Workshop, Leader, TEXT("recipe_ingot_to_plate")));
    bPassed &= TestTrue(TEXT("Plate recipe should complete"), ProductionSystem->CompleteNextProductionImmediately(Workshop, Leader));
    bPassed &= TestTrue(TEXT("Building plate should enter leader inventory"), Leader->GetInventory()->GetItemCount(TEXT("item_building_plate")) >= 1);

    if (CampKitchen) { CampKitchen->Destroy(); }
    if (StorageCrate) { StorageCrate->Destroy(); }
    if (ScrapMine) { ScrapMine->Destroy(); }
    if (Workshop) { Workshop->Destroy(); }
    Controller->Destroy();
    Leader->Destroy();
    Recruit->Destroy();
    Merchant->Destroy();

    return bPassed;
}

#endif
