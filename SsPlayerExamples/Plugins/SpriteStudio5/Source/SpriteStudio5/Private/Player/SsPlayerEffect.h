#pragma once

#include "MersenneTwister.h"
#include "SsPlayerCellmap.h"
#include "SsTypes.h"

class FSsEffectRenderer;
struct FSsEffectModel;
struct FSsEffectNode;
struct FSsPartState;

UENUM()
namespace SsRenderType
{
	enum Type
	{
		BaseNode,
		EmmiterNode,
		ParticleNode,
	};
}

UENUM()
namespace SsEmmiterType
{
	enum Type
	{
		EmmiterTypeNormal,
		EmmiterTypeRibbon,
	};
}



//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
class FSsEffectRenderAtom
{
public:
	FVector Position;
	float Rotation;
	FVector2D Scale;

	FSsEffectRenderAtom* Parent;
	FSsEffectNode* Data;
	bool bIsLive;
	bool bIsInit;
	bool bIsCreateChild;

	float Lifetime;		//オブジェクトの最大生存時間
	float ExsitTime;	//存在した時間
	float Life;			//寿命 = 0で死

	bool bUndead;

	float Alpha;

public:

	FSsEffectRenderAtom()
		: Position(0.f, 0.f, 0.f)
		, Rotation(0.f)
		, Scale(1.0f, 1.0f)
		, Parent(nullptr)
		, Data(nullptr)
		, bIsLive(true)
		, bIsInit(false)
		, bIsCreateChild(false)
		, Lifetime(10.0f)
		, ExsitTime(0.f)
		, Life(1.0f)
		, bUndead(false)
		, Alpha(1.f)
		{
		}

	FSsEffectRenderAtom(FSsEffectNode* refdata, FSsEffectRenderAtom* _p)
	{
		Data = refdata;
		SetParent(_p);

		Lifetime = 0;
		Position = FVector::ZeroVector;
		Scale = FVector2D::ZeroVector;
		Rotation = 0.0f;
	}

	virtual ~FSsEffectRenderAtom(){};

	void SetParent(FSsEffectRenderAtom* _p){ Parent = _p; }
	TEnumAsByte<SsRenderType::Type> GetMyType(){ return SsRenderType::BaseNode; }
	bool IsInit(){ return bIsInit; }

	virtual void Initialize()
	{
		Parent = nullptr;
		bIsInit = false;
		bIsLive = true;
		Lifetime = 10.0f;
		Life = 1.0f;
		Rotation = 0;
		Position = FVector::ZeroVector;
		Scale = FVector2D(1.f, 1.f);
		bIsCreateChild = false;
		bIsInit = false;
	}
	virtual bool Genarate(FSsEffectRenderer* render){ return true; }

	virtual void Update(float delta){}
	virtual bool CreateRenderVertices(FSsEffectRenderer* render, FSsRenderVertex* OutRenderVertices, const FVector2D& CurAnimeCanvasSize, const FVector2D& CurAnimePivot){ return false; }

	FVector GetPosition() const { return Position; }
	void SetPosition(float x, float y, float z)
	{
		Position.X = x;
		Position.Y = y;
		Position.Z = z;
	}
	void SetScale(float x , float y)
	{
		Scale.X = x;
		Scale.Y = y;
	}
	void SetRotation(float z)
	{
		FMath::Fmod(z, 360.f);
	}

	float GetRotation() const { return Rotation; }
	FVector2D GetScale() const { return Scale; }
	virtual void Count() {}
};

class FSsEffectDrawBatch
{
public:
	int32 Priority;
	FSsCellValue* DispCell;
	FSsEffectNode* TargetNode;

	TEnumAsByte<SsRenderBlendType::Type> BlendType;
	TArray<FSsEffectRenderAtom*> DrawList;

	FSsEffectDrawBatch()
		: Priority(0)
		, DispCell(nullptr)
		, TargetNode(nullptr)
	{}

	void DrawSetting();

	bool operator < (const FSsEffectDrawBatch& Other) const
	{
		return Priority < Other.Priority;
	}
};


//--------------------------------------------------------------------------
//パーティクル生成能力を持つオブジェクト
//--------------------------------------------------------------------------
class FSsEffectRenderEmitter : public FSsEffectRenderAtom
{
public:
	uint32 MySeed;
	FSsCellValue DispCell;

	//パーティクルパラメータ
	FSsEffectNode* ParamParticle;

	CMersenneTwister* MT;

	//以前からの移植
	int32 MaxParticle;
	int32 Delay;
	float Interval;
	float Intervalleft;
	float Frame;
	float FrameDelta;
	int32 Burst;

	TEnumAsByte<SsEmmiterType::Type> Type;

	FName MyName;
	uint64 ParticleCount;


	bool GenerateOK;
	int32 DrawPriority;

public:
	FSsEffectDrawBatch* MyBatchList;

public:
	void InitParameter()
	{
		if(MT == nullptr)
		{
			MT = new CMersenneTwister();
		}

		FSsEffectRenderAtom::Initialize();
		Delay = 0;
		Interval = 0;
		Intervalleft = 0;
		Frame = 0;
		FrameDelta = 0;
		ParticleCount = 0;
		ExsitTime = 0;

		GenerateOK = true;

		ParamParticle = 0;
		Type = SsEmmiterType::EmmiterTypeNormal;
	}

	FSsEffectRenderEmitter()
		: MT(nullptr)
	{}
	FSsEffectRenderEmitter(FSsEffectNode* refdata, FSsEffectRenderAtom* _p)
	{
		Data = refdata;
		Parent = _p;
		InitParameter();
	}

	virtual ~FSsEffectRenderEmitter()
	{
		if(MT)
		{
			delete MT;
		}
	}
	TEnumAsByte<SsRenderType::Type> GetMyType(){ return SsRenderType::EmmiterNode;}
	void SetMySeed(uint32 seed);
	void TrushRandom(int32 loop)
	{
		for(int32 i = 0; i < loop; i++)
		{
			MT->genrand_uint32();
		}
	}

	virtual void Initialize() override;
	virtual bool Genarate(FSsEffectRenderer* render) override;

	virtual void Update(float delta) override;
	virtual void Count() override { ParticleCount = 0 ; }
};


//--------------------------------------------------------------------------
//パーティクルオブジェクト
//--------------------------------------------------------------------------
class FSsEffectRenderParticle : public FSsEffectRenderAtom
{
public:
	FSsCellValue* DispCell;

	FSsEffectRenderEmitter* ParentEmitter;
	struct FSsEffectBehavior* RefBehavior;

	FVector2D BaseEmiterPosition;	//もしかしてもう使ってないかも
	FVector2D Backposition;			//force計算前のポジション
	FVector2D ParticlePosition;		//描画用ポジション

	float Rotation;
	float RotationAdd;
	float RotationAddDst;
	float RotationAddOrg;

	FVector2D Size;
	FVector2D StartSize;
	FVector2D Divsize;

	bool bUseColor;
	FSsU8Color Color;
	FSsU8Color Startcolor;
	FSsU8Color Endcolor;

	float Speed;		//現在持っている速度
	float Firstspeed;
	float Lastspeed;
	FVector2D Vector;

	FVector2D Force;
	FVector2D Gravity;

	float RadialAccel;
	float TangentialAccel;
	float Direction;
	bool bIsTurnDirection;

	FVector2D Execforce;	//処理中の力 最終的には単位当たりの力に変換

public:
	void InitParameter()
	{
		FSsEffectRenderAtom::Initialize();

		ParticlePosition = FVector2D::ZeroVector;
		BaseEmiterPosition = FVector2D::ZeroVector;
		Backposition = FVector2D::ZeroVector;
		Rotation = 0;
		Size = FVector2D(1.0f, 1.0f);
		StartSize = FVector2D(1.0f , 1.0f);
		Divsize = FVector2D::ZeroVector;
		Force = FVector2D::ZeroVector;
		Gravity = FVector2D::ZeroVector;
		RadialAccel = 0;
		TangentialAccel = 0;
		bUseColor = false;
		Color = Startcolor = Endcolor = FSsU8Color(255,255,255,255) ;
		ExsitTime = 0;
		Execforce = FVector2D::ZeroVector;
		ParentEmitter = 0;
		DispCell = nullptr;
	}
	
	FSsEffectRenderParticle()
		: ParentEmitter(nullptr)
	{}
	FSsEffectRenderParticle(FSsEffectNode* refdata, FSsEffectRenderAtom* _p)
	{
		Data = refdata;
		Parent = _p;
		InitParameter();
	}

	TEnumAsByte<SsRenderType::Type> GetMyType(){ return SsRenderType::ParticleNode; }

	//生成フェーズ
	virtual void Initialize() override;

	virtual bool Genarate(FSsEffectRenderer* render) override;

	virtual void Update(float delta) override;
	virtual bool CreateRenderVertices(FSsEffectRenderer* render, FSsRenderVertex* OutRenderVertices, const FVector2D& CurAnimeCanvasSize, const FVector2D& CurAnimePivot) override;

	virtual void Count() override
	{
		if(ParentEmitter)
		{
			ParentEmitter->ParticleCount++;
		}
	}

	void UpdateDelta(float delta);
	void UpdateForce(float delta);
};


#define SSEFFECTRENDER_EMMITER_MAX (1024)
#define SSEFFECTRENDER_PARTICLE_MAX (4096)
#define SSEFFECTRENDER_BACTH_MAX (256)


//--------------------------------------------------------------------------
//エフェクトの描画処理メイン
//--------------------------------------------------------------------------
class FSsEffectRenderer
{
private:
	FSsEffectModel* EffectData;

	bool bIsPlay;
	bool bIsPause;
	bool bIsLoop;
	bool bFirstUpdated;
	uint32 MySeed;

	FVector LayoutPosition;

	FSsCellMapList*	CurCellMapManager;/// セルマップのリスト（アニメデコーダーからもらう

	FSsEffectRenderEmitter EmitterPool[SSEFFECTRENDER_EMMITER_MAX+1];
	FSsEffectRenderParticle ParticlePool[SSEFFECTRENDER_PARTICLE_MAX+1];
	FSsEffectDrawBatch DprPool[SSEFFECTRENDER_BACTH_MAX+1];

	int32 EmitterPoolCount;
	int32 ParticlePoolCount;
	int32 DprPoolCount;

public:
	//アップデート物のリスト
	FSsEffectRenderAtom* RenderRoot;

	bool bUsePreMultiTexture;
	uint32 ParentAnimeStartFrame;
	bool bRenderTexture;
	float FrameDelta;
	FSsPartState* ParentState;

	TArray<FSsEffectRenderAtom*> UpdateList;
	TArray<FSsEffectDrawBatch*> DrawBatchList;

public:
	FSsEffectRenderer()
		: EffectData(nullptr)
		, bIsPlay(false)
		, bIsPause(false)
		, bIsLoop(false)
		, bFirstUpdated(false)
		, MySeed(0)
		, CurCellMapManager(nullptr)
		, EmitterPoolCount(0)
		, ParticlePoolCount(0)
		, DprPoolCount(0)
		, RenderRoot(nullptr)
		, bUsePreMultiTexture(true)
		, ParentAnimeStartFrame(0)
		, bRenderTexture(false)
		, FrameDelta(0.f)
		, ParentState(nullptr)
	{}

	virtual ~FSsEffectRenderer();

	void ClearUpdateList();

public:
	void SetSeed(uint32 seed){ MySeed = seed; }
	void Update(float delta);
	void Reload();

	void CreateRenderParts(TArray<FSsRenderPart>& OutRenderParts, FSsPartState* State, const FVector2D& CurAnimeCanvasSize, const FVector2D& CurAnimePivot);

	//操作
	void Play();
	void Stop();
	void Pause();
	void SetLoop(bool flag);
	bool GetPlayStatus(void);	//追加
	bool GetFirstUpdated(){ return bFirstUpdated; }

	int32 GetCurrentFPS();
	FSsEffectModel* GetEffectData()
	{
		return EffectData;
	}

	// データセット
	void SetEffectData(FSsEffectModel* data)
	{
		Stop();
		ClearUpdateList();
		EffectData = data;
	}
	void SetParentAnimeState(FSsPartState* state){ ParentState = state; }
	FSsEffectRenderAtom* CreateAtom(uint32 seed, FSsEffectRenderAtom* parent, FSsEffectNode* node);
	void SetCellmapManager(FSsCellMapList* plist) { CurCellMapManager = plist; }

};

