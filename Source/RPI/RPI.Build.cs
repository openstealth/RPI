using UnrealBuildTool;

public class RPI : ModuleRules
{
	public RPI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange
        (
            new string[]
            { 
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore"
            }
        );

		PrivateDependencyModuleNames.AddRange
        (
            new string[]
            { 
                "RenderCore",
                "Renderer",
                "RHI"
            }
        );
    }
}
