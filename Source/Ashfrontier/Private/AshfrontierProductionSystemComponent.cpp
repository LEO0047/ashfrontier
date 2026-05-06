#include "AshfrontierProductionSystemComponent.h"

#include "AshfrontierCharacter.h"
#include "AshfrontierInventoryComponent.h"
#include "AshfrontierPlacedBuilding.h"

UAshfrontierProductionSystemComponent::UAshfrontierProductionSystemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    AshfrontierBaseData::BuildDefaultRecipeDefinitions(RecipeDefinitions);
}

const FAshfrontierRecipeDefinition* UAshfrontierProductionSystemComponent::FindRecipeDefinition(const FName& RecipeId) const
{
    return RecipeDefinitions.Find(RecipeId);
}

bool UAshfrontierProductionSystemComponent::CanStartRecipe(AAshfrontierPlacedBuilding* Building, AAshfrontierCharacter* Worker, const FName& RecipeId) const
{
    if (!IsValid(Building) || !IsValid(Worker) || !Worker->GetInventory())
    {
        return false;
    }

    const FAshfrontierRecipeDefinition* Recipe = FindRecipeDefinition(RecipeId);
    if (!Recipe || Recipe->StationId != Building->GetBuildingId())
    {
        return false;
    }

    for (const FAshfrontierItemQuantity& Input : Recipe->Inputs)
    {
        if (Input.Id.IsNone() || Input.Count <= 0 || !Worker->GetInventory()->HasItem(Input.Id, Input.Count))
        {
            return false;
        }
    }

    return true;
}

bool UAshfrontierProductionSystemComponent::EnqueueRecipe(AAshfrontierPlacedBuilding* Building, AAshfrontierCharacter* Worker, const FName& RecipeId) const
{
    if (!CanStartRecipe(Building, Worker, RecipeId))
    {
        return false;
    }

    const FAshfrontierRecipeDefinition* Recipe = FindRecipeDefinition(RecipeId);
    for (const FAshfrontierItemQuantity& Input : Recipe->Inputs)
    {
        Worker->GetInventory()->RemoveItem(Input.Id, Input.Count);
    }

    Building->AddProductionJob(Recipe->RecipeId, Recipe->WorkSeconds);
    return true;
}

bool UAshfrontierProductionSystemComponent::AdvanceProduction(AAshfrontierPlacedBuilding* Building, AAshfrontierCharacter* Receiver, float DeltaSeconds) const
{
    if (!IsValid(Building))
    {
        return false;
    }

    FName CompletedRecipeId;
    if (!Building->AdvanceActiveProduction(DeltaSeconds, CompletedRecipeId))
    {
        return false;
    }

    const FAshfrontierRecipeDefinition* Recipe = FindRecipeDefinition(CompletedRecipeId);
    UAshfrontierInventoryComponent* OutputInventory = IsValid(Receiver) ? Receiver->GetInventory() : Building->GetStorageInventory();
    if (!Recipe || !OutputInventory)
    {
        return false;
    }

    for (const FAshfrontierItemQuantity& Output : Recipe->Outputs)
    {
        OutputInventory->AddItem(Output.Id, Output.Count);
    }

    return true;
}

bool UAshfrontierProductionSystemComponent::CompleteNextProductionImmediately(AAshfrontierPlacedBuilding* Building, AAshfrontierCharacter* Receiver) const
{
    if (!IsValid(Building) || Building->GetProductionQueueCount() == 0)
    {
        return false;
    }

    return AdvanceProduction(Building, Receiver, Building->GetCurrentProductionRemainingSeconds() + 0.1f);
}
