#pragma once

#include "CoreMinimal.h"
#include "AshfrontierCombatTypes.h"
#include "AshfrontierSquadTypes.h"
#include "GameFramework/Character.h"
#include "AshfrontierCharacter.generated.h"

class UAshfrontierDamageModelComponent;
class UAshfrontierInventoryComponent;
class UCameraComponent;
class UStaticMeshComponent;
class USpringArmComponent;

UCLASS()
class ASHFRONTIER_API AAshfrontierCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAshfrontierCharacter();

    virtual void Tick(float DeltaSeconds) override;

    void SetSquadIndex(int32 NewIndex);
    int32 GetSquadIndex() const;

    void SetSquadDisplayName(const FString& NewName);
    const FString& GetSquadDisplayName() const;

    void SetSelected(bool bNewSelected);
    bool IsSelected() const;

    void CommandMoveTo(const FVector& Destination);
    void CommandFollow(AAshfrontierCharacter* TargetCharacter);
    void CommandHold();

    EAshfrontierSquadOrder GetCurrentOrder() const;
    FVector GetCommandDestination() const;
    AAshfrontierCharacter* GetFollowTarget() const;
    FString GetOrderLabelZh() const;

    void SetCameraArmLength(float NewArmLength);
    float GetCameraArmLength() const;

    UAshfrontierDamageModelComponent* GetDamageModel() const;
    UAshfrontierInventoryComponent* GetInventory() const;

    void SetCharacterTeam(EAshfrontierCharacterTeam NewTeam);
    EAshfrontierCharacterTeam GetCharacterTeam() const;
    bool IsHostileToPlayer() const;

    void SetCarriedTarget(AAshfrontierCharacter* Target);
    AAshfrontierCharacter* GetCarriedTarget() const;
    void SetCarrier(AAshfrontierCharacter* NewCarrier);
    AAshfrontierCharacter* GetCarrier() const;
    bool IsBeingCarried() const;

    void SetRecruitable(bool bNewRecruitable, int32 NewRecruitCost);
    bool IsRecruitable() const;
    int32 GetRecruitCost() const;
    void SetShopkeeper(bool bNewShopkeeper);
    bool IsShopkeeper() const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prototype")
    TObjectPtr<UStaticMeshComponent> PlaceholderBody;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prototype")
    TObjectPtr<UStaticMeshComponent> SelectionMarker;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage")
    TObjectPtr<UAshfrontierDamageModelComponent> DamageModel;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<UAshfrontierInventoryComponent> Inventory;

private:
    void UpdateSquadOrder(float DeltaSeconds);
    void UpdateCarriedPose();

    UPROPERTY(VisibleAnywhere, Category = "Squad")
    int32 SquadIndex = INDEX_NONE;

    UPROPERTY(VisibleAnywhere, Category = "Squad")
    FString SquadDisplayName;

    UPROPERTY(VisibleAnywhere, Category = "Squad")
    bool bSelected = false;

    UPROPERTY(VisibleAnywhere, Category = "Squad")
    EAshfrontierSquadOrder CurrentOrder = EAshfrontierSquadOrder::Idle;

    UPROPERTY(VisibleAnywhere, Category = "Squad")
    FVector CommandDestination = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, Category = "Squad")
    TWeakObjectPtr<AAshfrontierCharacter> FollowTarget;

    UPROPERTY(EditDefaultsOnly, Category = "Squad")
    float ArrivalRadius = 70.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Squad")
    float FollowDistance = 180.0f;

    UPROPERTY(VisibleAnywhere, Category = "Combat")
    EAshfrontierCharacterTeam CharacterTeam = EAshfrontierCharacterTeam::Neutral;

    UPROPERTY(Transient)
    TWeakObjectPtr<AAshfrontierCharacter> CarriedTarget;

    UPROPERTY(Transient)
    TWeakObjectPtr<AAshfrontierCharacter> Carrier;

    UPROPERTY(VisibleAnywhere, Category = "Recruit")
    bool bRecruitable = false;

    UPROPERTY(VisibleAnywhere, Category = "Recruit")
    int32 RecruitCost = 0;

    UPROPERTY(VisibleAnywhere, Category = "Trading")
    bool bShopkeeper = false;
};
