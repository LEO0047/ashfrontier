#include "AshfrontierStorageSystemComponent.h"

#include "AshfrontierCharacter.h"
#include "AshfrontierInventoryComponent.h"
#include "AshfrontierPlacedBuilding.h"

UAshfrontierStorageSystemComponent::UAshfrontierStorageSystemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UAshfrontierStorageSystemComponent::DepositItem(AAshfrontierCharacter* FromCharacter, AAshfrontierPlacedBuilding* ToBuilding, const FName& ItemId, int32 Count) const
{
    if (!IsValid(FromCharacter) || !IsValid(ToBuilding) || Count <= 0 || !FromCharacter->GetInventory() || !ToBuilding->GetStorageInventory())
    {
        return false;
    }

    if (!FromCharacter->GetInventory()->RemoveItem(ItemId, Count))
    {
        return false;
    }

    ToBuilding->GetStorageInventory()->AddItem(ItemId, Count);
    return true;
}

bool UAshfrontierStorageSystemComponent::WithdrawItem(AAshfrontierPlacedBuilding* FromBuilding, AAshfrontierCharacter* ToCharacter, const FName& ItemId, int32 Count) const
{
    if (!IsValid(FromBuilding) || !IsValid(ToCharacter) || Count <= 0 || !FromBuilding->GetStorageInventory() || !ToCharacter->GetInventory())
    {
        return false;
    }

    if (!FromBuilding->GetStorageInventory()->RemoveItem(ItemId, Count))
    {
        return false;
    }

    ToCharacter->GetInventory()->AddItem(ItemId, Count);
    return true;
}
