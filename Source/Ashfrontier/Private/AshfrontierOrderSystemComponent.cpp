#include "AshfrontierOrderSystemComponent.h"

#include "AshfrontierCharacter.h"

UAshfrontierOrderSystemComponent::UAshfrontierOrderSystemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

int32 UAshfrontierOrderSystemComponent::IssueMove(const TArray<AAshfrontierCharacter*>& Members, const FVector& Destination) const
{
    int32 Commanded = 0;
    const int32 MemberCount = Members.Num();
    for (int32 Index = 0; Index < MemberCount; ++Index)
    {
        AAshfrontierCharacter* Member = Members[Index];
        if (!IsValid(Member))
        {
            continue;
        }

        Member->CommandMoveTo(Destination + FormationOffset(Index, MemberCount));
        ++Commanded;
    }
    return Commanded;
}

int32 UAshfrontierOrderSystemComponent::IssueFollow(const TArray<AAshfrontierCharacter*>& Members, AAshfrontierCharacter* TargetCharacter) const
{
    if (!IsValid(TargetCharacter))
    {
        return 0;
    }

    int32 Commanded = 0;
    for (AAshfrontierCharacter* Member : Members)
    {
        if (!IsValid(Member))
        {
            continue;
        }

        if (Member == TargetCharacter)
        {
            Member->CommandHold();
        }
        else
        {
            Member->CommandFollow(TargetCharacter);
        }
        ++Commanded;
    }
    return Commanded;
}

int32 UAshfrontierOrderSystemComponent::IssueHold(const TArray<AAshfrontierCharacter*>& Members) const
{
    int32 Commanded = 0;
    for (AAshfrontierCharacter* Member : Members)
    {
        if (IsValid(Member))
        {
            Member->CommandHold();
            ++Commanded;
        }
    }
    return Commanded;
}

FVector UAshfrontierOrderSystemComponent::FormationOffset(int32 MemberIndex, int32 MemberCount) const
{
    if (MemberCount <= 1)
    {
        return FVector::ZeroVector;
    }

    const float Spacing = 140.0f;
    const float CenteredIndex = static_cast<float>(MemberIndex) - (static_cast<float>(MemberCount - 1) * 0.5f);
    return FVector(0.0f, CenteredIndex * Spacing, 0.0f);
}
