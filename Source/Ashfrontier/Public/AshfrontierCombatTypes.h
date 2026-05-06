#pragma once

#include "CoreMinimal.h"
#include "AshfrontierCombatTypes.generated.h"

UENUM(BlueprintType)
enum class EAshfrontierBodyPart : uint8
{
    Head UMETA(DisplayName = "Head"),
    Chest UMETA(DisplayName = "Chest"),
    Abdomen UMETA(DisplayName = "Abdomen"),
    LeftArm UMETA(DisplayName = "Left Arm"),
    RightArm UMETA(DisplayName = "Right Arm"),
    LeftLeg UMETA(DisplayName = "Left Leg"),
    RightLeg UMETA(DisplayName = "Right Leg")
};

UENUM(BlueprintType)
enum class EAshfrontierConsciousState : uint8
{
    Conscious UMETA(DisplayName = "Conscious"),
    Downed UMETA(DisplayName = "Downed"),
    Unconscious UMETA(DisplayName = "Unconscious"),
    Stable UMETA(DisplayName = "Stable")
};

UENUM(BlueprintType)
enum class EAshfrontierCharacterTeam : uint8
{
    PlayerSquad UMETA(DisplayName = "Player Squad"),
    Neutral UMETA(DisplayName = "Neutral"),
    Hostile UMETA(DisplayName = "Hostile")
};

USTRUCT(BlueprintType)
struct ASHFRONTIER_API FAshfrontierBodyPartHealth
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EAshfrontierBodyPart BodyPart = EAshfrontierBodyPart::Chest;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float CurrentHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bCrippled = false;
};
