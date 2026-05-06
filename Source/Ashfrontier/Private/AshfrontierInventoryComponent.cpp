#include "AshfrontierInventoryComponent.h"

UAshfrontierInventoryComponent::UAshfrontierInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UAshfrontierInventoryComponent::AddItem(const FName& ItemId, int32 Count)
{
    if (ItemId.IsNone() || Count <= 0)
    {
        return false;
    }

    if (FAshfrontierItemStack* Stack = FindStack(ItemId))
    {
        Stack->Count += Count;
        return true;
    }

    FAshfrontierItemStack NewStack;
    NewStack.ItemId = ItemId;
    NewStack.Count = Count;
    Stacks.Add(NewStack);
    return true;
}

bool UAshfrontierInventoryComponent::RemoveItem(const FName& ItemId, int32 Count)
{
    if (ItemId.IsNone() || Count <= 0)
    {
        return false;
    }

    FAshfrontierItemStack* Stack = FindStack(ItemId);
    if (!Stack || Stack->Count < Count)
    {
        return false;
    }

    Stack->Count -= Count;
    Stacks.RemoveAll([](const FAshfrontierItemStack& Entry)
    {
        return Entry.Count <= 0;
    });
    return true;
}

bool UAshfrontierInventoryComponent::HasItem(const FName& ItemId, int32 Count) const
{
    return GetItemCount(ItemId) >= Count;
}

int32 UAshfrontierInventoryComponent::GetItemCount(const FName& ItemId) const
{
    const FAshfrontierItemStack* Stack = FindStack(ItemId);
    return Stack ? Stack->Count : 0;
}

const TArray<FAshfrontierItemStack>& UAshfrontierInventoryComponent::GetStacks() const
{
    return Stacks;
}

FAshfrontierItemStack* UAshfrontierInventoryComponent::FindStack(const FName& ItemId)
{
    return Stacks.FindByPredicate([ItemId](const FAshfrontierItemStack& Entry)
    {
        return Entry.ItemId == ItemId;
    });
}

const FAshfrontierItemStack* UAshfrontierInventoryComponent::FindStack(const FName& ItemId) const
{
    return Stacks.FindByPredicate([ItemId](const FAshfrontierItemStack& Entry)
    {
        return Entry.ItemId == ItemId;
    });
}
