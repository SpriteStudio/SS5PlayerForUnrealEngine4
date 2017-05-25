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
					"SpriteStudio5/Public",
					"SpriteStudio5/Public/Actor",
					"SpriteStudio5/Public/Component",
					"SpriteStudio5/Public/Data",
					"SpriteStudio5/Public/Player",
					"SpriteStudio5/Public/Render",
					"SpriteStudio5/Public/UMG",
					"SpriteStudio5/Public/Misc",
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