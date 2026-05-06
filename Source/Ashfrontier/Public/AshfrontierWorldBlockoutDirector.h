#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AshfrontierWorldBlockoutDirector.generated.h"

class AAshfrontierRouteAgent;
class UMaterialInterface;
class UStaticMeshComponent;

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
    int32 GetSpawnedActorCount() const;

private:
    void DefineWorldRecords();
    void SpawnStartupLighting();
    void SpawnWorldGeometry();
    void SpawnWorldArtMarkers();
    void SpawnRouteMarkers();
    void SpawnRouteAgents();
    void LoadArtSlotData();
    AActor* SpawnBlock(const FName& ActorName, const FVector& Location, const FVector& Size, const FLinearColor& DebugColor);
    AActor* SpawnDecalMarker(const FName& ActorName, const FVector& Location, const FRotator& Rotation, const FVector& DecalSize, const FLinearColor& DebugColor);
    AActor* SpawnArtPlane(const FName& ActorName, const FVector& Location, const FVector& Target, const FVector2D& Size, const FName& ArtSlot, const FLinearColor& DebugColor);
    FName FindArtSlotForActor(const FName& ActorName) const;
    UMaterialInterface* ResolveMaterialForSlot(const FName& ArtSlot) const;
    UMaterialInterface* ResolvePlaneMaterialForSlot(const FName& ArtSlot) const;
    void ApplyArtSlotMaterial(UStaticMeshComponent* MeshComponent, const FName& ActorName, const FLinearColor& DebugColor) const;

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

    UPROPERTY(Transient)
    TObjectPtr<UStaticMesh> PlaneMesh;

    TMap<FName, FString> ArtSlotMaterialPaths;
    TMap<FName, FName> ActorArtSlots;
};
