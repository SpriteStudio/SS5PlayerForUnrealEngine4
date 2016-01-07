#include "SpriteStudio5PrivatePCH.h"
#include "SsPlayerEffect.h"

#include "SsEffectFile.h"
#include "SsCellmap.h"
#include "SsPlayerAnimedecode.h"
#include "SsPlayerEffectFunction.h"
#include "SsPlayerPartState.h"
#include "SsPlayerMatrix.h"


namespace
{
	static int32 seed_table[] =
	{
		485, 583, 814, 907, 1311, 1901, 2236, 3051, 3676, 4338,
		4671, 4775, 4928, 4960, 5228, 5591, 5755, 5825, 5885, 5967, 6014, 6056,
		6399, 6938, 7553, 8280, 8510, 8641, 8893, 9043, 31043,
	};

	bool ParticleDelete(FSsEffectRenderAtom* d)
	{
		if(d->bIsInit)
		{
			if(d->bIsLive == false)
			{
				return true;
			}
			if(d->Life <= 0.0f)
			{
				d->bIsLive = false;
				return true;
			}
		}

		return false;
	}
	bool ParticleDeleteAll(FSsEffectRenderAtom* d)
	{
		delete d;
		return true;
	}


	float GetAngleUnit(const FVector2D& v0, const FVector2D& v1)
	{
		float ip = FVector2D::DotProduct(v0, v1);
		if (ip > 1.0f){ ip = 1.0f; }
		if (ip < -1.0f){ ip = -1.0f; }
		float f = FMath::Acos(ip);
		return f;
	}
	float GetAngle360Unit(const FVector2D& v0, const FVector2D& v1)
	{
		float ang = GetAngleUnit(v0, v1);
		float c = FVector2D::CrossProduct(v0, v1);

		if (c < 0)
		{
			ang = (3.1415926535897932385f)*2.0f - ang;
		}
		return ang;
	}
	float GetAngle360(const FVector2D& v0, const FVector2D& v1)
	{
		FVector2D uv0(v0), uv1(v1);
		uv0.Normalize();
		uv1.Normalize();
		return GetAngle360Unit(uv0, uv1);
	}

	bool ComparePriority(FSsEffectDrawBatch* left, FSsEffectDrawBatch* right)
	{
		return left->Priority < right->Priority ;
	}
}

//------------------------------------------------------------------------------
//	ユーティリティ
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//要素生成関数
//------------------------------------------------------------------------------
FSsEffectRenderAtom* FSsEffectRenderer::CreateAtom(uint32 seed, FSsEffectRenderAtom* parent, FSsEffectNode* node)
{
	FSsEffectRenderAtom* ret = nullptr;
	TEnumAsByte<SsEffectNodeType::Type> type = node->GetType(); 

	if(type == SsEffectNodeType::Particle)
	{
		if(SSEFFECTRENDER_PARTICLE_MAX <= ParticlePoolCount)
		{
			 return nullptr;
		}
		FSsEffectRenderParticle* p = &ParticlePool[ParticlePoolCount];
		p->InitParameter();
		ParticlePoolCount++;

		p->Data = node;
		p->Parent = parent;

		UpdateList.Add( p );
		FSsEffectRenderEmitter* em = (FSsEffectRenderEmitter*)parent;
		em->MyBatchList->DrawList.Add( p );

		ret = p;
	}
	else if(type == SsEffectNodeType::Emmiter)
	{
		if(SSEFFECTRENDER_EMMITER_MAX <= EmitterPoolCount)
		{
			return nullptr;
		}
		if(SSEFFECTRENDER_BACTH_MAX <= DprPoolCount)
		{
			 return nullptr;
		}
		FSsEffectRenderEmitter* p = &EmitterPool[EmitterPoolCount];

		p->InitParameter();
		EmitterPoolCount++;

		p->Data = node;
		p->Parent = parent;

		p->SetMySeed(seed);
		p->TrushRandom(EmitterPoolCount%9);

		FSsEffectFunctionExecuter::Initalize(&p->Data->Behavior, p);

		//セルデータの検索とセット
		//オリジナルでは上記initializeでやっているがクラス階層の関係からこちらでやる
		FSsCelMapLinker* link = p->Data->Behavior.CellMapName.IsNone() ? nullptr : this->CurCellMapManager->GetCellMapLink(p->Data->Behavior.CellMapName);
		if(link)
		{
			FSsCell* cell = link->FindCell(p->Data->Behavior.CellName);
		
			GetCellValue(
				this->CurCellMapManager,
				p->Data->Behavior.CellMapName,
				p->Data->Behavior.CellName,
				p->DispCell
				);
		}
		else
		{
			UE_LOG(LogSpriteStudio, Warning, TEXT("cell not found : %s , %s"),
				*(p->Data->Behavior.CellMapName.ToString()),
				*(p->Data->Behavior.CellName.ToString())
				);
		}

		UpdateList.Add( p );

		//バッチリストを調べる
		FSsEffectDrawBatch* bl = &(DprPool[DprPoolCount]);
		DprPoolCount++;
		bl->TargetNode = node;
		p->MyBatchList = bl;
		DrawBatchList.Add(bl);
		ret = p;
	}
	return ret;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void FSsEffectRenderEmitter::SetMySeed(uint32 seed)
{
	if(seed > 31){
		this->MT->init_genrand( seed );

	}else{
		this->MT->init_genrand( seed_table[seed] );
	}
	MySeed = seed;
}


//----------------------------------------------------------------------
//生成フェーズ           SsEffectRendererへ移動してもいいかも
//----------------------------------------------------------------------
void FSsEffectRenderEmitter::Initialize()
{
	//子要素を解析(一度だけ）
	if(!bIsInit)
	{
		FSsEffectNode* n = static_cast<FSsEffectNode*>(this->Data->Ctop);
		while(n)
		{
			if(n->GetType() == SsEffectNodeType::Particle)
			{
				ParamParticle = n;
			}
			n = static_cast<FSsEffectNode*>(n->Next);
		}

		if(this->Data->GetMyBehavior())
		{
			FSsEffectFunctionExecuter::Initalize(this->Data->GetMyBehavior() , this);
		}
		Intervalleft = this->Interval;
	}

	bIsInit = true;
}

//----------------------------------------------------------------------
//パーティクルオブジェクトの生成
//----------------------------------------------------------------------
bool FSsEffectRenderEmitter::Genarate(FSsEffectRenderer* render)
{
	if(!GenerateOK){ return true; }
	if(bIsLive == false){ return true; }

	int32 create_count = this->Burst;
	if(create_count <= 0){ create_count = 1; }

	int32 pc = ParticleCount;
	while(1)
	{
		if(this->Intervalleft >= this->Interval)
		{
			if(ParamParticle)
			{
				for(int32 i = 0; i < create_count; i++)//最大作成数
				{
					if(pc < MaxParticle)
					{
						FSsEffectRenderAtom* a = render->CreateAtom(0, this, ParamParticle);
						if(a)
						{
							a->Initialize();
							a->Update(render->FrameDelta);
							if(!a->Genarate(render))
							{
								return false;
							}
							pc++;
						}
						else
						{
							return false;
						}
					}
					else
					{
						break;
					}
				}
			}
			this->Intervalleft -= this->Interval;
			//if(this->Interval == 0){ return true; }
		}
		else
		{
			return true;
		}
	}
	return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void FSsEffectRenderEmitter::Update(float delta)
{
	ExsitTime += delta;
	Life = Lifetime - ExsitTime;
	Intervalleft += delta;

	if(this->Parent)
	{
		//以下は仮
		this->Position = this->Parent->Position;
		this->Rotation = this->Parent->Rotation;
		this->Scale = this->Parent->Scale;
		this->Alpha = this->Parent->Alpha;
	}
	if(this->Data->GetMyBehavior())
	{
		FSsEffectFunctionExecuter::UpdateEmmiter(this->Data->GetMyBehavior(), this);
	}

	if(this->MyBatchList)
	{
		this->MyBatchList->Priority = this->DrawPriority;
		this->MyBatchList->DispCell = &this->DispCell;
		this->MyBatchList->BlendType = this->Data->GetMyBehavior()->BlendType;
	}
}


//----------------------------------------------------------------------
//パーティクルクラス
//----------------------------------------------------------------------
//生成フェーズ
void FSsEffectRenderParticle::Initialize()
{
	if(!bIsInit)
	{
		FSsEffectNode* n = static_cast<FSsEffectNode*>(this->Data->Ctop);

		//子要素を解析  基本的にエミッターのみの生成のはず　（Ｐではエラーでいい）
		//処理を省いてエミッター生成のつもりで作成する
		//パーティクルに紐づいたエミッターが生成される
		ParentEmitter = nullptr;

		ParentEmitter = static_cast<FSsEffectRenderEmitter*>(this->Parent);

		DispCell = &ParentEmitter->DispCell;
		if(ParentEmitter->Data == nullptr)
		{
			this->Life = 0.0f;
			bIsInit = false;
			return;
		}

		this->RefBehavior = ParentEmitter->Data->GetMyBehavior();
		if(RefBehavior)
		{
			FSsEffectFunctionExecuter::InitializeParticle(RefBehavior, ParentEmitter, this);
		}
	}

	bIsInit = true;
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool FSsEffectRenderParticle::Genarate(FSsEffectRenderer* render)
{
	FSsEffectNode* n = static_cast<FSsEffectNode*>(this->Data->Ctop);
	if(bIsInit && !bIsCreateChild)
	{
		if(ParentEmitter)
		{
			while(n)
			{
				if(ParentEmitter == NULL){ return true; }
				FSsEffectRenderAtom* r = render->CreateAtom(ParentEmitter->MySeed, this, n);
				if(r)
				{
					n = static_cast<FSsEffectNode*>(n->Next);
					r->Initialize();
					r->Update(render->FrameDelta);
					r->Genarate(render);
				}
				else
				{
					return false;
				}
			}
		}
		bIsCreateChild = true;
	}
	return true;
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void FSsEffectRenderParticle::Update(float delta)
{
	if(!this->IsInit()){ return; }
	this->Position.X = this->ParticlePosition.X;
	this->Position.Y = this->ParticlePosition.Y;
	this->Scale = this->Parent->Scale;
	this->Alpha = this->Parent->Alpha;

	 //初期値突っ込んでおく、パーティクルメソッドのアップデートに持ってく？
	 this->Color = this->Startcolor;

	if(ParentEmitter)
	{
		UpdateDelta(delta);

		if(RefBehavior)
		{
			FSsEffectFunctionExecuter::UpdateParticle(RefBehavior, ParentEmitter, this);
		}

		UpdateForce(delta);

		if(Parent->Life > 0.0f)
		{
			this->Position.X = this->ParticlePosition.X;
			this->Position.Y = this->ParticlePosition.Y;
		}
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void FSsEffectRenderParticle::UpdateDelta(float delta)
{
	Rotation += (RotationAdd * delta);

	ExsitTime += delta;
	Life = Lifetime - ExsitTime;

	FVector2D tangential = FVector2D::ZeroVector;

	//接線加速度の計算
	FVector2D radial(this->ParticlePosition.X, this->ParticlePosition.Y);

	radial.Normalize();
	tangential = radial;

	radial = radial * RadialAccel;

	float newY = tangential.X;
	tangential.X = -tangential.Y;
	tangential.Y = newY;

	tangential = tangential* TangentialAccel;

	FVector2D tmp = radial + tangential;

	this->Execforce = tmp;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void  FSsEffectRenderParticle::UpdateForce(float delta)
{
	this->Backposition = this->ParticlePosition;

	this->Force = Gravity;
	FVector2D ff = (this->Vector * this->Speed) + this->Execforce + this->Force;


	if(bIsTurnDirection)
	{
		//this->Direction = SsPoint2::get_angle_360( SsVector2( 1.0f , 0.0f ) , ff ) - (float)DegreeToRadian(90);
		this->Direction = GetAngle360(FVector2D(1.0f , 0.0f), ff) - (float)FMath::DegreesToRadians(90.f);
	}
	else
	{
		this->Direction = 0;
	}

	//フォースを加算
	this->ParticlePosition.X += (ff.X * delta );
	this->ParticlePosition.Y += (ff.Y * delta );
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool FSsEffectRenderParticle::CreateRenderVertices(FSsEffectRenderer* render, FSsRenderVertex* OutRenderVertices, const FVector2D& CurAnimeCanvasSize, const FVector2D& CurAnimePivot)
{
	if(this->ParentEmitter == NULL) { return false; }
	if(RefBehavior == NULL) { return false; }
	if(DispCell == NULL) { return false; }
	if(DispCell->Cell == NULL) { return false; }
	if(DispCell->Texture == NULL) { return false; }

	float matrix[4 * 4];	///< 行列
	IdentityMatrix( matrix );


	if(render->ParentState)
	{
		memcpy(matrix, render->ParentState->Matrix, sizeof(float) * 16);
		this->Alpha = render->RenderRoot->Alpha;
	}

	if ((bUseColor && (0 == Color.A)) || (0.f == this->Alpha))
	{
		return false;
	}

	TranslationMatrixM(matrix, ParticlePosition.X, ParticlePosition.Y, 0.0f);

	RotationXYZMatrixM(matrix, 0, 0, FMath::DegreesToRadians(Rotation) + Direction);

	ScaleMatrixM(matrix, Size.X, Size.Y, 1.0f);

	SsFColor fcolor;
	fcolor.FromARGB(Color.ToARGB());
	fcolor.A = fcolor.A * this->Alpha;


	FVector2D pivot = FVector2D(DispCell->Cell->Pivot.X, DispCell->Cell->Pivot.Y);
	pivot.X = pivot.X * DispCell->Cell->Size.X;
	pivot.Y = pivot.Y * DispCell->Cell->Size.Y;

	FVector2D dispscale = DispCell->Cell->Size;

	// RenderTargetに対する描画基準位置
	float OffX = (float)(CurAnimeCanvasSize.X /2) + ((pivot.X + CurAnimePivot.X) * CurAnimeCanvasSize.X);
	float OffY = (float)(CurAnimeCanvasSize.Y /2) - ((pivot.Y + CurAnimePivot.Y) * CurAnimeCanvasSize.Y);

	// 頂点座標
	FMatrix ViewMatrix(
		FVector(matrix[ 0], matrix[ 1], matrix[ 2]),
		FVector(matrix[ 4], matrix[ 5], matrix[ 6]),
		FVector(matrix[ 8], matrix[ 9], matrix[10]),
		FVector(matrix[12], matrix[13], matrix[14])
		);
	FVector Vertices[4] =
		{
			FVector(-(dispscale.X / 2.f),  (dispscale.Y / 2.f), 0.f),
			FVector( (dispscale.X / 2.f),  (dispscale.Y / 2.f), 0.f),
			FVector(-(dispscale.X / 2.f), -(dispscale.Y / 2.f), 0.f),
			FVector( (dispscale.X / 2.f), -(dispscale.Y / 2.f), 0.f),
		};

	for(int32 i = 0; i < 4; ++i)
	{
		FVector4 V = ViewMatrix.TransformPosition(Vertices[i]);
		OutRenderVertices[i].Position.X = ( V.X + OffX) / CurAnimeCanvasSize.X;
		OutRenderVertices[i].Position.Y = (-V.Y + OffY) / CurAnimeCanvasSize.Y;

		OutRenderVertices[i].TexCoord = DispCell->Uvs[i];
		OutRenderVertices[i].Color = FColor(Color.R, Color.G, Color.B, (uint8)(Color.A * this->Alpha));
		OutRenderVertices[i].ColorBlendRate = bUseColor ? 1.f : 0.f;
	}

	return true;
}

//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
void FSsEffectRenderer::Update(float delta)
{
	if(bIsPause) { return; }
	if(!bIsPlay) { return; }
	if(this->RenderRoot == nullptr) { return; }
	bFirstUpdated = true;

	FrameDelta = delta;

	if(ParentState)
	{
		FVector pos = FVector(
			ParentState->Matrix[3*4],
			ParentState->Matrix[3*4+1],
			ParentState->Matrix[3*4+2]
			);
		LayoutPosition = pos;

		this->RenderRoot->SetPosition(0, 0, 0);

		this->RenderRoot->Rotation = 0;
		this->RenderRoot->Scale = FVector2D(1.0f,1.0f);
		this->RenderRoot->Alpha = ParentState->Alpha;
	}

	int32 loopnum = UpdateList.Num();
	for(int32 i = 0; i < loopnum; i++)
	{
		FSsEffectRenderAtom* re = UpdateList[i];
		re->Initialize();
		re->Count();
	}

	loopnum = UpdateList.Num();
	int32 updatecount = 0;
	for(int32 i = 0; i < loopnum; i++)
	{
		FSsEffectRenderAtom* re = UpdateList[i];

		if(re->bIsLive == false) { continue; }

		if((re->Parent && re->Parent->Life <= 0.0f) || re->Life <= 0.0f)
		{
			re->Update(delta);
		}
		else
		{
			re->Update(delta);
			re->Genarate(this);
		}

		updatecount++;
	}

	//後処理  寿命で削除
	//死亡検出、削除の2段階
	for(int32 i = 0; i < UpdateList.Num(); ++i)
	{
		if(!UpdateList[i]->bIsInit)
		{
			continue;
		}
		if((!UpdateList[i]->bIsLive) || (UpdateList[i]->Life <= 0.f))
		{
			UpdateList[i]->bIsLive = false;
			UpdateList.RemoveAt(i);
			--i;
		}
	}

	for (int32 i = 0; i < DrawBatchList.Num(); ++i)
	{
		for (int32 j = DrawBatchList[i]->DrawList.Num() - 1; 0 <= j; --j)
		{
			if (DrawBatchList[i]->DrawList[j]
				&& DrawBatchList[i]->DrawList[j]->bIsLive
				&& DrawBatchList[i]->DrawList[j]->Life > 0.0f
				)
			{
			}
			else
			{
				DrawBatchList[i]->DrawList.RemoveAt(j);
			}
		}
	}

	DrawBatchList.Sort();


	if(bIsLoop)
	{
		if(updatecount== 0)
		{
			Reload();
		}
	}
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void FSsEffectRenderer::CreateRenderParts(TArray<FSsRenderPart>& OutRenderParts, FSsPartState* State, const FVector2D& CurAnimeCanvasSize, const FVector2D& CurAnimePivot)
{
	FSsRenderPart RenderPart;
	RenderPart.PartIndex = State->Index;
	RenderPart.ColorBlendType = SsBlendType::Effect;

	for(int32 i = 0; i < DrawBatchList.Num(); ++i)
	{
		if(nullptr == DrawBatchList[i]->DispCell)
		{
			continue;
		}

		RenderPart.Texture = DrawBatchList[i]->DispCell->Texture;
		RenderPart.AlphaBlendType = SsRenderBlendTypeToBlendType(DrawBatchList[i]->BlendType);

		for(int32 j = 0; j < DrawBatchList[i]->DrawList.Num(); ++j)
		{
			if(    DrawBatchList[i]->DrawList[j]
				&& DrawBatchList[i]->DrawList[j]->bIsLive
				&& DrawBatchList[i]->DrawList[j]->Life > 0.0f
				)
			{
				if(DrawBatchList[i]->DrawList[j]->CreateRenderVertices(this, RenderPart.Vertices, CurAnimeCanvasSize, CurAnimePivot))
				{
					OutRenderParts.Add(RenderPart);
				}
			}
		}
	}
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
FSsEffectRenderer::~FSsEffectRenderer()
{
	ClearUpdateList();

	if(nullptr != RenderRoot)
	{
		delete RenderRoot;
		RenderRoot = nullptr;
	}
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void FSsEffectRenderer::ClearUpdateList()
{
	EmitterPoolCount = 0;
	ParticlePoolCount  = 0;
	DprPoolCount = 0;

	UpdateList.Empty();

	for(int32 i = 0; i < DrawBatchList.Num(); ++i)
	{
		DrawBatchList[i]->DrawList.Empty();
	}

	DrawBatchList.Empty();
	bFirstUpdated = false;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

void FSsEffectRenderer::Reload()
{
	ClearUpdateList();

	//座標操作のためのルートノードを作成する
	if(RenderRoot == nullptr)
	{
		RenderRoot = new FSsEffectRenderAtom();
	}

	//ルートの子要素を調査して作成する
	FSsEffectNode* root = this->EffectData->Root;

	//シード値の決定
	uint32 seed = 0;

	if(this->EffectData->IsLockRandSeed)
	{
		seed = this->EffectData->LockRandSeed;
	}
	else
	{
		seed = MySeed;
	}
	FSsSimpleTree* n = root->Ctop;

	//子要素だけつくってこれを種にする
	while(n)
	{
		FSsEffectNode* enode = static_cast<FSsEffectNode*>(n);
		CreateAtom(seed, RenderRoot, enode);

		n = n->Next;
	}
}

void FSsEffectRenderer::Play()
{
	bIsPlay = true;
	bIsPause = false;
}
	
void FSsEffectRenderer::Stop()
{
	bIsPlay = false;
}
	
void FSsEffectRenderer::Pause()
{
	bIsPause = true;
}

void FSsEffectRenderer::SetLoop(bool flag)
{
	bIsLoop = flag;
}

//再生ステータスを取得
bool FSsEffectRenderer::GetPlayStatus(void)
{
	return bIsPlay;
}

int32 FSsEffectRenderer::GetCurrentFPS()
{
	if(EffectData)
	{
		if(EffectData->FPS == 0)
		{
			return 30;
		}

		return EffectData->FPS;
	}
	return 30;
}





