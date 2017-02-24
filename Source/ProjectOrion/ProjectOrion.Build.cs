// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectOrion : ModuleRules
{
	public ProjectOrion(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
        PublicAdditionalLibraries.Add(@"C:\Users\localadmin\Documents\DanielEldar\Project-Orion\Binaries\Win64\libowlsock.lib");
    }
}
