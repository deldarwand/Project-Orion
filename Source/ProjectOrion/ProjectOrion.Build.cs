// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectOrion : ModuleRules
{
	public ProjectOrion(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
        string PhaseSpaceLibrary = System.IO.Path.GetFullPath(System.IO.Path.Combine(ModuleDirectory, "../../Binaries/Win64/libowlsock.lib"));
        PublicAdditionalLibraries.Add(PhaseSpaceLibrary);
    }
}
