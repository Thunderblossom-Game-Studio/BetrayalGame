using UnrealBuildTool;

public class BetrayalGameExtensionsEditor : ModuleRules
{
	public BetrayalGameExtensionsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		// PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		// PrivatePCHHeaderFile = "Public/BetrayalGameExtensionsEditor.h";

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore",
				"Slate",
				"UnrealEd",
				"BetrayalGame"
			});

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"InputCore", 
				"SlateCore",
				"PropertyEditor",
				"LevelEditor",
			});
	}
}
