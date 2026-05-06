#pragma once

#include "CoreMinimal.h"
#include "AshfrontierLegalTypes.generated.h"

UENUM(BlueprintType)
enum class EAshfrontierLegalEventType : uint8
{
    Theft UMETA(DisplayName = "Theft"),
    Attack UMETA(DisplayName = "Attack"),
    SelfDefense UMETA(DisplayName = "Self Defense"),
    RestrictedArea UMETA(DisplayName = "Restricted Area")
};

UENUM(BlueprintType)
enum class EAshfrontierGuardReaction : uint8
{
    Allow UMETA(DisplayName = "Allow"),
    Warning UMETA(DisplayName = "Warning"),
    Hostile UMETA(DisplayName = "Hostile"),
    Pursuit UMETA(DisplayName = "Pursuit")
};

UENUM(BlueprintType)
enum class EAshfrontierGuardState : uint8
{
    Idle UMETA(DisplayName = "Idle"),
    Allowing UMETA(DisplayName = "Allowing"),
    Warning UMETA(DisplayName = "Warning"),
    Hostile UMETA(DisplayName = "Hostile"),
    Pursuit UMETA(DisplayName = "Pursuit")
};

USTRUCT(BlueprintType)
struct ASHFRONTIER_API FAshfrontierLegalRule
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName LegalProfileId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName AuthorityFactionId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EAshfrontierLegalEventType EventType = EAshfrontierLegalEventType::Theft;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EAshfrontierGuardReaction Reaction = EAshfrontierGuardReaction::Allow;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 MemorySeconds = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 RelationDelta = 0;
};

USTRUCT(BlueprintType)
struct ASHFRONTIER_API FAshfrontierCrimeEventRecord
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EAshfrontierLegalEventType EventType = EAshfrontierLegalEventType::Theft;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EAshfrontierGuardReaction Reaction = EAshfrontierGuardReaction::Allow;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName OffenderFactionId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName AuthorityFactionId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 RelationDelta = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float RemainingMemorySeconds = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString SummaryZh;
};
