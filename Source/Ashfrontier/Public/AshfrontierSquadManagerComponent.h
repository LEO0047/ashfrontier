#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AshfrontierSquadManagerComponent.generated.h"

class AAshfrontierCharacter;

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierSquadManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierSquadManagerComponent();

    void EnsureStartingSquad(APlayerController* OwnerController, TSubclassOf<AAshfrontierCharacter> CharacterClass, int32 DesiredCount);

    void SelectMember(AAshfrontierCharacter* Member, bool bAppendSelection);
    void SelectMemberAtIndex(int32 MemberIndex, bool bAppendSelection);
    void SelectAll();
    void SelectNext();
    void ClearSelection();

    TArray<AAshfrontierCharacter*> GetSquadMembers() const;
    TArray<AAshfrontierCharacter*> GetSelectedMembers() const;
    AAshfrontierCharacter* GetLeader() const;
    AAshfrontierCharacter* GetPrimarySelected() const;

    int32 GetSquadCount() const;
    int32 GetSelectedCount() const;

private:
    void CompactMembers();
    void ApplySelectionVisuals();
    FString MakeDefaultSquadName(int32 MemberIndex) const;

    UPROPERTY(Transient)
    TArray<TObjectPtr<AAshfrontierCharacter>> SquadMembers;

    UPROPERTY(Transient)
    TArray<TObjectPtr<AAshfrontierCharacter>> SelectedMembers;
};
