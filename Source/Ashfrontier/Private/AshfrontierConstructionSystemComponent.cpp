#include "AshfrontierConstructionSystemComponent.h"

#include "AshfrontierInventoryComponent.h"

UAshfrontierConstructionSystemComponent::UAshfrontierConstructionSystemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UAshfrontierConstructionSystemComponent::CanAffordCost(const UAshfrontierInventoryComponent* Inventory, const TArray<FAshfrontierItemQuantity>& Cost) const
{
    if (!Inventory)
    {
        return false;
    }

    for (const FAshfrontierItemQuantity& Entry : Cost)
    {
        if (Entry.Id.IsNone() || Entry.Count <= 0 || !Inventory->HasItem(Entry.Id, Entry.Count))
        {
            return false;
        }
    }

    return true;
}

bool UAshfrontierConstructionSystemComponent::SpendCost(UAshfrontierInventoryComponent* Inventory, const TArray<FAshfrontierItemQuantity>& Cost) const
{
    if (!CanAffordCost(Inventory, Cost))
    {
        return false;
    }

    for (const FAshfrontierItemQuantity& Entry : Cost)
    {
        Inventory->RemoveItem(Entry.Id, Entry.Count);
    }

    return true;
}
