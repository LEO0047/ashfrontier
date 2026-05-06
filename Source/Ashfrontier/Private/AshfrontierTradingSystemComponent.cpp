#include "AshfrontierTradingSystemComponent.h"

#include "AshfrontierCharacter.h"
#include "AshfrontierInventoryComponent.h"

namespace AshfrontierEconomy
{
const FName CreditsItemId(TEXT("item_ash_credit"));
}

UAshfrontierTradingSystemComponent::UAshfrontierTradingSystemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    BuildPriceTable();
}

bool UAshfrontierTradingSystemComponent::BuyItem(AAshfrontierCharacter* Buyer, AAshfrontierCharacter* Seller, const FName& ItemId, int32 Count) const
{
    if (!IsValid(Buyer) || !IsValid(Seller) || Count <= 0)
    {
        return false;
    }

    UAshfrontierInventoryComponent* BuyerInventory = Buyer->GetInventory();
    UAshfrontierInventoryComponent* SellerInventory = Seller->GetInventory();
    if (!BuyerInventory || !SellerInventory)
    {
        return false;
    }

    const int32 TotalPrice = GetUnitPrice(ItemId) * Count;
    if (!SellerInventory->HasItem(ItemId, Count) || !BuyerInventory->HasItem(AshfrontierEconomy::CreditsItemId, TotalPrice))
    {
        return false;
    }

    SellerInventory->RemoveItem(ItemId, Count);
    BuyerInventory->RemoveItem(AshfrontierEconomy::CreditsItemId, TotalPrice);
    BuyerInventory->AddItem(ItemId, Count);
    SellerInventory->AddItem(AshfrontierEconomy::CreditsItemId, TotalPrice);
    return true;
}

bool UAshfrontierTradingSystemComponent::SellItem(AAshfrontierCharacter* Seller, AAshfrontierCharacter* Buyer, const FName& ItemId, int32 Count) const
{
    return BuyItem(Buyer, Seller, ItemId, Count);
}

int32 UAshfrontierTradingSystemComponent::GetUnitPrice(const FName& ItemId) const
{
    if (const int32* Price = PriceByItem.Find(ItemId))
    {
        return *Price;
    }
    return 1;
}

void UAshfrontierTradingSystemComponent::BuildPriceTable()
{
    PriceByItem.Reset();
    PriceByItem.Add(TEXT("item_ash_credit"), 1);
    PriceByItem.Add(TEXT("item_cooked_ration"), 14);
    PriceByItem.Add(TEXT("item_raw_grain"), 5);
    PriceByItem.Add(TEXT("item_water_flask"), 8);
    PriceByItem.Add(TEXT("item_scrap_ore"), 7);
    PriceByItem.Add(TEXT("item_crude_ingot"), 18);
    PriceByItem.Add(TEXT("item_salvaged_plank"), 6);
    PriceByItem.Add(TEXT("item_building_plate"), 26);
    PriceByItem.Add(TEXT("item_field_bandage"), 18);
    PriceByItem.Add(TEXT("item_medic_wrap"), 32);
    PriceByItem.Add(TEXT("item_rustblade"), 48);
    PriceByItem.Add(TEXT("item_quilted_vest"), 42);
}
