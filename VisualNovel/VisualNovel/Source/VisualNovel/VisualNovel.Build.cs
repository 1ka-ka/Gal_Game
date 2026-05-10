using UnrealBuildTool;

public class VisualNovel : ModuleRules
{
	public VisualNovel(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		bUseUnity = false;

		PublicIncludePaths.AddRange(new string[]
		{
			"VisualNovel",
			"VisualNovel/Core",
			"VisualNovel/Data",
			"VisualNovel/Game",
			"VisualNovel/UI"
		});

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"UMG",
			"Slate",
			"SlateCore",
			"Json",
			"JsonUtilities",
			"ImageWrapper",
			"RenderCore",
			"RHI",
			"DeveloperSettings"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"ApplicationCore"
		});

		DynamicallyLoadedModuleNames.AddRange(new string[] { });
	}
}
