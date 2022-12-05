// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class GameDatabase : ModuleRules
{
	public GameDatabase(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"Json", 
			"JsonUtilities"
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"NakamaUnreal", 
			"NakamaCore"
		});
	}
}
