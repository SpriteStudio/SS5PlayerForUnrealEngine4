#include "SpriteStudio5PrivatePCH.h"
#include "SsRenderPartsProxy.h"

#include "RHIStaticStates.h"
#include "DynamicMeshBuilder.h"

#include "SsPlayerComponent.h"


namespace
{
	struct FSsPartVertex
	{
		FVector Position;
		FVector2D TexCoord;
		FColor Color;
		FVector2D ColorBlend;	// [1]:ColorBlendRate 
		FPackedNormal TangentX;
		FPackedNormal TangentZ;
	};

	static SsBlendType::Type TypeMix = SsBlendType::Mix;
	static SsBlendType::Type TypeMul = SsBlendType::Mul;
	static SsBlendType::Type TypeAdd = SsBlendType::Add;
	static SsBlendType::Type TypeSub = SsBlendType::Sub;
	void* GetBlendTypeAddr(SsBlendType::Type Type)
	{
		switch(Type)
		{
			case SsBlendType::Mix: { return &TypeMix; }
			case SsBlendType::Mul: { return &TypeMul; }
			case SsBlendType::Add: { return &TypeAdd; }
			case SsBlendType::Sub: { return &TypeSub; }
		}
		return NULL;
	}
	SsBlendType::Type GetBlendTypeFromAddr(const void* Addr)
	{
		if(Addr == &TypeMul){ return SsBlendType::Mul; }
		if(Addr == &TypeAdd){ return SsBlendType::Add; }
		if(Addr == &TypeSub){ return SsBlendType::Sub; }
		return SsBlendType::Mix;
	}
};

//
// IndexBuffer 
//
void FSsPartsIndexBuffer::InitRHI()
{
	if(0 < NumIndices)
	{
		FRHIResourceCreateInfo CreateInfo;
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(uint32), NumIndices * sizeof(uint32), BUF_Dynamic, CreateInfo);

		void* IndicesPtr = RHILockIndexBuffer(IndexBufferRHI, 0, NumIndices * sizeof(uint32), RLM_WriteOnly);
		for(uint32 i = 0; i < NumIndices/6; ++i)
		{
			((uint32*)IndicesPtr)[i*6+0] = i*4+0;
			((uint32*)IndicesPtr)[i*6+1] = i*4+1;
			((uint32*)IndicesPtr)[i*6+2] = i*4+3;
			((uint32*)IndicesPtr)[i*6+3] = i*4+0;
			((uint32*)IndicesPtr)[i*6+4] = i*4+3;
			((uint32*)IndicesPtr)[i*6+5] = i*4+2;
		}
		RHIUnlockIndexBuffer(IndexBufferRHI);
	}
}

//
// VertexFactory 
//
IMPLEMENT_VERTEX_FACTORY_TYPE(FSsPartsVertexFactory, "/Engine/Private/LocalVertexFactory.ush", true, true, true, true, true);

FVertexFactoryShaderParameters* FSsPartsVertexFactory::ConstructShaderParameters(EShaderFrequency ShaderFrequency)
{
	return (ShaderFrequency == SF_Pixel)
		? (FVertexFactoryShaderParameters*)new FSsPartVertexFactoryShaderParameters()
		: FLocalVertexFactory::ConstructShaderParameters(ShaderFrequency);
}

//
// VertexFactoryShaderParameters
//
void FSsPartVertexFactoryShaderParameters::SetMesh(FRHICommandList& RHICmdList, FShader* Shader,const class FVertexFactory* VertexFactory,const class FSceneView& View,const struct FMeshBatchElement& BatchElement,uint32 DataFlags) const
{
	SsBlendType::Type AlphaBlendType = GetBlendTypeFromAddr(BatchElement.UserData);
	switch(AlphaBlendType)
	{
		case SsBlendType::Mix:
			{
			} break;
		case SsBlendType::Mul:
			{
				RHICmdList.GetContext().RHISetBlendState(
					TStaticBlendState<
						CW_RGBA,
						BO_Add, BF_Zero, BF_SourceColor,
						BO_Add, BF_InverseSourceAlpha, BF_One
						>::GetRHI(),
					FLinearColor::White
					);
			} break;
		case SsBlendType::Add:
			{
				RHICmdList.GetContext().RHISetBlendState(
					TStaticBlendState<
						CW_RGBA,
						BO_Add, BF_SourceAlpha, BF_One,
						BO_Add, BF_SourceAlpha, BF_One
						>::GetRHI(),
					FLinearColor::White
					);
			} break;
		case SsBlendType::Sub:
			{
				RHICmdList.GetContext().RHISetBlendState(
					TStaticBlendState<
						CW_RGBA,
						BO_ReverseSubtract, BF_SourceAlpha, BF_One,
						BO_Add, BF_Zero, BF_DestAlpha
						>::GetRHI(),
					FLinearColor::White
					);
			} break;
	}
}


// コンストラクタ
FSsRenderPartsProxy::FSsRenderPartsProxy(USsPlayerComponent* InComponent, uint32 InMaxPartsNum)
	: FPrimitiveSceneProxy(InComponent)
	, CanvasSizeUU(100.f, 100.f)
	, Pivot(0.f, 0.f)
	, MaxPartsNum(InMaxPartsNum)
	, VertexFactory(GetScene().GetFeatureLevel(), "FSsRenderPartsProxy")
{
	// FPrimitiveSceneProxy
	bWillEverBeLit = false;

	Component = InComponent;
	bVerifyUsedMaterials = false;
}

// デストラクタ
FSsRenderPartsProxy::~FSsRenderPartsProxy()
{
	VertexBuffers.PositionVertexBuffer.ReleaseResource();
	VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
	VertexBuffers.ColorVertexBuffer.ReleaseResource();
	IndexBuffer.ReleaseResource();
	VertexFactory.ReleaseResource();
}

SIZE_T FSsRenderPartsProxy::GetTypeHash() const
{
	static size_t UniquePointer;
	return reinterpret_cast<size_t>(&UniquePointer);
}

void FSsRenderPartsProxy::CreateRenderThreadResources()
{
	VertexBuffers.InitWithDummyData(&VertexFactory, MaxPartsNum * 4, 2);
	IndexBuffer.NumIndices = MaxPartsNum * 6;
	IndexBuffer.InitResource();
}

void FSsRenderPartsProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	QUICK_SCOPE_CYCLE_COUNTER( STAT_SsRenderSceneProxy_GetDynamicMeshElements );

	const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;
	FMaterialRenderProxy* MaterialProxy = NULL;
	if(bWireframe)
	{
		auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
			FLinearColor(0, 0.5f, 1.f)
			);
		Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);
		MaterialProxy = WireframeMaterialInstance;
	}

	for(int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if(VisibilityMap & (1 << ViewIndex))
		{
			// マテリアルが一致しているパーツ毎に描画 
			uint32 StartPartIndex = 0;
			uint32 NumParts = 1;
			for(int32 i = 0; i < RenderParts.Num(); ++i)
			{
				// 次パーツが同時に描画出来るか 
				if(    (i != (RenderParts.Num()-1))											// 最後の１つでない 
					&& (RenderParts[i].AlphaBlendType == RenderParts[i+1].AlphaBlendType)	// アルファブレンドモード
					&& (RenderParts[i].Material == RenderParts[i+1].Material)				// マテリアルが一致(参照セル/カラーブレンド毎にマテリアルが別れる) 
					)
				{
					++NumParts;
					continue;
				}

				const FSceneView* View = Views[ViewIndex];

				if(!bWireframe)
				{
					if(NULL == RenderParts[i].Material)
					{
						continue;
					}
					MaterialProxy = RenderParts[i].Material->GetRenderProxy(IsSelected());
				}

				// Draw the mesh.
				FMeshBatch& Mesh = Collector.AllocateMesh();
				Mesh.VertexFactory              = &VertexFactory;
				Mesh.MaterialRenderProxy        = MaterialProxy;
				Mesh.ReverseCulling             = IsLocalToWorldDeterminantNegative();
				Mesh.CastShadow                 = true;
				Mesh.DepthPriorityGroup         = SDPG_World;
				Mesh.Type                       = PT_TriangleList;
				Mesh.bDisableBackfaceCulling    = true;
				Mesh.bCanApplyViewModeOverrides = false;

				FMeshBatchElement& BatchElement = Mesh.Elements[0];
				BatchElement.IndexBuffer    = &IndexBuffer;
				BatchElement.FirstIndex     = (StartPartIndex * 6);
				BatchElement.MinVertexIndex = (StartPartIndex * 4);
				BatchElement.MaxVertexIndex = ((StartPartIndex + NumParts) * 4) - 1;
				BatchElement.NumPrimitives  = (NumParts * 2);
				BatchElement.UserData       = GetBlendTypeAddr(RenderParts[i].AlphaBlendType);
				BatchElement.PrimitiveUniformBufferResource = &GetUniformBuffer();

				Collector.AddMesh(ViewIndex, Mesh);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
				// Render bounds
				RenderBounds(Collector.GetPDI(ViewIndex), ViewFamily.EngineShowFlags, GetBounds(), IsSelected());
#endif

				StartPartIndex = i + 1;
				NumParts = 1;
			}
		}
	}
}

FPrimitiveViewRelevance FSsRenderPartsProxy::GetViewRelevance(const FSceneView* View) const
{
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance                 = IsShown(View);
	Result.bRenderCustomDepth             = ShouldRenderCustomDepth();
	Result.bRenderInMainPass              = ShouldRenderInMainPass();
	Result.bUsesLightingChannels          = false;
	Result.bOpaqueRelevance               = true;
	Result.bSeparateTranslucencyRelevance = true;
	Result.bNormalTranslucencyRelevance   = false;
	Result.bShadowRelevance               = IsShadowCast(View);
	Result.bDynamicRelevance              = true;
	return Result;
}

uint32 FSsRenderPartsProxy::GetMemoryFootprint() const
{
	return sizeof(*this) + GetAllocatedSize();
}

void FSsRenderPartsProxy::SetDynamicData_RenderThread(const TArray<FSsRenderPartWithMaterial>& InRenderParts)
{
	RenderParts = InRenderParts;

	if(RenderParts.Num() <= 0)
	{
		return;
	}

	FVector TangentY;
	{
		FDynamicMeshVertex DummyVert(FVector::ZeroVector, FVector::ForwardVector, FVector::UpVector, FVector2D::ZeroVector, FColor::Black);
		TangentY = DummyVert.GetTangentY();
	}

	for(auto ItPart = RenderParts.CreateConstIterator(); ItPart; ++ItPart)
	{
		for(int32 i = 0; i < 4; ++i)
		{
			int32 Index = ItPart.GetIndex() * 4 + i;
			VertexBuffers.PositionVertexBuffer.VertexPosition(Index) = FVector(
					0.f,
					( ItPart->Vertices[i].Position.X - 0.5f - Pivot.X) * CanvasSizeUU.X,
					(-ItPart->Vertices[i].Position.Y + 0.5f - Pivot.Y) * CanvasSizeUU.Y
				);
			VertexBuffers.StaticMeshVertexBuffer.SetVertexTangents(Index, FVector::ForwardVector, TangentY, FVector::UpVector);
			VertexBuffers.StaticMeshVertexBuffer.SetVertexUV(Index, 0, ItPart->Vertices[i].TexCoord);
			VertexBuffers.StaticMeshVertexBuffer.SetVertexUV(Index, 1, FVector2D(0.f, ItPart->Vertices[i].ColorBlendRate));
			VertexBuffers.ColorVertexBuffer.VertexColor(Index) = ItPart->Vertices[i].Color;
		}
	}

	{
		auto& VertexBuffer = VertexBuffers.PositionVertexBuffer;
		void* VertexBufferData = RHILockVertexBuffer(VertexBuffer.VertexBufferRHI, 0, InRenderParts.Num() * 4 * VertexBuffer.GetStride(), RLM_WriteOnly);
		FMemory::Memcpy(VertexBufferData, VertexBuffer.GetVertexData(), InRenderParts.Num() * 4 * VertexBuffer.GetStride());
		RHIUnlockVertexBuffer(VertexBuffer.VertexBufferRHI);
	}

	{
		auto& VertexBuffer = VertexBuffers.ColorVertexBuffer;
		void* VertexBufferData = RHILockVertexBuffer(VertexBuffer.VertexBufferRHI, 0, InRenderParts.Num() * 4 * VertexBuffer.GetStride(), RLM_WriteOnly);
		FMemory::Memcpy(VertexBufferData, VertexBuffer.GetVertexData(), InRenderParts.Num() * 4 * VertexBuffer.GetStride());
		RHIUnlockVertexBuffer(VertexBuffer.VertexBufferRHI);
	}

	{
		auto& VertexBuffer = VertexBuffers.StaticMeshVertexBuffer;
		void* VertexBufferData = RHILockVertexBuffer(VertexBuffer.TangentsVertexBuffer.VertexBufferRHI, 0, VertexBuffer.GetTangentSize(), RLM_WriteOnly);
		FMemory::Memcpy(VertexBufferData, VertexBuffer.GetTangentData(), VertexBuffer.GetTangentSize());
		RHIUnlockVertexBuffer(VertexBuffer.TangentsVertexBuffer.VertexBufferRHI);
	}

	{
		auto& VertexBuffer = VertexBuffers.StaticMeshVertexBuffer;
		void* VertexBufferData = RHILockVertexBuffer(VertexBuffer.TexCoordVertexBuffer.VertexBufferRHI, 0, VertexBuffer.GetTexCoordSize(), RLM_WriteOnly);
		FMemory::Memcpy(VertexBufferData, VertexBuffer.GetTexCoordData(), VertexBuffer.GetTexCoordSize());
		RHIUnlockVertexBuffer(VertexBuffer.TexCoordVertexBuffer.VertexBufferRHI);
	}
}
