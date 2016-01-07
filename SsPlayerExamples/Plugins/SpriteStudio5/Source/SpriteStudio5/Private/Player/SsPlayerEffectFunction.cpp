#include "SpriteStudio5PrivatePCH.h"
#include "SsPlayerEffectFunction.h"

#include "SsPlayerAnimedecode.h"
#include "SsPlayerEffect.h"
#include "SsEffectElement.h"
#include "SsEffectBehavior.h"


namespace
{
	//二つの値の範囲から値をランダムで得る
	static uint8 GetRandamNumberRange(FSsEffectRenderEmitter* e, uint8 a, uint8 b)
	{
		uint8 min = a < b ? a : b;
		uint8 max = a < b ? b : a;

		uint8 diff = ( max - min );


		if ( diff == 0 ) { return min; }
		return min + (e->MT->genrand_uint32() % diff);
	}

	static void VarianceCalcColor(FSsEffectRenderEmitter* e, FSsU8Color& out, FSsU8Color color1, FSsU8Color color2)
	{
		out.R = GetRandamNumberRange(e, color1.R, color2.R);
		out.G = GetRandamNumberRange(e, color1.G, color2.G);
		out.B = GetRandamNumberRange(e, color1.B, color2.B);
		out.A = GetRandamNumberRange(e, color1.A, color2.A);
	}

	static float FRand(uint32 v)
	{
		uint32 res = (v>>9) | 0x3f800000;
		return (*(float*)&res) - 1.0f;
	}

	static float VarianceCalc(FSsEffectRenderEmitter* e, float base, float variance)
	{
		uint32  r = e->MT->genrand_uint32();

		float len = variance - base;

		return base + len * FRand( r );
	}

	static float VarianceCalcFin(FSsEffectRenderEmitter* e, float base, float variance)
	{
		uint32 r = e->MT->genrand_uint32();

		return base + (-variance + variance* ( FRand(r) * 2.0f ));
	}

	static uint8 BlendNumber(uint8 a, uint8 b, float rate)
	{
		return ( a + ( b - a ) * rate );
	}

	static float BlendFloat(float a, float b, float rate)
	{
		return ( a + ( b - a ) * rate );
	}
}



class FEffectFuncBase
{
public:
	virtual void InitalizeEmmiter (FSsEffectElementBase* ele, FSsEffectRenderEmitter* emmiter){}
	virtual void UpdateEmmiter(FSsEffectElementBase* ele, FSsEffectRenderEmitter* emmiter){}
	virtual void InitializeParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* particle){}
	virtual void UpdateParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* particle ){}
};

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FFuncParticleElementBasic : public FEffectFuncBase
{
public:
	virtual void InitalizeEmmiter(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e) override
	{
		FSsParticleElementBasic* source = static_cast<FSsParticleElementBasic*>(ele);

		e->MaxParticle = source->MaximumParticle;
		e->Interval = source->Interval;
		e->Lifetime = source->Lifetime;
		e->Life = source->Lifetime;
		e->Burst = source->AttimeCreate;

		e->bUndead = false;
		e->DrawPriority = source->Priority;

		if(e->Lifetime == 0) { e->bUndead = true; }
	}

	virtual void InitializeParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* p) override
	{
		FSsParticleElementBasic* source = static_cast<FSsParticleElementBasic*>(ele);
		FVector eVec = e->GetPosition();
		float eAngle = 0;

		p->BaseEmiterPosition.X = eVec.X;
		p->BaseEmiterPosition.Y = eVec.Y;
		p->ParticlePosition.X = p->BaseEmiterPosition.X;
		p->ParticlePosition.Y = p->BaseEmiterPosition.Y;
		p->Size = FVector2D(1.0f , 1.0f);


		p->Color = FSsU8Color(255,255,255,255);
		p->Startcolor = FSsU8Color(255,255,255,255);
		p->Endcolor = p->Startcolor;


		p->Backposition = p->ParticlePosition;

		p->Lifetime = VarianceCalc(e , source->Lifespan.GetMinValue(), source->Lifespan.GetMaxValue());
		p->Life = source->Lifetime;
		float temp_angle = VarianceCalcFin(e,  source->Angle+eAngle, source->AngleVariance/2.0f);

		float angle_rad = FMath::DegreesToRadians( (temp_angle+90.0f) );
		float lspeed = VarianceCalc(e, source->Speed.GetMinValue(), source->Speed.GetMaxValue());

		p->Speed = lspeed;
		p->Firstspeed = lspeed;
		p->Vector.X = FMath::Cos(angle_rad);
		p->Vector.Y = FMath::Sin(angle_rad);

		p->Force = FVector2D::ZeroVector;
		p->Direction = 0;
		p->bIsTurnDirection = false;

		p->Rotation = 0;
		p->RotationAdd = 0;
		p->RotationAddDst = 0;
		p->RotationAddOrg = 0;

		p->Rotation = 0;
	}
};
static FFuncParticleElementBasic FuncBasic;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FFuncParticleElementRndSeedChange : public FEffectFuncBase
{
public:
	virtual void InitalizeEmmiter(FSsEffectElementBase* ele, FSsEffectRenderEmitter* emmiter) override
	{
		FSsParticleElementRndSeedChange* source = static_cast<FSsParticleElementRndSeedChange*>(ele);
		emmiter->SetMySeed(source->Seed);
	}
};
static FFuncParticleElementRndSeedChange FuncRndSeedChange;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FFuncParticleElementDelay : public FEffectFuncBase
{
public:
	virtual void InitalizeEmmiter(FSsEffectElementBase* ele, FSsEffectRenderEmitter* emmiter) override
	{
		FSsParticleElementDelay* source = static_cast<FSsParticleElementDelay*>(ele);
		emmiter->Delay = source->DelayTime;
		emmiter->Lifetime = emmiter->Lifetime + source->DelayTime;
		emmiter->Life = emmiter->Lifetime;
		emmiter->GenerateOK = false;
	}

	virtual void UpdateEmmiter(FSsEffectElementBase* ele, FSsEffectRenderEmitter* emmiter) override
	{
		FSsParticleElementDelay* source = static_cast<FSsParticleElementDelay*>(ele);
		//既定の時間までストップ？
		if(emmiter->ExsitTime >= source->DelayTime)
		{
			emmiter->GenerateOK = true;
		}	
	}
};
static FFuncParticleElementDelay FuncDelay;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FFuncParticleElementGravity : public FEffectFuncBase
{
public:
	virtual void InitializeParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* p) override
	{
		FSsParticleElementGravity* source = static_cast<FSsParticleElementGravity*>(ele);
		p->Gravity = source->Gravity;
	}
	virtual void UpdateParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* particle) override
	{
		FSsParticleElementGravity* source = static_cast<FSsParticleElementGravity*>(ele);
		particle->Gravity = source->Gravity * particle->ExsitTime;
	}
};
static FFuncParticleElementGravity FuncGravity;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FFuncParticleElementPosition : public FEffectFuncBase
{
public:
	virtual void InitializeParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* p) override
	{
		FSsParticleElementPosition* source = static_cast<FSsParticleElementPosition*>(ele);
		p->ParticlePosition.X = p->BaseEmiterPosition.X + VarianceCalc(e , source->OffsetX.GetMinValue(), source->OffsetX.GetMaxValue() );
		p->ParticlePosition.Y = p->BaseEmiterPosition.Y + VarianceCalc(e , source->OffsetY.GetMinValue(), source->OffsetY.GetMaxValue() );
	}
};
static FFuncParticleElementPosition FuncPosition;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FFuncParticleElementRotation: public FEffectFuncBase
{
public:
	virtual void InitializeParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* p) override
	{
		FSsParticleElementRotation* source = static_cast<FSsParticleElementRotation*>(ele);

		p->Rotation = VarianceCalc(e, source->Rotation.GetMinValue(), source->Rotation.GetMaxValue());
		p->RotationAdd =  VarianceCalc(e, source->RotationAdd.GetMinValue(), source->RotationAdd.GetMaxValue());
		p->RotationAddDst = p->RotationAdd;
	}
};
static FFuncParticleElementRotation FuncRotation;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FFuncParticleElementRotationTrans : public FEffectFuncBase
{
public:
	virtual void InitializeParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* p) override
	{
		FSsParticleElementRotationTrans* source = static_cast<FSsParticleElementRotationTrans*>(ele);
		if(p->Lifetime == 0) { return; }
		if(source->EndLifeTimePer == 0)
		{
			p->RotationAddDst = p->RotationAdd * source->RotationFactor;
			p->RotationAddOrg = p->RotationAdd;
			return;
		}
		p->RotationAddDst = p->RotationAdd * source->RotationFactor;
		p->RotationAddOrg = p->RotationAdd;
	}
	virtual void UpdateParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* p) override
	{
		FSsParticleElementRotationTrans* source = static_cast<FSsParticleElementRotationTrans*>(ele);

		if((p->Lifetime*source->EndLifeTimePer) == 0)
		{
			p->RotationAdd = BlendFloat(p->RotationAddOrg, p->RotationAddDst, 1.0f);
			return;
		}
		float per = ((float)p->ExsitTime / ((float)p->Lifetime*( source->EndLifeTimePer / 100.0f)));

		if(per > 1.0f) { per = 1.0f; }

		p->RotationAdd = BlendFloat(p->RotationAddOrg, p->RotationAddDst, per);
	}
};
static FFuncParticleElementRotationTrans FuncRotationTrans ;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FFuncParticleElementTransSpeed : public FEffectFuncBase
{
public:
	virtual void InitializeParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* p) override
	{
		FSsParticleElementTransSpeed* source = static_cast<FSsParticleElementTransSpeed*>(ele);
		p->Lastspeed = VarianceCalc(e, source->Speed.GetMinValue(), source->Speed.GetMaxValue());
	}

	virtual void UpdateParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* p) override
	{
		float per = ((float)p->ExsitTime / (float)p->Lifetime);
		p->Speed = (p->Firstspeed + (p->Lastspeed - p->Firstspeed ) * per);
	}
};
static FFuncParticleElementTransSpeed FuncTransSpeed;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FFuncParticleElementTangentialAcceleration : public FEffectFuncBase
{
public:
	virtual void InitializeParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* p) override
	{
		FSsParticleElementTangentialAcceleration* source = static_cast<FSsParticleElementTangentialAcceleration*>(ele);
		p->TangentialAccel = VarianceCalc(e , source->Acceleration.GetMinValue(), source->Acceleration.GetMaxValue());
	}
};
static FFuncParticleElementTangentialAcceleration FuncTangentialAcceleration;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FFuncParticleElementInitColor : public FEffectFuncBase
{
public:
	virtual void InitializeParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* p) override
	{
		FSsParticleElementInitColor* source = static_cast<FSsParticleElementInitColor*>(ele);
		VarianceCalcColor(e , p->Startcolor, source->Color.GetMinValue(), source->Color.GetMaxValue());
		p->Color = p->Startcolor;
		p->bUseColor = true;
	}
};
static FFuncParticleElementInitColor FuncInitColor;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FFuncParticleElementTransColor : public FEffectFuncBase
{
public:
	virtual void InitializeParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* p) override
	{
		FSsParticleElementTransColor* source = static_cast<FSsParticleElementTransColor*>(ele);
		VarianceCalcColor(e , p->Endcolor, source->Color.GetMinValue(), source->Color.GetMaxValue());
		p->bUseColor = true;
	}

	virtual void UpdateParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* p) override
	{
		float per = ((float)p->ExsitTime / (float)p->Lifetime);

		if(per > 1.0f) { per = 1.0f; }

		p->Color.A = BlendNumber(p->Startcolor.A, p->Endcolor.A, per);
		p->Color.R = BlendNumber(p->Startcolor.R, p->Endcolor.R, per);
		p->Color.G = BlendNumber(p->Startcolor.G, p->Endcolor.G, per);
		p->Color.B = BlendNumber(p->Startcolor.B, p->Endcolor.B, per);
		p->bUseColor = true;
	}
};
static FFuncParticleElementTransColor FuncTransColor;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FFuncParticleElementAlphaFade : public FEffectFuncBase
{
public:
	virtual void UpdateParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* particle) override
	{
		FSsParticleElementAlphaFade* source = static_cast<FSsParticleElementAlphaFade*>(ele);
	
		if(particle->Lifetime == 0) { return; }

		float per = ((float)particle->ExsitTime / (float)particle->Lifetime) * 100.0f;

		float start = source->Disprange.GetMinValue();
		float end = source->Disprange.GetMaxValue();

		if(per < start)
		{
			float alpha = (start - per) / start;
			particle->Color.A *= 1.0f - alpha;
			return;
		}

		if(per > end)
		{
			if(end >= 100.0f)
			{
				particle->Color.A = 0;
				return;
			}
			float alpha = (per-end) / (100.0f-end);
			particle->Color.A *= 1.0f - alpha;
			return;
		}
	}
};
static FFuncParticleElementAlphaFade FuncAlphaFade;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FFuncParticleElementSize : public FEffectFuncBase
{
public:
	virtual void InitializeParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* p) override
	{
		FSsParticleElementSize* source = static_cast<FSsParticleElementSize*>(ele);

		p->Size.X = VarianceCalc(e, source->SizeX.GetMinValue(), source->SizeX.GetMaxValue());
		p->Size.Y = VarianceCalc(e, source->SizeY.GetMinValue(), source->SizeY.GetMaxValue());
		float sf = VarianceCalc(e, source->ScaleFactor.GetMinValue(), source->ScaleFactor.GetMaxValue());

		p->Size = p->Size * sf;
		p->StartSize = p->Size;
	}
};
static FFuncParticleElementSize FuncSize;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FFuncParticleElementTransSize : public FEffectFuncBase
{
public:
	virtual void InitializeParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* p) override
	{
		FSsParticleElementTransSize* source = static_cast<FSsParticleElementTransSize*>(ele);
		FVector2D endsize;
		endsize.X = VarianceCalc(e, source->SizeX.GetMinValue(), source->SizeX.GetMaxValue());
		endsize.Y = VarianceCalc(e, source->SizeY.GetMinValue(), source->SizeY.GetMaxValue());

		float sf = VarianceCalc(e, source->ScaleFactor.GetMinValue(), source->ScaleFactor.GetMaxValue());

		endsize = endsize * sf;

		p->Divsize = (endsize - p->StartSize) / p->Lifetime;
	}
	virtual void UpdateParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* p) override
	{
		p->Size = p->StartSize + (p->Divsize * (p->ExsitTime));
	}
};
static FFuncParticleElementTransSize FuncTransSize;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FFuncParticlePointGravity : public FEffectFuncBase
{
public:
	virtual void UpdateParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* p) override
	{
		FSsParticlePointGravity* source = static_cast<FSsParticlePointGravity*>(ele);

		FVector2D Target;
		Target.X = source->Position.X + p->ParentEmitter->Position.X;
		Target.Y = source->Position.Y + p->ParentEmitter->Position.Y;

		//現在地点から指定された点に対してのベクトル*パワーを与える
		FVector2D v2 = Target - p->ParticlePosition;
		FVector2D v2_temp = v2;

		v2.Normalize();
		v2 = v2 * source->Power;

		p->Gravity = p->Gravity + v2;
	}
};
static FFuncParticlePointGravity FuncPointGravity;

//-----------------------------------------------------------------
//
//-----------------------------------------------------------------
class FFuncParticleTurnToDirectionEnabled : public FEffectFuncBase
{
public:
	virtual void InitializeParticle(FSsEffectElementBase* ele, FSsEffectRenderEmitter* e, FSsEffectRenderParticle* particle) override
	{
		particle->bIsTurnDirection = true;
	}
};
static FFuncParticleTurnToDirectionEnabled FuncTurnToDirectionEnabled;

//-------------------------------------------------------------------
//挙動反映クラスの呼び出しテーブル
//SsEffectFunctionTypeの順に並べること
//-------------------------------------------------------------------
static FEffectFuncBase* callTable[] =
{
	nullptr,
	&FuncBasic,
	&FuncRndSeedChange,
	&FuncDelay,
	&FuncGravity,
	&FuncPosition,
	&FuncRotation,
	&FuncRotationTrans,
	&FuncTransSpeed,
	&FuncTangentialAcceleration,
	&FuncInitColor,
	&FuncTransColor,
	&FuncAlphaFade,
	&FuncSize,
	&FuncTransSize,
	&FuncPointGravity,
	&FuncTurnToDirectionEnabled,
};


///----------------------------------------------------------------------------------------------------
//
///----------------------------------------------------------------------------------------------------
void FSsEffectFunctionExecuter::Initalize(FSsEffectBehavior* beh, FSsEffectRenderEmitter* emmiter)
{
	for(int32 i = 0; i < beh->PList.Num(); ++i)
	{
		FEffectFuncBase* cf = callTable[beh->PList[i]->MyType];
		cf->InitalizeEmmiter(beh->PList[i].Get() , emmiter);
	}
}

void FSsEffectFunctionExecuter::UpdateEmmiter(FSsEffectBehavior* beh, FSsEffectRenderEmitter* emmiter)
{
	for(int32 i = 0; i < beh->PList.Num(); ++i)
	{
		FEffectFuncBase* cf = callTable[beh->PList[i]->MyType];
		cf->UpdateEmmiter(beh->PList[i].Get(), emmiter);
	}
}

void FSsEffectFunctionExecuter::InitializeParticle(FSsEffectBehavior* beh, FSsEffectRenderEmitter* emmiter, FSsEffectRenderParticle* particle)
{
	for(int32 i = 0; i < beh->PList.Num(); ++i)
	{
		FEffectFuncBase* cf = callTable[beh->PList[i]->MyType];
		cf->InitializeParticle(beh->PList[i].Get(), emmiter, particle);
	}
}

void FSsEffectFunctionExecuter::UpdateParticle(FSsEffectBehavior* beh, FSsEffectRenderEmitter* emmiter, FSsEffectRenderParticle* particle)
{
	for(int32 i = 0; i < beh->PList.Num(); ++i)
	{
		FEffectFuncBase* cf = callTable[beh->PList[i]->MyType];
		cf->UpdateParticle(beh->PList[i].Get(), emmiter, particle);
	}
}

