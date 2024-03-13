// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BetrayalGame : ModuleRules
{
	public BetrayalGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "AIModule", "NavigationSystem", "UMG", "OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemEOS", "GameplayTags"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate", "SlateCore", "EnhancedInput", "VoiceChat"
		});
		
		// Used for EOS P2P connection
		PrivateDefinitions.Add("P2PMODE=1");

	}
}
