﻿#pragma once

#include "SsTypes.h"

#include "SsCellMap.generated.h"


///パーツに使用される画素の矩形範囲を示した構造です。
USTRUCT()
struct SPRITESTUDIO5_API FSsCell 
{
	GENERATED_USTRUCT_BODY()

public:
	//--------- ランタイム用データとして保存すべきもの
	UPROPERTY(VisibleAnywhere, Category=FSsCell)
	FName		CellName;		///< セル名称

	UPROPERTY(VisibleAnywhere, Category=FSsCell)
	FVector2D	Pos;			///< 左上の座標

	UPROPERTY(VisibleAnywhere, Category=FSsCell)
	FVector2D	Size;			///< WHピクセルサイズ

	UPROPERTY(VisibleAnywhere, Category=FSsCell)
	FVector2D	Pivot;			///< 原点。size /2 が中央=0,0になる。

	UPROPERTY(VisibleAnywhere, Category=FSsCell)
	bool		Rotated;		///< 左方向に９０度回転されている。uvs の割り当てが変わる。
};



//!セルマップデータを表現するためのクラスです。
/*!
@class SsCellMap
@breif セルマップは１つのテクスチャファイルとそのテクスチャ内でパーツとして使用する矩形範囲を示したセルをコンテナとして保持するための構造です。<BR>
<BR>
このデータコンテナはエディット用として下記を読み飛ばします。<BR>
imagePathAtImport;///< インポート時の参照元画像のフルパス<BR>
packInfoFilePath;	///< パック情報ファイル。TexturePacker 等のデータをインポートした場合のみ有効<BR>
texPackSettings;	///< パック時の参照情報<BR>
*/
USTRUCT()
struct SPRITESTUDIO5_API FSsCellMap
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category=SsCellMap)
	FString		Version;

	UPROPERTY(VisibleAnywhere, Category=SsCellMap)
	FName		FileName;			///< セルマップのファイルネーム

	UPROPERTY(VisibleAnywhere, Category=SsCellMap)
	FName		CellMapName;		///< このセルマップの名称です。

	UPROPERTY(VisibleAnywhere, Category=SsCellMap)
	FName		CellMapNameEx;		///< 拡張子付きセルマップ名

	UPROPERTY(VisibleAnywhere, Category=SsCellMap)
	FString		ImagePath;			///< 参照画像ファイルパス。プロジェクトの画像基準相対

	UPROPERTY(VisibleAnywhere, Category=SsCellMap)
	UTexture*	Texture;

	UPROPERTY(VisibleAnywhere, Category=SsCellMap)
	FVector2D	PixelSize;			///< 画像のピクセルWHサイズ

	UPROPERTY(VisibleAnywhere, Category=SsCellMap)
	bool		OverrideTexSettings;///< テクスチャ設定をプロジェクトの設定ではなく下記設定を使う

	UPROPERTY(VisibleAnywhere, Category=SsCellMap)
	TEnumAsByte<SsTexWrapMode::Type>	WrapMode;			///< テクスチャのラップモード

	UPROPERTY(VisibleAnywhere, Category=SsCellMap)
	TEnumAsByte<SsTexFilterMode::Type>	FilterMode;			///< テクスチャのフィルタモード

	UPROPERTY(VisibleAnywhere, Category=SsCellMap)
	TArray<FSsCell>		Cells;
};


