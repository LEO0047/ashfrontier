#include "AshfrontierRouteAgent.h"

AAshfrontierRouteAgent::AAshfrontierRouteAgent()
{
    PrimaryActorTick.bCanEverTick = true;
    SetCharacterTeam(EAshfrontierCharacterTeam::Neutral);
}

void AAshfrontierRouteAgent::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    AdvanceRouteIfNeeded();
}

void AAshfrontierRouteAgent::SetRoute(const FName& NewRouteId, const FString& NewRouteNameZh, const TArray<FVector>& NewWaypoints, bool bShouldLoop)
{
    RouteId = NewRouteId;
    RouteNameZh = NewRouteNameZh;
    Waypoints = NewWaypoints;
    bLoopRoute = bShouldLoop;
    ActiveWaypointIndex = Waypoints.Num() > 0 ? 0 : INDEX_NONE;

    SetSquadDisplayName(RouteNameZh);
    SetSquadIndex(INDEX_NONE);

    if (Waypoints.IsValidIndex(ActiveWaypointIndex))
    {
        SetActorLocation(Waypoints[ActiveWaypointIndex]);
        const int32 NextIndex = Waypoints.Num() > 1 ? 1 : 0;
        ActiveWaypointIndex = NextIndex;
        CommandMoveTo(Waypoints[ActiveWaypointIndex]);
    }
}

FName AAshfrontierRouteAgent::GetRouteId() const
{
    return RouteId;
}

int32 AAshfrontierRouteAgent::GetWaypointCount() const
{
    return Waypoints.Num();
}

int32 AAshfrontierRouteAgent::GetActiveWaypointIndex() const
{
    return ActiveWaypointIndex;
}

void AAshfrontierRouteAgent::AdvanceRouteIfNeeded()
{
    if (!Waypoints.IsValidIndex(ActiveWaypointIndex))
    {
        return;
    }

    if (FVector::Dist2D(GetActorLocation(), Waypoints[ActiveWaypointIndex]) > RouteArrivalRadius)
    {
        return;
    }

    int32 NextIndex = ActiveWaypointIndex + 1;
    if (NextIndex >= Waypoints.Num())
    {
        if (!bLoopRoute)
        {
            CommandHold();
            return;
        }
        NextIndex = 0;
    }

    ActiveWaypointIndex = NextIndex;
    CommandMoveTo(Waypoints[ActiveWaypointIndex]);
}
