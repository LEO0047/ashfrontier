#if WITH_DEV_AUTOMATION_TESTS

#include "AshfrontierWorldBlockoutDirector.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/AutomationTest.h"

namespace AshfrontierWorldTest
{
UWorld* FindEditorTestWorld()
{
    if (!GEngine)
    {
        return nullptr;
    }

    for (const FWorldContext& Context : GEngine->GetWorldContexts())
    {
        UWorld* World = Context.World();
        if (World && (Context.WorldType == EWorldType::Editor || Context.WorldType == EWorldType::PIE || Context.WorldType == EWorldType::Game))
        {
            return World;
        }
    }

    return nullptr;
}

bool HasZoneType(const TArray<FAshfrontierWorldZoneRecord>& Zones, EAshfrontierWorldZoneType ZoneType)
{
    return Zones.ContainsByPredicate([ZoneType](const FAshfrontierWorldZoneRecord& Zone)
    {
        return Zone.ZoneType == ZoneType;
    });
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FAshfrontierWorldBlockoutRoutesTest,
    "Ashfrontier.World.BlockoutRoutes",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAshfrontierWorldBlockoutRoutesTest::RunTest(const FString& Parameters)
{
    using namespace AshfrontierWorldTest;

    UWorld* World = FindEditorTestWorld();
    if (!TestNotNull(TEXT("Editor world should be available"), World))
    {
        return false;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParams.ObjectFlags |= RF_Transient;

    AAshfrontierWorldBlockoutDirector* Director = World->SpawnActor<AAshfrontierWorldBlockoutDirector>(AAshfrontierWorldBlockoutDirector::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    if (!TestNotNull(TEXT("World blockout director should spawn"), Director))
    {
        return false;
    }

    Director->BuildPrototypeWorld();

    const TArray<FAshfrontierWorldZoneRecord>& Zones = Director->GetZoneRecords();
    const TArray<FAshfrontierRouteRecord>& Routes = Director->GetRouteRecords();

    bool bPassed = true;
    bPassed &= TestEqual(TEXT("Gate 04 should define three world zones"), Zones.Num(), 3);
    bPassed &= TestTrue(TEXT("Main city zone should exist"), HasZoneType(Zones, EAshfrontierWorldZoneType::MainCity));
    bPassed &= TestTrue(TEXT("Outpost zone should exist"), HasZoneType(Zones, EAshfrontierWorldZoneType::Outpost));
    bPassed &= TestTrue(TEXT("Wilderness build zone should exist"), HasZoneType(Zones, EAshfrontierWorldZoneType::WildernessBuildZone));
    bPassed &= TestEqual(TEXT("Gate 04 should define patrol and merchant routes"), Routes.Num(), 2);

    int32 TotalWaypoints = 0;
    bool bHasMerchantRoute = false;
    for (const FAshfrontierRouteRecord& Route : Routes)
    {
        TotalWaypoints += Route.Waypoints.Num();
        bPassed &= TestTrue(FString::Printf(TEXT("%s should have at least three waypoints"), *Route.RouteId.ToString()), Route.Waypoints.Num() >= 3);
        bHasMerchantRoute = bHasMerchantRoute || Route.bMerchantRoute;
    }

    bPassed &= TestTrue(TEXT("Routes should include merchant movement"), bHasMerchantRoute);
    bPassed &= TestTrue(TEXT("Routes should include at least seven total waypoints"), TotalWaypoints >= 7);
    bPassed &= TestEqual(TEXT("Route agents should spawn for both routes"), Director->GetRouteAgentCount(), 2);

    Director->DestroySpawnedBlockout();
    Director->Destroy();

    return bPassed;
}

#endif
