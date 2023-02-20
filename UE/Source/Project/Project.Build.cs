// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Project : ModuleRules
{
	public Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.NoSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore", 
				"ProceduralMeshComponent"
			});
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"GameDatabase", 
				"NakamaUnreal", 
				"NakamaCore"
			});
		
		// Utility
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Utility"
			});

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"UMG", 
				"Slate", 
				"SlateCore", 
				"Niagara", 
				"ProceduralMeshComponent"
			});

		PrivatePCHHeaderFile = "Project.h";
		
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		UnsafeTypeCastWarningLevel = WarningLevel.Warning;
		ShadowVariableWarningLevel = WarningLevel.Warning; 

	}
}
