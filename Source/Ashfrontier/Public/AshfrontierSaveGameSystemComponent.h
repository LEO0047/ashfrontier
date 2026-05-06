#pragma once

#include "CoreMinimal.h"
#include "AshfrontierSaveGame.h"
#include "Components/ActorComponent.h"
#include "AshfrontierSaveGameSystemComponent.generated.h"

class AAshfrontierCharacter;
class AAshfrontierPlacedBuilding;
class UAshfrontierCrimeEventMemoryComponent;
class UAshfrontierFactionSystemComponent;
UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierSaveGameSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierSaveGameSystemComponent();

    UAshfrontierSaveGame* CaptureState(
        const TArray<AAshfrontierCharacter*>& Characters,
        const TArray<AAshfrontierPlacedBuilding*>& Buildings,
        const UAshfrontierFactionSystemComponent* Factions,
        const UAshfrontierCrimeEventMemoryComponent* Memory) const;

    bool ApplyState(
        UAshfrontierSaveGame* SaveGame,
        TArray<AAshfrontierCharacter*>& Characters,
        TArray<AAshfrontierPlacedBuilding*>& Buildings,
        UAshfrontierFactionSystemComponent* Factions,
        UAshfrontierCrimeEventMemoryComponent* Memory) const;

    bool SaveToSlot(
        const FString& SlotName,
        int32 UserIndex,
        const TArray<AAshfrontierCharacter*>& Characters,
        const TArray<AAshfrontierPlacedBuilding*>& Buildings,
        const UAshfrontierFactionSystemComponent* Factions,
        const UAshfrontierCrimeEventMemoryComponent* Memory) const;

    UAshfrontierSaveGame* LoadFromSlot(const FString& SlotName, int32 UserIndex) const;

private:
    AAshfrontierCharacter* SpawnCharacterForSave(const FAshfrontierSavedCharacterState& SavedState) const;
    AAshfrontierPlacedBuilding* SpawnBuildingForSave(const FAshfrontierSavedBuildingState& SavedState) const;
};
