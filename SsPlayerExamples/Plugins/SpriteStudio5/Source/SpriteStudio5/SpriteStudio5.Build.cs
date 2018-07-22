// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class SpriteStudio5 : ModuleRules
	{
		public SpriteStudio5(ReadOnlyTargetRules Target) : base(Target)
		{
			DynamicallyLoadedModuleNames.AddRange(
				new string[] {
				}
				);

			PublicIncludePaths.AddRange(
				new string[] {
					ModuleDirectory + "/Public",
					ModuleDirectory + "/Public/Actor",
					ModuleDirectory + "/Public/Component",
					ModuleDirectory + "/Public/Data",
					ModuleDirectory + "/Public/Player",
					ModuleDirectory + "/Public/Render",
					ModuleDirectory + "/Public/UMG",
					ModuleDirectory + "/Public/Misc",
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"SpriteStudio5/Private",
					"SpriteStudio5/Private/Actor",
					"SpriteStudio5/Private/Component",
					"SpriteStudio5/Private/Data",
					"SpriteStudio5/Private/Player",
					"SpriteStudio5/Private/Render",
					"SpriteStudio5/Private/UMG",
					"SpriteStudio5/Private/Misc",
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"InputCore",
					"RHI",
					"RenderCore",
					"ShaderCore",
					"UtilityShaders",
					"SlateCore",
					"SlateRHIRenderer",
					"Slate",
					"UMG",
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Renderer",
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
				}
				);
		}
	}
}