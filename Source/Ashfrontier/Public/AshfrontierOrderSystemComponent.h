#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AshfrontierOrderSystemComponent.generated.h"

class AAshfrontierCharacter;

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierOrderSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierOrderSystemComponent();

    int32 IssueMove(const TArray<AAshfrontierCharacter*>& Members, const FVector& Destination) const;
    int32 IssueFollow(const TArray<AAshfrontierCharacter*>& Members, AAshfrontierCharacter* TargetCharacter) const;
    int32 IssueHold(const TArray<AAshfrontierCharacter*>& Members) const;

private:
    FVector FormationOffset(int32 MemberIndex, int32 MemberCount) const;
};
