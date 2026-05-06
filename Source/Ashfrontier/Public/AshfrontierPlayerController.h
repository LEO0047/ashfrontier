#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AshfrontierPlayerController.generated.h"

class AAshfrontierCharacter;
class AAshfrontierPlacedBuilding;
class UAshfrontierBuildingPlacementSystemComponent;
class UAshfrontierCameraControllerComponent;
class UAshfrontierCarrySystemComponent;
class UAshfrontierCombatResolverComponent;
class UAshfrontierConstructionSystemComponent;
class UAshfrontierMedicalSystemComponent;
class UAshfrontierOrderSystemComponent;
class UAshfrontierProductionSystemComponent;
class UAshfrontierRecruitSystemComponent;
class UAshfrontierResourceNodeSystemComponent;
class UAshfrontierSquadManagerComponent;
class UAshfrontierStorageSystemComponent;
class UAshfrontierTradingSystemComponent;

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
    UAshfrontierRecruitSystemComponent* GetRecruitSystem() const;
    UAshfrontierTradingSystemComponent* GetTradingSystem() const;
    UAshfrontierBuildingPlacementSystemComponent* GetBuildingPlacementSystem() const;
    UAshfrontierConstructionSystemComponent* GetConstructionSystem() const;
    UAshfrontierResourceNodeSystemComponent* GetResourceNodeSystem() const;
    UAshfrontierProductionSystemComponent* GetProductionSystem() const;
    UAshfrontierStorageSystemComponent* GetStorageSystem() const;
    AAshfrontierPlacedBuilding* GetLastPlacedBuilding() const;

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
    void HandleRecruitPressed();
    void HandleTradePressed();
    void HandleBuildPressed();
    void HandleGatherPressed();
    void HandleProducePressed();
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

    UPROPERTY(VisibleAnywhere, Category = "Recruit")
    TObjectPtr<UAshfrontierRecruitSystemComponent> RecruitSystem;

    UPROPERTY(VisibleAnywhere, Category = "Trading")
    TObjectPtr<UAshfrontierTradingSystemComponent> TradingSystem;

    UPROPERTY(VisibleAnywhere, Category = "Building")
    TObjectPtr<UAshfrontierBuildingPlacementSystemComponent> BuildingPlacementSystem;

    UPROPERTY(VisibleAnywhere, Category = "Building")
    TObjectPtr<UAshfrontierConstructionSystemComponent> ConstructionSystem;

    UPROPERTY(VisibleAnywhere, Category = "Building")
    TObjectPtr<UAshfrontierResourceNodeSystemComponent> ResourceNodeSystem;

    UPROPERTY(VisibleAnywhere, Category = "Building")
    TObjectPtr<UAshfrontierProductionSystemComponent> ProductionSystem;

    UPROPERTY(VisibleAnywhere, Category = "Building")
    TObjectPtr<UAshfrontierStorageSystemComponent> StorageSystem;

    UPROPERTY(Transient)
    TWeakObjectPtr<AAshfrontierPlacedBuilding> LastPlacedBuilding;
};
