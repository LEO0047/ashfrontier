using UnrealBuildTool;
using System.Collections.Generic;

public class AshfrontierEditorTarget : TargetRules
{
    public AshfrontierEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("Ashfrontier");
    }
}
