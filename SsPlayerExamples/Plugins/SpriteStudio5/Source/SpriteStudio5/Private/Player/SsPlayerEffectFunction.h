#pragma once

#include "SsPlayerEffect.h"


class FSsEffectFunctionExecuter
{
public:
	static void	Initalize(FSsEffectBehavior* beh, FSsEffectRenderEmitter* emmiter);
	static void	UpdateEmmiter(FSsEffectBehavior* beh, FSsEffectRenderEmitter* emmiter);
	static void	InitializeParticle(FSsEffectBehavior* beh, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* particle);
	static void	UpdateParticle(FSsEffectBehavior* beh, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* particle);
};
