// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Project : ModuleRules
{
	public Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.NoSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent"});

		PrivateDependencyModuleNames.AddRange(new string[] { "UMG", "Slate", "SlateCore", "Niagara", "ProceduralMeshComponent"});

		PrivatePCHHeaderFile = "Project.h";
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
