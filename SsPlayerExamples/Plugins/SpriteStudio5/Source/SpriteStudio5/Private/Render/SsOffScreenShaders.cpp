﻿#include "SpriteStudio5PrivatePCH.h"
#include "SsOffScreenShaders.h"

#include "ShaderParameterUtils.h"


IMPLEMENT_SHADER_TYPE(, FSsOffScreenVS, TEXT("/Plugin/SpriteStudio5/Private/SsOffScreenShader.usf"), TEXT("MainVS"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FSsOffScreenPS, TEXT("/Plugin/SpriteStudio5/Private/SsOffScreenShader.usf"), TEXT("MainPS"), SF_Pixel);

TGlobalResource<FSsOffScreenVertexDeclaration> GSsOffScreenVertexDeclaration;

// 
void FSsOffScreenVertexDeclaration::InitRHI()
{
	FVertexDeclarationElementList Elements;
	uint32 Stride = sizeof(FSsOffScreenVertex);
	Elements.Add(FVertexElement(0, STRUCT_OFFSET(FSsOffScreenVertex, Position), VET_Float2, 0, Stride));
	Elements.Add(FVertexElement(0, STRUCT_OFFSET(FSsOffScreenVertex, Color), VET_Color, 1, Stride));
	Elements.Add(FVertexElement(0, STRUCT_OFFSET(FSsOffScreenVertex, TexCoord), VET_Float4, 2, Stride));
	Elements.Add(FVertexElement(0, STRUCT_OFFSET(FSsOffScreenVertex, ColorBlend), VET_Float2, 3, Stride));
	
	VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
}
void FSsOffScreenVertexDeclaration::ReleaseRHI()
{
	VertexDeclarationRHI.SafeRelease();
}

//
FSsOffScreenPS::FSsOffScreenPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	CellTextureParameter.Bind(Initializer.ParameterMap, TEXT("CellTexture"));
	CellTextureParameterSampler.Bind(Initializer.ParameterMap, TEXT("CellTextureSampler"));
}
bool FSsOffScreenPS::Serialize(FArchive& Ar)
{
	bool bShaderHasOutdatedParams = FGlobalShader::Serialize(Ar);

	Ar << CellTextureParameter;
	Ar << CellTextureParameterSampler;

	return bShaderHasOutdatedParams;
}
void FSsOffScreenPS::SetCellTexture(FRHICommandList& RHICmdList, const FTextureRHIParamRef InTexture, const FSamplerStateRHIRef SamplerState )
{
	SetTextureParameter(RHICmdList, GetPixelShader(), CellTextureParameter, CellTextureParameterSampler, SamplerState, InTexture );
}
