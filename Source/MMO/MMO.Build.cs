// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MMO : ModuleRules
{
	public MMO(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "Niagara", "OnlineSubsystemSteam", "OnlineSubsystem", "Slate", "SlateCore", "GeometryCore", "GeometryFramework", "EnhancedInput", "PCG" });

		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTags", "GameplayTasks", "GeometryScriptingCore", "Imath", "AIModule", "PCG" });
    }
}
