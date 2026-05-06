#pragma once

#include "CoreMinimal.h"
#include "AshfrontierBaseTypes.h"
#include "GameFramework/Actor.h"
#include "AshfrontierPlacedBuilding.generated.h"

class UAshfrontierInventoryComponent;
class UStaticMeshComponent;

UCLASS()
class ASHFRONTIER_API AAshfrontierPlacedBuilding : public AActor
{
    GENERATED_BODY()

public:
    AAshfrontierPlacedBuilding();

    void InitializeBuilding(const FAshfrontierBuildingDefinition& Definition);

    FName GetBuildingId() const;
    const FString& GetBuildingNameZh() const;
    FName GetCategory() const;
    FName GetResourceId() const;
    UAshfrontierInventoryComponent* GetStorageInventory() const;

    void AddProductionJob(const FName& RecipeId, float WorkSeconds);
    bool AdvanceActiveProduction(float DeltaSeconds, FName& OutCompletedRecipeId);
    int32 GetProductionQueueCount() const;
    float GetCurrentProductionRemainingSeconds() const;
    const TArray<FAshfrontierProductionQueueEntry>& GetProductionQueue() const;

private:
    UPROPERTY(VisibleAnywhere, Category = "Building")
    TObjectPtr<UStaticMeshComponent> PlaceholderMesh;

    UPROPERTY(VisibleAnywhere, Category = "Storage")
    TObjectPtr<UAshfrontierInventoryComponent> StorageInventory;

    UPROPERTY(VisibleAnywhere, Category = "Building")
    FName BuildingId;

    UPROPERTY(VisibleAnywhere, Category = "Building")
    FString BuildingNameZh;

    UPROPERTY(VisibleAnywhere, Category = "Building")
    FName Category;

    UPROPERTY(VisibleAnywhere, Category = "Building")
    FName ResourceId;

    UPROPERTY(VisibleAnywhere, Category = "Production")
    TArray<FAshfrontierProductionQueueEntry> ProductionQueue;
};
