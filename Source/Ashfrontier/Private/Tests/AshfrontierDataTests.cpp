#if WITH_DEV_AUTOMATION_TESTS

#include "Dom/JsonObject.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace AshfrontierDataTest
{
struct FExpectedDataFile
{
    const TCHAR* FileName;
    const TCHAR* TypeName;
    int32 MinimumRecords;
};

FString MakeGeneratedArtAssetPath(const FString& GamePath)
{
    FString PackagePath;
    FString AssetName;
    if (!GamePath.StartsWith(TEXT("/Game/")) || !GamePath.Split(TEXT("/"), &PackagePath, &AssetName, ESearchCase::CaseSensitive, ESearchDir::FromEnd))
    {
        return FString();
    }

    const FString RelativePath = GamePath.RightChop(6);
    return FPaths::Combine(FPaths::ProjectContentDir(), RelativePath + TEXT(".uasset"));
}

bool LoadRecords(const FString& RelativeContentPath, TArray<TSharedPtr<FJsonValue>>& OutRecords)
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FAshfrontierContentDataLoadsTest,
    "Ashfrontier.Data.ContentJsonLoads",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAshfrontierContentDataLoadsTest::RunTest(const FString& Parameters)
{
    using namespace AshfrontierDataTest;

    const FExpectedDataFile ExpectedFiles[] = {
        {TEXT("factions.json"), TEXT("factions"), 3},
        {TEXT("items.json"), TEXT("items"), 12},
        {TEXT("resources.json"), TEXT("resources"), 5},
        {TEXT("recipes.json"), TEXT("recipes"), 2},
        {TEXT("buildings.json"), TEXT("buildings"), 5},
        {TEXT("npc_schedules.json"), TEXT("npc_schedules"), 3},
        {TEXT("legal_rules.json"), TEXT("legal_rules"), 1},
        {TEXT("dialogue_conditions.json"), TEXT("dialogue_conditions"), 1},
    };

    bool bPassed = true;

    for (const FExpectedDataFile& ExpectedFile : ExpectedFiles)
    {
        const FString DataPath = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Data"), ExpectedFile.FileName);
        if (!TestTrue(FString::Printf(TEXT("%s should exist"), ExpectedFile.FileName), FPaths::FileExists(DataPath)))
        {
            bPassed = false;
            continue;
        }

        FString RawJson;
        if (!TestTrue(FString::Printf(TEXT("%s should load from disk"), ExpectedFile.FileName), FFileHelper::LoadFileToString(RawJson, *DataPath)))
        {
            bPassed = false;
            continue;
        }

        TSharedPtr<FJsonObject> RootObject;
        const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(RawJson);
        if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
        {
            AddError(FString::Printf(TEXT("%s should be valid JSON object"), ExpectedFile.FileName));
            bPassed = false;
            continue;
        }

        FString TypeName;
        if (!RootObject->TryGetStringField(TEXT("type"), TypeName))
        {
            AddError(FString::Printf(TEXT("%s should include type"), ExpectedFile.FileName));
            bPassed = false;
            continue;
        }
        if (!TestEqual(FString::Printf(TEXT("%s type should match"), ExpectedFile.FileName), TypeName, FString(ExpectedFile.TypeName)))
        {
            bPassed = false;
        }

        const TArray<TSharedPtr<FJsonValue>>* Records = nullptr;
        if (!RootObject->TryGetArrayField(TEXT("records"), Records) || Records == nullptr)
        {
            AddError(FString::Printf(TEXT("%s should include records array"), ExpectedFile.FileName));
            bPassed = false;
            continue;
        }

        if (!TestTrue(FString::Printf(TEXT("%s should have minimum records"), ExpectedFile.FileName), Records->Num() >= ExpectedFile.MinimumRecords))
        {
            bPassed = false;
        }

        for (int32 Index = 0; Index < Records->Num(); ++Index)
        {
            const TSharedPtr<FJsonObject> RecordObject = (*Records)[Index]->AsObject();
            if (!RecordObject.IsValid())
            {
                AddError(FString::Printf(TEXT("%s records[%d] should be object"), ExpectedFile.FileName, Index));
                bPassed = false;
                continue;
            }

            FString Id;
            FString NameZh;
            if (!RecordObject->TryGetStringField(TEXT("id"), Id) || Id.IsEmpty())
            {
                AddError(FString::Printf(TEXT("%s records[%d] should include id"), ExpectedFile.FileName, Index));
                bPassed = false;
            }
            if (!RecordObject->TryGetStringField(TEXT("name_zh"), NameZh) || NameZh.IsEmpty())
            {
                AddError(FString::Printf(TEXT("%s records[%d] should include name_zh"), ExpectedFile.FileName, Index));
                bPassed = false;
            }
        }
    }

    return bPassed;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FAshfrontierUIArtAssignmentsLoadTest,
    "Ashfrontier.Data.UIArtAssignmentsLoad",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAshfrontierUIArtAssignmentsLoadTest::RunTest(const FString& Parameters)
{
    using namespace AshfrontierDataTest;

    bool bPassed = true;
    TArray<TSharedPtr<FJsonValue>> UIRecords;
    TArray<TSharedPtr<FJsonValue>> FactionRecords;
    TArray<TSharedPtr<FJsonValue>> PortraitRecords;

    bPassed &= TestTrue(TEXT("UIArtAssignments should load"), LoadRecords(TEXT("Data/Art/UIArtAssignments.json"), UIRecords));
    bPassed &= TestTrue(TEXT("FactionArtAssignments should load"), LoadRecords(TEXT("Data/Art/FactionArtAssignments.json"), FactionRecords));
    bPassed &= TestTrue(TEXT("CharacterPortraitAssignments should load"), LoadRecords(TEXT("Data/Art/CharacterPortraitAssignments.json"), PortraitRecords));

    bPassed &= TestEqual(TEXT("All 12 items should have UI icons"), UIRecords.Num(), 12);
    bPassed &= TestEqual(TEXT("All 3 factions should have emblems"), FactionRecords.Num(), 3);
    bPassed &= TestEqual(TEXT("All 4 recruits should have portraits"), PortraitRecords.Num(), 4);

    for (const TSharedPtr<FJsonValue>& Value : UIRecords)
    {
        const TSharedPtr<FJsonObject> RecordObject = Value.IsValid() ? Value->AsObject() : nullptr;
        if (!RecordObject.IsValid())
        {
            AddError(TEXT("UI assignment record should be object"));
            bPassed = false;
            continue;
        }

        FString ItemId;
        FString TexturePath;
        bPassed &= TestTrue(TEXT("UI assignment should include item_id"), RecordObject->TryGetStringField(TEXT("item_id"), ItemId) && !ItemId.IsEmpty());
        bPassed &= TestTrue(TEXT("UI assignment should include icon_texture_path"), RecordObject->TryGetStringField(TEXT("icon_texture_path"), TexturePath) && TexturePath.StartsWith(TEXT("/Game/GeneratedArt/UI/")));
        if (!TexturePath.IsEmpty())
        {
            bPassed &= TestTrue(FString::Printf(TEXT("%s icon texture asset should exist"), *ItemId), FPaths::FileExists(MakeGeneratedArtAssetPath(TexturePath)));
        }
    }

    for (const TSharedPtr<FJsonValue>& Value : FactionRecords)
    {
        const TSharedPtr<FJsonObject> RecordObject = Value.IsValid() ? Value->AsObject() : nullptr;
        if (!RecordObject.IsValid())
        {
            AddError(TEXT("Faction assignment record should be object"));
            bPassed = false;
            continue;
        }

        FString FactionId;
        FString TexturePath;
        bPassed &= TestTrue(TEXT("Faction assignment should include faction_id"), RecordObject->TryGetStringField(TEXT("faction_id"), FactionId) && !FactionId.IsEmpty());
        bPassed &= TestTrue(TEXT("Faction assignment should include emblem_texture_path"), RecordObject->TryGetStringField(TEXT("emblem_texture_path"), TexturePath) && TexturePath.StartsWith(TEXT("/Game/GeneratedArt/Factions/")));
        if (!TexturePath.IsEmpty())
        {
            bPassed &= TestTrue(FString::Printf(TEXT("%s emblem texture asset should exist"), *FactionId), FPaths::FileExists(MakeGeneratedArtAssetPath(TexturePath)));
        }
    }

    for (const TSharedPtr<FJsonValue>& Value : PortraitRecords)
    {
        const TSharedPtr<FJsonObject> RecordObject = Value.IsValid() ? Value->AsObject() : nullptr;
        if (!RecordObject.IsValid())
        {
            AddError(TEXT("Portrait assignment record should be object"));
            bPassed = false;
            continue;
        }

        FString CharacterId;
        FString TexturePath;
        bPassed &= TestTrue(TEXT("Portrait assignment should include character_id"), RecordObject->TryGetStringField(TEXT("character_id"), CharacterId) && !CharacterId.IsEmpty());
        bPassed &= TestTrue(TEXT("Portrait assignment should include portrait_texture_path"), RecordObject->TryGetStringField(TEXT("portrait_texture_path"), TexturePath) && TexturePath.StartsWith(TEXT("/Game/GeneratedArt/Characters/")));
        if (!TexturePath.IsEmpty())
        {
            bPassed &= TestTrue(FString::Printf(TEXT("%s portrait texture asset should exist"), *CharacterId), FPaths::FileExists(MakeGeneratedArtAssetPath(TexturePath)));
        }
    }

    return bPassed;
}

#endif
