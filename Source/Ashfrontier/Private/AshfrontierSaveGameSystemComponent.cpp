#include "AshfrontierSaveGameSystemComponent.h"

#include "AshfrontierCharacter.h"
#include "AshfrontierCrimeEventMemoryComponent.h"
#include "AshfrontierDamageModelComponent.h"
#include "AshfrontierFactionSystemComponent.h"
#include "AshfrontierInventoryComponent.h"
#include "AshfrontierPlacedBuilding.h"
#include "AshfrontierSaveGame.h"
#include "Kismet/GameplayStatics.h"

UAshfrontierSaveGameSystemComponent::UAshfrontierSaveGameSystemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

UAshfrontierSaveGame* UAshfrontierSaveGameSystemComponent::CaptureState(
    const TArray<AAshfrontierCharacter*>& Characters,
    const TArray<AAshfrontierPlacedBuilding*>& Buildings,
    const UAshfrontierFactionSystemComponent* Factions,
    const UAshfrontierCrimeEventMemoryComponent* Memory) const
{
    UAshfrontierSaveGame* SaveGame = Cast<UAshfrontierSaveGame>(UGameplayStatics::CreateSaveGameObject(UAshfrontierSaveGame::StaticClass()));
    if (!SaveGame)
    {
        return nullptr;
    }

    SaveGame->SchemaVersion = 1;
    for (AAshfrontierCharacter* Character : Characters)
    {
        if (!IsValid(Character))
        {
            continue;
        }

        FAshfrontierSavedCharacterState SavedCharacter;
        SavedCharacter.CharacterId = Character->GetName();
        SavedCharacter.DisplayNameZh = Character->GetSquadDisplayName();
        SavedCharacter.ArtCharacterId = Character->GetArtCharacterId();
        SavedCharacter.PortraitTexturePath = Character->GetPortraitTexturePath();
        SavedCharacter.FactionId = Character->GetFactionId();
        SavedCharacter.Team = Character->GetCharacterTeam();
        SavedCharacter.Location = Character->GetActorLocation();
        SavedCharacter.bRecruitable = Character->IsRecruitable();
        SavedCharacter.RecruitCost = Character->GetRecruitCost();
        SavedCharacter.bShopkeeper = Character->IsShopkeeper();
        if (const UAshfrontierDamageModelComponent* Damage = Character->GetDamageModel())
        {
            SavedCharacter.BodyParts = Damage->GetBodyPartsForSave();
            SavedCharacter.BleedingRate = Damage->GetBleedingRate();
            SavedCharacter.ConsciousState = Damage->GetConsciousState();
        }
        if (const UAshfrontierInventoryComponent* Inventory = Character->GetInventory())
        {
            SavedCharacter.InventoryStacks = Inventory->GetStacks();
        }
        SaveGame->Characters.Add(SavedCharacter);
    }

    for (AAshfrontierPlacedBuilding* Building : Buildings)
    {
        if (!IsValid(Building))
        {
            continue;
        }

        FAshfrontierSavedBuildingState SavedBuilding;
        SavedBuilding.BuildingId = Building->GetBuildingId();
        SavedBuilding.NameZh = Building->GetBuildingNameZh();
        SavedBuilding.Category = Building->GetCategory();
        SavedBuilding.ResourceId = Building->GetResourceId();
        SavedBuilding.Location = Building->GetActorLocation();
        if (const UAshfrontierInventoryComponent* Storage = Building->GetStorageInventory())
        {
            SavedBuilding.StorageStacks = Storage->GetStacks();
        }
        SavedBuilding.ProductionQueue = Building->GetProductionQueue();
        SaveGame->Buildings.Add(SavedBuilding);
    }

    if (Factions)
    {
        SaveGame->FactionRelations = Factions->GetRelationsForSave();
    }
    if (Memory)
    {
        SaveGame->CrimeEvents = Memory->GetEvents();
    }

    return SaveGame;
}

bool UAshfrontierSaveGameSystemComponent::ApplyState(
    UAshfrontierSaveGame* SaveGame,
    TArray<AAshfrontierCharacter*>& Characters,
    TArray<AAshfrontierPlacedBuilding*>& Buildings,
    UAshfrontierFactionSystemComponent* Factions,
    UAshfrontierCrimeEventMemoryComponent* Memory) const
{
    if (!SaveGame || SaveGame->SchemaVersion <= 0)
    {
        return false;
    }

    while (Characters.Num() < SaveGame->Characters.Num())
    {
        AAshfrontierCharacter* NewCharacter = SpawnCharacterForSave(SaveGame->Characters[Characters.Num()]);
        if (!NewCharacter)
        {
            return false;
        }
        Characters.Add(NewCharacter);
    }

    for (int32 Index = 0; Index < SaveGame->Characters.Num(); ++Index)
    {
        AAshfrontierCharacter* Character = Characters[Index];
        if (!IsValid(Character))
        {
            return false;
        }

        const FAshfrontierSavedCharacterState& SavedCharacter = SaveGame->Characters[Index];
        Character->SetActorLocation(SavedCharacter.Location);
        Character->SetSquadDisplayName(SavedCharacter.DisplayNameZh);
        Character->SetArtCharacterId(SavedCharacter.ArtCharacterId);
        Character->SetPortraitTexturePath(SavedCharacter.PortraitTexturePath);
        Character->SetFactionId(SavedCharacter.FactionId);
        Character->SetCharacterTeam(SavedCharacter.Team);
        Character->SetRecruitable(SavedCharacter.bRecruitable, SavedCharacter.RecruitCost);
        Character->SetShopkeeper(SavedCharacter.bShopkeeper);
        if (UAshfrontierDamageModelComponent* Damage = Character->GetDamageModel())
        {
            Damage->RestoreFromSave(SavedCharacter.BodyParts, SavedCharacter.BleedingRate, SavedCharacter.ConsciousState);
        }
        if (UAshfrontierInventoryComponent* Inventory = Character->GetInventory())
        {
            Inventory->SetStacksForSave(SavedCharacter.InventoryStacks);
        }
    }

    while (Buildings.Num() < SaveGame->Buildings.Num())
    {
        AAshfrontierPlacedBuilding* NewBuilding = SpawnBuildingForSave(SaveGame->Buildings[Buildings.Num()]);
        if (!NewBuilding)
        {
            return false;
        }
        Buildings.Add(NewBuilding);
    }

    for (int32 Index = 0; Index < SaveGame->Buildings.Num(); ++Index)
    {
        AAshfrontierPlacedBuilding* Building = Buildings[Index];
        if (!IsValid(Building))
        {
            return false;
        }

        const FAshfrontierSavedBuildingState& SavedBuilding = SaveGame->Buildings[Index];
        FAshfrontierBuildingDefinition Definition;
        Definition.BuildingId = SavedBuilding.BuildingId;
        Definition.NameZh = SavedBuilding.NameZh;
        Definition.Category = SavedBuilding.Category;
        Definition.ResourceId = SavedBuilding.ResourceId;
        Building->InitializeBuilding(Definition);
        Building->SetActorLocation(SavedBuilding.Location);
        Building->SetProductionQueueForSave(SavedBuilding.ProductionQueue);
        if (UAshfrontierInventoryComponent* Storage = Building->GetStorageInventory())
        {
            Storage->SetStacksForSave(SavedBuilding.StorageStacks);
        }
    }

    if (Factions)
    {
        Factions->SetRelationsForSave(SaveGame->FactionRelations);
    }
    if (Memory)
    {
        Memory->SetEventsForSave(SaveGame->CrimeEvents);
    }

    return true;
}

bool UAshfrontierSaveGameSystemComponent::SaveToSlot(
    const FString& SlotName,
    int32 UserIndex,
    const TArray<AAshfrontierCharacter*>& Characters,
    const TArray<AAshfrontierPlacedBuilding*>& Buildings,
    const UAshfrontierFactionSystemComponent* Factions,
    const UAshfrontierCrimeEventMemoryComponent* Memory) const
{
    UAshfrontierSaveGame* SaveGame = CaptureState(Characters, Buildings, Factions, Memory);
    return SaveGame && UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, UserIndex);
}

UAshfrontierSaveGame* UAshfrontierSaveGameSystemComponent::LoadFromSlot(const FString& SlotName, int32 UserIndex) const
{
    return Cast<UAshfrontierSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
}

AAshfrontierCharacter* UAshfrontierSaveGameSystemComponent::SpawnCharacterForSave(const FAshfrontierSavedCharacterState& SavedState) const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    AAshfrontierCharacter* Character = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), SavedState.Location, FRotator::ZeroRotator, SpawnParams);
    if (Character)
    {
        Character->SpawnDefaultController();
    }
    return Character;
}

AAshfrontierPlacedBuilding* UAshfrontierSaveGameSystemComponent::SpawnBuildingForSave(const FAshfrontierSavedBuildingState& SavedState) const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    return World->SpawnActor<AAshfrontierPlacedBuilding>(AAshfrontierPlacedBuilding::StaticClass(), SavedState.Location, FRotator::ZeroRotator, SpawnParams);
}
