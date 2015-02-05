#include "SpriteStudio5PrivatePCH.h"
#include "SsHUD.h"

#include "SsPlayerComponent.h"


// �R���X�g���N�^
#if defined(SS_UE4_4) || defined(SS_UE4_5)
ASsHUD::ASsHUD(const FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
#else
ASsHUD::ASsHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
#endif
{
}

// SsPlayer��HUD�ɕ`��
void ASsHUD::DrawSsPlayer(USsPlayerComponent* PlayerComponent, FVector2D Location, float Rotation, FVector2D Scale)
{
	if(PlayerComponent && Canvas)
	{
		PlayerComponent->DrawToCanvas(Canvas->Canvas, Location, Rotation, Scale);
	}
}
