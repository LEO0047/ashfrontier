#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AshfrontierCameraControllerComponent.generated.h"

class AAshfrontierCharacter;

UCLASS(ClassGroup = (Ashfrontier), meta = (BlueprintSpawnableComponent))
class ASHFRONTIER_API UAshfrontierCameraControllerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAshfrontierCameraControllerComponent();

    void FocusCharacter(APlayerController* OwnerController, AAshfrontierCharacter* Character) const;
    void ToggleTacticalCamera(APlayerController* OwnerController, AAshfrontierCharacter* Character);
    bool IsTacticalCamera() const;
    float GetDesiredArmLength() const;

private:
    void ApplyCameraMode(AAshfrontierCharacter* Character) const;

    UPROPERTY(EditDefaultsOnly, Category = "Camera")
    float ThirdPersonArmLength = 420.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Camera")
    float TacticalArmLength = 1150.0f;

    UPROPERTY(VisibleAnywhere, Category = "Camera")
    bool bTacticalCamera = false;
};
