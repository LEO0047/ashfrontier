#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AshfrontierInventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct ASHFRONTIER_API FAshfrontierItemStack
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName ItemId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 Count = 0;
};

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierInventoryComponent();

    bool AddItem(const FName& ItemId, int32 Count);
    bool RemoveItem(const FName& ItemId, int32 Count);
    bool HasItem(const FName& ItemId, int32 Count) const;
    int32 GetItemCount(const FName& ItemId) const;
    const TArray<FAshfrontierItemStack>& GetStacks() const;
    void SetStacksForSave(const TArray<FAshfrontierItemStack>& NewStacks);
    void ClearInventory();

private:
    FAshfrontierItemStack* FindStack(const FName& ItemId);
    const FAshfrontierItemStack* FindStack(const FName& ItemId) const;

    UPROPERTY(VisibleAnywhere, Category = "Inventory")
    TArray<FAshfrontierItemStack> Stacks;
};
