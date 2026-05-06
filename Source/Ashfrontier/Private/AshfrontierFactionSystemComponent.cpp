#include "AshfrontierFactionSystemComponent.h"

UAshfrontierFactionSystemComponent::UAshfrontierFactionSystemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    BuildDefaultRelations();
}

int32 UAshfrontierFactionSystemComponent::GetRelation(const FName& FactionA, const FName& FactionB) const
{
    if (FactionA.IsNone() || FactionB.IsNone() || FactionA == FactionB)
    {
        return 100;
    }

    if (const int32* Relation = RelationByPair.Find(MakeRelationKey(FactionA, FactionB)))
    {
        return *Relation;
    }

    return 0;
}

void UAshfrontierFactionSystemComponent::SetRelation(const FName& FactionA, const FName& FactionB, int32 Value)
{
    if (FactionA.IsNone() || FactionB.IsNone() || FactionA == FactionB)
    {
        return;
    }

    RelationByPair.Add(MakeRelationKey(FactionA, FactionB), FMath::Clamp(Value, -100, 100));
}

int32 UAshfrontierFactionSystemComponent::AdjustRelation(const FName& FactionA, const FName& FactionB, int32 Delta)
{
    const int32 NewValue = FMath::Clamp(GetRelation(FactionA, FactionB) + Delta, -100, 100);
    SetRelation(FactionA, FactionB, NewValue);
    return NewValue;
}

bool UAshfrontierFactionSystemComponent::AreHostile(const FName& FactionA, const FName& FactionB) const
{
    return GetRelation(FactionA, FactionB) <= -25;
}

const TMap<FName, int32>& UAshfrontierFactionSystemComponent::GetRelationsForSave() const
{
    return RelationByPair;
}

void UAshfrontierFactionSystemComponent::SetRelationsForSave(const TMap<FName, int32>& NewRelations)
{
    RelationByPair = NewRelations;
}

void UAshfrontierFactionSystemComponent::BuildDefaultRelations()
{
    RelationByPair.Reset();
    SetRelation(TEXT("faction_saltwardens"), TEXT("faction_glasshouse"), 15);
    SetRelation(TEXT("faction_saltwardens"), TEXT("faction_dustrunners"), -10);
    SetRelation(TEXT("faction_glasshouse"), TEXT("faction_dustrunners"), 0);
    SetRelation(TEXT("faction_player_squad"), TEXT("faction_saltwardens"), 0);
    SetRelation(TEXT("faction_player_squad"), TEXT("faction_glasshouse"), 0);
    SetRelation(TEXT("faction_player_squad"), TEXT("faction_dustrunners"), 0);
}

FName UAshfrontierFactionSystemComponent::MakeRelationKey(const FName& FactionA, const FName& FactionB) const
{
    FString A = FactionA.ToString();
    FString B = FactionB.ToString();
    if (A > B)
    {
        Swap(A, B);
    }

    return FName(*FString::Printf(TEXT("%s|%s"), *A, *B));
}
