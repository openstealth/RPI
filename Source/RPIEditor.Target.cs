using UnrealBuildTool;
using System.Collections.Generic;
using System;
using System.IO;

public class RPIEditorTarget : TargetRules
{
	public RPIEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.AddRange( new string[] { "RPI" } );
		ExtraModuleNames.AddRange( new string[] { "RPIEditor" } );
	}	
}
