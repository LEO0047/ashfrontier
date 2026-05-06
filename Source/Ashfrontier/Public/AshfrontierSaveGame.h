#pragma once

#include "CoreMinimal.h"
#include "AshfrontierBaseTypes.h"
#include "AshfrontierCombatTypes.h"
#include "AshfrontierInventoryComponent.h"
#include "AshfrontierLegalTypes.h"
#include "GameFramework/SaveGame.h"
#include "AshfrontierSaveGame.generated.h"

USTRUCT(BlueprintType)
struct ASHFRONTIER_API FAshfrontierSavedCharacterState
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString CharacterId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString DisplayNameZh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString ArtCharacterId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString PortraitTexturePath;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString PrototypeBodyMaterialPath;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName FactionId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EAshfrontierCharacterTeam Team = EAshfrontierCharacterTeam::Neutral;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FVector Location = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<FAshfrontierBodyPartHealth> BodyParts;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float BleedingRate = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EAshfrontierConsciousState ConsciousState = EAshfrontierConsciousState::Conscious;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<FAshfrontierItemStack> InventoryStacks;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bRecruitable = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 RecruitCost = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bShopkeeper = false;
};

USTRUCT(BlueprintType)
struct ASHFRONTIER_API FAshfrontierSavedBuildingState
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName BuildingId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString NameZh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName Category;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName ResourceId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FVector Location = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<FAshfrontierItemStack> StorageStacks;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<FAshfrontierProductionQueueEntry> ProductionQueue;
};

UCLASS()
class ASHFRONTIER_API UAshfrontierSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 SchemaVersion = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<FAshfrontierSavedCharacterState> Characters;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<FAshfrontierSavedBuildingState> Buildings;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TMap<FName, int32> FactionRelations;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<FAshfrontierCrimeEventRecord> CrimeEvents;
};
