#pragma once

#include "CoreMinimal.h"
#include "AshfrontierSquadTypes.generated.h"

UENUM(BlueprintType)
enum class EAshfrontierSquadOrder : uint8
{
    Idle UMETA(DisplayName = "Idle"),
    Moving UMETA(DisplayName = "Moving"),
    Following UMETA(DisplayName = "Following"),
    Holding UMETA(DisplayName = "Holding")
};
