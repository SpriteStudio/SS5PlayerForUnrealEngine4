#pragma once

#include "PrimitiveSceneProxy.h"


// VertexBuffer
class FSsVertexBuffer : public FVertexBuffer
{
public:
	virtual void InitRHI() override;
	int32 NumVerts;
};

// IndexBuffer
class FSsIndexBuffer : public FIndexBuffer
{
public:
	virtual void InitRHI() override;
	int32 NumIndices;
};

// VertexFactory
class FSsVertexFactory : public FLocalVertexFactory
{
public:
	void Init(const FSsVertexBuffer* VertexBuffer);
};


// RenderProxy
class FSsRenderSceneProxy : public FPrimitiveSceneProxy
{
public:
	FSsRenderSceneProxy(class USsPlayerComponent* InComponent, UMaterialInterface* InMaterial);
	virtual ~FSsRenderSceneProxy();

	// FPrimitiveSceneProxy interface
#if defined(SS_UE4_4) || defined(SS_UE4_5) || defined(SS_UE4_6)
	virtual void DrawDynamicElements(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
#else
	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;
#endif
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) override;
	virtual void OnTransformChanged() override;
	virtual uint32 GetMemoryFootprint() const override;


	void SetDynamicData_RenderThread();
	void SetMaterial(UMaterialInterface* InMaterial) { Material = InMaterial; }

public:
	FVector2D MeshPlaneSize;

private:
	FVector Origin;

	USsPlayerComponent* Component;

	UPROPERTY()
	UMaterialInterface* Material;

	FSsVertexBuffer     VertexBuffer;
	FSsIndexBuffer      IndexBuffer;
	FSsVertexFactory    VertexFactory;
};
