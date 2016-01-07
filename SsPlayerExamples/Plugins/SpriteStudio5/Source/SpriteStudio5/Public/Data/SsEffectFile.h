#pragma once

#include "SsTypes.h"
#include "SsEffectBehavior.h"

#include "SsEffectFile.generated.h"


USTRUCT()
struct SPRITESTUDIO5_API FSsSimpleTree
{
	GENERATED_USTRUCT_BODY()

public:
	FSsSimpleTree *Parent;
	FSsSimpleTree *Ctop;
	FSsSimpleTree *Prev;
	FSsSimpleTree *Next;

public:
	FSsSimpleTree()
		: Parent(nullptr),Ctop(nullptr),Prev(nullptr),Next(nullptr)
	{}

	void AddChildEnd(FSsSimpleTree* c)
	{
		if(!Ctop)
		{
			Ctop = c; 
		}
		else
		{
			Ctop->AddSiblingEnd(c);
		}
		c->Parent = this;
	}
	void AddSiblingEnd(FSsSimpleTree* c)
	{
		if(!Next)
		{
			c->Prev = this;
			Next = c;
		}
		else
		{
			Next->AddSiblingEnd(c);
		}

		c->Parent = this->Parent;
	}
};


USTRUCT()
struct SPRITESTUDIO5_API FSsEffectNode : public FSsSimpleTree
{
	GENERATED_USTRUCT_BODY()
	void Serialize(FArchive& Ar);

public:
	UPROPERTY(VisibleAnywhere, Category=SsEffectNode, BlueprintReadOnly)
	int32 ArrayIndex;

	UPROPERTY(VisibleAnywhere, Category=SsEffectNode, BlueprintReadOnly)
	int32 ParentIndex;

	UPROPERTY(VisibleAnywhere, Category=SsEffectNode, BlueprintReadOnly)
	TEnumAsByte<SsEffectNodeType::Type> Type;

	UPROPERTY(VisibleAnywhere, Category=SsEffectNode, BlueprintReadOnly)
	bool Visible;

	UPROPERTY(VisibleAnywhere, Category=SsEffectNode, BlueprintReadOnly)
	FSsEffectBehavior Behavior;

public:
	FSsEffectNode()
		: ArrayIndex(0)
		, ParentIndex(0)
		, Type(SsEffectNodeType::Invalid)
	{}

	SsEffectNodeType::Type GetType(){ return Type; }
	FSsEffectBehavior* GetMyBehavior(){ return &Behavior;}
};


USTRUCT()
struct SPRITESTUDIO5_API FSsEffectModel
{
	GENERATED_USTRUCT_BODY()
	void Serialize(FArchive& Ar);

public:
	FSsEffectNode* Root;

	UPROPERTY(VisibleAnywhere, Category=SsEffectModel, BlueprintReadOnly)
	TArray<FSsEffectNode> NodeList;

	//	ランダムシード固定値 
	UPROPERTY(VisibleAnywhere, Category=SsEffectModel, BlueprintReadOnly)
	int32		LockRandSeed;

	//	ランダムシードを固定するか否か 
	UPROPERTY(VisibleAnywhere, Category=SsEffectModel, BlueprintReadOnly)
	bool		IsLockRandSeed;

	UPROPERTY(VisibleAnywhere, Category=SsEffectModel, BlueprintReadOnly)
	int32		FPS;

	UPROPERTY(VisibleAnywhere, Category=SsEffectModel, BlueprintReadOnly)
	FString		BgColor;

	UPROPERTY(VisibleAnywhere, Category=SsEffectModel, BlueprintReadOnly)
	FName		EffectName;

public:
	FSsEffectModel()
		: Root(nullptr)
	{}

	void BuildTree();
};


USTRUCT()
struct SPRITESTUDIO5_API FSsEffectFile 
{
	GENERATED_USTRUCT_BODY()
	void Serialize(FArchive& Ar);

public:
	UPROPERTY(VisibleAnywhere, Category=SsEffectFile, BlueprintReadOnly)
	FSsEffectModel EffectData;

	UPROPERTY(VisibleAnywhere, Category=SsEffectFile, BlueprintReadOnly)
	FName Name;
};

