#include "AshfrontierRecruitSystemComponent.h"

#include "AshfrontierCharacter.h"
#include "AshfrontierInventoryComponent.h"
#include "AshfrontierSquadManagerComponent.h"

namespace AshfrontierRecruit
{
const FName CreditsItemId(TEXT("item_ash_credit"));
}

UAshfrontierRecruitSystemComponent::UAshfrontierRecruitSystemComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UAshfrontierRecruitSystemComponent::Recruit(AAshfrontierCharacter* Recruiter, AAshfrontierCharacter* Candidate, UAshfrontierSquadManagerComponent* SquadManager) const
{
    if (!IsValid(Recruiter) || !IsValid(Candidate) || !SquadManager || !Candidate->IsRecruitable())
    {
        return false;
    }

    UAshfrontierInventoryComponent* RecruiterInventory = Recruiter->GetInventory();
    if (!RecruiterInventory || !RecruiterInventory->HasItem(AshfrontierRecruit::CreditsItemId, Candidate->GetRecruitCost()))
    {
        return false;
    }

    if (!RecruiterInventory->RemoveItem(AshfrontierRecruit::CreditsItemId, Candidate->GetRecruitCost()))
    {
        return false;
    }

    Candidate->SetRecruitable(false, 0);
    Candidate->SetCharacterTeam(EAshfrontierCharacterTeam::PlayerSquad);
    SquadManager->AddExistingMember(Candidate);
    return true;
}
