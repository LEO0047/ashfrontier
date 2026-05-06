#include "AshfrontierHUD.h"

#include "AshfrontierCameraControllerComponent.h"
#include "AshfrontierCharacter.h"
#include "AshfrontierDamageModelComponent.h"
#include "AshfrontierInventoryComponent.h"
#include "AshfrontierCrimeEventMemoryComponent.h"
#include "AshfrontierGuardAIComponent.h"
#include "AshfrontierPlacedBuilding.h"
#include "AshfrontierPlayerController.h"
#include "AshfrontierSquadManagerComponent.h"
#include "Dom/JsonObject.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "EngineUtils.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
bool LoadArtRecords(const FString& RelativeContentPath, TArray<TSharedPtr<FJsonValue>>& OutRecords)
{
    const FString DataPath = FPaths::Combine(FPaths::ProjectContentDir(), RelativeContentPath);
    FString RawJson;
    if (!FFileHelper::LoadFileToString(RawJson, *DataPath))
    {
        return false;
    }

    TSharedPtr<FJsonObject> RootObject;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(RawJson);
    if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
    {
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* Records = nullptr;
    if (!RootObject->TryGetArrayField(TEXT("records"), Records) || Records == nullptr)
    {
        return false;
    }

    OutRecords = *Records;
    return true;
}
}

void AAshfrontierHUD::DrawHUD()
{
    Super::DrawHUD();

    EnsureArtDataLoaded();

    const float PanelWidth = Canvas ? FMath::Min(Canvas->SizeX - 36.0f, 1040.0f) : 1040.0f;
    DrawRect(FLinearColor(0.02f, 0.025f, 0.03f, 0.82f), 18.0f, 18.0f, PanelWidth, 226.0f);
    DrawText(TEXT("Ashfrontier Gate 15 生成美術 UI 套用中"), FLinearColor(0.98f, 0.86f, 0.28f, 1.0f), 32.0f, 30.0f, nullptr, 1.45f);
    DrawText(TEXT("滑鼠左鍵選取｜右鍵移動 / 攻擊｜Tab 切換鏡頭｜R 招募｜T 交易｜G 採集｜B 建造｜P 生產｜F5 存檔｜F9 讀檔"), FLinearColor::White, 32.0f, 72.0f, nullptr, 0.95f);
    DrawText(TEXT("庫存 UI：生成物品 icon 已接到資料表；交易 UI 與招募 UI 會在右側面板顯示可用美術。"), FLinearColor(0.6f, 0.95f, 0.95f, 1.0f), 32.0f, 104.0f, nullptr, 0.9f);

    const AAshfrontierPlayerController* AshController = Cast<AAshfrontierPlayerController>(PlayerOwner);
    if (!AshController)
    {
        return;
    }

    const UAshfrontierSquadManagerComponent* SquadManager = AshController->GetSquadManager();
    const UAshfrontierCameraControllerComponent* CameraController = AshController->GetCameraController();
    if (!SquadManager)
    {
        return;
    }

    const FString CameraLabel = CameraController && CameraController->IsTacticalCamera() ? TEXT("戰術視角") : TEXT("第三人稱視角");
    DrawText(FString::Printf(TEXT("小隊：%d / 選取：%d / 鏡頭：%s"), SquadManager->GetSquadCount(), SquadManager->GetSelectedCount(), *CameraLabel), FLinearColor::White, 32.0f, 142.0f, nullptr, 1.05f);
    DrawFactionLegend(PanelWidth - 210.0f, 30.0f, 42.0f);

    const AAshfrontierCharacter* Leader = SquadManager->GetLeader();
    DrawInventoryIconStrip(Leader ? Leader->GetInventory() : nullptr, 32.0f, 174.0f, 30.0f);

    const TArray<AAshfrontierCharacter*> Members = SquadManager->GetSquadMembers();
    float Y = 258.0f;
    for (const AAshfrontierCharacter* Member : Members)
    {
        if (!Member)
        {
            continue;
        }

        const FLinearColor Color = Member->IsSelected() ? FLinearColor(0.2f, 0.9f, 0.6f, 1.0f) : FLinearColor::White;
        const FString Marker = Member->IsSelected() ? TEXT("[X]") : TEXT("[ ]");
        const UAshfrontierDamageModelComponent* DamageModel = Member->GetDamageModel();
        const FString HealthLabel = DamageModel ? DamageModel->GetStateLabelZh() : TEXT("未知");
        const UAshfrontierInventoryComponent* Inventory = Member->GetInventory();
        const int32 Credits = Inventory ? Inventory->GetItemCount(TEXT("item_ash_credit")) : 0;
        const int32 Planks = Inventory ? Inventory->GetItemCount(TEXT("item_salvaged_plank")) : 0;
        const int32 Ore = Inventory ? Inventory->GetItemCount(TEXT("item_scrap_ore")) : 0;
        const int32 Grain = Inventory ? Inventory->GetItemCount(TEXT("item_raw_grain")) : 0;
        const int32 Water = Inventory ? Inventory->GetItemCount(TEXT("item_water_flask")) : 0;
        const int32 Rations = Inventory ? Inventory->GetItemCount(TEXT("item_cooked_ration")) : 0;
        DrawTextureTile(FactionEmblemTextures.FindRef(Member->GetFactionId()), 32.0f, Y - 2.0f, 22.0f, Color);
        DrawText(FString::Printf(TEXT("%s %d  %s  %s  %s  灰印幣:%d 木板:%d 碎鐵:%d 穀:%d 水:%d 糧:%d"), *Marker, Member->GetSquadIndex() + 1, *Member->GetSquadDisplayName(), *Member->GetOrderLabelZh(), *HealthLabel, Credits, Planks, Ore, Grain, Water, Rations), Color, 64.0f, Y);
        Y += 24.0f;
    }

    const AAshfrontierPlacedBuilding* LastBuilding = AshController->GetLastPlacedBuilding();
    if (LastBuilding)
    {
        const FString QueueLabel = LastBuilding->GetProductionQueueCount() > 0
            ? FString::Printf(TEXT("生產中 %.1f 秒"), LastBuilding->GetCurrentProductionRemainingSeconds())
            : TEXT("無生產隊列");
        DrawText(FString::Printf(TEXT("最近建築：%s / %s"), *LastBuilding->GetBuildingNameZh(), *QueueLabel), FLinearColor(0.8f, 0.95f, 0.6f, 1.0f), 32.0f, Y + 8.0f);
        Y += 32.0f;
    }

    const UAshfrontierGuardAIComponent* GuardAI = AshController->GetGuardAI();
    const UAshfrontierCrimeEventMemoryComponent* CrimeMemory = AshController->GetCrimeEventMemory();
    if (GuardAI && CrimeMemory)
    {
        DrawText(FString::Printf(TEXT("城市反應：%s / 事件記憶:%d"), *GuardAI->GetStateLabelZh(), CrimeMemory->GetMemoryCount()), FLinearColor(0.95f, 0.75f, 0.45f, 1.0f), 32.0f, Y + 8.0f);
    }

    if (Canvas && Canvas->SizeX >= 1180)
    {
        DrawRecruitPanel(Canvas->SizeX - 392.0f, 18.0f, 374.0f);
        DrawTradePanel(Canvas->SizeX - 392.0f, 366.0f, 374.0f);
    }
}

void AAshfrontierHUD::EnsureArtDataLoaded()
{
    if (bArtDataLoaded)
    {
        return;
    }

    LoadItemArtAssignments();
    LoadFactionArtAssignments();
    LoadCharacterPortraitAssignments();
    bArtDataLoaded = true;
}

void AAshfrontierHUD::LoadItemArtAssignments()
{
    TArray<TSharedPtr<FJsonValue>> Records;
    if (!LoadArtRecords(TEXT("Data/Art/UIArtAssignments.json"), Records))
    {
        return;
    }

    for (const TSharedPtr<FJsonValue>& Value : Records)
    {
        const TSharedPtr<FJsonObject> Record = Value.IsValid() ? Value->AsObject() : nullptr;
        if (!Record.IsValid())
        {
            continue;
        }

        FString ItemId;
        FString NameZh;
        FString TexturePath;
        if (!Record->TryGetStringField(TEXT("item_id"), ItemId) || !Record->TryGetStringField(TEXT("icon_texture_path"), TexturePath))
        {
            continue;
        }
        Record->TryGetStringField(TEXT("name_zh"), NameZh);

        const FName ItemKey(*ItemId);
        OrderedItemIds.Add(ItemKey);
        ItemNamesZh.Add(ItemKey, NameZh);
        ItemIconTextures.Add(ItemKey, LoadTextureFromGeneratedPath(TexturePath));
    }
}

void AAshfrontierHUD::LoadFactionArtAssignments()
{
    TArray<TSharedPtr<FJsonValue>> Records;
    if (!LoadArtRecords(TEXT("Data/Art/FactionArtAssignments.json"), Records))
    {
        return;
    }

    for (const TSharedPtr<FJsonValue>& Value : Records)
    {
        const TSharedPtr<FJsonObject> Record = Value.IsValid() ? Value->AsObject() : nullptr;
        if (!Record.IsValid())
        {
            continue;
        }

        FString FactionId;
        FString NameZh;
        FString TexturePath;
        if (!Record->TryGetStringField(TEXT("faction_id"), FactionId) || !Record->TryGetStringField(TEXT("emblem_texture_path"), TexturePath))
        {
            continue;
        }
        Record->TryGetStringField(TEXT("name_zh"), NameZh);

        const FName FactionKey(*FactionId);
        OrderedFactionIds.Add(FactionKey);
        FactionNamesZh.Add(FactionKey, NameZh);
        FactionEmblemTextures.Add(FactionKey, LoadTextureFromGeneratedPath(TexturePath));
    }
}

void AAshfrontierHUD::LoadCharacterPortraitAssignments()
{
    TArray<TSharedPtr<FJsonValue>> Records;
    if (!LoadArtRecords(TEXT("Data/Art/CharacterPortraitAssignments.json"), Records))
    {
        return;
    }

    for (const TSharedPtr<FJsonValue>& Value : Records)
    {
        const TSharedPtr<FJsonObject> Record = Value.IsValid() ? Value->AsObject() : nullptr;
        if (!Record.IsValid())
        {
            continue;
        }

        FString CharacterId;
        FString TexturePath;
        if (Record->TryGetStringField(TEXT("character_id"), CharacterId) && Record->TryGetStringField(TEXT("portrait_texture_path"), TexturePath))
        {
            PortraitTexturesByCharacterId.Add(CharacterId, LoadTextureFromGeneratedPath(TexturePath));
        }
    }
}

UTexture2D* AAshfrontierHUD::LoadTextureFromGeneratedPath(const FString& TexturePath)
{
    if (TexturePath.IsEmpty())
    {
        return nullptr;
    }

    if (TObjectPtr<UTexture2D>* CachedTexture = TextureCache.Find(TexturePath))
    {
        return CachedTexture->Get();
    }

    UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *MakeTextureObjectPath(TexturePath));
    TextureCache.Add(TexturePath, Texture);
    return Texture;
}

FString AAshfrontierHUD::MakeTextureObjectPath(const FString& TexturePath) const
{
    if (TexturePath.Contains(TEXT(".")))
    {
        return TexturePath;
    }

    FString PackagePath;
    FString AssetName;
    if (TexturePath.Split(TEXT("/"), &PackagePath, &AssetName, ESearchCase::CaseSensitive, ESearchDir::FromEnd))
    {
        return FString::Printf(TEXT("%s.%s"), *TexturePath, *AssetName);
    }
    return TexturePath;
}

void AAshfrontierHUD::DrawTextureTile(UTexture2D* Texture, float X, float Y, float Size, const FLinearColor& BorderColor)
{
    DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.55f), X, Y, Size, Size);
    DrawRect(BorderColor.CopyWithNewOpacity(0.55f), X, Y, Size, 1.0f);
    DrawRect(BorderColor.CopyWithNewOpacity(0.55f), X, Y + Size - 1.0f, Size, 1.0f);
    DrawRect(BorderColor.CopyWithNewOpacity(0.55f), X, Y, 1.0f, Size);
    DrawRect(BorderColor.CopyWithNewOpacity(0.55f), X + Size - 1.0f, Y, 1.0f, Size);

    if (Texture)
    {
        DrawTexture(Texture, X + 2.0f, Y + 2.0f, Size - 4.0f, Size - 4.0f, 0.0f, 0.0f, 1.0f, 1.0f, FLinearColor::White, BLEND_Translucent);
    }
    else
    {
        DrawText(TEXT("?"), FLinearColor(1.0f, 0.25f, 0.2f, 1.0f), X + Size * 0.34f, Y + Size * 0.18f, nullptr, 0.9f);
    }
}

void AAshfrontierHUD::DrawInventoryIconStrip(const UAshfrontierInventoryComponent* Inventory, float X, float Y, float IconSize)
{
    float CursorX = X;
    for (const FName& ItemId : OrderedItemIds)
    {
        const int32 Count = Inventory ? Inventory->GetItemCount(ItemId) : 0;
        DrawTextureTile(ItemIconTextures.FindRef(ItemId), CursorX, Y, IconSize, Count > 0 ? FLinearColor(0.98f, 0.86f, 0.28f, 1.0f) : FLinearColor(0.42f, 0.42f, 0.38f, 1.0f));
        DrawText(FString::FromInt(Count), Count > 0 ? FLinearColor::White : FLinearColor(0.62f, 0.62f, 0.58f, 1.0f), CursorX + IconSize - 12.0f, Y + IconSize - 13.0f, nullptr, 0.62f);
        CursorX += IconSize + 8.0f;
    }
}

void AAshfrontierHUD::DrawFactionLegend(float X, float Y, float IconSize)
{
    float CursorY = Y;
    for (const FName& FactionId : OrderedFactionIds)
    {
        DrawTextureTile(FactionEmblemTextures.FindRef(FactionId), X, CursorY, IconSize, FLinearColor(0.85f, 0.78f, 0.62f, 1.0f));
        const FString Name = FactionNamesZh.FindRef(FactionId);
        DrawText(Name, FLinearColor(0.86f, 0.84f, 0.74f, 1.0f), X + IconSize + 8.0f, CursorY + 10.0f, nullptr, 0.72f);
        CursorY += IconSize + 8.0f;
    }
}

void AAshfrontierHUD::DrawRecruitPanel(float X, float Y, float Width)
{
    DrawRect(FLinearColor(0.018f, 0.02f, 0.022f, 0.84f), X, Y, Width, 324.0f);
    DrawText(TEXT("招募 UI"), FLinearColor(0.98f, 0.86f, 0.28f, 1.0f), X + 14.0f, Y + 12.0f, nullptr, 1.0f);

    float RowY = Y + 46.0f;
    int32 Drawn = 0;
    for (TActorIterator<AAshfrontierCharacter> It(GetWorld()); It && Drawn < 4; ++It)
    {
        const AAshfrontierCharacter* Candidate = *It;
        if (!Candidate || !Candidate->IsRecruitable())
        {
            continue;
        }

        UTexture2D* PortraitTexture = PortraitTexturesByCharacterId.FindRef(Candidate->GetArtCharacterId());
        if (!PortraitTexture)
        {
            PortraitTexture = LoadTextureFromGeneratedPath(Candidate->GetPortraitTexturePath());
        }
        DrawTextureTile(PortraitTexture, X + 14.0f, RowY, 54.0f, FLinearColor(0.76f, 0.66f, 0.52f, 1.0f));
        DrawTextureTile(FactionEmblemTextures.FindRef(Candidate->GetFactionId()), X + 78.0f, RowY + 4.0f, 22.0f, FLinearColor(0.72f, 0.72f, 0.64f, 1.0f));
        DrawText(Candidate->GetSquadDisplayName(), FLinearColor::White, X + 108.0f, RowY + 4.0f, nullptr, 0.84f);
        DrawText(FString::Printf(TEXT("招募費：%d 灰印幣"), Candidate->GetRecruitCost()), FLinearColor(0.85f, 0.82f, 0.66f, 1.0f), X + 108.0f, RowY + 28.0f, nullptr, 0.72f);
        RowY += 66.0f;
        ++Drawn;
    }

    if (Drawn == 0)
    {
        DrawText(TEXT("目前沒有可招募角色"), FLinearColor(0.72f, 0.72f, 0.68f, 1.0f), X + 14.0f, RowY, nullptr, 0.82f);
    }
}

void AAshfrontierHUD::DrawTradePanel(float X, float Y, float Width)
{
    DrawRect(FLinearColor(0.018f, 0.02f, 0.022f, 0.84f), X, Y, Width, 140.0f);
    DrawText(TEXT("交易 UI"), FLinearColor(0.98f, 0.86f, 0.28f, 1.0f), X + 14.0f, Y + 12.0f, nullptr, 1.0f);

    const AAshfrontierCharacter* Merchant = nullptr;
    for (TActorIterator<AAshfrontierCharacter> It(GetWorld()); It; ++It)
    {
        if (It->IsShopkeeper())
        {
            Merchant = *It;
            break;
        }
    }

    if (!Merchant)
    {
        DrawText(TEXT("尚未找到商人"), FLinearColor(0.72f, 0.72f, 0.68f, 1.0f), X + 14.0f, Y + 50.0f, nullptr, 0.82f);
        return;
    }

    DrawTextureTile(FactionEmblemTextures.FindRef(Merchant->GetFactionId()), X + 14.0f, Y + 48.0f, 38.0f, FLinearColor(0.72f, 0.72f, 0.64f, 1.0f));
    DrawText(Merchant->GetSquadDisplayName(), FLinearColor::White, X + 62.0f, Y + 50.0f, nullptr, 0.78f);

    if (const UAshfrontierInventoryComponent* Inventory = Merchant->GetInventory())
    {
        float CursorX = X + 14.0f;
        const float IconY = Y + 88.0f;
        for (const FName& ItemId : OrderedItemIds)
        {
            const int32 Count = Inventory->GetItemCount(ItemId);
            if (Count <= 0)
            {
                continue;
            }
            DrawTextureTile(ItemIconTextures.FindRef(ItemId), CursorX, IconY, 28.0f, FLinearColor(0.78f, 0.7f, 0.45f, 1.0f));
            DrawText(FString::FromInt(Count), FLinearColor::White, CursorX + 18.0f, IconY + 15.0f, nullptr, 0.58f);
            CursorX += 36.0f;
        }
    }
}
