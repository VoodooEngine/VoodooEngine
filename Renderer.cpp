#include "Renderer.h"
#include "VoodooEngine.h"

ID2D1HwndRenderTarget* SetupRenderer(ID2D1HwndRenderTarget* Renderer, HWND HWind)
{
	ID2D1Factory* Factory = nullptr;
	HRESULT Result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &Factory);

	RECT WinRect;
	// Get user screen size
	GetClientRect(HWind, &WinRect);

	Result = Factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			HWind, D2D1::SizeU(WinRect.right, WinRect.bottom)),
		&Renderer);

	Factory->Release();

	return Renderer;
}

void AssignCollisionRectangleToRender(
	ID2D1HwndRenderTarget* Renderer, CollisionComponent* CollisionRectToRender)
{
	if (!CollisionRectToRender->RenderCollisionRect)
	{
		return;
	}

	// "1" is alpha value
	const D2D1_COLOR_F Color =
	{ CollisionRectToRender->CollisionRectColor.R,
	CollisionRectToRender->CollisionRectColor.G,
	CollisionRectToRender->CollisionRectColor.B, 1 };

	ID2D1SolidColorBrush* Brush;
	Renderer->CreateSolidColorBrush(Color, &Brush);

	Brush->SetOpacity(CollisionRectToRender->Opacity);

	D2D1_RECT_F Rect = D2D1::RectF(
		CollisionRectToRender->ComponentLocation.X,
		CollisionRectToRender->ComponentLocation.Y,
		CollisionRectToRender->ComponentLocation.X +
		CollisionRectToRender->CollisionRect.X,
		CollisionRectToRender->ComponentLocation.Y +
		CollisionRectToRender->CollisionRect.Y);

	if (CollisionRectToRender->DrawFilledRectangle)
	{
		Renderer->FillRectangle(Rect, Brush);
	}
	else
	{
		Renderer->DrawRectangle(Rect, Brush);
	}

	Brush->Release();
}

void RenderCollisionRectangles(ID2D1HwndRenderTarget* Renderer,
	std::vector<CollisionComponent*> CollisionRectsToRender)
{
	for (int i = 0; i < CollisionRectsToRender.size(); ++i)
	{
		AssignCollisionRectangleToRender(Renderer, CollisionRectsToRender[i]);
	}
}

void RenderBitmap(ID2D1HwndRenderTarget* Renderer, BitmapComponent* BitmapToRender)
{
	if (!BitmapToRender)
	{
		return;
	}

	D2D_RECT_F DestRect =
		D2D1::RectF(
			BitmapToRender->ComponentLocation.X,
			BitmapToRender->ComponentLocation.Y,
			BitmapToRender->ComponentLocation.X + BitmapToRender->BitmapParams.BitmapOffsetRight.X,
			BitmapToRender->ComponentLocation.Y + BitmapToRender->BitmapParams.BitmapOffsetRight.Y);

	D2D_RECT_F SourceRect =
		D2D1::RectF(
			BitmapToRender->BitmapParams.BitmapOffsetLeft.X,
			BitmapToRender->BitmapParams.BitmapOffsetLeft.Y,
			BitmapToRender->BitmapParams.BitmapSource.X,
			BitmapToRender->BitmapParams.BitmapSource.Y);

	Renderer->DrawBitmap(
		BitmapToRender->Bitmap,
		DestRect,
		BitmapToRender->BitmapParams.Opacity,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
		SourceRect);
}

void RenderBitmapByLayer(ID2D1HwndRenderTarget* Renderer,
	std::vector<BitmapComponent*> StoredBitmaps, int RenderLayer)
{
	for (int i = 0; i < StoredBitmaps.size(); ++i)
	{
		// Go to next if bitmap is not valid
		if (!StoredBitmaps[i]->Bitmap)
		{ 
			continue;
		}

		// Go to next bitmap if set to be hidden in game
		if (StoredBitmaps[i]->BitmapParams.BitmapSetToNotRender)
		{
			continue;
		}

		// Go to next if bitmap is not assigned as current render layer
		if (StoredBitmaps[i]->BitmapParams.RenderLayer != RenderLayer)
		{ 
			continue;
		}

		RenderBitmap(Renderer, StoredBitmaps[i]);
	}
}

void RenderBitmaps(ID2D1HwndRenderTarget* Renderer,
	std::vector<BitmapComponent*> BitmapsToRender, int MaxNumRenderLayers)
{
	// "+1" is there to account for the last render layer 
	for (int i = 0; i < (MaxNumRenderLayers + 1); ++i)
	{
		RenderBitmapByLayer(Renderer, BitmapsToRender, i);
	}
}

void RenderLevelEditor(VoodooEngine* Engine)
{
	if (!Engine->EditorMode)
	{
		return;
	}

	RenderBitmaps(Engine->Renderer, Engine->StoredEditorBitmapComponents);
	RenderBitmaps(Engine->Renderer, Engine->StoredButtonBitmapComponents);
	RenderBitmaps(Engine->Renderer, Engine->StoredButtonTexts);
}

void RenderUITextsRenderLayer(VoodooEngine* Engine)
{
	D2D1_RECT_F OriginTextLocation = D2D1::RectF(1680.f, 110.f, 2000.f, 110.f);
	float OffsetLocationY = 50;
	for (int i = 0; i < Engine->StoredLevelEditorRenderLayers.size(); ++i)
	{
		auto Iterator = Engine->StoredLevelEditorRenderLayers.find(i);
		if (Iterator->second.HideText)
		{
			return;
		}

		if (Iterator->second.TextRenderType == ETextBrushColorType::BlackBrush)
		{
			Engine->Renderer->DrawText(
				Iterator->second.Text,
				wcslen(Iterator->second.Text),
				Engine->TextFormat,
				OriginTextLocation,
				Engine->BlackBrush);
		}
		else if (Iterator->second.TextRenderType == ETextBrushColorType::WhiteBrush)
		{
			Engine->Renderer->DrawText(
				Iterator->second.Text,
				wcslen(Iterator->second.Text),
				Engine->TextFormat,
				OriginTextLocation,
				Engine->WhiteBrush);
		}

		OriginTextLocation.top += OffsetLocationY;
		OriginTextLocation.bottom += OffsetLocationY;
	}
}

void RenderCustomMouseCursor(ID2D1HwndRenderTarget* Renderer, VoodooEngine* Engine)
{
	// Render mouse collider as fallback if no custom cursor image file is found or in debug mode
	if (Engine->Mouse.MouseBitmap.Bitmap == nullptr ||
		Engine->DebugMode)
	{
		// Skip rendereing altogether if set to not render
		if (!Engine->Mouse.MouseCollider.RenderCollisionRect)
		{
			return;
		}

		D2D1_RECT_F Rect = D2D1::RectF(
			Engine->Mouse.MouseCollider.ComponentLocation.X,
			Engine->Mouse.MouseCollider.ComponentLocation.Y,
			Engine->Mouse.MouseCollider.ComponentLocation.X +
			Engine->Mouse.MouseCollider.CollisionRect.X,
			Engine->Mouse.MouseCollider.ComponentLocation.Y +
			Engine->Mouse.MouseCollider.CollisionRect.Y);

		Renderer->DrawRectangle(Rect, Engine->WhiteBrush);
	}

	if (Engine->Mouse.MouseBitmap.Bitmap == nullptr ||
		Engine->Mouse.MouseBitmap.BitmapParams.BitmapSetToNotRender)
	{
		return;
	}

	D2D_RECT_F DestRect =
		D2D1::RectF(
			Engine->Mouse.MouseBitmap.ComponentLocation.X,
			Engine->Mouse.MouseBitmap.ComponentLocation.Y,
			Engine->Mouse.MouseBitmap.ComponentLocation.X +
			Engine->Mouse.MouseBitmap.BitmapParams.BitmapOffsetRight.X,
			Engine->Mouse.MouseBitmap.ComponentLocation.Y +
			Engine->Mouse.MouseBitmap.BitmapParams.BitmapOffsetRight.Y);

	D2D_RECT_F SourceRect =
		D2D1::RectF(
			Engine->Mouse.MouseBitmap.BitmapParams.BitmapOffsetLeft.X,
			Engine->Mouse.MouseBitmap.BitmapParams.BitmapOffsetLeft.Y,
			Engine->Mouse.MouseBitmap.BitmapParams.BitmapSource.X,
			Engine->Mouse.MouseBitmap.BitmapParams.BitmapSource.Y);

	Renderer->DrawBitmap(
		Engine->Mouse.MouseBitmap.Bitmap,
		DestRect,
		// Always render mouse cursor at full opacity
		1,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
		SourceRect);
}

void Render(VoodooEngine* Engine)
{
	// NOTE - 
	// We use painter's algorithm so the stuff that gets called to render last will be in front of everything else

	// Render the game background
	RenderBitmap(Engine->Renderer, Engine->CurrentLevelBackground);

	// Render all bitmaps (from gameobjects) stored in engine
	RenderBitmaps(Engine->Renderer, Engine->StoredBitmapComponents, RENDERLAYER_MAXNUM);
	
	// Render all collision rects
	RenderCollisionRectangles(
		Engine->Renderer, Engine->StoredCollisionComponents);

	// Call render interface to all inherited objects 
	// (If you want to override an object to render in front of everything else)
	for (int i = 0; i < Engine->InterfaceObjects_Render.size(); ++i)
	{
		Engine->InterfaceObjects_Render[i]->InterfaceEvent_Render(Engine->Renderer);
	}

	// Render level editor related stuff
	if (Engine->EditorMode)
	{
		RenderLevelEditor(Engine);
		RenderUITextsRenderLayer(Engine);
	}

	// Render debug related stuff
	if (Engine->DebugMode)
	{
		RenderCollisionRectangles(
			Engine->Renderer, Engine->StoredEditorCollisionComponents);
		RenderBitmaps(
			Engine->Renderer, Engine->StoredScreenPrintTexts);
	}

	// This replaces the default windows system mouse cursor 
	// (The windows system mouse cursor is hidden)
	// Always rendered on top of everything else
	RenderCustomMouseCursor(Engine->Renderer, Engine);
}
