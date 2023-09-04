// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WFCGeneration : ModuleRules
{
	public WFCGeneration(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[] { });
		PrivateIncludePaths.AddRange(new string[] {});
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",	
			}
		);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Utility"
			});
		
		DynamicallyLoadedModuleNames.AddRange( new string[] { });
		
		//PrivatePCHHeaderFile = "Public/WaveFunctionCollapse/pch.h";
	}
}
