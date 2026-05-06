#pragma once

#include "CoreMinimal.h"
#include "AshfrontierBaseTypes.generated.h"

USTRUCT(BlueprintType)
struct ASHFRONTIER_API FAshfrontierItemQuantity
{
    GENERATED_BODY()

    FAshfrontierItemQuantity() = default;
    FAshfrontierItemQuantity(const FName& InId, int32 InCount);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName Id;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 Count = 0;
};

USTRUCT(BlueprintType)
struct ASHFRONTIER_API FAshfrontierBuildingDefinition
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName BuildingId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString NameZh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName Category;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName AllowedZone;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<FAshfrontierItemQuantity> Cost;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName ResourceId;
};

USTRUCT(BlueprintType)
struct ASHFRONTIER_API FAshfrontierResourceDefinition
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName ResourceId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString NameZh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName ProducesItemId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float GatherTimeSeconds = 0.0f;
};

USTRUCT(BlueprintType)
struct ASHFRONTIER_API FAshfrontierRecipeDefinition
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName RecipeId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString NameZh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName Chain;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName StationId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<FAshfrontierItemQuantity> Inputs;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<FAshfrontierItemQuantity> Outputs;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float WorkSeconds = 0.0f;
};

USTRUCT(BlueprintType)
struct ASHFRONTIER_API FAshfrontierProductionQueueEntry
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName RecipeId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float RemainingSeconds = 0.0f;
};

namespace AshfrontierBaseData
{
ASHFRONTIER_API void BuildDefaultBuildingDefinitions(TMap<FName, FAshfrontierBuildingDefinition>& OutDefinitions);
ASHFRONTIER_API void BuildDefaultResourceDefinitions(TMap<FName, FAshfrontierResourceDefinition>& OutDefinitions);
ASHFRONTIER_API void BuildDefaultRecipeDefinitions(TMap<FName, FAshfrontierRecipeDefinition>& OutDefinitions);
}
