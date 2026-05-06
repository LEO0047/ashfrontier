#include "AshfrontierCameraControllerComponent.h"

#include "AshfrontierCharacter.h"
#include "GameFramework/PlayerController.h"

UAshfrontierCameraControllerComponent::UAshfrontierCameraControllerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAshfrontierCameraControllerComponent::FocusCharacter(APlayerController* OwnerController, AAshfrontierCharacter* Character) const
{
    if (!OwnerController || !IsValid(Character))
    {
        return;
    }

    ApplyCameraMode(Character);
    OwnerController->SetViewTarget(Character);
}

void UAshfrontierCameraControllerComponent::ToggleTacticalCamera(APlayerController* OwnerController, AAshfrontierCharacter* Character)
{
    bTacticalCamera = !bTacticalCamera;
    FocusCharacter(OwnerController, Character);
}

bool UAshfrontierCameraControllerComponent::IsTacticalCamera() const
{
    return bTacticalCamera;
}

float UAshfrontierCameraControllerComponent::GetDesiredArmLength() const
{
    return bTacticalCamera ? TacticalArmLength : ThirdPersonArmLength;
}

void UAshfrontierCameraControllerComponent::ApplyCameraMode(AAshfrontierCharacter* Character) const
{
    if (IsValid(Character))
    {
        Character->SetCameraArmLength(GetDesiredArmLength());
    }
}
