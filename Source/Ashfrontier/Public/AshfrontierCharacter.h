#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AshfrontierCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ASHFRONTIER_API AAshfrontierCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAshfrontierCharacter();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<UCameraComponent> FollowCamera;
};
