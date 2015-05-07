﻿#include "SpriteStudio5EdPrivatePCH.h"
#include "SsProjectViewerViewportClient.h"

#include "Engine/Canvas.h"

#include "SsPlayer.h"
#include "SsRenderOffScreen.h"


FSsProjectViewerViewportClient::FSsProjectViewerViewportClient()
	: bDrawGrid(false)
	, GridSize(64)
	, GridColor(FLinearColor::Green)
	, RenderScale(1.f)
	, RenderScaleStep(0.05f)
	, RenderOffset(0.f ,0.f)
	, BackgroundColor(.2f, .2f, .2f)
	, Player(NULL)
	, Render(NULL)
{
}

void FSsProjectViewerViewportClient::Draw(FViewport* Viewport, FCanvas* Canvas)
{
	FIntPoint ViewportSize = Viewport->GetSizeXY();

	FVector2D AnimCanvasSize = Player->GetAnimCanvasSize();
	FVector2D AnimPivot = Player->GetAnimPivot();

	// アニメーション範囲外は黒塗りつぶし 
	Canvas->Clear(FLinearColor::Black);

	// Ssの描画
	UTexture* Texture = Render->GetRenderTarget();
	if(Texture)
	{
		FCanvasTileItem Tile(
			FVector2D(
				ViewportSize.X/2 - (AnimCanvasSize.X/2 * RenderScale) + RenderOffset.X,
				ViewportSize.Y/2 - (AnimCanvasSize.Y/2 * RenderScale) + RenderOffset.Y
				),
			Texture->Resource,
			FLinearColor::White
			);
		Tile.Size = (AnimCanvasSize * RenderScale);
		Tile.BlendMode = SE_BLEND_Opaque;
		Canvas->DrawItem(Tile);
	}

	// グリッド
	if(bDrawGrid && (0 < GridSize))
	{
		FVector2D GridCenter(
			ViewportSize.X/2 + (AnimPivot.X * AnimCanvasSize.X * RenderScale) + RenderOffset.X,
			ViewportSize.Y/2 - (AnimPivot.Y * AnimCanvasSize.Y * RenderScale) + RenderOffset.Y
			);
		DrawGrid(Viewport, Canvas, GridCenter);
	}
}

bool FSsProjectViewerViewportClient::InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad)
{
	if(Key == EKeys::MouseScrollUp)
	{
		RenderScale += RenderScaleStep;
		return true;
	}
	else if(Key == EKeys::MouseScrollDown)
	{
		RenderScale -= RenderScaleStep;
		if(RenderScale < 0.1f)
		{
			RenderScale = 0.1f;
		}
		return true;
	}
	else if((Key == EKeys::F) || (Key == EKeys::A))
	{
		RenderScale = 1.f;
		RenderOffset.X = RenderOffset.Y = 0;
		return true;
	}
	return false;
}

bool FSsProjectViewerViewportClient::InputAxis(FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	if(Key == EKeys::MouseX)
	{
		RenderOffset.X += Delta;
		return true;
	}
	else if(Key == EKeys::MouseY)
	{
		RenderOffset.Y -= Delta;
		return true;
	}
	return false;
}

void FSsProjectViewerViewportClient::DrawGrid(FViewport* Viewport, FCanvas* Canvas, const FVector2D& Center)
{
	FIntPoint Size = Viewport->GetSizeXY();

	int32 RenderGridSize = FMath::Max(2, (int32)(GridSize * RenderScale));

	for(int32 X = Center.X+RenderGridSize; X <= Size.X; X+=RenderGridSize)
	{
		FCanvasLineItem Line(FVector2D(X,0.f), FVector2D(X,Size.Y));
		Line.SetColor(GridColor);
		Canvas->DrawItem(Line);
	}
	for(int32 X = Center.X-RenderGridSize; 0 <= X; X-=RenderGridSize)
	{
		FCanvasLineItem Line(FVector2D(X,0.f), FVector2D(X,Size.Y));
		Line.SetColor(GridColor);
		Canvas->DrawItem(Line);
	}
	{
		FCanvasLineItem Line(FVector2D(Center.X,0.f), FVector2D(Center.X,Size.Y));
		Line.SetColor(GridColor);
		Line.LineThickness = 3.f;
		Canvas->DrawItem(Line);
	}

	for(int32 Y = Center.Y+RenderGridSize; Y <= Size.Y; Y+=RenderGridSize)
	{
		FCanvasLineItem Line(FVector2D(0.f,Y), FVector2D(Size.X,Y));
		Line.SetColor(GridColor);
		Canvas->DrawItem(Line);
	}
	for(int32 Y = Center.Y-RenderGridSize; 0 <= Y; Y-=RenderGridSize)
	{
		FCanvasLineItem Line(FVector2D(0.f,Y), FVector2D(Size.X,Y));
		Line.SetColor(GridColor);
		Canvas->DrawItem(Line);
	}
	{
		FCanvasLineItem Line(FVector2D(0.f,Center.Y), FVector2D(Size.X,Center.Y));
		Line.SetColor(GridColor);
		Line.LineThickness = 3.f;
		Canvas->DrawItem(Line);
	}
}

void FSsProjectViewerViewportClient::SetPlayer(FSsPlayer* InPlayer, FSsRenderOffScreen* InRender)
{
	Player = InPlayer;
	Render = InRender;
	SetBackgroundColor(BackgroundColor);
}

void FSsProjectViewerViewportClient::SetBackgroundColor(const FLinearColor& InBackgroundColor)
{
	BackgroundColor = InBackgroundColor;

	if(Render)
	{
		// ガンマ補正を逆算して適用 
		// ビューアの背景色だけは、指定したカラーを直接反映させられるように 
		Render->ClearColor = FColor(
			(uint8)(FMath::Pow(BackgroundColor.R, GEngine->GetDisplayGamma()) * 255.f),
			(uint8)(FMath::Pow(BackgroundColor.G, GEngine->GetDisplayGamma()) * 255.f),
			(uint8)(FMath::Pow(BackgroundColor.B, GEngine->GetDisplayGamma()) * 255.f),
			(uint8)(BackgroundColor.A * 255.f)
			);
	}
}
