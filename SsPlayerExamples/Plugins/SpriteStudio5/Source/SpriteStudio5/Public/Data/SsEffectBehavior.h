#pragma once

#include "SsEffectElement.h"
#include "SsTypes.h"
#include "SsValue.h"

#include "SsEffectBehavior.generated.h"


USTRUCT()
struct SPRITESTUDIO5_API FSsEffectBehavior
{
	GENERATED_USTRUCT_BODY()
	void Serialize(FArchive& Ar);

public:
	struct FSsCell* RefCell;
	TArray<TSharedPtr<FSsEffectElementBase>> PList;

	UPROPERTY(VisibleAnywhere, Category=SsEffectBehavior, BlueprintReadOnly)
	FName CellName;

	UPROPERTY(VisibleAnywhere, Category=SsEffectBehavior, BlueprintReadOnly)
	FName CellMapName;

	UPROPERTY(VisibleAnywhere, Category=SsEffectBehavior, BlueprintReadOnly)
	TEnumAsByte<SsRenderBlendType::Type> BlendType;

public:
	FSsEffectBehavior()
		: RefCell(nullptr)
		, BlendType(SsRenderBlendType::Invalid)
	{
	}
};

