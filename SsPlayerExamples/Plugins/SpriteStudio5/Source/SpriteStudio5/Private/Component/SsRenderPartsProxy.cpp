﻿#include "SpriteStudio5PrivatePCH.h"
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
// VertexBuffer 
//
void FSsPartsVertexBuffer::InitRHI()
{
	if(0 < NumVerts)
	{
		FRHIResourceCreateInfo CreateInfo;
		VertexBufferRHI = RHICreateVertexBuffer(NumVerts * sizeof(FSsPartVertex), BUF_Dynamic, CreateInfo);
	}
}

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

bool FSsPartsVertexFactory::ShouldCache(EShaderPlatform Platform, const class FMaterial* Material, const class FShaderType* ShaderType)
{
	return (   Material->GetFriendlyName().StartsWith("WorldGridMaterial")
			|| Material->GetFriendlyName().StartsWith("WireframeMaterial")
			|| Material->GetFriendlyName().StartsWith("SsPart_")
			)
		&& FLocalVertexFactory::ShouldCache(Platform, Material, ShaderType);
}
void FSsPartsVertexFactory::ModifyCompilationEnvironment(EShaderPlatform Platform, const FMaterial* Material, FShaderCompilerEnvironment& OutEnvironment)
{
	FLocalVertexFactory::ModifyCompilationEnvironment(Platform, Material, OutEnvironment);
}
FVertexFactoryShaderParameters* FSsPartsVertexFactory::ConstructShaderParameters(EShaderFrequency ShaderFrequency)
{
	return (ShaderFrequency == SF_Pixel) ? new FSsPartVertexFactoryShaderParameters() : NULL;
}
void FSsPartsVertexFactory::Init(const FSsPartsVertexBuffer* VertexBuffer)
{
	if(IsInRenderingThread())
	{
		FLocalVertexFactory::FDataType NewData;
		NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FSsPartVertex, Position, VET_Float3);
		NewData.TextureCoordinates.Add(FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FSsPartVertex,TexCoord), sizeof(FSsPartVertex), VET_Float2));
		NewData.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FSsPartVertex, Color, VET_Color);
		NewData.TextureCoordinates.Add(FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FSsPartVertex,ColorBlend), sizeof(FSsPartVertex), VET_Float2));
		NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FSsPartVertex, TangentX, VET_PackedNormal);
		NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FSsPartVertex, TangentZ, VET_PackedNormal);
		SetData(NewData);
	}
	else
	{
		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			FInitSsPartsVertexFactory,
			FSsPartsVertexFactory*, VertexFactory, this,
			const FSsPartsVertexBuffer*, VertexBuffer, VertexBuffer,
		{
			FDataType NewData;
			NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FSsPartVertex, Position, VET_Float3);
			NewData.TextureCoordinates.Add(FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FSsPartVertex,TexCoord), sizeof(FSsPartVertex), VET_Float2));
			NewData.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FSsPartVertex, Color, VET_Color);
			NewData.TextureCoordinates.Add(FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FSsPartVertex,ColorBlend), sizeof(FSsPartVertex), VET_Float2));
			NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FSsPartVertex, TangentX, VET_PackedNormal);
			NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FSsPartVertex, TangentZ, VET_PackedNormal);
			VertexFactory->SetData(NewData);
		});
	}
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
{
	// FPrimitiveSceneProxy
	bWillEverBeLit = false;

	Component = InComponent;

	VertexBuffer.NumVerts  = 4 * InMaxPartsNum;
	IndexBuffer.NumIndices = 6 * InMaxPartsNum;
	VertexFactory.Init(&VertexBuffer);

	BeginInitResource(&VertexBuffer);
	BeginInitResource(&IndexBuffer);
	BeginInitResource(&VertexFactory);

	bVerifyUsedMaterials = false;
}

// デストラクタ
FSsRenderPartsProxy::~FSsRenderPartsProxy()
{
	VertexBuffer.ReleaseResource();
	IndexBuffer.ReleaseResource();
	VertexFactory.ReleaseResource();
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
				FMeshBatchElement& BatchElement = Mesh.Elements[0];
				BatchElement.IndexBuffer = &IndexBuffer;
				Mesh.bDisableBackfaceCulling = true;
				Mesh.bWireframe = bWireframe;
				Mesh.VertexFactory = &VertexFactory;
				Mesh.MaterialRenderProxy = MaterialProxy;
				BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
				BatchElement.FirstIndex = (StartPartIndex * 6);
				BatchElement.NumPrimitives = (NumParts * 2);
				BatchElement.MinVertexIndex = (StartPartIndex * 4);
				BatchElement.MaxVertexIndex = ((StartPartIndex + NumParts) * 4) - 1;
				BatchElement.UserData = GetBlendTypeAddr(RenderParts[i].AlphaBlendType);
				Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
				Mesh.Type = PT_TriangleList;
				Mesh.DepthPriorityGroup = SDPG_World;
				Mesh.bCanApplyViewModeOverrides = false;
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
	// どこかでちゃんと精査しないと・・・
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = IsShown(View);
	Result.bOpaqueRelevance = true;
	Result.bNormalTranslucencyRelevance = false;
	Result.bDynamicRelevance = true;
	Result.bShadowRelevance = IsShadowCast(View);
	Result.bSeparateTranslucencyRelevance = true;
//	Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
	return Result;
}

uint32 FSsRenderPartsProxy::GetMemoryFootprint() const
{
	return sizeof(*this) + GetAllocatedSize();
}

void FSsRenderPartsProxy::SetDynamicData_RenderThread(const TArray<FSsRenderPartWithMaterial>& InRenderParts)
{
	RenderParts = InRenderParts;

	if(0 < RenderParts.Num())
	{
		void* VertexBufferData = RHILockVertexBuffer(VertexBuffer.VertexBufferRHI, 0, RenderParts.Num() * 4 * sizeof(FSsPartVertex), RLM_WriteOnly);
		for(int32 i = 0; i < RenderParts.Num(); ++i)
		{
			for(int32 v = 0; v < 4; ++v)
			{
				((FSsPartVertex*)VertexBufferData)[i*4+v].Position  = FVector(
					0.f,
					( RenderParts[i].Vertices[v].Position.X - 0.5f - Pivot.X) * CanvasSizeUU.X,
					(-RenderParts[i].Vertices[v].Position.Y + 0.5f - Pivot.Y) * CanvasSizeUU.Y
					);

				((FSsPartVertex*)VertexBufferData)[i*4+v].Color     = RenderParts[i].Vertices[v].Color;
				((FSsPartVertex*)VertexBufferData)[i*4+v].TangentX  = FVector(1.f, 0.f, 0.f);
				((FSsPartVertex*)VertexBufferData)[i*4+v].TangentZ  = FVector(0.f, 0.f, 1.f);
				((FSsPartVertex*)VertexBufferData)[i*4+v].TexCoord  = RenderParts[i].Vertices[v].TexCoord;

				// ColorBlend.X にカラーブレンドモード (マテリアルを分けるのでコレは不要) 
/*				if(RenderParts[i].ColorBlendType != SsBlendType::Invalid)
				{
					((FSsPartVertex*)VertexBufferData)[i*4+v].ColorBlend.X = (float)(RenderParts[i].ColorBlendType + 0.01f);
				}
				else
				{
					((FSsPartVertex*)VertexBufferData)[i*4+v].ColorBlend.X = 4.01f;
				}
*/
				((FSsPartVertex*)VertexBufferData)[i*4+v].ColorBlend.Y = RenderParts[i].Vertices[v].ColorBlendRate;
			}
		}
		RHIUnlockVertexBuffer(VertexBuffer.VertexBufferRHI);
	}
}
