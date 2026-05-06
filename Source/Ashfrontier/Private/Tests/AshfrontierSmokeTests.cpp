#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FAshfrontierModuleSmokeTest,
    "Ashfrontier.Smoke.ModuleLoads",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAshfrontierModuleSmokeTest::RunTest(const FString& Parameters)
{
    TestTrue(TEXT("Ashfrontier module should be loaded"), FModuleManager::Get().IsModuleLoaded(TEXT("Ashfrontier")));
    return true;
}

#endif
