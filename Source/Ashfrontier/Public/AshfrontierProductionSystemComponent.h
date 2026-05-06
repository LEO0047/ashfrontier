#pragma once

#include "CoreMinimal.h"
#include "AshfrontierBaseTypes.h"
#include "Components/ActorComponent.h"
#include "AshfrontierProductionSystemComponent.generated.h"

class AAshfrontierCharacter;
class AAshfrontierPlacedBuilding;

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierProductionSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierProductionSystemComponent();

    const FAshfrontierRecipeDefinition* FindRecipeDefinition(const FName& RecipeId) const;
    bool CanStartRecipe(AAshfrontierPlacedBuilding* Building, AAshfrontierCharacter* Worker, const FName& RecipeId) const;
    bool EnqueueRecipe(AAshfrontierPlacedBuilding* Building, AAshfrontierCharacter* Worker, const FName& RecipeId) const;
    bool AdvanceProduction(AAshfrontierPlacedBuilding* Building, AAshfrontierCharacter* Receiver, float DeltaSeconds) const;
    bool CompleteNextProductionImmediately(AAshfrontierPlacedBuilding* Building, AAshfrontierCharacter* Receiver) const;

private:
    UPROPERTY(VisibleAnywhere, Category = "Production")
    TMap<FName, FAshfrontierRecipeDefinition> RecipeDefinitions;
};
