#pragma once

#include "VoodooEngineDLLExport.h"
#include "TransformComponent.h"
#include <string>
#include <d2d1.h>

struct SBitmapParameters
{
	int RenderLayer = 0;
	float Opacity = 1;
	bool BitmapSetToNotRender = false;
	SVector BitmapOffsetLeft;
	SVector BitmapOffsetRight;
	SVector BitmapSource;
};

class BitmapComponent : public TransformComponent
{
public:
	ID2D1Bitmap* Bitmap = nullptr;
	SBitmapParameters BitmapParams = {};
};

extern "C" VOODOOENGINE_API ID2D1Bitmap* SetupBitmap(
	ID2D1Bitmap* BitmapToSetup, const wchar_t* FileName, ID2D1HwndRenderTarget* Renderer, bool FlipBitmap = false);

extern "C" VOODOOENGINE_API void SetupBitmapComponent(
	BitmapComponent* BitmapComponentToSetup,
	ID2D1Bitmap* TextureAtlasBitmap,
	SVector TextureAtlasWidthHeight = {},
	int TextureAtlasOffsetMultiplierHeight = 1,
	bool UseEntireTextureAtlasAsBitmapSource = true);

extern "C" VOODOOENGINE_API void SetBitmapSourceLocationX(
	BitmapComponent* BitmapToUpdate, int BitmapSourceWidth, int LocationOffsetMultiplier = 1);

extern "C" VOODOOENGINE_API void SetBitmapSourceLocationY(
	BitmapComponent* BitmapToUpdate, int BitmapSourceHeight, int LocationOffsetMultiplier = 1);
