#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AshfrontierFactionSystemComponent.generated.h"

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierFactionSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierFactionSystemComponent();

    int32 GetRelation(const FName& FactionA, const FName& FactionB) const;
    void SetRelation(const FName& FactionA, const FName& FactionB, int32 Value);
    int32 AdjustRelation(const FName& FactionA, const FName& FactionB, int32 Delta);
    bool AreHostile(const FName& FactionA, const FName& FactionB) const;

private:
    void BuildDefaultRelations();
    FName MakeRelationKey(const FName& FactionA, const FName& FactionB) const;

    UPROPERTY(VisibleAnywhere, Category = "Faction")
    TMap<FName, int32> RelationByPair;
};
