#include "AshfrontierResourceNodeSystemComponent.h"

#include "AshfrontierCharacter.h"
#include "AshfrontierInventoryComponent.h"
#include "AshfrontierPlacedBuilding.h"

UAshfrontierResourceNodeSystemComponent::UAshfrontierResourceNodeSystemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    AshfrontierBaseData::BuildDefaultResourceDefinitions(ResourceDefinitions);
}

const FAshfrontierResourceDefinition* UAshfrontierResourceNodeSystemComponent::FindResourceDefinition(const FName& ResourceId) const
{
    return ResourceDefinitions.Find(ResourceId);
}

bool UAshfrontierResourceNodeSystemComponent::GatherResource(AAshfrontierCharacter* Gatherer, const FName& ResourceId, int32 Cycles) const
{
    if (!IsValid(Gatherer) || Cycles <= 0 || !Gatherer->GetInventory())
    {
        return false;
    }

    const FAshfrontierResourceDefinition* Definition = FindResourceDefinition(ResourceId);
    if (!Definition || Definition->ProducesItemId.IsNone())
    {
        return false;
    }

    return Gatherer->GetInventory()->AddItem(Definition->ProducesItemId, Cycles);
}

bool UAshfrontierResourceNodeSystemComponent::GatherFromBuilding(AAshfrontierCharacter* Gatherer, AAshfrontierPlacedBuilding* Building, int32 Cycles) const
{
    if (!IsValid(Building) || Building->GetResourceId().IsNone())
    {
        return false;
    }

    return GatherResource(Gatherer, Building->GetResourceId(), Cycles);
}
