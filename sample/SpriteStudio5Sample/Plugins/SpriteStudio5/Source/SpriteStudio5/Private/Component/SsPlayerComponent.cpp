#include "SpriteStudio5PrivatePCH.h"
#include "SsPlayerComponent.h"

#include "SsRenderSceneProxy.h"
#include "SsProject.h"
#include "SsAnimePack.h"
#include "SsPlayer.h"
#include "SsPlayerAnimedecode.h"


// �R���X�g���N�^
#if defined(SS_UE4_4) || defined(SS_UE4_5)
USsPlayerComponent::USsPlayerComponent(const FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
#else
USsPlayerComponent::USsPlayerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
#endif
	, SsProject(NULL)
	, bAutoUpdate(true)
	, bAutoPlay(true)
	, AutoPlayAnimPackIndex(0)
	, AutoPlayAnimationIndex(0)
	, AutoPlayStartFrame(0)
	, AutoPlayRate(1.f)
	, AutoPlayLoopCount(0)
	, bAutoPlayRoundTrip(false)
	, bDrawMesh(true)
	, BaseMaterial(NULL)
	, Material(NULL)
	, CanvasResolution(1024, 1024)
	, MeshDrawScale(1.f, 1.f)
	, MeshPlaneSize(300.f, 300.f)
	, RenderTarget(NULL)
	, RenderTargetMask(NULL)
#if WITH_EDITOR
	, bPlayerRenderInitialized(false)
#endif
{
	// Structure to hold one-time initialization
	// �Q�ƁFhttps://docs.unrealengine.com/latest/INT/Programming/UnrealArchitecture/Reference/Classes/index.html#assetreferences
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinder<UMaterialInterface> BaseMaterial;
		FConstructorStatics()
			: BaseMaterial(TEXT("/SpriteStudio5/SsMaterial_MeshDefault"))
		{}
	};
	static FConstructorStatics ConstructorStatics;

	BaseMaterial = ConstructorStatics.BaseMaterial.Object;

	// UActorComponent
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bTickInEditor = true;
	bAutoActivate = true;

	// UPrimitiveComponent
	CastShadow = false;
	bUseAsOccluder = false;
	bCanEverAffectNavigation = false;
}

// �R���|�[�l���g�o�^���̏�����
void USsPlayerComponent::OnRegister()
{
	Super::OnRegister();

	if(SsProject)
	{
		// Player�̐���
		Player = MakeShareable(new FSsPlayer());
		Player->SetSsProject(SsProject);

		// �����Đ�
		if(bAutoPlay)
		{
			Player->Play(AutoPlayAnimPackIndex, AutoPlayAnimationIndex, AutoPlayStartFrame, AutoPlayRate, AutoPlayLoopCount, bAutoPlayRoundTrip);
		}
	}
}

// SsPlayer�̏�����
void USsPlayerComponent::InitializePlayerRender()
{
#if WITH_EDITOR
	bPlayerRenderInitialized = true;
#endif

#if !defined(SS_DISABLE_DRAW_MESH)
	if(bDrawMesh && Player.IsValid())
	{
		// RenderTarget�̏�����
		if((0 < CanvasResolution.X) && (0 < CanvasResolution.Y))
		{
			RenderTarget = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(UCanvasRenderTarget2D::StaticClass(), CanvasResolution.X, CanvasResolution.Y);
			if(RenderTarget)
			{
				RenderTarget->SetFlags(RF_Transient);
				RenderTarget->OnCanvasRenderTargetUpdate.AddDynamic(this, &USsPlayerComponent::OnDraw);
				RenderTarget->ClearColor = FLinearColor(0.f, 0.f, 0.f, 0.f);
				RenderTarget->UpdateResource();
				MarkRenderStateDirty();
			}

			RenderTargetMask = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(UCanvasRenderTarget2D::StaticClass(), CanvasResolution.X, CanvasResolution.Y);
			if(RenderTargetMask)
			{
				RenderTargetMask->SetFlags(RF_Transient);
				RenderTargetMask->OnCanvasRenderTargetUpdate.AddDynamic(this, &USsPlayerComponent::OnDrawMask);
				RenderTargetMask->ClearColor = FLinearColor(0.f, 0.f, 0.f, 0.f);
				RenderTargetMask->UpdateResource();
				MarkRenderStateDirty();
			}
		}

		if(BaseMaterial)
		{
			Material = UMaterialInstanceDynamic::Create(BaseMaterial, this);
			if(Material)
			{
				Material->SetFlags(RF_Transient);
				Material->SetTextureParameterValue(FName(TEXT("RenderTarget")), RenderTarget);
				Material->SetTextureParameterValue(FName(TEXT("RenderTargetMask")), RenderTargetMask);

				if(SceneProxy)
				{
					((FSsRenderSceneProxy*)SceneProxy)->SetMaterial(Material);
				}
			}
		}
	}
#endif
}

// �Q�[���J�n���̏�����
void USsPlayerComponent::InitializeComponent()
{
	Super::InitializeComponent();
	InitializePlayerRender();
}

// �X�V
void USsPlayerComponent::TickComponent(float DeltaTime, enum ELevelTick /*TickType*/, FActorComponentTickFunction* /*ThisTickFunction*/)
{
#if WITH_EDITOR
	// for Editor�v���r���[ (Editor�ł�InitializeComponent()�����Ȃ��̂�)
	if(!bPlayerRenderInitialized)
	{
		InitializePlayerRender();
	}

	// SsProject��Reimport���ꂽ��AActorComponent���ēo�^������
	if(Player.IsValid() && Player->GetSsProject().IsStale())
	{
		GetOwner()->ReregisterAllComponents();
		return;
	}
#endif

	if(bAutoUpdate)
	{
		UpdatePlayer(DeltaTime);
	}
}

void USsPlayerComponent::SendRenderDynamicData_Concurrent()
{
	if(NULL == SceneProxy)
	{
		return;
	}

	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
		FSendSsDynamicData,
		FSsRenderSceneProxy*, SsSceneProxy, (FSsRenderSceneProxy*)SceneProxy,
	{
		SsSceneProxy->SetDynamicData_RenderThread();
	});
}

FPrimitiveSceneProxy* USsPlayerComponent::CreateSceneProxy()
{
#if !defined(SS_DISABLE_DRAW_MESH)
	if(bDrawMesh)
	{
		FSsRenderSceneProxy* NewProxy = new FSsRenderSceneProxy(this, Material);
		NewProxy->MeshPlaneSize = MeshPlaneSize;
		return NewProxy;
	}
#endif
	return NULL;
}

FBoxSphereBounds USsPlayerComponent::CalcBounds(const FTransform& LocalToWorld) const
{
#if !defined(SS_DISABLE_DRAW_MESH)
	if(bDrawMesh)
	{
		float r = (MeshPlaneSize.X > MeshPlaneSize.Y) ? MeshPlaneSize.X : MeshPlaneSize.Y;
		return FBoxSphereBounds(
			LocalToWorld.GetLocation() + FVector(0.f, 0.f, MeshPlaneSize.Y/2.f),
			FVector(r, 1.f, r),
			r
			);
	}
#endif
	return FBoxSphereBounds(EForceInit::ForceInitToZero);
}

// AutoUpdate=false �̏ꍇ�̍X�V 
void USsPlayerComponent::UpdatePlayer(float DeltaTime)
{
	if(!bIsActive)
	{
		return;
	}

	if(SceneProxy && Material)
	{
		((FSsRenderSceneProxy*)SceneProxy)->SetMaterial(Material);
	}

	//	�A�j���[�V�����̍Đ��X�V
	if(Player.IsValid())
	{
		FSsPlayerTickResult Result = Player->Tick(DeltaTime);

		// ���[�U�[�f�[�^�C�x���g
		for(int32 i = 0; i < Result.UserData.Num(); ++i)
		{
			OnSsUserData.Broadcast(
				Result.UserData[i].PartName,
				Result.UserData[i].PartIndex,
				Result.UserData[i].KeyFrame,
				Result.UserData[i].Value
				);
		}

		// �Đ��I���C�x���g
		if(Result.bEndPlay)
		{
			OnSsEndPlay.Broadcast();
		}
	}

	if(RenderTarget)
	{
		RenderTarget->UpdateResource();
	}
	if(RenderTargetMask)
	{
		RenderTargetMask->UpdateResource();
	}

#if !defined(SS_DISABLE_DRAW_MESH)
	if(bDrawMesh)
	{
		MarkRenderDynamicDataDirty();

		//	�A�^�b�`�̉���
		for(int i = 0; i < AttachComponents.Num(); ++i)
		{
			if(NULL != AttachComponents[i].Component.Get())
			{
				AttachComponents[i].Component->SetRelativeTransform(
					GetPartTransformInAutoDrawMesh( AttachComponents[i].PartIndex )
					);
			}
		}
	}
#endif
}


// �w�肵��Canvas�ɑ΂��ĕ`�悷��. ���O�ŗp�ӂ���Canvas�֕`�悷��ۂɎg�p����. 
void USsPlayerComponent::DrawToCanvas(FCanvas* Canvas, FVector2D Location, float Rotation, FVector2D Scale, bool bMask)
{
	if(Player.IsValid())
	{
		Player->Draw(Canvas, Location, Rotation, Scale, bMask);
	}
}


// �e�N�X�`���̎擾
UTexture* USsPlayerComponent::GetRenderTarget()
{
	return RenderTarget;
}

// �p�[�c��Transform���擾(Actor����̑��΍��W�n. bDrawMesh=true�̏ꍇ�̂ݗL��)
//		bAutoUpdate=true��Actor��Tick()����Ăяo�����ꍇ�AComponent��Tick()�̕�����Ȃ̂łP�t���[���x���̂Œ���. 
//		�ʏ�� AttachComponents ���g�p���ĉ�����. 
FTransform USsPlayerComponent::GetPartTransformInAutoDrawMesh(int32 PartIndex) const
{
	if(Player.IsValid())
	{
		// Canvas��2D���W�n����3D���W�n�ɕϊ�
		//	(���̕ύX�C���S�_�̕ϊ�)
		FVector2D Center(
			(float)(CanvasResolution.X/2) + (Player->GetAnimPivot().X * CanvasResolution.X),
			(float)(CanvasResolution.Y/2) - (Player->GetAnimPivot().Y * CanvasResolution.Y)
			);
		FTransform Trans = Player->GetPartTransformInCanvas(PartIndex, CanvasResolution.X, CanvasResolution.Y, Center, 0.f, MeshDrawScale);
		Trans.SetTranslation(
			FVector(
				0.f,
				(0.f >= CanvasResolution.X) ? 0.f : ((Trans.GetTranslation().X / CanvasResolution.X) * MeshPlaneSize.X - (MeshPlaneSize.X / 2.f)),
				(0.f >= CanvasResolution.Y) ? 0.f : ((MeshPlaneSize.Y - (Trans.GetTranslation().Y / CanvasResolution.Y) * MeshPlaneSize.Y))
			));
		Trans.SetRotation(FQuat(FRotator(Trans.GetRotation()) * -1.f));
		Trans.SetScale3D(FVector(1.f, Trans.GetScale3D().X, Trans.GetScale3D().Y));
		return Trans;
	}
	return FTransform();
}

// �A�^�b�`�R���|�[�l���g�̒ǉ�
void USsPlayerComponent::AddAttachComponent(int32 PartIndex, USceneComponent* Component)
{
	FAttachComponent Attach;
	Attach.PartIndex = PartIndex;
	Attach.Component = Component;

	AttachComponents.Add(Attach);
}

// �A�^�b�`�R���|�[�l���g�̒ǉ��i���O�w��j
void USsPlayerComponent::AddAttachComponentByName(FName PartName, USceneComponent* Component)
{
	if(Player.IsValid())
	{
		int32 PartIndex = Player->GetPartIndexFromName(PartName);
		if(0 < PartIndex)
		{
			AddAttachComponent(PartIndex, Component);
		}
	}
}


// �`��X�V
void USsPlayerComponent::OnDraw(UCanvas* Canvas, int32 Width, int32 Height)
{
	if(Canvas && Player.IsValid())
	{
		FVector2D Center(
			(float)(Width /2) + (Player->GetAnimPivot().X * Width),
			(float)(Height/2) - (Player->GetAnimPivot().Y * Height)
			);
		DrawToCanvas(Canvas->Canvas, Center, 0.f, MeshDrawScale);
	}
}
void USsPlayerComponent::OnDrawMask(UCanvas* Canvas, int32 Width, int32 Height)
{
	if(Canvas && Player.IsValid())
	{
		FVector2D Center(
			(float)(Width /2) + (Player->GetAnimPivot().X * Width),
			(float)(Height/2) - (Player->GetAnimPivot().Y * Height)
			);
		DrawToCanvas(Canvas->Canvas, Center, 0.f, MeshDrawScale, true);
	}
}

// Blueprint interface
bool USsPlayerComponent::Play(int32 AnimPackIndex, int32 AnimationIndex, int32 StartFrame, float PlayRate, int32 LoopCount, bool bRoundTrip)
{
	if(Player.IsValid())
	{
		if(Player->Play(AnimPackIndex, AnimationIndex, StartFrame, PlayRate, LoopCount, bRoundTrip))
		{
			if(bAutoUpdate)
			{
				UpdatePlayer(0.f);
			}
			return true;
		}
	}
	return false;
}
bool USsPlayerComponent::PlayByName(const FName& AnimPackName, const FName& AnimationName, int32 StartFrame, float PlayRate, int32 LoopCount, bool bRoundTrip)
{
	if(Player.IsValid())
	{
		int32 AnimPackIndex, AnimationIndex;
		if(Player->GetAnimationIndex(AnimPackName, AnimationName, AnimPackIndex, AnimationIndex))
		{
			return Play(AnimPackIndex, AnimationIndex, StartFrame, PlayRate, LoopCount, bRoundTrip);
		}
	}
	return false;
}
void USsPlayerComponent::Pause()
{
	if(Player.IsValid())
	{
		Player->Pause();
	}
}
bool USsPlayerComponent::Resume()
{
	if(Player.IsValid())
	{
		return Player->Resume();
	}
	return false;
}
bool USsPlayerComponent::IsPlaying() const
{
	if(Player.IsValid())
	{
		return Player->IsPlaying();
	}
	return false;
}
int32 USsPlayerComponent::GetNumAnimPacks() const
{
	if(Player.IsValid())
	{
		TWeakObjectPtr<USsProject> SsProject = Player->GetSsProject();
		if(SsProject.IsValid())
		{
			return SsProject->AnimeList.Num();
		}
		return 0;
	}
	return 0;
}
int32 USsPlayerComponent::GetNumAnimations(int32 AnimPackIndex) const
{
	if(Player.IsValid())
	{
		TWeakObjectPtr<USsProject> SsProject = Player->GetSsProject();
		if(SsProject.IsValid() && (AnimPackIndex < SsProject->AnimeList.Num()))
		{
			return SsProject->AnimeList[AnimPackIndex].AnimeList.Num();
		}
		return 0;
	}
	return 0;
}
void USsPlayerComponent::SetPlayFrame(float Frame)
{
	if(Player.IsValid())
	{
		Player->SetPlayFrame(Frame);
	}
}
float USsPlayerComponent::GetPlayFrame() const
{
	if(Player.IsValid())
	{
		return Player->GetPlayFrame();
	}
	return 0.f;
}
void USsPlayerComponent::SetLoopCount(int32 InLoopCount)
{
	if(Player.IsValid())
	{
		Player->LoopCount = InLoopCount;
	}
}
int32 USsPlayerComponent::GetLoopCount() const
{
	if(Player.IsValid())
	{
		return Player->LoopCount;
	}
	return false;
}
void USsPlayerComponent::SetRoundTrip(bool bInRoundTrip)
{
	if(Player.IsValid())
	{
		Player->bRoundTrip = bInRoundTrip;
	}
}
bool USsPlayerComponent::IsRoundTrip() const
{
	if(Player.IsValid())
	{
		return Player->bRoundTrip;
	}
	return false;
}
void USsPlayerComponent::SetPlayRate(float InRate)
{
	if(Player.IsValid())
	{
		Player->PlayRate = InRate;
	}
}
float USsPlayerComponent::GetPlayRate() const
{
	if(Player.IsValid())
	{
		return Player->PlayRate;
	}
	return 0.f;
}


void USsPlayerComponent::SetFlipH(bool InFlipH)
{
	if(Player.IsValid())
	{
		Player->bFlipH = InFlipH;
	}
}
bool USsPlayerComponent::GetFlipH() const
{
	if(Player.IsValid())
	{
		return Player->bFlipH;
	}
	return false;
}
void USsPlayerComponent::SetFlipV(bool InFlipV)
{
	if(Player.IsValid())
	{
		Player->bFlipV = InFlipV;
	}
}
bool USsPlayerComponent::GetFlipV() const
{
	if(Player.IsValid())
	{
		return Player->bFlipV;
	}
	return false;
}

void USsPlayerComponent::AddTextureReplacement(int32 PartIndex, UTexture* Texture)
{
	if(Player.IsValid() && Texture)
	{
		Player->TextureReplacements.Add(PartIndex, TWeakObjectPtr<UTexture>(Texture));
	}
}
void USsPlayerComponent::AddTextureReplacementByName(FName PartName, UTexture* Texture)
{
	if(Player.IsValid() && Texture)
	{
		int32 PartIndex = Player->GetPartIndexFromName(PartName);
		if(0 <= PartIndex)
		{
			Player->TextureReplacements.Add(PartIndex, TWeakObjectPtr<UTexture>(Texture));
		}
	}
}
void USsPlayerComponent::RemoveTextureReplacement(int32 PartIndex)
{
	if(Player.IsValid())
	{
		Player->TextureReplacements.Remove(PartIndex);
	}
}
void USsPlayerComponent::RemoveTextureReplacementByName(FName PartName)
{
	if(Player.IsValid())
	{
		int32 PartIndex = Player->GetPartIndexFromName(PartName);
		if(0 <= PartIndex)
		{
			Player->TextureReplacements.Remove(PartIndex);
		}
	}
}
void USsPlayerComponent::RemoveTextureReplacementAll()
{
	if(Player.IsValid())
	{
		Player->TextureReplacements.Empty();
	}
}
