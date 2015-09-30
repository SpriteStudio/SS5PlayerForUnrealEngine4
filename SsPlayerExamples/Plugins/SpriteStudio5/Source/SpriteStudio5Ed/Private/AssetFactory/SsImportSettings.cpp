﻿#include "SpriteStudio5EdPrivatePCH.h"
#include "SsImportSettings.h"


USsImportSettings::USsImportSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bOverwriteMipGenSettings(true)
	, bOverwriteTextureGroup(true)
	, TextureGroup(TEXTUREGROUP_Pixels2D)
	, bOverwriteCompressionSettings(true)
	, bOverwriteTilingMethodFromSspj(true)
	, bOverwriteNeverStream(true)
	, bOverwriteFilterFromSspj(true)
{
}
