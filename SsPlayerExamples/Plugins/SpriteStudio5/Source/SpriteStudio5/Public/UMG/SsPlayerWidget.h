﻿#pragma once

#include "UMG.h"

#include "SsPlayerTickResult.h"
#include "SsPlayer.h"
#include "SsPlayPropertySync.h"

class USsProject;
class FSsRenderOffScreen;

#include "SsPlayerWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSsWidgetEndPlaySignature, FName, AnimPackName, FName, AnimationName, int32, AnimPackIndex, int32, AnimationIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSsWidgetUserDataSignature, FName, PartName, int32, PartIndex, int32, KeyFrame, FSsUserDataValue, UserData);


//
// SpriteStudio5 Player Widget 
// sspjデータを再生/UMG上で描画する 
//
UCLASS(ClassGroup=SpriteStudio)
class SPRITESTUDIO5_API USsPlayerWidget : public UImage, public FTickableGameObject, public FSsPlayPropertySync 
{
	GENERATED_UCLASS_BODY()

public:
	virtual ~USsPlayerWidget();

	// UObject interface
	virtual void Serialize(FArchive& Ar) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// UWidget interface
	virtual void SynchronizeProperties() override;
#if WITH_EDITOR
	//virtual const FText GetPaletteCategory() override { return LOCTEXT("Sprite Studio", "Sprite Studio"); }
#endif

	// FTickableObjectBase interface
	virtual bool IsTickable() const override { return (NULL != SsProject); }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(USsPlayerWidget, STATGROUP_Tickables); }

	// FTickableGameObject interface
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickableInEditor() const override { return true; }

private:
	FSsPlayer Player;
	FSsRenderOffScreen* RenderOffScreen;

#if WITH_EDITOR
	// １フレームに複数回Tick呼び出しされてしまう問題の対処用 
	UWorld* TickWorld;
	float BackWorldTime;
#endif

public:
	// 公開PROPERTY 

	//
	// SpriteStudioAsset
	//

	// 再生するSsProject 
	UPROPERTY(Category=SpriteStudioAsset, EditAnywhere, BlueprintReadOnly, meta=(DisplayThumbnail="true"))
	USsProject* SsProject;


	//
	// SpriteStudioPlayerSettings
	//

	// 自動更新. Offの場合はUpdatePlayer()を呼び出してアニメーションを更新します. 
	UPROPERTY(Category=SpriteStudioPlaySettings, EditAnywhere, BlueprintReadOnly)
	bool bAutoUpdate;

	// 自動再生 
	UPROPERTY(Category=SpriteStudioPlaySettings, EditAnywhere, BlueprintReadOnly)
	bool bAutoPlay;

	// 自動再生時のAnimPack名 
	UPROPERTY(Category=SpriteStudioPlaySettings, EditAnywhere, BlueprintReadOnly)
	FName AutoPlayAnimPackName;

	// 自動再生時のAnimation名 
	UPROPERTY(Category=SpriteStudioPlaySettings, EditAnywhere, BlueprintReadOnly)
	FName AutoPlayAnimationName;

	// 自動再生時のAnimPackIndex 
	UPROPERTY(Category=SpriteStudioPlaySettings, EditAnywhere, BlueprintReadOnly)
	int32 AutoPlayAnimPackIndex;

	// 自動再生時のAnimationIndex 
	UPROPERTY(Category=SpriteStudioPlaySettings, EditAnywhere, BlueprintReadOnly)
	int32 AutoPlayAnimationIndex;

	// 自動再生時の開始フレーム 
	UPROPERTY(Category=SpriteStudioPlaySettings, EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	int32 AutoPlayStartFrame;

	// 自動再生時の再生速度(負の値で逆再生) 
	UPROPERTY(Category=SpriteStudioPlaySettings, EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	float AutoPlayRate;

	// 自動再生時のループ回数(0で無限ループ) 
	UPROPERTY(Category=SpriteStudioPlaySettings, EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	int32 AutoPlayLoopCount;

	// 自動再生時の往復再生 
	UPROPERTY(Category=SpriteStudioPlaySettings, EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	bool bAutoPlayRoundTrip;


	//
	// SpriteStudioRenderSettings 
	//

	// ベースマテリアル 
	UPROPERTY(Category=SpriteStudioRenderSettings, EditAnywhere, BlueprintReadOnly, meta=(DisplayThumbnail="true"))
	UMaterialInterface* BaseMaterial;

	// オフスクリーンレンダリング解像度 
	UPROPERTY(Category=SpriteStudioRenderSettings, EditAnywhere, BlueprintReadOnly)
	FVector2D OffScreenRenderResolution;


	//
	// SpriteStudioCallback 
	//

	// 再生終了イベント
	UPROPERTY(BlueprintAssignable, Category=SpriteStudioCallback)
	FSsWidgetEndPlaySignature OnSsEndPlay;

	// ユーザーデータイベント
	UPROPERTY(BlueprintAssignable, Category=SpriteStudioCallback)
	FSsWidgetUserDataSignature OnSsUserData;

public:
	// BP公開関数 

	// アニメーションの更新 
	// AutoUpdate=false の場合はこの関数を直接呼び出して下さい 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void UpdatePlayer(float DeltaSeconds);

	// アニメーションの再生開始 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable, meta=(AdvancedDisplay="2"))
	bool Play(FName AnimPackName, FName AnimationName, int32 StartFrame=0, float PlayRate=1.f, int32 LoopCount=0, bool bRoundTrip=false);

	// アニメーションの再生開始(インデックス指定) 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable, meta=(AdvancedDisplay="2"))
	bool PlayByIndex(int32 AnimPackIndex, int32 AnimationIndex, int32 StartFrame=0, float PlayRate=1.f, int32 LoopCount=0, bool bRoundTrip=false);

	// 再生中のアニメーション名を取得 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void GetPlayingAnimationName(FName& OutAnimPackName, FName& OutAnimationName) const;

	// 再生中のアニメーションインデックスを取得 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void GetPlayingAnimationIndex(int32& OutAnimPackIndex, int32& OutAnimationIndex) const;

	// アニメーション再生の一時停止 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void Pause();

	// 一時停止したアニメーション再生の再開 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	bool Resume();

	// アニメーションが再生中かを取得 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	bool IsPlaying() const;

	// セットされたSsProjectのAnimPack数を取得 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	int32 GetNumAnimPacks() const;

	// 指定されたAnimPackのAnimation数を取得 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	int32 GetNumAnimations(FName AnimPackName) const;

	// 指定されたAnimPackのAnimation数を取得(インデックス指定) 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	int32 GetNumAnimationsByIndex(int32 AnimPackIndex) const;

	// 指定フレームへジャンプ 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetPlayFrame(float Frame);

	// 現在のフレームを取得 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	float GetPlayFrame() const;

	// ループ数を設定(0で無限ループ) 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetLoopCount(int32 InLoopCount);

	// 残りループ数を取得 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	int32 GetLoopCount() const;

	// 往復再生するかを設定 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetRoundTrip(bool bInRoundTrip);

	// 往復再生中かを取得 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	bool IsRoundTrip() const;

	// 再生速度を設定(負の値で逆再生) 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetPlayRate(float InRate=1.f);

	// 再生速度を取得 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	float GetPlayRate() const;

	// 水平反転の設定 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetFlipH(bool InFlipH);

	// 水平反転の取得 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	bool GetFlipH() const;

	// 垂直反転の設定 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetFlipV(bool InFlipV);

	// 垂直反転の取得 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	bool GetFlipV() const;

	// 置き換えテクスチャの登録 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void AddTextureReplacement(FName PartName, UTexture* Texture);

	// 置き換えテクスチャの登録(インデックス指定) 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void AddTextureReplacementByIndex(int32 PartIndex, UTexture* Texture);

	// 置き換えテクスチャの登録解除 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void RemoveTextureReplacement(FName PartName);

	// 置き換えテクスチャの登録解除(インデックス指定) 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void RemoveTextureReplacementByIndex(int32 PartIndex);

	// 全ての置き換えテクスチャの登録解除 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void RemoveTextureReplacementAll();
};

