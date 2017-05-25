// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class SpriteStudio5Ed : ModuleRules
	{
		public SpriteStudio5Ed(ReadOnlyTargetRules Target) : base(Target)
		{
			DynamicallyLoadedModuleNames.AddRange(
				new string[] {
					"PropertyEditor",
				}
				);

			PublicIncludePaths.AddRange(
				new string[] {
					"SpriteStudio5Ed/Public",
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					"SpriteStudio5Ed/Private",
					"SpriteStudio5Ed/Private/ActorFactory",
					"SpriteStudio5Ed/Private/AssetFactory",
					"SpriteStudio5Ed/Private/Loader",
					"SpriteStudio5Ed/Private/Loader/babel",
					"SpriteStudio5Ed/Private/Loader/tinyxml2",
					"SpriteStudio5Ed/Private/Viewer",
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"InputCore",
					"UnrealEd",
					"AssetTools",
					"Slate",
					"SlateCore",
					"EditorStyle",
					"AppFramework",
					"MessageLog",

					"SpriteStudio5",
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
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