#include "AshfrontierPlacedBuilding.h"

#include "AshfrontierInventoryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Dom/JsonObject.h"
#include "Materials/MaterialInterface.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/ConstructorHelpers.h"

namespace AshfrontierBuildingVisuals
{
FString MakeObjectPath(const FString& AssetPath)
{
    if (AssetPath.Contains(TEXT(".")))
    {
        return AssetPath;
    }

    FString PackagePath;
    FString AssetName;
    if (AssetPath.Split(TEXT("/"), &PackagePath, &AssetName, ESearchCase::CaseSensitive, ESearchDir::FromEnd))
    {
        return FString::Printf(TEXT("%s.%s"), *AssetPath, *AssetName);
    }
    return AssetPath;
}

FString FindMaterialForBuilding(const FName& BuildingId)
{
    const FString DataPath = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Data/Art/BuildingVisualVariants.json"));
    FString RawJson;
    if (!FFileHelper::LoadFileToString(RawJson, *DataPath))
    {
        return FString();
    }

    TSharedPtr<FJsonObject> RootObject;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(RawJson);
    if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
    {
        return FString();
    }

    const TArray<TSharedPtr<FJsonValue>>* Records = nullptr;
    if (!RootObject->TryGetArrayField(TEXT("records"), Records) || Records == nullptr)
    {
        return FString();
    }

    for (const TSharedPtr<FJsonValue>& Value : *Records)
    {
        const TSharedPtr<FJsonObject> Record = Value.IsValid() ? Value->AsObject() : nullptr;
        if (!Record.IsValid())
        {
            continue;
        }

        FString RecordBuildingId;
        FString MaterialPath;
        if (Record->TryGetStringField(TEXT("building_id"), RecordBuildingId)
            && RecordBuildingId == BuildingId.ToString()
            && Record->TryGetStringField(TEXT("body_material_path"), MaterialPath))
        {
            return MaterialPath;
        }
    }

    return FString();
}
}

AAshfrontierPlacedBuilding::AAshfrontierPlacedBuilding()
{
    PrimaryActorTick.bCanEverTick = false;

    PlaceholderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderMesh"));
    SetRootComponent(PlaceholderMesh);
    PlaceholderMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    PlaceholderMesh->SetCollisionResponseToAllChannels(ECR_Block);
    PlaceholderMesh->SetRelativeScale3D(FVector(2.0f, 2.0f, 0.7f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        PlaceholderMesh->SetStaticMesh(CubeMesh.Object);
    }

    StorageInventory = CreateDefaultSubobject<UAshfrontierInventoryComponent>(TEXT("StorageInventory"));
    BuildingNameZh = TEXT("未命名建築");
}

void AAshfrontierPlacedBuilding::InitializeBuilding(const FAshfrontierBuildingDefinition& Definition)
{
    BuildingId = Definition.BuildingId;
    BuildingNameZh = Definition.NameZh;
    Category = Definition.Category;
    ResourceId = Definition.ResourceId;

    if (Category == TEXT("defense"))
    {
        SetActorScale3D(FVector(2.8f, 0.45f, 1.3f));
    }
    else if (Category == TEXT("storage"))
    {
        SetActorScale3D(FVector(1.3f, 1.1f, 0.8f));
    }
    else if (Category == TEXT("gathering"))
    {
        SetActorScale3D(FVector(1.5f, 1.5f, 1.1f));
    }
    else
    {
        SetActorScale3D(FVector(1.6f, 1.4f, 0.9f));
    }

    SetPrototypeMaterialPath(AshfrontierBuildingVisuals::FindMaterialForBuilding(BuildingId));
}

FName AAshfrontierPlacedBuilding::GetBuildingId() const
{
    return BuildingId;
}

const FString& AAshfrontierPlacedBuilding::GetBuildingNameZh() const
{
    return BuildingNameZh;
}

FName AAshfrontierPlacedBuilding::GetCategory() const
{
    return Category;
}

FName AAshfrontierPlacedBuilding::GetResourceId() const
{
    return ResourceId;
}

void AAshfrontierPlacedBuilding::SetPrototypeMaterialPath(const FString& NewMaterialPath)
{
    PrototypeMaterialPath = NewMaterialPath;
    if (!PlaceholderMesh || PrototypeMaterialPath.IsEmpty())
    {
        return;
    }

    if (UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, *AshfrontierBuildingVisuals::MakeObjectPath(PrototypeMaterialPath)))
    {
        PlaceholderMesh->SetMaterial(0, Material);
    }
}

const FString& AAshfrontierPlacedBuilding::GetPrototypeMaterialPath() const
{
    return PrototypeMaterialPath;
}

UAshfrontierInventoryComponent* AAshfrontierPlacedBuilding::GetStorageInventory() const
{
    return StorageInventory;
}

void AAshfrontierPlacedBuilding::AddProductionJob(const FName& RecipeId, float WorkSeconds)
{
    if (RecipeId.IsNone() || WorkSeconds <= 0.0f)
    {
        return;
    }

    FAshfrontierProductionQueueEntry Entry;
    Entry.RecipeId = RecipeId;
    Entry.RemainingSeconds = WorkSeconds;
    ProductionQueue.Add(Entry);
}

bool AAshfrontierPlacedBuilding::AdvanceActiveProduction(float DeltaSeconds, FName& OutCompletedRecipeId)
{
    OutCompletedRecipeId = NAME_None;
    if (ProductionQueue.Num() == 0 || DeltaSeconds <= 0.0f)
    {
        return false;
    }

    ProductionQueue[0].RemainingSeconds -= DeltaSeconds;
    if (ProductionQueue[0].RemainingSeconds > 0.0f)
    {
        return false;
    }

    OutCompletedRecipeId = ProductionQueue[0].RecipeId;
    ProductionQueue.RemoveAt(0);
    return true;
}

int32 AAshfrontierPlacedBuilding::GetProductionQueueCount() const
{
    return ProductionQueue.Num();
}

float AAshfrontierPlacedBuilding::GetCurrentProductionRemainingSeconds() const
{
    return ProductionQueue.Num() > 0 ? ProductionQueue[0].RemainingSeconds : 0.0f;
}

const TArray<FAshfrontierProductionQueueEntry>& AAshfrontierPlacedBuilding::GetProductionQueue() const
{
    return ProductionQueue;
}

void AAshfrontierPlacedBuilding::SetProductionQueueForSave(const TArray<FAshfrontierProductionQueueEntry>& NewQueue)
{
    ProductionQueue.Reset();
    for (const FAshfrontierProductionQueueEntry& Entry : NewQueue)
    {
        if (!Entry.RecipeId.IsNone() && Entry.RemainingSeconds > 0.0f)
        {
            ProductionQueue.Add(Entry);
        }
    }
}
