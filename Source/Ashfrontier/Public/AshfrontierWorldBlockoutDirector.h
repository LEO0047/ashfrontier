#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AshfrontierWorldBlockoutDirector.generated.h"

class AAshfrontierRouteAgent;

UENUM(BlueprintType)
enum class EAshfrontierWorldZoneType : uint8
{
    MainCity UMETA(DisplayName = "Main City"),
    Outpost UMETA(DisplayName = "Outpost"),
    WildernessBuildZone UMETA(DisplayName = "Wilderness Build Zone")
};

USTRUCT(BlueprintType)
struct ASHFRONTIER_API FAshfrontierWorldZoneRecord
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName ZoneId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString NameZh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EAshfrontierWorldZoneType ZoneType = EAshfrontierWorldZoneType::MainCity;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FVector Center = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FVector Size = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct ASHFRONTIER_API FAshfrontierRouteRecord
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FName RouteId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString NameZh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<FVector> Waypoints;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bMerchantRoute = false;
};

UCLASS()
class ASHFRONTIER_API AAshfrontierWorldBlockoutDirector : public AActor
{
    GENERATED_BODY()

public:
    AAshfrontierWorldBlockoutDirector();

    void BuildPrototypeWorld();
    void DestroySpawnedBlockout();

    const TArray<FAshfrontierWorldZoneRecord>& GetZoneRecords() const;
    const TArray<FAshfrontierRouteRecord>& GetRouteRecords() const;
    int32 GetRouteAgentCount() const;

private:
    void DefineWorldRecords();
    void SpawnWorldGeometry();
    void SpawnRouteMarkers();
    void SpawnRouteAgents();
    AActor* SpawnBlock(const FName& ActorName, const FVector& Location, const FVector& Size, const FLinearColor& DebugColor);

    UPROPERTY(VisibleAnywhere, Category = "World")
    TArray<FAshfrontierWorldZoneRecord> ZoneRecords;

    UPROPERTY(VisibleAnywhere, Category = "World")
    TArray<FAshfrontierRouteRecord> RouteRecords;

    UPROPERTY(Transient)
    TArray<TObjectPtr<AActor>> SpawnedActors;

    UPROPERTY(Transient)
    TArray<TObjectPtr<AAshfrontierRouteAgent>> RouteAgents;

    UPROPERTY(Transient)
    TObjectPtr<UStaticMesh> CubeMesh;
};
