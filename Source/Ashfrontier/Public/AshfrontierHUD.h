#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AshfrontierHUD.generated.h"

class AAshfrontierCharacter;
class UAshfrontierInventoryComponent;
class UTexture2D;

UCLASS()
class ASHFRONTIER_API AAshfrontierHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

private:
    void EnsureArtDataLoaded();
    void LoadItemArtAssignments();
    void LoadFactionArtAssignments();
    void LoadCharacterPortraitAssignments();
    UTexture2D* LoadTextureFromGeneratedPath(const FString& TexturePath);
    FString MakeTextureObjectPath(const FString& TexturePath) const;
    void DrawTextureTile(UTexture2D* Texture, float X, float Y, float Size, const FLinearColor& BorderColor);
    void DrawInventoryIconStrip(const UAshfrontierInventoryComponent* Inventory, float X, float Y, float IconSize);
    void DrawFactionLegend(float X, float Y, float IconSize);
    void DrawRecruitPanel(float X, float Y, float Width);
    void DrawTradePanel(float X, float Y, float Width);

    bool bArtDataLoaded = false;

    UPROPERTY(Transient)
    TArray<FName> OrderedItemIds;

    UPROPERTY(Transient)
    TMap<FName, FString> ItemNamesZh;

    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UTexture2D>> ItemIconTextures;

    UPROPERTY(Transient)
    TArray<FName> OrderedFactionIds;

    UPROPERTY(Transient)
    TMap<FName, FString> FactionNamesZh;

    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UTexture2D>> FactionEmblemTextures;

    UPROPERTY(Transient)
    TMap<FString, TObjectPtr<UTexture2D>> PortraitTexturesByCharacterId;

    UPROPERTY(Transient)
    TMap<FString, TObjectPtr<UTexture2D>> TextureCache;
};
