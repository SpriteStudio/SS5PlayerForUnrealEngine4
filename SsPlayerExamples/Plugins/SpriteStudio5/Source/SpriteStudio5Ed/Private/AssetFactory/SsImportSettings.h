#pragma once

#include "SsImportSettings.generated.h"

UCLASS(config=Editor, defaultconfig)
class USsImportSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	//
	//	インポートしたテクスチャのMigGenSettingsをNoMipmapsに上書きします 
	//
	UPROPERTY(EditAnywhere, config, Category=ImportTexture)
	bool bOverwriteMipGenSettings;

	//
	//	インポートしたテクスチャのTextureGroupを上書きします 
	//
	UPROPERTY(EditAnywhere, config, Category=ImportTexture)
	bool bOverwriteTextureGroup;

	//
	//	上書きするTextureGroupを指定します 
	//
	UPROPERTY(EditAnywhere, config, Category=ImportTexture,  meta=(EditCondition=bOverwriteTextureGroup))
	TEnumAsByte<TextureGroup> TextureGroup;

	//
	//	インポートしたテクスチャのCompressionSettingsをUserInterface2Dに上書きします 
	//
	UPROPERTY(EditAnywhere, config, Category=ImportTexture)
	bool bOverwriteCompressionSettings;

	//
	//	インポートしたテクスチャのTilingMethodを、SpriteStudioのWrapMode設定に従って上書きします 
	//
	UPROPERTY(EditAnywhere, config, Category=ImportTexture)
	bool bOverwriteTilingMethodFromSspj;

	//
	//	インポートしたテクスチャのNeverStreamをONに上書きします 
	//
	UPROPERTY(EditAnywhere, config, Category=ImportTexture)
	bool bOverwriteNeverStream;

	//
	//	インポートしたテクスチャのFilterを、SpriteStudioのFilterMode設定に従って上書きします 
	//
	UPROPERTY(EditAnywhere, config, Category=ImportTexture)
	bool bOverwriteFilterFromSspj;
};



