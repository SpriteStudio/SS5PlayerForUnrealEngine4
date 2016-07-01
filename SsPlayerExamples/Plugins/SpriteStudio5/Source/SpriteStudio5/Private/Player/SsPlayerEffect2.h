#pragma once

#include "xorshift32.h"
#include "SsPlayerCellmap.h"

struct FSsPartState;
struct FSsEffectBehavior;
struct FSsEffectModel;
struct FSsEffectNode;

#define SS_EFFECT_SEED_MAGIC (7573)
#define SS_EFFECT_LIFE_EXTEND_SCALE (8)
#define SS_EFFECT_LIFE_EXTEND_MIN   (64)

struct FTimeAndValue
{
	float Time;
	float Value;
};

struct FParticleExistSt
{
	int32	Id;
	int32	Cycle;
	int32	Exist;
	int32	Born;
	int32	STime;
	int32	EndTime;
};

struct FEmitPattern
{
	int32	Uid;		// std::sort��TArray::Sort�̃A���S���Y���̈Ⴂ���z�����邽�߂́A�\�[�g�p��Q��r�l 
	int32	Life;
	int32	Cycle;

	// �\�[�g�p 
	bool operator < (const FEmitPattern& Other) const
	{
		return (Life != Other.Life) ? (Life < Other.Life) : (Uid < Other.Uid);
	}
};

//�ŏI�`��p�f�[�^
struct FParticleDrawData
{
	int32	Id;
	int32	PId;
	int32	STime;		//�������ꂽ�J�n����
	int32	Lifetime;

	//�`��p���[�N
	float	X;
	float	Y;
	float	Rot;
	float	Direc;

	FSsU8Color	Color;
	FVector2D	Scale;
};

//�G�~�b�^�[�����p�����[�^
//�G�f�B�^��t�@�C��������͂����
struct FEmitterParameter
{
	int32	Life;
	int32	Interval;
	int32	Emitnum;		//��x�ɔr�o������
	int32	Emitmax;		//�ő�r�o��
	int32	ParticleLife;	//���������p�[�e�B�N���̎���
	int32	ParticleLife2;	//�����ő�l
	bool	bInfinite;		//��������

	int32	LoopStart;
	int32	LoopEnd;
	int32	LoopLen;
	int32	LoopGen;


	//�e�X�g�p�f�t�H���g
	FEmitterParameter()
		: Life(15)
		, Interval(1)
		, Emitnum(2)
		, Emitmax(32)
		, ParticleLife(15)
		, ParticleLife2(15)
		, bInfinite(false)
	{}
};

//�p�[�e�B�N�������p�����[�^
//�G�f�B�^��t�@�C��������͂����
struct FParticleParameter
{

	FVector2D	Scale;

	FSsU8Color	StartColor;	//�X�^�[�g���̃J���[
	FSsU8Color	EndColor;	//�I�����̃J���[

	//����
	float		Speed;		//����
	float		Speed2;		//�����ő�l

	float		Angle;			//�����Ă�p�x
	float		AngleVariance;	//�ύ�

	bool		bUseGravity;
	FVector2D	Gravity;

	bool		bUseOffset;
	FVector2D	Offset;
	FVector2D	Offset2;

	bool		bUseRotation;
	float		Rotation;
	float		Rotation2;

	float		RotationAdd;
	float		RotationAdd2;

	bool		bUseRotationTrans;
	float		RotationFactor;
	float		EndLifeTimePer;

	bool		bUseTanAccel;
	float		TangentialAccel;
	float		TangentialAccel2;

	bool		bUseColor;
	FSsU8Color	InitColor;
	FSsU8Color	InitColor2;

	bool		bUseTransColor;
	FSsU8Color	TransColor;
	FSsU8Color	TransColor2;

	bool		bUseInitScale;
	FVector2D	ScaleRange;
	float		ScaleFactor;
	float		ScaleFactor2;

	bool		bUseTransScale;
	FVector2D	Transscale;
	FVector2D	TransscaleRange;
	float		TransscaleFactor;
	float		TransscaleFactor2;

	float		Delay;

	bool		bUsePGravity;
	FVector2D	GravityPos;
	float		GravityPower;

	bool		bUseAlphaFade;
	float		AlphaFade;
	float		AlphaFade2;

	bool		bUseTransSpeed;
	float		TransSpeed;
	float		TransSpeed2;

	bool		bUseTurnDirec;
	float		DirecRotAdd;

	bool		bUserOverrideRSeed;
	int32		OverrideRSeed;
};

//�G�~�b�^����N���X
struct FSsEffectEmitter
{
public:
	FSsCellValue	DispCell;

	int32	Priority;

	FEmitterParameter	Emitter;
	FParticleParameter	Particle;
	xorshift32			Rand;

	int32	EmitterSeed;
	int32	SeedOffset;

	//�����p�̃����O�o�b�t�@
	TArray<FEmitPattern>	EmitPattern;
	TArray<int32>			OffsetPattern;

	FParticleExistSt*		ParticleExistList;

	//���O�v�Z�o�b�t�@
	int32	ParticleIdMax;

	size_t		ParticleListBufferSize;
	uint32*		SeedList;


	FVector2D			Position;
	FSsEffectEmitter*	Parent;

	int32	ParentIndex;

	FSsCell*			RefCell;	//�`��p�Z��
	FSsEffectBehavior*	RefData;	//�f�[�^�X�V�p

	size_t	GlobalTime;
	size_t	SeedTableLen;

	int32	Uid;

public:
	FSsEffectEmitter()
		: SeedOffset(0)
		, ParticleExistList(0)
		, ParticleListBufferSize(180 * 100)	//�����o����p�[�e�B�N���̍ő�l
		, SeedList(nullptr)
		, Position(0.f, 0.f)
		, Parent(nullptr)
		, ParentIndex(-1)
		, GlobalTime(0)
	{
		EmitterSeed = SS_EFFECT_SEED_MAGIC;
	}
	virtual ~FSsEffectEmitter()
	{
		delete [] ParticleExistList;
		delete [] SeedList;

	}

	void SetSeedOffset(int32 Offset)
	{
		SeedOffset = Offset; 
	}

	int32 GetParticleIDMax(){ return OffsetPattern.Num(); }
	const FParticleExistSt* GetParticleDataFromID(int32 Id);
	void UpdateEmitter(double Time, int32 Slide);

	int32 GetTimeLength(){ return Emitter.Life + (Emitter.ParticleLife + Emitter.ParticleLife2); }

	//���ݎ��Ԃ���Y�o�����ʒu�����߂�
	//time�ϐ����狁�߂��鎮�Ƃ���
	void UpdateParticle(float InTime, FParticleDrawData* ParticleDrawData, bool bRecalc=false);

	//�p�[�e�B�N���̔����Ԋu�����O�v�Z����
	//�����ŏo�͂��m�肷��

	void Precalculate2();


	// �\�[�g�p 
	bool operator < (const FSsEffectEmitter& Other) const
	{
		return (Priority != Other.Priority) ? (Priority < Other.Priority) : (Uid < Other.Uid);
	}
};

class FSsEffectRenderV2
{
public:
	//�G�t�F�N�g�̃p�����[�^�f�[�^
	FSsEffectModel*		EffectData;

	//Model�ɋL�ڂ���Ă���G�~�b�^�̃��X�g
	TArray<FSsEffectEmitter*>	EmitterList;
	TArray<FSsEffectEmitter*>	UpdateList;

	//�����_���V�[�h
	uint32		MySeed;

	FVector		LayoutPosition;
	FVector2D	LayoutScale;

	float	NowFrame;
	float	TargetFrame;
	float	SecondNowFrame;

	int32	EffectTimeLength;

	bool	bInfinite;	//�����ɔ����o���邩�ǂ���

	FSsPartState*	ParentState;

	bool	bIsIntFrame;

	bool	bIsPlay;
	bool	bIsPause;
	bool	bIsLoop;

	int32	SeedOffset;

	FSsCellMapList*	CurCellMapManager;/// �Z���}�b�v�̃��X�g�i�A�j���f�R�[�_�[������炤

	bool bIsWarningData;

private:
	void InitEmitter(FSsEffectEmitter* Emitter, FSsEffectNode* Node);
	void ClearEmitterList();

public:
	FSsEffectRenderV2()
		: MySeed(0)
		, EffectTimeLength(0)
		, bIsIntFrame(true)
		, SeedOffset(0)
		, bIsWarningData(false)
	{}
	virtual ~FSsEffectRenderV2()
	{
		ClearEmitterList();
	}

	void Play(){ bIsPause=false; bIsPlay=true; }
	void Stop(){ bIsPlay=false; }
	void Pause(){ bIsPause=true; bIsPlay=false; }
	void SetLoop(bool Flag){ bIsLoop=Flag; }
	bool IsPlay(){ return bIsPlay; }
	bool IsPause(){ return bIsPause; }
	bool IsLoop(){ return bIsLoop; }

	void SetEffectData(FSsEffectModel* Data);

	void SetSeed(uint32 Seed)
	{
		MySeed = Seed * SS_EFFECT_SEED_MAGIC;
	}
	void SetFrame(float Frame)
	{
		NowFrame = Frame;
	}
	float GetFrame(){ return NowFrame; }

	void Update();
	void Reload();

	void CreateRenderParts(TArray<FSsRenderPart>& OutRenderParts, FSsPartState* State, const FVector2D& CurAnimeCanvasSize, const FVector2D& CurAnimePivot);
	void CreateRenderPart(TArray<FSsRenderPart>& OutRenderParts, FSsPartState* State, const FVector2D& CurAnimeCanvasSize, const FVector2D& CurAnimePivot
		,FSsEffectEmitter* Emitter, float Time, FSsEffectEmitter* Parent = nullptr, FParticleDrawData* DrawData = nullptr);

	void SetParentAnimeState(FSsPartState* State){ ParentState = State; }

	int32 GetCurrentFPS();

	void SetCellmapManager(FSsCellMapList* PList){ CurCellMapManager=PList; }

	bool GetPlayStatus()
	{
		return bIsPlay;
	}

	void SetSeedOffset(int32 Offset);
};

