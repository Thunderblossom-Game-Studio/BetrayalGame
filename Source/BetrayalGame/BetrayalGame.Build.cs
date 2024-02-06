// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BetrayalGame : ModuleRules
{
	public BetrayalGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "AIModule", "NavigationSystem", "UMG", "OnlineSubsystem", "OnlineSubsystemSteam", "OnlineSubsystemUtils", "Steamworks"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate", "SlateCore", "EnhancedInput"
		});
	}
}
