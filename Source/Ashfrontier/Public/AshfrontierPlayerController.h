#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AshfrontierPlayerController.generated.h"

class AAshfrontierCharacter;
class UAshfrontierCameraControllerComponent;
class UAshfrontierOrderSystemComponent;
class UAshfrontierSquadManagerComponent;

UCLASS()
class ASHFRONTIER_API AAshfrontierPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AAshfrontierPlayerController();

    UAshfrontierSquadManagerComponent* GetSquadManager() const;
    UAshfrontierOrderSystemComponent* GetOrderSystem() const;
    UAshfrontierCameraControllerComponent* GetCameraController() const;

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

private:
    void HandleSelectPressed();
    void HandleIssueMovePressed();
    void HandleFollowPressed();
    void HandleHoldPressed();
    void HandleSelectAllPressed();
    void HandleSelectNextPressed();
    void HandleToggleTacticalCamera();
    void SelectSlot1();
    void SelectSlot2();
    void SelectSlot3();
    void SelectSlot4();
    void SelectSlot(int32 ZeroBasedIndex);
    bool TraceCursor(FHitResult& OutHit, ECollisionChannel Channel) const;
    AAshfrontierCharacter* CurrentViewCharacter() const;

    UPROPERTY(VisibleAnywhere, Category = "Squad")
    TObjectPtr<UAshfrontierSquadManagerComponent> SquadManager;

    UPROPERTY(VisibleAnywhere, Category = "Squad")
    TObjectPtr<UAshfrontierOrderSystemComponent> OrderSystem;

    UPROPERTY(VisibleAnywhere, Category = "Camera")
    TObjectPtr<UAshfrontierCameraControllerComponent> CameraController;
};
