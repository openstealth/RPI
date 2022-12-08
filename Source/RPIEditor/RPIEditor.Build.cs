using UnrealBuildTool;

public class RPIEditor : ModuleRules
{
	public RPIEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange
        (
            new string[]
            {
                "RPI",
                "Core",
                "CoreUObject",
                "Engine",
                "EngineSettings",
                "UnrealEd",
                "RHI"
            }
        );
    }
}

