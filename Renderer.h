#pragma once

#include "VoodooEngineDLLExport.h"
#include "CollisionComponent.h"
#include "BitmapComponent.h"
#include <vector> 
#include <d2d1.h>

class VoodooEngine;

// Render layer names used by the level editor
struct SRenderLayerNames
{
	const wchar_t* RenderlayerName_0 = { L"RenderLayer 0" };
	const wchar_t* RenderlayerName_1 = { L"RenderLayer 1" };
	const wchar_t* RenderlayerName_2 = { L"RenderLayer 2" };
	const wchar_t* RenderlayerName_3 = { L"RenderLayer 3" };
	const wchar_t* RenderlayerName_4 = { L"RenderLayer 4" };
	const wchar_t* RenderlayerName_5 = { L"RenderLayer 5" };
	const wchar_t* RenderlayerName_6 = { L"RenderLayer 6" };
	const wchar_t* RenderlayerName_7 = { L"RenderLayer 7" };
	const wchar_t* RenderlayerName_8 = { L"RenderLayer 8" };
	const wchar_t* RenderlayerName_9 = { L"RenderLayer 9" };
	const wchar_t* RenderlayerName_10 = { L"RenderLayer 10" };
};

// Called by the engine during init
extern "C" VOODOOENGINE_API ID2D1HwndRenderTarget* SetupRenderer(
	ID2D1HwndRenderTarget* Renderer, HWND HWind);

// Optional can be called during the game in conjuction with the interface "IRender" 
// to override rendering of an object to be rendered on top of every other game object regardless of render layer
extern "C" void RenderBitmap(ID2D1HwndRenderTarget* Renderer, BitmapComponent* BitmapToRender);

// Called during the game loop
extern "C" VOODOOENGINE_API void Render(VoodooEngine* Engine);

// Only called internally in Renderer.cpp
//---------------------
void RenderBitmaps(
	ID2D1HwndRenderTarget* Renderer, std::vector<BitmapComponent*> BitmapsToRender,
	int MaxNumRenderLayers = 0);
//---------------------
