#pragma once

#include "CoreMinimal.h"
#include "AshfrontierCharacter.h"
#include "AshfrontierRouteAgent.generated.h"

UCLASS()
class ASHFRONTIER_API AAshfrontierRouteAgent : public AAshfrontierCharacter
{
    GENERATED_BODY()

public:
    AAshfrontierRouteAgent();

    virtual void Tick(float DeltaSeconds) override;

    void SetRoute(const FName& NewRouteId, const FString& NewRouteNameZh, const TArray<FVector>& NewWaypoints, bool bShouldLoop);

    FName GetRouteId() const;
    int32 GetWaypointCount() const;
    int32 GetActiveWaypointIndex() const;

private:
    void AdvanceRouteIfNeeded();

    UPROPERTY(VisibleAnywhere, Category = "Route")
    FName RouteId;

    UPROPERTY(VisibleAnywhere, Category = "Route")
    FString RouteNameZh;

    UPROPERTY(VisibleAnywhere, Category = "Route")
    TArray<FVector> Waypoints;

    UPROPERTY(VisibleAnywhere, Category = "Route")
    int32 ActiveWaypointIndex = INDEX_NONE;

    UPROPERTY(VisibleAnywhere, Category = "Route")
    bool bLoopRoute = true;

    UPROPERTY(EditDefaultsOnly, Category = "Route")
    float RouteArrivalRadius = 120.0f;
};
