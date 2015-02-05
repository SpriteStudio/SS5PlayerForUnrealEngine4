#pragma once

#include "Engine/CanvasRenderTarget2D.h"
#include "SsPlayerTickResult.h"

class USsProject;
class FSsPlayer;
class FSsRenderSceneProxy;

#include "SsPlayerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSsEndPlaySignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSsUserDataSignature, FName, PartName, int32, PartIndex, int32, KeyFrame, FSsUserDataValue, UserData);


USTRUCT()
struct FAttachComponent
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(Category=AttachComponent, BlueprintReadOnly)
	int32 PartIndex;

	UPROPERTY(Category=AttachComponent, BlueprintReadOnly)
	TWeakObjectPtr<USceneComponent> Component;
};


UCLASS(meta=(BlueprintSpawnableComponent))
class USsPlayerComponent : public UMeshComponent
{
	GENERATED_UCLASS_BODY()

public:
	// UActorComponent interface
	virtual void OnRegister() override;
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SendRenderDynamicData_Concurrent() override;

	// UPrimitiveComponent interface
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	// AutoUpdate=false �̏ꍇ�̍X�V 
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void UpdatePlayer(float DeltaTime);

	// �w�肵��Canvas�ɑ΂��ĕ`�悷��. ���O�ŗp�ӂ���Canvas�֕`�悷��ۂɎg�p����. 
	void DrawToCanvas(class FCanvas* Canvas, FVector2D Location, float Rotation=0.f, FVector2D Scale=FVector2D(1.f, 1.f), bool bMask=false);

	// �`��Ώۃe�N�X�`�����擾
	UFUNCTION(Category=SpriteStudio, BlueprintPure)
	UTexture* GetRenderTarget();

	// �p�[�c��Transform���擾(Actor����̑��΍��W�n. bDrawMesh=true�̏ꍇ�̂ݗL��)
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	FTransform GetPartTransformInAutoDrawMesh(int32 PartIndex) const;

	// �A�^�b�`�R���|�[�l���g�̒ǉ�
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void AddAttachComponent(int32 PartIndex, USceneComponent* Component);

	// �A�^�b�`�R���|�[�l���g�̒ǉ��i���O�w��j
	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void AddAttachComponentByName(FName PartName, USceneComponent* Component);


protected:
	UFUNCTION()
	void OnDraw(class UCanvas* Canvas, int32 Width, int32 Height);

	UFUNCTION()
	void OnDrawMask(class UCanvas* Canvas, int32 Width, int32 Height);

public:
	// �Đ�����SsProject
	UPROPERTY(Category=SpriteStudioAsset, EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayThumbnail="true"))
	USsProject* SsProject;


	// �����X�V. Off�ɂ���ꍇ�͎��O��UpdatePlayer()���Ăяo���čX�V����. 
	UPROPERTY(Category=SpriteStudioPlaySettings, EditDefaultsOnly, BlueprintReadOnly)
	bool bAutoUpdate;

	// �����Đ�. 
	UPROPERTY(Category=SpriteStudioPlaySettings, EditDefaultsOnly, BlueprintReadOnly)
	bool bAutoPlay;

	// �����Đ�����AnimPackIndex
	UPROPERTY(Category=SpriteStudioPlaySettings, EditDefaultsOnly, BlueprintReadOnly)
	int32 AutoPlayAnimPackIndex;

	// �����Đ�����AnimationIndex
	UPROPERTY(Category=SpriteStudioPlaySettings, EditDefaultsOnly, BlueprintReadOnly)
	int32 AutoPlayAnimationIndex;

	// �����Đ����̊J�n�t���[��
	UPROPERTY(Category=SpriteStudioPlaySettings, EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay)
	int32 AutoPlayStartFrame;

	// �����Đ����̍Đ����x
	UPROPERTY(Category=SpriteStudioPlaySettings, EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay)
	float AutoPlayRate;

	// �����Đ����̃��[�v��
	UPROPERTY(Category=SpriteStudioPlaySettings, EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay)
	int32 AutoPlayLoopCount;

	// �����Đ����̉����Đ�
	UPROPERTY(Category=SpriteStudioPlaySettings, EditDefaultsOnly, BlueprintReadOnly, AdvancedDisplay)
	bool bAutoPlayRoundTrip;


	// ���b�V���̕`��. Off�ɂ���ꍇ�͎��O��Canvas��p�ӂ��āADrawToCanvas()���Ăяo���ĕ`�悷��. 
	UPROPERTY(Category=SpriteStudioDrawSettings, EditDefaultsOnly, BlueprintReadOnly)
	bool bDrawMesh;

	// �x�[�X�}�e���A��
	UPROPERTY(Category=SpriteStudioDrawSettings, EditDefaultsOnly, BlueprintReadOnly, meta=(DisplayThumbnail="true"))
	UMaterialInterface* BaseMaterial;

	// ���b�V���̕`��Ɏg�p����MID
	UPROPERTY(Category=SpriteStudioDrawSettings, BlueprintReadOnly)
	UMaterialInstanceDynamic* Material;

	// ���b�V���̕`��̍ۂ�Canvas�̉𑜓x. 
	UPROPERTY(Category=SpriteStudioDrawSettings, EditDefaultsOnly, BlueprintReadOnly)
	FVector2D CanvasResolution;

	// ���b�V���̕`��̍ۂ̕`��X�P�[��. 
	UPROPERTY(Category=SpriteStudioDrawSettings, EditDefaultsOnly, BlueprintReadOnly)
	FVector2D MeshDrawScale;

	// ���b�V���̕`��̍ۂ�Plane���b�V���̃T�C�Y
	UPROPERTY(Category=SpriteStudioDrawSettings, EditDefaultsOnly, BlueprintReadOnly)
	FVector2D MeshPlaneSize;


	// �A�^�b�`�R���|�[�l���g���X�g
	UPROPERTY(Category=SpriteStudioAttachComponent, BlueprintReadWrite)
	TArray<FAttachComponent> AttachComponents;


	// �Đ��I���C�x���g // C++���痘�p����ۂ́A�R�R�ɒ���Delegate��ǉ����ĉ�����.
	UPROPERTY(BlueprintAssignable, Category=SpriteStudioCallback)
	FSsEndPlaySignature OnSsEndPlay;

	// ���[�U�[�f�[�^�C�x���g // C++���痘�p����ۂ́A�R�R�ɒ���Delegate��ǉ����ĉ�����.
	UPROPERTY(BlueprintAssignable, Category=SpriteStudioCallback)
	FSsUserDataSignature OnSsUserData;

protected:
	TSharedPtr<FSsPlayer> Player;
	UCanvasRenderTarget2D* RenderTarget;
	UCanvasRenderTarget2D* RenderTargetMask;

	//
	// MEMO:
	//	CanvasRenderTarget2D�̃o�O(�H)�ŁA�A���t�@�l��FCanvas�ւ̕`��Ɠ������ʂɂȂ炸�A����ɔ��f����Ȃ��B 
	//	SE_BLEND_MaskedDistanceField�Ń}�X�N�����A�A���t�@�l����������擾���邱�ƂŁA���b�V���֕`�悷��ۂ̌��ʂ����킹��B 
	//	v1.0�̐��������[�X�܂łɂ͂ǂ��ɂ����������AFCanvasRenderTarget2D�̎g�p���̂��������ĕʂ̕��@��T������...
	//

private:
	void InitializePlayerRender();
#if WITH_EDITOR
	bool bPlayerRenderInitialized;
#endif

public:
	// Blueprint interface �b��

	UFUNCTION(Category=SpriteStudio, BlueprintCallable, meta=(AdvancedDisplay="2"))
	bool Play(int32 AnimPackIndex, int32 AnimationIndex, int32 StartFrame=0, float PlayRate=1.f, int32 LoopCount=0, bool bRoundTrip=false);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable, meta=(AdvancedDisplay="2"))
	bool PlayByName(const FName& AnimPackName, const FName& AnimationName, int32 StartFrame=0, float PlayRate=1.f, int32 LoopCount=0, bool bRoundTrip=false);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void Pause();

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	bool Resume();

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	bool IsPlaying() const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	int32 GetNumAnimPacks() const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	int32 GetNumAnimations(int32 AnimPackIndex) const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetPlayFrame(float Frame);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	float GetPlayFrame() const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetLoopCount(int32 InLoopCount=0);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	int32 GetLoopCount() const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetRoundTrip(bool bInRoundTrip);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	bool IsRoundTrip() const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetPlayRate(float InRate);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	float GetPlayRate() const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetFlipH(bool InFlipH);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	bool GetFlipH() const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void SetFlipV(bool InFlipV);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	bool GetFlipV() const;

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void AddTextureReplacement(int32 PartIndex, UTexture* Texture);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void AddTextureReplacementByName(FName PartName, UTexture* Texture);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void RemoveTextureReplacement(int32 PartIndex);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void RemoveTextureReplacementByName(FName PartName);

	UFUNCTION(Category=SpriteStudio, BlueprintCallable)
	void RemoveTextureReplacementAll();

};
