#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AshfrontierPlayerController.generated.h"

class AAshfrontierCharacter;
class UAshfrontierCameraControllerComponent;
class UAshfrontierCarrySystemComponent;
class UAshfrontierCombatResolverComponent;
class UAshfrontierMedicalSystemComponent;
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
    UAshfrontierCombatResolverComponent* GetCombatResolver() const;
    UAshfrontierCarrySystemComponent* GetCarrySystem() const;
    UAshfrontierMedicalSystemComponent* GetMedicalSystem() const;

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

private:
    void HandleSelectPressed();
    void HandleIssueMovePressed();
    void HandleFollowPressed();
    void HandleHoldPressed();
    void HandleCarryPressed();
    void HandleMedicalPressed();
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

    UPROPERTY(VisibleAnywhere, Category = "Combat")
    TObjectPtr<UAshfrontierCombatResolverComponent> CombatResolver;

    UPROPERTY(VisibleAnywhere, Category = "Combat")
    TObjectPtr<UAshfrontierCarrySystemComponent> CarrySystem;

    UPROPERTY(VisibleAnywhere, Category = "Combat")
    TObjectPtr<UAshfrontierMedicalSystemComponent> MedicalSystem;
};
