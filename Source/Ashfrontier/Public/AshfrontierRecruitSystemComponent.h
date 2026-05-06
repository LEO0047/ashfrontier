#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AshfrontierRecruitSystemComponent.generated.h"

class AAshfrontierCharacter;
class UAshfrontierSquadManagerComponent;

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierRecruitSystemComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierRecruitSystemComponent();

    bool Recruit(AAshfrontierCharacter* Recruiter, AAshfrontierCharacter* Candidate, UAshfrontierSquadManagerComponent* SquadManager) const;
};
