#include "AshfrontierSquadManagerComponent.h"

#include "AshfrontierCharacter.h"
#include "AshfrontierInventoryComponent.h"
#include "GameFramework/PlayerController.h"

UAshfrontierSquadManagerComponent::UAshfrontierSquadManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAshfrontierSquadManagerComponent::EnsureStartingSquad(APlayerController* OwnerController, TSubclassOf<AAshfrontierCharacter> CharacterClass, int32 DesiredCount)
{
    if (!OwnerController || !CharacterClass || DesiredCount <= 0)
    {
        return;
    }

    UWorld* World = OwnerController->GetWorld();
    if (!World)
    {
        return;
    }

    DesiredCount = FMath::Clamp(DesiredCount, 1, 4);
    CompactMembers();

    const FVector SpawnOrigin(0.0f, 0.0f, 120.0f);
    AAshfrontierCharacter* ExistingPawn = Cast<AAshfrontierCharacter>(OwnerController->GetPawn());
    if (ExistingPawn && !SquadMembers.Contains(ExistingPawn))
    {
        ExistingPawn->SetActorLocation(SpawnOrigin);
        SquadMembers.Add(ExistingPawn);
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = OwnerController;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    if (!ExistingPawn && SquadMembers.Num() == 0)
    {
        AAshfrontierCharacter* Leader = World->SpawnActor<AAshfrontierCharacter>(CharacterClass, SpawnOrigin, FRotator::ZeroRotator, SpawnParams);
        if (Leader)
        {
            SquadMembers.Add(Leader);
            OwnerController->Possess(Leader);
        }
    }

    while (SquadMembers.Num() < DesiredCount)
    {
        const int32 MemberIndex = SquadMembers.Num();
        const FVector Offset(0.0f, MemberIndex * 180.0f, 0.0f);
        AAshfrontierCharacter* Member = World->SpawnActor<AAshfrontierCharacter>(CharacterClass, SpawnOrigin + Offset, FRotator::ZeroRotator, SpawnParams);
        if (!Member)
        {
            break;
        }

        Member->SpawnDefaultController();
        SquadMembers.Add(Member);
    }

    for (int32 Index = 0; Index < SquadMembers.Num(); ++Index)
    {
        if (AAshfrontierCharacter* Member = SquadMembers[Index])
        {
            Member->SetSquadIndex(Index);
            Member->SetSquadDisplayName(MakeDefaultSquadName(Index));
            Member->SetCharacterTeam(EAshfrontierCharacterTeam::PlayerSquad);
            Member->SetFactionId(TEXT("faction_player_squad"));
            if (Index == 0 && Member->GetInventory() && Member->GetInventory()->GetItemCount(TEXT("item_ash_credit")) == 0)
            {
                Member->GetInventory()->AddItem(TEXT("item_ash_credit"), 180);
                Member->GetInventory()->AddItem(TEXT("item_field_bandage"), 2);
            }
        }
    }

    if (SelectedMembers.Num() == 0 && SquadMembers.Num() > 0)
    {
        SelectMemberAtIndex(0, false);
    }
    else
    {
        ApplySelectionVisuals();
    }
}

bool UAshfrontierSquadManagerComponent::AddExistingMember(AAshfrontierCharacter* Member)
{
    CompactMembers();
    if (!IsValid(Member) || SquadMembers.Contains(Member) || SquadMembers.Num() >= 4)
    {
        return false;
    }

    Member->SetCharacterTeam(EAshfrontierCharacterTeam::PlayerSquad);
    Member->SetFactionId(TEXT("faction_player_squad"));
    Member->SetSquadIndex(SquadMembers.Num());
    Member->SetSquadDisplayName(MakeDefaultSquadName(SquadMembers.Num()));
    SquadMembers.Add(Member);
    SelectMember(Member, true);
    return true;
}

void UAshfrontierSquadManagerComponent::SelectMember(AAshfrontierCharacter* Member, bool bAppendSelection)
{
    CompactMembers();
    if (!Member || !SquadMembers.Contains(Member))
    {
        return;
    }

    if (!bAppendSelection)
    {
        SelectedMembers.Reset();
    }

    if (!SelectedMembers.Contains(Member))
    {
        SelectedMembers.Add(Member);
    }

    ApplySelectionVisuals();
}

void UAshfrontierSquadManagerComponent::SelectMemberAtIndex(int32 MemberIndex, bool bAppendSelection)
{
    CompactMembers();
    if (!SquadMembers.IsValidIndex(MemberIndex))
    {
        return;
    }

    SelectMember(SquadMembers[MemberIndex], bAppendSelection);
}

void UAshfrontierSquadManagerComponent::SelectAll()
{
    CompactMembers();
    SelectedMembers.Reset();
    for (AAshfrontierCharacter* Member : SquadMembers)
    {
        if (Member)
        {
            SelectedMembers.Add(Member);
        }
    }
    ApplySelectionVisuals();
}

void UAshfrontierSquadManagerComponent::SelectNext()
{
    CompactMembers();
    if (SquadMembers.Num() == 0)
    {
        return;
    }

    const AAshfrontierCharacter* Current = GetPrimarySelected();
    int32 CurrentIndex = Current ? SquadMembers.IndexOfByKey(Current) : INDEX_NONE;
    CurrentIndex = CurrentIndex == INDEX_NONE ? 0 : (CurrentIndex + 1) % SquadMembers.Num();
    SelectMemberAtIndex(CurrentIndex, false);
}

void UAshfrontierSquadManagerComponent::ClearSelection()
{
    SelectedMembers.Reset();
    ApplySelectionVisuals();
}

TArray<AAshfrontierCharacter*> UAshfrontierSquadManagerComponent::GetSquadMembers() const
{
    TArray<AAshfrontierCharacter*> Result;
    for (AAshfrontierCharacter* Member : SquadMembers)
    {
        if (IsValid(Member))
        {
            Result.Add(Member);
        }
    }
    return Result;
}

TArray<AAshfrontierCharacter*> UAshfrontierSquadManagerComponent::GetSelectedMembers() const
{
    TArray<AAshfrontierCharacter*> Result;
    for (AAshfrontierCharacter* Member : SelectedMembers)
    {
        if (IsValid(Member))
        {
            Result.Add(Member);
        }
    }
    return Result;
}

AAshfrontierCharacter* UAshfrontierSquadManagerComponent::GetLeader() const
{
    return SquadMembers.Num() > 0 ? SquadMembers[0].Get() : nullptr;
}

AAshfrontierCharacter* UAshfrontierSquadManagerComponent::GetPrimarySelected() const
{
    return SelectedMembers.Num() > 0 ? SelectedMembers[0].Get() : GetLeader();
}

int32 UAshfrontierSquadManagerComponent::GetSquadCount() const
{
    return GetSquadMembers().Num();
}

int32 UAshfrontierSquadManagerComponent::GetSelectedCount() const
{
    return GetSelectedMembers().Num();
}

void UAshfrontierSquadManagerComponent::CompactMembers()
{
    SquadMembers.RemoveAll([](const TObjectPtr<AAshfrontierCharacter>& Member)
    {
        return !IsValid(Member);
    });
    SelectedMembers.RemoveAll([](const TObjectPtr<AAshfrontierCharacter>& Member)
    {
        return !IsValid(Member);
    });
}

void UAshfrontierSquadManagerComponent::ApplySelectionVisuals()
{
    for (AAshfrontierCharacter* Member : SquadMembers)
    {
        if (Member)
        {
            Member->SetSelected(SelectedMembers.Contains(Member));
        }
    }
}

FString UAshfrontierSquadManagerComponent::MakeDefaultSquadName(int32 MemberIndex) const
{
    static const TCHAR* Names[] = {
        TEXT("灰燼前鋒"),
        TEXT("鹽路斥候"),
        TEXT("野地醫士"),
        TEXT("工坊守手"),
    };

    if (MemberIndex >= 0 && MemberIndex < UE_ARRAY_COUNT(Names))
    {
        return Names[MemberIndex];
    }

    return FString::Printf(TEXT("隊員 %d"), MemberIndex + 1);
}
