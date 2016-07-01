﻿#pragma once

#include "SsTypes.h"
#include "SsValue.h"

#include "SsAttribute.generated.h"


//アニメーション中のキーフレームの内容を表現するクラス
USTRUCT()
struct SPRITESTUDIO5_API FSsKeyframe
{
	GENERATED_USTRUCT_BODY()
	void Serialize(FArchive& Ar);

public:
	UPROPERTY(VisibleAnywhere, Category=SsKeyframe, BlueprintReadOnly)
	int32	Time;	///< 時間

	UPROPERTY(VisibleAnywhere, Category=SsKeyframe, BlueprintReadOnly)
	TEnumAsByte<SsInterpolationType::Type>	IpType;	///< 補間タイプ

	UPROPERTY(VisibleAnywhere, Category=SsKeyframe, BlueprintReadOnly)
	FSsCurve	Curve;	///< 曲線補間計算用パラメータ

	UPROPERTY(VisibleAnywhere, Category=SsKeyframe, BlueprintReadOnly)
	FSsValue	Value;	///< 値
public:
	FSsKeyframe()
		: Time(0)
		, IpType(SsInterpolationType::Invalid)
	{}
};


//タグ毎に存在するキーフレームを格納するクラス
USTRUCT()
struct SPRITESTUDIO5_API FSsAttribute	//Tag毎に存在する
{
	GENERATED_USTRUCT_BODY()
	void Serialize(FArchive& Ar);

public:
	//キーフレームデータ : Value（タグによって異なるの組み)
	UPROPERTY(VisibleAnywhere, Category=SsAttribute, BlueprintReadOnly)
	TEnumAsByte<SsAttributeKind::Type> Tag;

	UPROPERTY(VisibleAnywhere, Category=SsAttribute, BlueprintReadOnly)
	TArray<FSsKeyframe> Key;

public:
	bool isEmpty()
	{
		return 0 == Key.Num();
	}

	const FSsKeyframe* FirstKey();

	///時間から左側のキーを取得
	const FSsKeyframe* FindLeftKey(int time);

	//時間から右側のキーを取得する
	const FSsKeyframe* FindRightKey(int time);

private:
	int32 GetLowerBoundKeyIndex(int32 Time);	// std::map::lower_bound()代替
	int32 GetUpperBoundKeyIndex(int32 Time);	// std::map::upper_bound()代替

};

void GetSsColorValue(const FSsKeyframe* key , FSsColorAnime& v);
void GetFSsVertexAnime(const FSsKeyframe* key , FSsVertexAnime& v);
void GetFSsRefCell(const FSsKeyframe* key , FSsRefCell& v);
void GetSsUserDataAnime(const FSsKeyframe* key , FSsUserDataAnime& v);
void GetSsInstparamAnime(const FSsKeyframe* key , FSsInstanceAttr& v);
void GetSsEffectParamAnime(const FSsKeyframe* key , FSsEffectAttr& v);

bool SPRITESTUDIO5_API StringToPoint2(const FString& str , FVector2D& point);
bool SPRITESTUDIO5_API StringToIRect(const FString& str , SsIRect& rect);
