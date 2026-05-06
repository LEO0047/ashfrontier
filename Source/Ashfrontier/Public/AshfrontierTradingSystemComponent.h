#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AshfrontierTradingSystemComponent.generated.h"

class AAshfrontierCharacter;

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierTradingSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierTradingSystemComponent();

    bool BuyItem(AAshfrontierCharacter* Buyer, AAshfrontierCharacter* Seller, const FName& ItemId, int32 Count) const;
    bool SellItem(AAshfrontierCharacter* Seller, AAshfrontierCharacter* Buyer, const FName& ItemId, int32 Count) const;
    int32 GetUnitPrice(const FName& ItemId) const;

private:
    void BuildPriceTable();

    UPROPERTY(VisibleAnywhere, Category = "Trading")
    TMap<FName, int32> PriceByItem;
};
