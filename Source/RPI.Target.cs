using UnrealBuildTool;
using System.Collections.Generic;

public class RPITarget : TargetRules
{
	public RPITarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.AddRange( new string[] { "RPI" } );
	}
}
