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

#endif
