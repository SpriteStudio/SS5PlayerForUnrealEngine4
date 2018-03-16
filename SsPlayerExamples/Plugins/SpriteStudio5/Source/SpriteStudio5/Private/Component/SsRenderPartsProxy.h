#pragma once

#include "RHIDefinitions.h"
#include "PrimitiveSceneProxy.h"
#include "StaticMeshResources.h"


// IndexBuffer
class FSsPartsIndexBuffer : public FIndexBuffer
{
public:
	virtual void InitRHI() override;
	uint32 NumIndices;
};

// VertexFactory
class FSsPartsVertexFactory : public FLocalVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE(FSsPartsVertexFactory);
public:
	FSsPartsVertexFactory(ERHIFeatureLevel::Type InFeatureLevel, const char* InDebugName)
		: FLocalVertexFactory(InFeatureLevel, InDebugName)
	{}

	static bool ShouldCompilePermutation(EShaderPlatform Platform, const class FMaterial* Material, const class FShaderType* ShaderType)
	{
		return FLocalVertexFactory::ShouldCompilePermutation(Platform, Material, ShaderType);
	}
	static void ModifyCompilationEnvironment(EShaderPlatform Platform, const FMaterial* Material, FShaderCompilerEnvironment& OutEnvironment)
	{
		FLocalVertexFactory::ModifyCompilationEnvironment(Platform, Material, OutEnvironment);
	}
	static bool SupportsTessellationShaders()
	{
		return FLocalVertexFactory::SupportsTessellationShaders();
	}
	static FVertexFactoryShaderParameters* ConstructShaderParameters(EShaderFrequency ShaderFrequency);
};

// VertexFactoryShaderParameters
class FSsPartVertexFactoryShaderParameters : public FVertexFactoryShaderParameters
{
public:
	virtual void Bind(const class FShaderParameterMap& ParameterMap) override {}
	virtual void Serialize(FArchive& Ar) override {}
	virtual void SetMesh(FRHICommandList& RHICmdList, FShader* Shader,const class FVertexFactory* VertexFactory,const class FSceneView& View,const struct FMeshBatchElement& BatchElement,uint32 DataFlags) const override;
	virtual uint32 GetSize() const override { return sizeof(*this); }
};


// RenderProxy
class FSsRenderPartsProxy : public FPrimitiveSceneProxy
{
public:
	FSsRenderPartsProxy(class USsPlayerComponent* InComponent, uint32 InMaxPartsNum);
	virtual ~FSsRenderPartsProxy();

	// FPrimitiveSceneProxy interface
	virtual SIZE_T GetTypeHash() const override;
	virtual void CreateRenderThreadResources() override;
	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;
	virtual uint32 GetMemoryFootprint() const override;


	void SetDynamicData_RenderThread(const TArray<FSsRenderPartWithMaterial>& InRenderParts);
	void SetPivot(const FVector2D& InPivot) { Pivot = InPivot; }

public:
	FVector2D CanvasSizeUU;

private:
	USsPlayerComponent* Component;

	TArray<FSsRenderPartWithMaterial> RenderParts;
	FVector2D Pivot;

	uint32 MaxPartsNum;

	FStaticMeshVertexBuffers VertexBuffers;
	FSsPartsIndexBuffer IndexBuffer;
	FSsPartsVertexFactory VertexFactory;
};
